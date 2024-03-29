
/*
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "thread_pool.h"

#include <sys/mman.h>
#include <sys/resource.h>
#include <sys/time.h>

#include <pthread.h>

#include "base/bit_utils.h"
#include "base/stl_util.h"
#include "base/time_utils.h"
#include "base/utils.h"
#include "base/macros.h"

namespace art {

using android::base::StringPrintf;

static constexpr bool kMeasureWaitTime = false;

ThreadPoolWorker::ThreadPoolWorker(ThreadPool* thread_pool, const std::string& name,
                                   size_t stack_size)
    : thread_pool_(thread_pool),
      name_(name) {
  // Add an inaccessible page to catch stack overflow.
  stack_size += kPageSize;
  std::string error_msg;
  stack_ = MemMap::MapAnonymous(name.c_str(),
                                stack_size,
                                PROT_READ | PROT_WRITE,
                                /*low_4gb=*/ false,
                                &error_msg);
  CHECK(stack_.IsValid()) << error_msg;
  CHECK_ALIGNED(stack_.Begin(), kPageSize);
  CheckedCall(mprotect,
              "mprotect bottom page of thread pool worker stack",
              stack_.Begin(),
              kPageSize,
              PROT_NONE);
  const char* reason = "new thread pool worker thread";
  pthread_attr_t attr;
  CHECK_PTHREAD_CALL(pthread_attr_init, (&attr), reason);
  CHECK_PTHREAD_CALL(pthread_attr_setstack, (&attr, stack_.Begin(), stack_.Size()), reason);
  CHECK_PTHREAD_CALL(pthread_create, (&pthread_, &attr, &Callback, this), reason);
  CHECK_PTHREAD_CALL(pthread_attr_destroy, (&attr), reason);
}

ThreadPoolWorker::~ThreadPoolWorker() {
  CHECK_PTHREAD_CALL(pthread_join, (pthread_, nullptr), "thread pool worker shutdown");
}

void ThreadPoolWorker::SetPthreadPriority(int priority) {
  CHECK_GE(priority, PRIO_MIN);
  CHECK_LE(priority, PRIO_MAX);
#if defined(ART_TARGET_ANDROID)
  int result = setpriority(PRIO_PROCESS, pthread_gettid_np(pthread_), priority);
  if (result != 0) {
    PLOG(ERROR) << "Failed to setpriority to :" << priority;
  }
#else
  UNUSED(priority);
#endif
}

void ThreadPoolWorker::Run() {
  Task* task = nullptr;
  thread_pool_->creation_barier_.Pass();
  while ((task = thread_pool_->GetTask()) != nullptr) {
    task->Run();
    task->Finalize();
  }
}

void* ThreadPoolWorker::Callback(void* arg) {
  ThreadPoolWorker* worker = reinterpret_cast<ThreadPoolWorker*>(arg);
  // Do work until its time to shut down.
  worker->Run();
  return nullptr;
}

void ThreadPool::AddTask(Task* task) {
  MutexLock mu(task_queue_lock_);
  tasks_.push_back(task);
  // If we have any waiters, signal one.
  if (started_ && waiting_count_ != 0) {
    task_queue_condition_.Signal();
  }
}

void ThreadPool::RemoveAllTasks() {
  MutexLock mu(task_queue_lock_);
  tasks_.clear();
}

ThreadPool::ThreadPool(const char* name,
                       size_t num_threads,
                       size_t worker_stack_size)
  : name_(name),
    task_queue_lock_("task queue lock"),
    task_queue_condition_("task queue condition", task_queue_lock_),
    completion_condition_("task completion condition", task_queue_lock_),
    started_(false),
    shutting_down_(false),
    waiting_count_(0),
    start_time_(0),
    total_wait_time_(0),
    creation_barier_(0),
    max_active_workers_(num_threads),
    worker_stack_size_(worker_stack_size) {
  CreateThreads();
}

void ThreadPool::CreateThreads() {
  CHECK(threads_.empty());
  {
    MutexLock mu(task_queue_lock_);
    shutting_down_ = false;
    // Add one since the caller of constructor waits on the barrier too.
    creation_barier_.Init(max_active_workers_);
    while (GetThreadCount() < max_active_workers_) {
      const std::string worker_name = StringPrintf("%s worker thread %zu", name_.c_str(),
                                                   GetThreadCount());
      threads_.push_back(
          new ThreadPoolWorker(this, worker_name, worker_stack_size_));
    }
  }
}

void ThreadPool::WaitForWorkersToBeCreated() {
  creation_barier_.Increment(0);
}

const std::vector<ThreadPoolWorker*>& ThreadPool::GetWorkers() {
  // Wait for all the workers to be created before returning them.
  WaitForWorkersToBeCreated();
  return threads_;
}

void ThreadPool::DeleteThreads() {
  {
    MutexLock mu(task_queue_lock_);
    // Tell any remaining workers to shut down.
    shutting_down_ = true;
    // Broadcast to everyone waiting.
    task_queue_condition_.Broadcast();
    completion_condition_.Broadcast();
  }
  // Wait for the threads to finish. We expect the user of the pool
  // not to run multi-threaded calls to `CreateThreads` and `DeleteThreads`,
  // so we don't guard the field here.
  STLDeleteElements(&threads_);
}

void ThreadPool::SetMaxActiveWorkers(size_t max_workers) {
  MutexLock mu(task_queue_lock_);
  CHECK_LE(max_workers, GetThreadCount());
  max_active_workers_ = max_workers;
}

ThreadPool::~ThreadPool() {
  DeleteThreads();
}

void ThreadPool::StartWorkers() {
  MutexLock mu(task_queue_lock_);
  started_ = true;
  task_queue_condition_.Broadcast();
  start_time_ = NanoTime();
  total_wait_time_ = 0;
}

void ThreadPool::StopWorkers() {
  MutexLock mu(task_queue_lock_);
  started_ = false;
}

Task* ThreadPool::GetTask() {
  MutexLock mu(task_queue_lock_);
  while (!IsShuttingDown()) {
    const size_t thread_count = GetThreadCount();
    // Ensure that we don't use more threads than the maximum active workers.
    const size_t active_threads = thread_count - waiting_count_;
    // <= since self is considered an active worker.
    if (active_threads <= max_active_workers_) {
      Task* task = TryGetTaskLocked();
      if (task != nullptr) {
        return task;
      }
    }

    ++waiting_count_;
    if (waiting_count_ == GetThreadCount() && !HasOutstandingTasks()) {
      // We may be done, lets broadcast to the completion condition.
      completion_condition_.Broadcast();
    }
    const uint64_t wait_start = kMeasureWaitTime ? NanoTime() : 0;
    task_queue_condition_.Wait();
    if (kMeasureWaitTime) {
      const uint64_t wait_end = NanoTime();
      total_wait_time_ += wait_end - std::max(wait_start, start_time_);
    }
    --waiting_count_;
  }

  // We are shutting down, return null to tell the worker thread to stop looping.
  return nullptr;
}

Task* ThreadPool::TryGetTask() {
  MutexLock mu(task_queue_lock_);
  return TryGetTaskLocked();
}

Task* ThreadPool::TryGetTaskLocked() {
  if (HasOutstandingTasks()) {
    Task* task = tasks_.front();
    tasks_.pop_front();
    return task;
  }
  return nullptr;
}

void ThreadPool::Wait(bool do_work, bool may_hold_locks) {
  if (do_work) {
    Task* task = nullptr;
    while ((task = TryGetTask()) != nullptr) {
      task->Run();
      task->Finalize();
    }
  }
  // Wait until each thread is waiting and the task list is empty.
  MutexLock mu(task_queue_lock_);
  while (!shutting_down_ && (waiting_count_ != GetThreadCount() || HasOutstandingTasks())) {
    if (!may_hold_locks) {
      completion_condition_.Wait();
    } else {
      completion_condition_.WaitHoldingLocks();
    }
  }
}

size_t ThreadPool::GetTaskCount() {
  MutexLock mu(task_queue_lock_);
  return tasks_.size();
}

void ThreadPool::SetPthreadPriority(int priority) {
  for (ThreadPoolWorker* worker : threads_) {
    worker->SetPthreadPriority(priority);
  }
}

}  // namespace art

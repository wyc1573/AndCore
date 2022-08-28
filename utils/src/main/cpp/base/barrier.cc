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

#include "barrier.h"

#include <android-base/logging.h>

#include "base/aborting.h"
#include "base/mutex.h"
#include "base/time_utils.h"

namespace art {

Barrier::Barrier(int count, bool verify_count_on_shutdown)
    : count_(count),
      lock_(new Mutex("GC barrier lock", kThreadSuspendCountLock)),
      condition_(new ConditionVariable("GC barrier condition", *lock_)),
      verify_count_on_shutdown_(verify_count_on_shutdown) {
}

template void Barrier::Increment<Barrier::kAllowHoldingLocks>(int delta);
template void Barrier::Increment<Barrier::kDisallowHoldingLocks>(int delta);

void Barrier::Pass() {
  MutexLock mu(*GetLock());
  SetCountLocked(count_ - 1);
}

void Barrier::Wait() {
  Increment(-1);
}

void Barrier::Init(int count) {
  MutexLock mu(*GetLock());
  SetCountLocked(count);
}

template <Barrier::LockHandling locks>
void Barrier::Increment(int delta) {
  MutexLock mu(*GetLock());
  SetCountLocked(count_ + delta);

  // Increment the count.  If it becomes zero after the increment
  // then all the threads have already passed the barrier.  If
  // it is non-zero then there is still one or more threads
  // that have not yet called the Pass function.  When the
  // Pass function is called by the last thread, the count will
  // be decremented to zero and a Broadcast will be made on the
  // condition variable, thus waking this up.
  while (count_ != 0) {
    if (locks == kAllowHoldingLocks) {
      condition_->WaitHoldingLocks();
    } else {
      condition_->Wait();
    }
  }
}

bool Barrier::Increment(int delta, uint32_t timeout_ms) {
  MutexLock mu(*GetLock());
  SetCountLocked(count_ + delta);
  bool timed_out = false;
  if (count_ != 0) {
    uint32_t timeout_ns = 0;
    uint64_t abs_timeout = NanoTime() + MsToNs(timeout_ms);
    for (;;) {
      timed_out = condition_->TimedWait(timeout_ms, timeout_ns);
      if (timed_out || count_ == 0) return timed_out;
      // Compute time remaining on timeout.
      uint64_t now = NanoTime();
      int64_t time_left = abs_timeout - now;
      if (time_left <= 0) return true;
      timeout_ns = time_left % (1000*1000);
      timeout_ms = time_left / (1000*1000);
    }
  }
  return timed_out;
}

int Barrier::GetCount() {
  MutexLock mu(*GetLock());
  return count_;
}

void Barrier::SetCountLocked(int count) {
  count_ = count;
  if (count == 0) {
    condition_->Broadcast();
  }
}

Barrier::~Barrier() {
  if (count_ != 0) {
    // Only check when not aborting and if we verify the count on shutdown.
    LOG((gAborting == 0 && verify_count_on_shutdown_) ? FATAL : WARNING)
        << "Attempted to destroy barrier with non zero count " << count_;
  }
}

}  // namespace art

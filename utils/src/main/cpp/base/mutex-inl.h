/*
 * Copyright (C) 2011 The Android Open Source Project
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

#ifndef ART_RUNTIME_BASE_MUTEX_INL_H_
#define ART_RUNTIME_BASE_MUTEX_INL_H_

#include <inttypes.h>

#include "mutex.h"

#include "base/utils.h"
#include "base/value_object.h"

#if ART_USE_FUTEXES
#include "linux/futex.h"
#include "sys/syscall.h"
#ifndef SYS_futex
#define SYS_futex __NR_futex
#endif
#endif  // ART_USE_FUTEXES

#define CHECK_MUTEX_CALL(call, args) CHECK_PTHREAD_CALL(call, args, name_)

namespace art {

#if ART_USE_FUTEXES
static inline int futex(volatile int *uaddr, int op, int val, const struct timespec *timeout,
                        volatile int *uaddr2, int val3) {
  return syscall(SYS_futex, uaddr, op, val, timeout, uaddr2, val3);
}
#endif  // ART_USE_FUTEXES

// The following isn't strictly necessary, but we want updates on Atomic<pid_t> to be lock-free.
// TODO: Use std::atomic::is_always_lock_free after switching to C++17 atomics.
static_assert(sizeof(pid_t) <= sizeof(int32_t), "pid_t should fit in 32 bits");

static inline pid_t SafeGetTid() {
  // todo 这里应该是跟Thread关联的，由于目前还没有把Thread移植过来，先用GetTid代替。效率较低
  return GetTid();
}

inline void BaseMutex::RegisterAsLocked() {
// todo...
}

inline void BaseMutex::RegisterAsUnlocked() {
// todo...
}

inline void ReaderWriterMutex::SharedLock() {
#if ART_USE_FUTEXES
  bool done = false;
  do {
    int32_t cur_state = state_.load(std::memory_order_relaxed);
    if (LIKELY(cur_state >= 0)) {
      // Add as an extra reader.
      done = state_.CompareAndSetWeakAcquire(cur_state, cur_state + 1);
    } else {
      HandleSharedLockContention(cur_state);
    }
  } while (!done);
#else
  CHECK_MUTEX_CALL(pthread_rwlock_rdlock, (&rwlock_));
#endif
  DCHECK(GetExclusiveOwnerTid() == 0 || GetExclusiveOwnerTid() == -1);
  RegisterAsLocked();
  AssertSharedHeld();
}

inline void ReaderWriterMutex::SharedUnlock() {
  DCHECK(GetExclusiveOwnerTid() == 0 || GetExclusiveOwnerTid() == -1);
  AssertSharedHeld();
  RegisterAsUnlocked();
#if ART_USE_FUTEXES
  bool done = false;
  do {
    int32_t cur_state = state_.load(std::memory_order_relaxed);
    if (LIKELY(cur_state > 0)) {
      // Reduce state by 1 and impose lock release load/store ordering.
      // Note, the relaxed loads below musn't reorder before the CompareAndSet.
      // TODO: the ordering here is non-trivial as state is split across 3 fields, fix by placing
      // a status bit into the state on contention.
      done = state_.CompareAndSetWeakSequentiallyConsistent(cur_state, cur_state - 1);
      if (done && (cur_state - 1) == 0) {  // Weak CAS may fail spuriously.
        if (num_pending_writers_.load(std::memory_order_seq_cst) > 0 ||
            num_pending_readers_.load(std::memory_order_seq_cst) > 0) {
          // Wake any exclusive waiters as there are now no readers.
          futex(state_.Address(), FUTEX_WAKE_PRIVATE, kWakeAll, nullptr, nullptr, 0);
        }
      }
    } else {
      LOG(FATAL) << "Unexpected state_:" << cur_state << " for " << name_;
    }
  } while (!done);
#else
  CHECK_MUTEX_CALL(pthread_rwlock_unlock, (&rwlock_));
#endif
}

inline bool Mutex::IsExclusiveHeld() const {
  bool result = (GetExclusiveOwnerTid() == SafeGetTid());
  return result;
}

inline pid_t Mutex::GetExclusiveOwnerTid() const {
  return exclusive_owner_.load(std::memory_order_relaxed);
}

inline void Mutex::AssertExclusiveHeld() const {
// todo...
}

inline void Mutex::AssertHeld() const {
  // todo...
  AssertExclusiveHeld();
}

inline bool ReaderWriterMutex::IsExclusiveHeld() const {
  // todo...
  return true;
}

inline pid_t ReaderWriterMutex::GetExclusiveOwnerTid() const {
#if ART_USE_FUTEXES
  int32_t state = state_.load(std::memory_order_relaxed);
  if (state == 0) {
    return 0;  // No owner.
  } else if (state > 0) {
    return -1;  // Shared.
  } else {
    return exclusive_owner_.load(std::memory_order_relaxed);
  }
#else
  return exclusive_owner_.load(std::memory_order_relaxed);
#endif
}

inline void ReaderWriterMutex::AssertExclusiveHeld() const {
// todo...
}

inline void ReaderWriterMutex::AssertWriterHeld() const {
  //todo...
}

inline ReaderMutexLock::ReaderMutexLock(ReaderWriterMutex& mu)
    : mu_(mu) {
  mu_.SharedLock();
}

inline ReaderMutexLock::~ReaderMutexLock() {
  mu_.SharedUnlock();
}

}  // namespace art

#endif  // ART_RUNTIME_BASE_MUTEX_INL_H_

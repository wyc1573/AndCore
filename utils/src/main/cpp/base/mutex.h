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

#ifndef ART_RUNTIME_BASE_MUTEX_H_
#define ART_RUNTIME_BASE_MUTEX_H_

#include <limits.h>  // for INT_MAX
#include <pthread.h>
#include <stdint.h>
#include <unistd.h>  // for pid_t

#include <iosfwd>
#include <string>

#include "atomic.h"
#include "macros.h"
#include "locks.h"

#if defined(__linux__)
#define ART_USE_FUTEXES 1
#else
#define ART_USE_FUTEXES 0
#endif

// Currently Darwin doesn't support locks with timeouts.
#if !defined(__APPLE__)
#define HAVE_TIMED_RWLOCK 1
#else
#define HAVE_TIMED_RWLOCK 0
#endif

namespace art {

class ReaderWriterMutex;
class MutatorMutex;
class ScopedContentionRecorder;
class Mutex;

constexpr bool kDebugLocking = kIsDebugBuild;

// Record Log contention information, dumpable via SIGQUIT.
#ifdef ART_USE_FUTEXES
// To enable lock contention logging, set this to true.
constexpr bool kLogLockContentions = true;
// FUTEX_WAKE first argument:
constexpr int kWakeOne = 1;
constexpr int kWakeAll = INT_MAX;
#else
// Keep this false as lock contention logging is supported only with
// futex.
constexpr bool kLogLockContentions = false;
#endif
constexpr size_t kContentionLogSize = 4;
constexpr size_t kContentionLogDataSize = kLogLockContentions ? 1 : 0;
constexpr size_t kAllMutexDataSize = kLogLockContentions ? 1 : 0;

// Base class for all Mutex implementations
class BaseMutex {
 public:
  const char* GetName() const {
    return name_;
  }

  virtual bool IsMutex() const { return false; }
  virtual bool IsReaderWriterMutex() const { return false; }
  virtual bool IsMutatorMutex() const { return false; }

  virtual void Dump(std::ostream& os) const = 0;

  static void DumpAll(std::ostream& os);

  bool ShouldRespondToEmptyCheckpointRequest() const {
    return should_respond_to_empty_checkpoint_request_;
  }

  void SetShouldRespondToEmptyCheckpointRequest(bool value) {
    should_respond_to_empty_checkpoint_request_ = value;
  }

  virtual void WakeupToRespondToEmptyCheckpoint() = 0;

 protected:
  friend class ConditionVariable;

  BaseMutex(const char* name, LockLevel level);
  virtual ~BaseMutex();
  void RegisterAsLocked();
  void RegisterAsUnlocked();
  void CheckSafeToWait();

  friend class ScopedContentionRecorder;

  void RecordContention(uint64_t blocked_tid, uint64_t owner_tid, uint64_t nano_time_blocked);
  void DumpContention(std::ostream& os) const;

  const char* const name_;

  // A log entry that records contention but makes no guarantee that either tid will be held live.
  struct ContentionLogEntry {
    ContentionLogEntry() : blocked_tid(0), owner_tid(0) {}
    uint64_t blocked_tid;
    uint64_t owner_tid;
    AtomicInteger count;
  };

  struct ContentionLogData {
    ContentionLogEntry contention_log[kContentionLogSize];
    // The next entry in the contention log to be updated. Value ranges from 0 to
    // kContentionLogSize - 1.
    AtomicInteger cur_content_log_entry;
    // Number of times the Mutex has been contended.
    AtomicInteger contention_count;
    // Sum of time waited by all contenders in ns.
    Atomic<uint64_t> wait_time;
    void AddToWaitTime(uint64_t value);
    ContentionLogData() : wait_time(0) {}
  };

  ContentionLogData contention_log_data_[kContentionLogDataSize];

  const LockLevel level_;  // Support for lock hierarchy.
  bool should_respond_to_empty_checkpoint_request_;

 public:
  bool HasEverContended() const {
    if (kLogLockContentions) {
      return contention_log_data_->contention_count.load(std::memory_order_seq_cst) > 0;
    }
    return false;
  }
};

// A Mutex is used to achieve mutual exclusion between threads. A Mutex can be used to gain
// exclusive access to what it guards. A Mutex can be in one of two states:
// - Free - not owned by any thread,
// - Exclusive - owned by a single thread.
//
// The effect of locking and unlocking operations on the state is:
// State     | ExclusiveLock | ExclusiveUnlock
// -------------------------------------------
// Free      | Exclusive     | error
// Exclusive | Block*        | Free
// * Mutex is not reentrant and so an attempt to ExclusiveLock on the same thread will result in
//   an error. Being non-reentrant simplifies Waiting on ConditionVariables.
std::ostream& operator<<(std::ostream& os, const Mutex& mu);
class Mutex : public BaseMutex {
 public:
  explicit Mutex(const char* name, LockLevel level = kDefaultMutexLevel, bool recursive = false);
  ~Mutex();

  bool IsMutex() const override { return true; }

  // Block until mutex is free then acquire exclusive access.
  void ExclusiveLock();
  void Lock() {  ExclusiveLock(); }

  // Returns true if acquires exclusive access, false otherwise.
  bool ExclusiveTryLock();
  bool TryLock() { return ExclusiveTryLock(); }

  // Release exclusive access.
  void ExclusiveUnlock();
  void Unlock() {  ExclusiveUnlock(); }

  // Is the current thread the exclusive holder of the Mutex.
  ALWAYS_INLINE bool IsExclusiveHeld() const;

  // Assert that the Mutex is exclusively held by the current thread.
  ALWAYS_INLINE void AssertExclusiveHeld() const;
  ALWAYS_INLINE void AssertHeld() const ;

  // Assert that the Mutex is not held by the current thread.
  void AssertNotHeldExclusive() {

  }
  void AssertNotHeld() {
    AssertNotHeldExclusive();
  }

  // Id associated with exclusive owner. No memory ordering semantics if called from a thread
  // other than the owner. GetTid() == GetExclusiveOwnerTid() is a reliable way to determine
  // whether we hold the lock; any other information may be invalidated before we return.
  pid_t GetExclusiveOwnerTid() const;

  // Returns how many times this Mutex has been locked, it is better to use AssertHeld/NotHeld.
  unsigned int GetDepth() const {
    return recursion_count_;
  }

  void Dump(std::ostream& os) const override;

  // For negative capabilities in clang annotations.
  const Mutex& operator!() const { return *this; }

  void WakeupToRespondToEmptyCheckpoint() override;

 private:
#if ART_USE_FUTEXES
  // Low order bit: 0 is unheld, 1 is held.
  // High order bits: Number of waiting contenders.
  AtomicInteger state_and_contenders_;

  static constexpr int32_t kHeldMask = 1;

  static constexpr int32_t kContenderShift = 1;

  static constexpr int32_t kContenderIncrement = 1 << kContenderShift;

  void increment_contenders() {
    state_and_contenders_.fetch_add(kContenderIncrement);
  }

  void decrement_contenders() {
    state_and_contenders_.fetch_sub(kContenderIncrement);
  }

  int32_t get_contenders() {
    // Result is guaranteed to include any contention added by this thread; otherwise approximate.
    // Treat contenders as unsigned because we're paranoid about overflow; should never matter.
    return static_cast<uint32_t>(state_and_contenders_.load(std::memory_order_relaxed))
        >> kContenderShift;
  }

  // Exclusive owner.
  Atomic<pid_t> exclusive_owner_;
#else
  pthread_mutex_t mutex_;
  Atomic<pid_t> exclusive_owner_;  // Guarded by mutex_. Asynchronous reads are OK.
#endif

  unsigned int recursion_count_;
  const bool recursive_;  // Can the lock be recursively held?

  friend class ConditionVariable;
  DISALLOW_COPY_AND_ASSIGN(Mutex);
};

// A ReaderWriterMutex is used to achieve mutual exclusion between threads, similar to a Mutex.
// Unlike a Mutex a ReaderWriterMutex can be used to gain exclusive (writer) or shared (reader)
// access to what it guards. A flaw in relation to a Mutex is that it cannot be used with a
// condition variable. A ReaderWriterMutex can be in one of three states:
// - Free - not owned by any thread,
// - Exclusive - owned by a single thread,
// - Shared(n) - shared amongst n threads.
//
// The effect of locking and unlocking operations on the state is:
//
// State     | ExclusiveLock | ExclusiveUnlock | SharedLock       | SharedUnlock
// ----------------------------------------------------------------------------
// Free      | Exclusive     | error           | SharedLock(1)    | error
// Exclusive | Block         | Free            | Block            | error
// Shared(n) | Block         | error           | SharedLock(n+1)* | Shared(n-1) or Free
// * for large values of n the SharedLock may block.
std::ostream& operator<<(std::ostream& os, const ReaderWriterMutex& mu);
class ReaderWriterMutex : public BaseMutex {
 public:
  explicit ReaderWriterMutex(const char* name, LockLevel level = kDefaultMutexLevel);
  ~ReaderWriterMutex();

  bool IsReaderWriterMutex() const override { return true; }

  // Block until ReaderWriterMutex is free then acquire exclusive access.
  void ExclusiveLock();
  void WriterLock() {  ExclusiveLock(); }

  // Release exclusive access.
  void ExclusiveUnlock();
  void WriterUnlock() {  ExclusiveUnlock(); }

  // Block until ReaderWriterMutex is free and acquire exclusive access. Returns true on success
  // or false if timeout is reached.
#if HAVE_TIMED_RWLOCK
  bool ExclusiveLockWithTimeout(int64_t ms, int32_t ns);
#endif

  // Block until ReaderWriterMutex is shared or free then acquire a share on the access.
  void SharedLock() ALWAYS_INLINE;
  void ReaderLock() { SharedLock(); }

  // Try to acquire share of ReaderWriterMutex.
  bool SharedTryLock();

  // Release a share of the access.
  void SharedUnlock() ALWAYS_INLINE;
  void ReaderUnlock() { SharedUnlock(); }

  // Is the current thread the exclusive holder of the ReaderWriterMutex.
  ALWAYS_INLINE bool IsExclusiveHeld() const;

  // Assert the current thread has exclusive access to the ReaderWriterMutex.
  ALWAYS_INLINE void AssertExclusiveHeld() const;
  ALWAYS_INLINE void AssertWriterHeld() const;

  // Assert the current thread doesn't have exclusive access to the ReaderWriterMutex.
  void AssertNotExclusiveHeld(){

  }
  void AssertNotWriterHeld() {
    AssertNotExclusiveHeld();
  }

  // Is the current thread a shared holder of the ReaderWriterMutex.
  bool IsSharedHeld() const;

  // Assert the current thread has shared access to the ReaderWriterMutex.
  ALWAYS_INLINE void AssertSharedHeld() {
  }
  ALWAYS_INLINE void AssertReaderHeld() {
    AssertSharedHeld();
  }

  // Assert the current thread doesn't hold this ReaderWriterMutex either in shared or exclusive
  // mode.
  ALWAYS_INLINE void AssertNotHeld() {
  }

  // Id associated with exclusive owner. No memory ordering semantics if called from a thread other
  // than the owner. Returns 0 if the lock is not held. Returns either 0 or -1 if it is held by
  // one or more readers.
  pid_t GetExclusiveOwnerTid() const;

  void Dump(std::ostream& os) const override;

  // For negative capabilities in clang annotations.
  const ReaderWriterMutex& operator!() const { return *this; }

  void WakeupToRespondToEmptyCheckpoint() override;

 private:
#if ART_USE_FUTEXES
  // Out-of-inline path for handling contention for a SharedLock.
  void HandleSharedLockContention(int32_t cur_state);

  // -1 implies held exclusive, +ve shared held by state_ many owners.
  AtomicInteger state_;
  // Exclusive owner. Modification guarded by this mutex.
  Atomic<pid_t> exclusive_owner_;
  // Number of contenders waiting for a reader share.
  AtomicInteger num_pending_readers_;
  // Number of contenders waiting to be the writer.
  AtomicInteger num_pending_writers_;
#else
  pthread_rwlock_t rwlock_;
  Atomic<pid_t> exclusive_owner_;  // Writes guarded by rwlock_. Asynchronous reads are OK.
#endif
  DISALLOW_COPY_AND_ASSIGN(ReaderWriterMutex);
};

// MutatorMutex is a special kind of ReaderWriterMutex created specifically for the
// Locks::mutator_lock_ mutex. The behaviour is identical to the ReaderWriterMutex except that
// thread state changes also play a part in lock ownership. The mutator_lock_ will not be truly
// held by any mutator threads. However, a thread in the kRunnable state is considered to have
// shared ownership of the mutator lock and therefore transitions in and out of the kRunnable
// state have associated implications on lock ownership. Extra methods to handle the state
// transitions have been added to the interface but are only accessible to the methods dealing
// with state transitions. The thread state and flags attributes are used to ensure thread state
// transitions are consistent with the permitted behaviour of the mutex.
//
// *) The most important consequence of this behaviour is that all threads must be in one of the
// suspended states before exclusive ownership of the mutator mutex is sought.
//
std::ostream& operator<<(std::ostream& os, const MutatorMutex& mu);
class MutatorMutex : public ReaderWriterMutex {
 public:
  explicit MutatorMutex(const char* name, LockLevel level = kDefaultMutexLevel)
    : ReaderWriterMutex(name, level) {}
  ~MutatorMutex() {}

  virtual bool IsMutatorMutex() const { return true; }

  // For negative capabilities in clang annotations.
  const MutatorMutex& operator!() const { return *this; }

 private:

  DISALLOW_COPY_AND_ASSIGN(MutatorMutex);
};

// ConditionVariables allow threads to queue and sleep. Threads may then be resumed individually
// (Signal) or all at once (Broadcast).
class ConditionVariable {
 public:
  ConditionVariable(const char* name, Mutex& mutex);
  ~ConditionVariable();

  // Requires the mutex to be held.
  void Broadcast();
  // Requires the mutex to be held.
  void Signal();
  // TODO: No thread safety analysis on Wait and TimedWait as they call mutex operations via their
  //       pointer copy, thereby defeating annotalysis.
  void Wait();
  bool TimedWait(int64_t ms, int32_t ns);
  // Variant of Wait that should be used with caution. Doesn't validate that no mutexes are held
  // when waiting.
  // TODO: remove this.
  void WaitHoldingLocks();

  void CheckSafeToWait() {
    if (kDebugLocking) {
      guard_.CheckSafeToWait();
    }
  }

 private:
  const char* const name_;
  // The Mutex being used by waiters. It is an error to mix condition variables between different
  // Mutexes.
  Mutex& guard_;
#if ART_USE_FUTEXES
  // A counter that is modified by signals and broadcasts. This ensures that when a waiter gives up
  // their Mutex and another thread takes it and signals, the waiting thread observes that sequence_
  // changed and doesn't enter the wait. Modified while holding guard_, but is read by futex wait
  // without guard_ held.
  AtomicInteger sequence_;
  // Number of threads that have come into to wait, not the length of the waiters on the futex as
  // waiters may have been requeued onto guard_. Guarded by guard_.
  int32_t num_waiters_;

  void RequeueWaiters(int32_t count);
#else
  pthread_cond_t cond_;
#endif
  DISALLOW_COPY_AND_ASSIGN(ConditionVariable);
};

// Scoped locker/unlocker for a regular Mutex that acquires mu upon construction and releases it
// upon destruction.
class MutexLock {
 public:
  MutexLock(Mutex& mu) :  mu_(mu) {
    mu_.ExclusiveLock();
  }

  ~MutexLock() {
    mu_.ExclusiveUnlock();
  }

 private:
  Mutex& mu_;
  DISALLOW_COPY_AND_ASSIGN(MutexLock);
};

// Scoped locker/unlocker for a ReaderWriterMutex that acquires read access to mu upon
// construction and releases it upon destruction.
class ReaderMutexLock {
 public:
  ALWAYS_INLINE ReaderMutexLock(ReaderWriterMutex& mu);

  ALWAYS_INLINE ~ReaderMutexLock();

 private:
  ReaderWriterMutex& mu_;
  DISALLOW_COPY_AND_ASSIGN(ReaderMutexLock);
};

// Scoped locker/unlocker for a ReaderWriterMutex that acquires write access to mu upon
// construction and releases it upon destruction.
class WriterMutexLock {
 public:
  WriterMutexLock(ReaderWriterMutex& mu) : mu_(mu) {
    mu_.ExclusiveLock();
  }

  ~WriterMutexLock() {
    mu_.ExclusiveUnlock();
  }

 private:
  ReaderWriterMutex& mu_;
  DISALLOW_COPY_AND_ASSIGN(WriterMutexLock);
};

}  // namespace art

#endif  // ART_RUNTIME_BASE_MUTEX_H_

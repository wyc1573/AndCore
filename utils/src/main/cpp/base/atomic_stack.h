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

#ifndef ART_RUNTIME_GC_ACCOUNTING_ATOMIC_STACK_H_
#define ART_RUNTIME_GC_ACCOUNTING_ATOMIC_STACK_H_

#include <sys/mman.h>  // For the PROT_* and MAP_* constants.

#include <algorithm>
#include <memory>
#include <string>

#include <android-base/logging.h>

#include "base/atomic.h"
#include "base/macros.h"
#include "base/mem_map.h"

// This implements a double-ended queue (deque) with various flavors of PushBack operations,
// as well as PopBack and PopFront operations. We expect that all calls are performed
// by a single thread (normally the GC). There is one exception, which accounts for the
// name:
// - Multiple calls to AtomicPushBack*() and AtomicBumpBack() may be made concurrently,
// provided no other calls are made at the same time.

namespace art {
namespace gc {
namespace accounting {

template <typename T>
class AtomicStack {
 public:

  // Capacity is how many elements we can store in the stack.
  static AtomicStack* Create(const std::string& name, size_t growth_limit, size_t capacity) {
    std::unique_ptr<AtomicStack> mark_stack(new AtomicStack(name, growth_limit, capacity));
    mark_stack->Init();
    return mark_stack.release();
  }

  ~AtomicStack() {}

  void Reset() {
    DCHECK(mem_map_.IsValid());
    DCHECK(begin_ != nullptr);
    front_index_.store(0, std::memory_order_relaxed);
    back_index_.store(0, std::memory_order_relaxed);
    debug_is_sorted_ = true;
    mem_map_.MadviseDontNeedAndZero();
  }

  // Beware: Mixing atomic pushes and atomic pops will cause ABA problem.

  // Returns false if we overflowed the stack.
  bool AtomicPushBackIgnoreGrowthLimit(T value) {
    return AtomicPushBackInternal(value, capacity_);
  }

  // Returns false if we overflowed the stack.
  bool AtomicPushBack(T value) {
    return AtomicPushBackInternal(value, growth_limit_);
  }

  // Atomically bump the back index by the given number of
  // slots. Returns false if we overflowed the stack.
  bool AtomicBumpBack(size_t num_slots, T** start_address,
                      T** end_address) {
    if (kIsDebugBuild) {
      debug_is_sorted_ = false;
    }
    int32_t index;
    int32_t new_index;
    do {
      index = back_index_.load(std::memory_order_relaxed);
      new_index = index + num_slots;
      if (UNLIKELY(static_cast<size_t>(new_index) >= growth_limit_)) {
        // Stack overflow.
        return false;
      }
    } while (!back_index_.CompareAndSetWeakRelaxed(index, new_index));

    *start_address = begin_ + index;
    *end_address = begin_ + new_index;
    return true;
  }

  void PushBack(T value) {
    if (kIsDebugBuild) {
      debug_is_sorted_ = false;
    }
    const int32_t index = back_index_.load(std::memory_order_relaxed);
    DCHECK_LT(static_cast<size_t>(index), growth_limit_);
    back_index_.store(index + 1, std::memory_order_relaxed);
    begin_[index]= value;
  }

  T PopBack() {
    DCHECK_GT(back_index_.load(std::memory_order_relaxed),
              front_index_.load(std::memory_order_relaxed));
    // Decrement the back index non atomically.
    back_index_.store(back_index_.load(std::memory_order_relaxed) - 1, std::memory_order_relaxed);
    return begin_[back_index_.load(std::memory_order_relaxed)];
  }

  // Take an item from the front of the stack.
  T PopFront() {
    int32_t index = front_index_.load(std::memory_order_relaxed);
    DCHECK_LT(index, back_index_.load(std::memory_order_relaxed));
    front_index_.store(index + 1, std::memory_order_relaxed);
    return begin_[index];
  }

  // Pop a number of elements.
  void PopBackCount(int32_t n) {
    DCHECK_GE(Size(), static_cast<size_t>(n));
    back_index_.store(back_index_.load(std::memory_order_relaxed) - n, std::memory_order_relaxed);
  }

  bool IsEmpty() const {
    return Size() == 0;
  }

  bool IsFull() const {
    return Size() == growth_limit_;
  }

  size_t Size() const {
    DCHECK_LE(front_index_.load(std::memory_order_relaxed),
              back_index_.load(std::memory_order_relaxed));
    return
        back_index_.load(std::memory_order_relaxed) - front_index_.load(std::memory_order_relaxed);
  }

  T* Begin() const {
    return begin_ + front_index_.load(std::memory_order_relaxed);
  }
  T* End() const {
    return begin_ + back_index_.load(std::memory_order_relaxed);
  }

  size_t Capacity() const {
    return capacity_;
  }

  // Will clear the stack.
  void Resize(size_t new_capacity) {
    capacity_ = new_capacity;
    growth_limit_ = new_capacity;
    Init();
  }

  bool Contains(const T value) const {
    for (auto cur = Begin(), end = End(); cur != end; ++cur) {
      if (*cur == value) {
        return true;
      }
    }
    return false;
  }

 private:
  AtomicStack(const std::string& name, size_t growth_limit, size_t capacity)
      : name_(name),
        back_index_(0),
        front_index_(0),
        begin_(nullptr),
        growth_limit_(growth_limit),
        capacity_(capacity),
        debug_is_sorted_(true) {
  }

  // Returns false if we overflowed the stack.
  bool AtomicPushBackInternal(T value, size_t limit) ALWAYS_INLINE {
    if (kIsDebugBuild) {
      debug_is_sorted_ = false;
    }
    int32_t index;
    do {
      index = back_index_.load(std::memory_order_relaxed);
      if (UNLIKELY(static_cast<size_t>(index) >= limit)) {
        // Stack overflow.
        return false;
      }
    } while (!back_index_.CompareAndSetWeakRelaxed(index, index + 1));
    begin_[index] = value;
    return true;
  }

  // Size in number of elements.
  void Init() {
    std::string error_msg;
    mem_map_ = MemMap::MapAnonymous(name_.c_str(),
                                    capacity_ * sizeof(begin_[0]),
                                    PROT_READ | PROT_WRITE,
                                    /*low_4gb=*/ false,
                                    &error_msg);
    CHECK(mem_map_.IsValid()) << "couldn't allocate stack.\n" << error_msg;
    uint8_t* addr = mem_map_.Begin();
    CHECK(addr != nullptr);
    debug_is_sorted_ = true;
    begin_ = reinterpret_cast<T*>(addr);
    Reset();
  }

  // Name of the mark stack.
  std::string name_;
  // Memory mapping of the atomic stack.
  MemMap mem_map_;
  // Back index (index after the last element pushed).
  AtomicInteger back_index_;
  // Front index, used for implementing PopFront.
  AtomicInteger front_index_;
  // Base of the atomic stack.
  T* begin_;
  // Current maximum which we can push back to, must be <= capacity_.
  size_t growth_limit_;
  // Maximum number of elements.
  size_t capacity_;
  // Whether or not the stack is sorted, only updated in debug mode to avoid performance overhead.
  bool debug_is_sorted_;

  DISALLOW_COPY_AND_ASSIGN(AtomicStack);
};

}  // namespace accounting
}  // namespace gc
}  // namespace art

#endif  // ART_RUNTIME_GC_ACCOUNTING_ATOMIC_STACK_H_

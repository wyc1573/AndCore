/*
 * Copyright (C) 2008 The Android Open Source Project
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

#include "space_bitmap-inl.h"
#include <sys/mman.h>

#include "android-base/stringprintf.h"
#include "base/mem_map.h"

namespace art {
namespace gc {
namespace accounting {

using android::base::StringPrintf;

template<size_t kAlignment>
size_t SpaceBitmap<kAlignment>::ComputeBitmapSize(uint64_t capacity) {
  // Number of space (heap) bytes covered by one bitmap word.
  // (Word size in bytes = `sizeof(intptr_t)`, which is expected to be
  // 4 on a 32-bit architecture and 8 on a 64-bit one.)
  const uint64_t kBytesCoveredPerWord = kAlignment * kBitsPerIntPtrT;
  // Calculate the number of words required to cover a space (heap)
  // having a size of `capacity` bytes.
  return (RoundUp(capacity, kBytesCoveredPerWord) / kBytesCoveredPerWord) * sizeof(intptr_t);
}

template<size_t kAlignment>
size_t SpaceBitmap<kAlignment>::ComputeHeapSize(uint64_t bitmap_bytes) {
  return bitmap_bytes * kBitsPerByte * kAlignment;
}

template<size_t kAlignment>
SpaceBitmap<kAlignment>* SpaceBitmap<kAlignment>::CreateFromMemMap(
    const std::string& name, MemMap&& mem_map, uint8_t* heap_begin, size_t heap_capacity) {
  CHECK(mem_map.IsValid());
  uintptr_t* bitmap_begin = reinterpret_cast<uintptr_t*>(mem_map.Begin());
  const size_t bitmap_size = ComputeBitmapSize(heap_capacity);
  return new SpaceBitmap(
      name, std::move(mem_map), bitmap_begin, bitmap_size, heap_begin, heap_capacity);
}

template<size_t kAlignment>
SpaceBitmap<kAlignment>::SpaceBitmap(const std::string& name,
                                     MemMap&& mem_map,
                                     uintptr_t* bitmap_begin,
                                     size_t bitmap_size,
                                     const void* heap_begin,
                                     size_t heap_capacity)
    : mem_map_(std::move(mem_map)),
      bitmap_begin_(reinterpret_cast<Atomic<uintptr_t>*>(bitmap_begin)),
      bitmap_size_(bitmap_size),
      heap_begin_(reinterpret_cast<uintptr_t>(heap_begin)),
      heap_limit_(reinterpret_cast<uintptr_t>(heap_begin) + heap_capacity),
      name_(name) {
  CHECK(bitmap_begin_ != nullptr);
  CHECK_NE(bitmap_size, 0U);
}

template<size_t kAlignment>
SpaceBitmap<kAlignment>::~SpaceBitmap() {}

template<size_t kAlignment>
SpaceBitmap<kAlignment>* SpaceBitmap<kAlignment>::Create(
    const std::string& name, uint8_t* heap_begin, size_t heap_capacity) {

  // Round up since `heap_capacity` is not necessarily a multiple of `kAlignment * kBitsPerIntPtrT`
  // (we represent one word as an `intptr_t`).
  const size_t bitmap_size = ComputeBitmapSize(heap_capacity);
  std::string error_msg;
  MemMap mem_map = MemMap::MapAnonymous(name.c_str(),
                                        bitmap_size,
                                        PROT_READ | PROT_WRITE,
                                        /*low_4gb=*/ false,
                                        &error_msg);
  if (UNLIKELY(!mem_map.IsValid())) {
    LOG(ERROR) << "Failed to allocate bitmap " << name << ": " << error_msg;
    return nullptr;
  }
  return CreateFromMemMap(name, std::move(mem_map), heap_begin, heap_capacity);
}

template<size_t kAlignment>
void SpaceBitmap<kAlignment>::SetHeapLimit(uintptr_t new_end) {
  DCHECK_ALIGNED(new_end, kBitsPerIntPtrT * kAlignment);
  size_t new_size = OffsetToIndex(new_end - heap_begin_) * sizeof(intptr_t);
  if (new_size < bitmap_size_) {
    bitmap_size_ = new_size;
  }
  heap_limit_ = new_end;
  // Not sure if doing this trim is necessary, since nothing past the end of the heap capacity
  // should be marked.
}

template<size_t kAlignment>
std::string SpaceBitmap<kAlignment>::Dump() const {
  return StringPrintf("%s: %p-%p", name_.c_str(), reinterpret_cast<void*>(HeapBegin()),
                      reinterpret_cast<void*>(HeapLimit()));
}

template<size_t kAlignment>
void SpaceBitmap<kAlignment>::Clear() {
  if (bitmap_begin_ != nullptr) {
    mem_map_.MadviseDontNeedAndZero();
  }
}

template<size_t kAlignment>
void SpaceBitmap<kAlignment>::ClearRange(const void* begin, const void* end) {
  uintptr_t begin_offset = reinterpret_cast<uintptr_t>(begin) - heap_begin_;
  uintptr_t end_offset = reinterpret_cast<uintptr_t>(end) - heap_begin_;
  // Align begin and end to bitmap word boundaries.
  while (begin_offset < end_offset && OffsetBitIndex(begin_offset) != 0) {
    Clear(reinterpret_cast<void*>(heap_begin_ + begin_offset));
    begin_offset += kAlignment;
  }
  while (begin_offset < end_offset && OffsetBitIndex(end_offset) != 0) {
    end_offset -= kAlignment;
    Clear(reinterpret_cast<void*>(heap_begin_ + end_offset));
  }
  // Bitmap word boundaries.
  const uintptr_t start_index = OffsetToIndex(begin_offset);
  const uintptr_t end_index = OffsetToIndex(end_offset);
  ZeroAndReleasePages(reinterpret_cast<uint8_t*>(&bitmap_begin_[start_index]),
                      (end_index - start_index) * sizeof(*bitmap_begin_));
}

template<size_t kAlignment>
void SpaceBitmap<kAlignment>::CopyFrom(SpaceBitmap* source_bitmap) {
  DCHECK_EQ(Size(), source_bitmap->Size());
  const size_t count = source_bitmap->Size() / sizeof(intptr_t);
  Atomic<uintptr_t>* const src = source_bitmap->Begin();
  Atomic<uintptr_t>* const dest = Begin();
  for (size_t i = 0; i < count; ++i) {
    dest[i].store(src[i].load(std::memory_order_relaxed), std::memory_order_relaxed);
  }
}

template class SpaceBitmap<kObjectAlignment>;
template class SpaceBitmap<kPageSize>;

}  // namespace accounting
}  // namespace gc
}  // namespace art

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

#ifndef ART_LIBARTBASE_ARCH_INSTRUCTION_SET_H_
#define ART_LIBARTBASE_ARCH_INSTRUCTION_SET_H_

#include <iosfwd>
#include <string>

#include "base/enums.h"
#include "base/macros.h"

namespace art {

enum class InstructionSet {
  kNone,
  kArm,
  kArm64,
  kThumb2,
  kX86,
  kX86_64,
  kMips,
  kMips64,
  kLast = kMips64
};
std::ostream& operator<<(std::ostream& os, const InstructionSet& rhs);

#if defined(__arm__)
static constexpr InstructionSet kRuntimeISA = InstructionSet::kArm;
#elif defined(__aarch64__)
static constexpr InstructionSet kRuntimeISA = InstructionSet::kArm64;
#elif defined(__mips__) && !defined(__LP64__)
static constexpr InstructionSet kRuntimeISA = InstructionSet::kMips;
#elif defined(__mips__) && defined(__LP64__)
static constexpr InstructionSet kRuntimeISA = InstructionSet::kMips64;
#elif defined(__i386__)
static constexpr InstructionSet kRuntimeISA = InstructionSet::kX86;
#elif defined(__x86_64__)
static constexpr InstructionSet kRuntimeISA = InstructionSet::kX86_64;
#else
static constexpr InstructionSet kRuntimeISA = InstructionSet::kNone;
#endif

// Architecture-specific pointer sizes
static constexpr PointerSize kArmPointerSize = PointerSize::k32;
static constexpr PointerSize kArm64PointerSize = PointerSize::k64;
static constexpr PointerSize kMipsPointerSize = PointerSize::k32;
static constexpr PointerSize kMips64PointerSize = PointerSize::k64;
static constexpr PointerSize kX86PointerSize = PointerSize::k32;
static constexpr PointerSize kX86_64PointerSize = PointerSize::k64;

// ARM instruction alignment. ARM processors require code to be 4-byte aligned,
// but ARM ELF requires 8..
static constexpr size_t kArmAlignment = 8;

// ARM64 instruction alignment. This is the recommended alignment for maximum performance.
static constexpr size_t kArm64Alignment = 16;

// MIPS instruction alignment.  MIPS processors require code to be 4-byte aligned,
// but 64-bit literals must be 8-byte aligned.
static constexpr size_t kMipsAlignment = 8;

// X86 instruction alignment. This is the recommended alignment for maximum performance.
static constexpr size_t kX86Alignment = 16;

// Different than code alignment since code alignment is only first instruction of method.
static constexpr size_t kThumb2InstructionAlignment = 2;
static constexpr size_t kArm64InstructionAlignment = 4;
static constexpr size_t kX86InstructionAlignment = 1;
static constexpr size_t kX86_64InstructionAlignment = 1;
static constexpr size_t kMipsInstructionAlignment = 4;
static constexpr size_t kMips64InstructionAlignment = 4;

const char* GetInstructionSetString(InstructionSet isa);

// Note: Returns kNone when the string cannot be parsed to a known value.
InstructionSet GetInstructionSetFromString(const char* instruction_set);

// Fatal logging out of line to keep the header clean of logging.h.
NO_RETURN void InstructionSetAbort(InstructionSet isa);

constexpr PointerSize GetInstructionSetPointerSize(InstructionSet isa) {
  switch (isa) {
    case InstructionSet::kArm:
      // Fall-through.
    case InstructionSet::kThumb2:
      return kArmPointerSize;
    case InstructionSet::kArm64:
      return kArm64PointerSize;
    case InstructionSet::kX86:
      return kX86PointerSize;
    case InstructionSet::kX86_64:
      return kX86_64PointerSize;
    case InstructionSet::kMips:
      return kMipsPointerSize;
    case InstructionSet::kMips64:
      return kMips64PointerSize;

    case InstructionSet::kNone:
      break;
  }
  InstructionSetAbort(isa);
}

constexpr size_t GetInstructionSetInstructionAlignment(InstructionSet isa) {
  switch (isa) {
    case InstructionSet::kArm:
      // Fall-through.
    case InstructionSet::kThumb2:
      return kThumb2InstructionAlignment;
    case InstructionSet::kArm64:
      return kArm64InstructionAlignment;
    case InstructionSet::kX86:
      return kX86InstructionAlignment;
    case InstructionSet::kX86_64:
      return kX86_64InstructionAlignment;
    case InstructionSet::kMips:
      return kMipsInstructionAlignment;
    case InstructionSet::kMips64:
      return kMips64InstructionAlignment;

    case InstructionSet::kNone:
      break;
  }
  InstructionSetAbort(isa);
}

constexpr bool IsValidInstructionSet(InstructionSet isa) {
  switch (isa) {
    case InstructionSet::kArm:
    case InstructionSet::kThumb2:
    case InstructionSet::kArm64:
    case InstructionSet::kX86:
    case InstructionSet::kX86_64:
    case InstructionSet::kMips:
    case InstructionSet::kMips64:
      return true;

    case InstructionSet::kNone:
      return false;
  }
  return false;
}

size_t GetInstructionSetAlignment(InstructionSet isa);

constexpr bool Is64BitInstructionSet(InstructionSet isa) {
  switch (isa) {
    case InstructionSet::kArm:
    case InstructionSet::kThumb2:
    case InstructionSet::kX86:
    case InstructionSet::kMips:
      return false;

    case InstructionSet::kArm64:
    case InstructionSet::kX86_64:
    case InstructionSet::kMips64:
      return true;

    case InstructionSet::kNone:
      break;
  }
  InstructionSetAbort(isa);
}

constexpr PointerSize InstructionSetPointerSize(InstructionSet isa) {
  return Is64BitInstructionSet(isa) ? PointerSize::k64 : PointerSize::k32;
}

constexpr size_t GetBytesPerGprSpillLocation(InstructionSet isa) {
  switch (isa) {
    case InstructionSet::kArm:
      // Fall-through.
    case InstructionSet::kThumb2:
      return 4;
    case InstructionSet::kArm64:
      return 8;
    case InstructionSet::kX86:
      return 4;
    case InstructionSet::kX86_64:
      return 8;
    case InstructionSet::kMips:
      return 4;
    case InstructionSet::kMips64:
      return 8;

    case InstructionSet::kNone:
      break;
  }
  InstructionSetAbort(isa);
}

constexpr size_t GetBytesPerFprSpillLocation(InstructionSet isa) {
  switch (isa) {
    case InstructionSet::kArm:
      // Fall-through.
    case InstructionSet::kThumb2:
      return 4;
    case InstructionSet::kArm64:
      return 8;
    case InstructionSet::kX86:
      return 8;
    case InstructionSet::kX86_64:
      return 8;
    case InstructionSet::kMips:
      return 4;
    case InstructionSet::kMips64:
      return 8;

    case InstructionSet::kNone:
      break;
  }
  InstructionSetAbort(isa);
}

}  // namespace art

#endif  // ART_LIBARTBASE_ARCH_INSTRUCTION_SET_H_

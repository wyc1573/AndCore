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

#ifndef ART_LIBARTBASE_BASE_GLOBALS_H_
#define ART_LIBARTBASE_BASE_GLOBALS_H_

#include <stddef.h>
#include <stdint.h>

namespace art {

static constexpr size_t KB = 1024;
static constexpr size_t MB = KB * KB;
static constexpr size_t GB = KB * KB * KB;

// System page size. We check this against sysconf(_SC_PAGE_SIZE) at runtime, but use a simple
// compile-time constant so the compiler can generate better code.
static constexpr int kPageSize = 4096;

// Runtime sizes.
static constexpr size_t kBitsPerByte = 8;
static constexpr size_t kBitsPerByteLog2 = 3;
static constexpr int kBitsPerIntPtrT = sizeof(intptr_t) * kBitsPerByte;

static constexpr size_t kObjectAlignmentShift = 3;
static constexpr size_t kObjectAlignment = 1u << kObjectAlignmentShift;
static constexpr size_t kLargeObjectAlignment = kPageSize;

// Required stack alignment
static constexpr size_t kStackAlignment = 16;

#if defined(ART_TARGET)
    // Useful in conditionals where ART_TARGET isn't.
static constexpr bool kIsTargetBuild = true;
# if defined(ART_TARGET_LINUX)
static constexpr bool kIsTargetLinux = true;
static constexpr bool kIsTargetFuchsia = false;
# elif defined(ART_TARGET_ANDROID)
static constexpr bool kIsTargetLinux = false;
static constexpr bool kIsTargetFuchsia = false;
# elif defined(ART_TARGET_FUCHSIA)
static constexpr bool kIsTargetLinux = false;
static constexpr bool kIsTargetFuchsia = true;
# else
# error "Either ART_TARGET_LINUX, ART_TARGET_ANDROID or ART_TARGET_FUCHSIA " \
        "needs to be defined for target builds."
# endif
#else
    static constexpr bool kIsTargetBuild = false;
# if defined(ART_TARGET_LINUX)
# error "ART_TARGET_LINUX defined for host build."
# elif defined(ART_TARGET_ANDROID)
# error "ART_TARGET_ANDROID defined for host build."
# elif defined(ART_TARGET_FUCHSIA)
# error "ART_TARGET_FUCHSIA defined for host build."
# else
    static constexpr bool kIsTargetLinux = false;
    static constexpr bool kIsTargetFuchsia = false;
# endif
#endif

//实际使用时采用随机值
#define ART_BASE_ADDRESS 0xff
}  // namespace art

#endif  // ART_LIBARTBASE_BASE_GLOBALS_H_

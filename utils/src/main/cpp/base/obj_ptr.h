/*
 * Copyright (C) 2016 The Android Open Source Project
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

#ifndef ART_RUNTIME_OBJ_PTR_H_
#define ART_RUNTIME_OBJ_PTR_H_

#include <iosfwd>
#include <type_traits>

#include "base/macros.h"

// Always inline ObjPtr methods even in debug builds.
#define OBJPTR_INLINE __attribute__ ((always_inline))

namespace art {

static constexpr size_t kHeapReferenceSize = sizeof(uint32_t);
constexpr bool kObjPtrPoisoning = kIsDebugBuild;

// It turns out that most of the performance overhead comes from copying. Don't validate for now.
// This defers finding stale ObjPtr objects until they are used.
constexpr bool kObjPtrPoisoningValidateOnCopy = false;

// Value type representing a pointer to a mirror::Object of type MirrorType
// Since the cookie is thread based, it is not safe to share an ObjPtr between threads.
template<class MirrorType>
class ObjPtr {

 public:
  OBJPTR_INLINE ObjPtr() : reference_(0u) {}

  // Note: The following constructors allow implicit conversion. This simplifies code that uses
  //       them, e.g., for parameter passing. However, in general, implicit-conversion constructors
  //       are discouraged and detected by clang-tidy.

  OBJPTR_INLINE ObjPtr(std::nullptr_t)
      : reference_(0u) {}

  template <typename Type,
              typename = typename std::enable_if_t<std::is_base_of_v<MirrorType, Type>>>
  OBJPTR_INLINE ObjPtr(Type* ptr);

  template <typename Type,
            typename = typename std::enable_if_t<std::is_base_of_v<MirrorType, Type>>>
  OBJPTR_INLINE ObjPtr(const ObjPtr<Type>& other);

  template <typename Type,
            typename = typename std::enable_if_t<std::is_base_of_v<MirrorType, Type>>>
  OBJPTR_INLINE ObjPtr& operator=(const ObjPtr<Type>& other);

  OBJPTR_INLINE ObjPtr& operator=(MirrorType* ptr);

  OBJPTR_INLINE void Assign(MirrorType* ptr);

  OBJPTR_INLINE MirrorType* operator->() const;

  OBJPTR_INLINE bool IsNull() const {
    return reference_ == 0;
  }

  // Ptr makes sure that the object pointer is valid.
  OBJPTR_INLINE MirrorType* Ptr() const;

  // Ptr unchecked does not check that object pointer is valid. Do not use if you can avoid it.
  OBJPTR_INLINE MirrorType* PtrUnchecked() const {
    return reinterpret_cast<MirrorType*>(reference_);
  }

  // Static function to be friendly with null pointers.
  template <typename SourceType>
  static ObjPtr<MirrorType> DownCast(ObjPtr<SourceType> ptr);

  // Static function to be friendly with null pointers.
  template <typename SourceType>
  static ObjPtr<MirrorType> DownCast(SourceType* ptr);

 private:
 OBJPTR_INLINE static uintptr_t Encode(MirrorType* ptr);
  // The encoded reference and cookie.
  uintptr_t reference_;

  template <class T> friend class ObjPtr;  // Required for reference_ access in copy cons/operator.
};

static_assert(std::is_trivially_copyable<ObjPtr<void>>::value,
              "ObjPtr should be trivially copyable");

template<class MirrorType1, class MirrorType2>
OBJPTR_INLINE std::enable_if_t<std::is_base_of_v<MirrorType1, MirrorType2> ||
                               std::is_base_of_v<MirrorType2, MirrorType1>, bool>
operator==(ObjPtr<MirrorType1> lhs, ObjPtr<MirrorType2> rhs);

template<class MirrorType1, class MirrorType2>
OBJPTR_INLINE std::enable_if_t<std::is_base_of_v<MirrorType1, MirrorType2> ||
                               std::is_base_of_v<MirrorType2, MirrorType1>, bool>
operator==(const MirrorType1* lhs, ObjPtr<MirrorType2> rhs);

template<class MirrorType1, class MirrorType2>
OBJPTR_INLINE std::enable_if_t<std::is_base_of_v<MirrorType1, MirrorType2> ||
                               std::is_base_of_v<MirrorType2, MirrorType1>, bool>
operator==(ObjPtr<MirrorType1> lhs, const MirrorType2* rhs);

template<class MirrorType>
OBJPTR_INLINE bool operator==(ObjPtr<MirrorType> ptr, std::nullptr_t) {
  return ptr.IsNull();
}

template<class MirrorType>
OBJPTR_INLINE bool operator==(std::nullptr_t, ObjPtr<MirrorType> ptr) {
  return ptr.IsNull();
}

template<class MirrorType1, class MirrorType2>
OBJPTR_INLINE std::enable_if_t<std::is_base_of_v<MirrorType1, MirrorType2> ||
                               std::is_base_of_v<MirrorType2, MirrorType1>, bool>
operator!=(ObjPtr<MirrorType1> lhs, ObjPtr<MirrorType2> rhs);

template<class MirrorType1, class MirrorType2>
OBJPTR_INLINE std::enable_if_t<std::is_base_of_v<MirrorType1, MirrorType2> ||
                               std::is_base_of_v<MirrorType2, MirrorType1>, bool>
operator!=(const MirrorType1* lhs, ObjPtr<MirrorType2> rhs);

template<class MirrorType1, class MirrorType2>
OBJPTR_INLINE std::enable_if_t<std::is_base_of_v<MirrorType1, MirrorType2> ||
                               std::is_base_of_v<MirrorType2, MirrorType1>, bool>
operator!=(ObjPtr<MirrorType1> lhs, const MirrorType2* rhs);

template<class MirrorType>
OBJPTR_INLINE bool operator!=(ObjPtr<MirrorType> ptr, std::nullptr_t) {
  return !(ptr == nullptr);
}

template<class MirrorType>
OBJPTR_INLINE bool operator!=(std::nullptr_t, ObjPtr<MirrorType> ptr) {
  return !(nullptr == ptr);
}

template<class MirrorType>
OBJPTR_INLINE std::ostream& operator<<(std::ostream& os, ObjPtr<MirrorType> ptr);

}  // namespace art

#endif  // ART_RUNTIME_OBJ_PTR_H_

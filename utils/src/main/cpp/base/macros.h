#ifndef PAPM_MACRO_H
#define PAPM_MACRO_H

#include <stddef.h>

#define DISALLOW_COPY(TypeName) \
TypeName(const TypeName&) = delete

#define DISALLOW_ASSIGN(TypeName) \
TypeName& operator=(const TypeName&) = delete

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    DISALLOW_COPY(TypeName);               \
    DISALLOW_ASSIGN(TypeName)

#define DISALLOW_IMPLICIT_CONSTRUCTORS(TypeName) \
    TypeName() = delete;                         \
    DISALLOW_COPY_AND_ASSIGN(TypeName)

#endif


// A macro to disallow new and delete operators for a class. It goes in the private: declarations.
// NOTE: Providing placement new (and matching delete) for constructing container elements.
#define DISALLOW_ALLOCATION() \
public: \
  NO_RETURN ALWAYS_INLINE void operator delete(void*, size_t) { UNREACHABLE(); } \
  ALWAYS_INLINE void* operator new(size_t, void* ptr) noexcept { return ptr; } \
  ALWAYS_INLINE void operator delete(void*, void*) noexcept { } \
private: \
  void* operator new(size_t) = delete


// offsetof is not defined by the spec on types with non-standard layout,
// however it is implemented by compilers in practice.
// (note that reinterpret_cast is not valid constexpr)
//
// Alternative approach would be something like:
// #define OFFSETOF_HELPER(t, f) \
//   (reinterpret_cast<uintptr_t>(&reinterpret_cast<t*>(16)->f) - static_cast<uintptr_t>(16u))
// #define OFFSETOF_MEMBER(t, f) \
//   (__builtin_constant_p(OFFSETOF_HELPER(t,f)) ? OFFSETOF_HELPER(t,f) : OFFSETOF_HELPER(t,f))
#define OFFSETOF_MEMBER(t, f) offsetof(t, f)

#define UNREACHABLE  __builtin_unreachable

#ifndef NDEBUG
#define ALWAYS_INLINE
#else
#define ALWAYS_INLINE  __attribute__ ((always_inline))
#endif

#define NO_RETURN [[ noreturn ]]
#define WARN_UNUSED __attribute__((warn_unused_result))
// An attribute to place on a parameter to a function, for example:
//   int foo(int x ATTRIBUTE_UNUSED) { return 10; }
// to avoid compiler warnings.
#define ATTRIBUTE_UNUSED __attribute__((__unused__))

#define PACKED(x) __attribute__ ((__aligned__(x), __packed__))

#define LIKELY( exp )       (__builtin_expect( (exp) != 0, true  ))
#define UNLIKELY( exp )     (__builtin_expect( (exp) != 0, false ))

// bionic and glibc both have TEMP_FAILURE_RETRY, but eg Mac OS' libc doesn't.
#ifndef TEMP_FAILURE_RETRY
#define TEMP_FAILURE_RETRY(exp)            \
({                                       \
  decltype(exp) _rc;                     \
  do {                                   \
    _rc = (exp);                         \
  } while (_rc == -1 && errno == EINTR); \
  _rc;                                   \
})
#endif

// The arraysize(arr) macro returns the # of elements in an array arr.
// The expression is a compile-time constant, and therefore can be
// used in defining new arrays, for example.  If you use arraysize on
// a pointer by mistake, you will get a compile-time error.
//
// One caveat is that arraysize() doesn't accept any array of an
// anonymous type or a type defined inside a function.  In these rare
// cases, you have to use the unsafe ARRAYSIZE_UNSAFE() macro below.  This is
// due to a limitation in C++'s template system.  The limitation might
// eventually be removed, but it hasn't happened yet.

// This template function declaration is used in defining arraysize.
// Note that the function doesn't need an implementation, as we only
// use its type.
template <typename T, size_t N>
char(&ArraySizeHelper(T(&array)[N]))[N];  // NOLINT(readability/casting)

#define arraysize(array) (sizeof(ArraySizeHelper(array)))

#define kIsDebugBuild true
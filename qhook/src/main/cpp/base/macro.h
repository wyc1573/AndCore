//
// Created by wyc on 2022/8/6.
//

#pragma once

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

#define ALWAYS_INLINE inline
#define LIKELY( exp )       (__builtin_expect( (exp) != 0, true  ))
#define UNLIKELY( exp )     (__builtin_expect( (exp) != 0, false ))



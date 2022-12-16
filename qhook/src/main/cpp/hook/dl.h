//
// Created by wyc on 2022/12/8.
//

#pragma once
#include <jni.h>
#include "base/dl_sym.h"

bool init_DL();

#define FUNC_(SYM) SYM##Func
#define FUNC(SYM) FUNC_(SYM)
#define FUNC_DL(SYM) FUNC(DL_SYM(SYM))

#define FUNC_TYPE_(SYM) SYM##FuncType
#define FUNC_TYPE(SYM) FUNC_TYPE_(SYM)
#define FUNC_TYPE_DL(SYM) FUNC_TYPE(DL_SUFFIX(SYM))

// Func Pointer
#define D(TYPE, SYM, VAL)
#define L(TYPE, LIB, SYM, VAL, RET, PARAM) extern void* FUNC_DL(SYM);
#include "dl_sym"
#undef D
#undef L

// FuncType
#define D(TYPE, SYM, VAL)
#define L(TYPE, LIB, SYM, VAL, RET, PARAM) typedef RET (*FUNC_TYPE_DL(SYM)) PARAM;
#include "dl_sym"
#undef D
#undef L
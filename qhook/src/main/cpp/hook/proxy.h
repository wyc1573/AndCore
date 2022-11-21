//
// Created by wyc on 2022/8/6.
//

#pragma once

#include <jni.h>
#include "base/sym.h"

void init(JNIEnv*);

#define PROXY_FUNC(SYM) SYM##Proxy
#define ORIGIN_FUNC(SYM) origin##SYM

// 声明origin函数和proxy方法
#define W(TYPE, SYM, VAL)
#define V(TYPE, LIB, SYM, VAL, RET, PARAM) extern void* ORIGIN_FUNC(SYM); \
RET PROXY_FUNC(SYM) PARAM;
#include "symbols"


#undef W
#undef V

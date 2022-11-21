//
// Created by wyc on 2022/8/6.
//

#pragma once

//eg. constexpr const char* const kLibArt = "libart.so";
#define W(TYPE, SYM, VAL) constexpr const char* const SYM = #VAL;
#define V(TYPE, LIB, SYM, VAL, RET, PARAM)
#include "symbols"

//eg. constexpr const char* const kAddressOf = "_ZN3artL19VMRuntime_addressOfEP7_JNIEnvP8_jobjectS3_";
#define W(TYPE, SYM, VAL)
#define V(TYPE, LIB, SYM, VAL, RET, PARAM) constexpr const char* const SYM = #VAL;
#include "symbols"

#define REAL_NAME(kSym) [](){ return kSym; }()

#undef W
#undef V
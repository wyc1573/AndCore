#pragma once

#define DL_SUFFIX(SYM) SYM##_DL
#define DL_SYM(SYM) DL_SUFFIX(SYM)
//eg. constexpr const char* const kLibArt_DL = "libart.so";
#define D(TYPE, SYM, VAL) constexpr const char* const DL_SYM(SYM) = #VAL;
#define L(TYPE, LIB, SYM, VAL, RET, PARAM)
#include "dl_sym"
#undef D
#undef L

//eg. constexpr const char* const kAddressOf_DL = "_ZN3artL19VMRuntime_addressOfEP7_JNIEnvP8_jobjectS3_";
#define D(TYPE, SYM, VAL)
#define L(TYPE, LIB, SYM, VAL, RET, PARAM) constexpr const char* const DL_SYM(SYM) = #VAL;
#include "dl_sym"
#undef D
#undef L
//
// Created by wyc on 2022/8/6.
//

#pragma once

#include <jni.h>
#include <cstdlib>
#include <android/log.h>
#include <android/set_abort_message.h>
#include "macro.h"

#define LOG_TAG "AndCore"
#ifdef NDEBUG
#define ALOGD(...)
#else
#define ALOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#endif

#define ALOG_ALWAYS(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

ALWAYS_INLINE jclass make_globalref(JNIEnv* env, const char classname[])
{
    jclass c = env->FindClass(classname);
    return (jclass) env->NewGlobalRef(c);
}

ALWAYS_INLINE bool is_little_endian() {
    int local[1] = {0x12345678};
    int8_t* res = reinterpret_cast<int8_t*>(local);
    int8_t lowest = *res;
    if (lowest == 0x78) {
        return true;
    }
    return false;
}

ALWAYS_INLINE void Abort(const char* msg) {
#if __ANDROID_API__ >= 21
    android_set_abort_message(msg);
#endif
    abort();
}
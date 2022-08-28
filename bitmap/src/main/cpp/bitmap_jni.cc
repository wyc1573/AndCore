//
// Created by wyc on 2022/8/6.
//

#include "bitmap_jni.h"
#include "base/utils.h"
#include "hook/hook.h"

extern "C" JNIEXPORT void JNICALL Java_com_wyc_bitmap_JNI_start(JNIEnv *env, jclass clazz) {
    start_bitmap_opt(env);
}

extern "C" JNIEXPORT void JNICALL Java_com_wyc_bitmap_JNI_stop(JNIEnv *env, jclass clazz) {
    stop_bitmap_opt();
}
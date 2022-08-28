//
// Created by wyc on 2022/8/6.
//

#pragma once
#include <jni.h>

extern "C" JNIEXPORT void JNICALL Java_com_wyc_bitmap_JNI_start(JNIEnv *env, jclass clazz);
extern "C" JNIEXPORT void JNICALL Java_com_wyc_bitmap_JNI_stop(JNIEnv *env, jclass clazz);
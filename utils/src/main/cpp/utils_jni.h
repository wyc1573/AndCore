//
// Created by wyc on 2022/8/6.
//

#pragma once
#include <jni.h>

extern "C"
JNIEXPORT void JNICALL
Java_com_wyc_utils_JNI_test(JNIEnv *env, jclass clazz, jint mode);

extern "C"
JNIEXPORT void JNICALL
Java_com_example_utils_JNI_setPath(JNIEnv *env, jclass clazz, jstring);

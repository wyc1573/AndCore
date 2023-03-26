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
Java_com_wyc_utils_JNI_setPath(JNIEnv *env, jclass clazz, jstring);

extern "C"
JNIEXPORT void JNICALL
Java_com_wyc_utils_JNI_socketNormal(JNIEnv *env, jclass clazz);

extern "C"
JNIEXPORT void JNICALL
Java_com_wyc_utils_JNI_socketDiagram(JNIEnv *env, jclass clazz);

extern "C"
JNIEXPORT void JNICALL
Java_com_wyc_utils_JNI_socketMsg(JNIEnv *env, jclass clazz);

extern "C"
JNIEXPORT void JNICALL
Java_com_wyc_utils_JNI_socketPair(JNIEnv *env, jclass clazz);

extern "C"
JNIEXPORT void JNICALL
Java_com_wyc_utils_JNI_socketPipe(JNIEnv *env, jclass clazz);

extern "C"
JNIEXPORT void JNICALL
Java_com_wyc_utils_JNI_eventFd(JNIEnv *env, jclass clazz);


extern "C"
JNIEXPORT void JNICALL
Java_com_wyc_utils_JNI_epoll(JNIEnv *env, jclass clazz);

extern "C"
JNIEXPORT void JNICALL
Java_com_wyc_utils_JNI_cppObjModel(JNIEnv *env, jclass clazz);

extern "C"
JNIEXPORT void JNICALL
Java_com_wyc_utils_JNI_cppConcurrency(JNIEnv *env, jclass clazz);


extern "C"
JNIEXPORT void JNICALL
Java_com_wyc_utils_JNI_cppMisc(JNIEnv *env, jclass clazz);

extern "C"
JNIEXPORT void JNICALL
Java_com_wyc_utils_JNI_statistics(JNIEnv *env, jclass clazz);
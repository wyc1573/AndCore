//
// Created by wyc on 2022/11/21.
//

#ifndef ANDCORE_QJNI_H
#define ANDCORE_QJNI_H

#include <jni.h>

extern "C"
JNIEXPORT void JNICALL
Java_com_wyc_qhook_JNI_nativeStartHook(JNIEnv *env, jclass clazz, jint mode);

extern "C"
JNIEXPORT void JNICALL
Java_com_wyc_qhook_JNI_nativeStopHook(JNIEnv *env, jclass clazz, jint mode);

extern "C"
JNIEXPORT void JNICALL
Java_com_wyc_qhook_JNI_nativeDlInit(JNIEnv *env, jclass clazz, jint mode);

extern "C"
JNIEXPORT void JNICALL
Java_com_wyc_qhook_JNI_nativeAction(JNIEnv *env, jclass clazz, jint mode);

#endif //ANDCORE_QJNI_H

//
// Created by qiuxu on 2022/11/21.
//

#include "qhook.h"
#include "hook/hook.h"
#include "base/utils.h"

extern "C"
JNIEXPORT void JNICALL
Java_com_wyc_qhook_JNI_nativeStartHook(JNIEnv *env, jclass clazz, jint mode) {
    ALOGD("start hook...");
    start_hook(env);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_wyc_qhook_JNI_nativeStopHook(JNIEnv *env, jclass clazz, jint mode) {
    ALOGD("stop hook...");
    stop_hook();
}


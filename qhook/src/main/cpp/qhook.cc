//
// Created by wyc on 2022/11/21.
//

#include "qhook.h"
#include "hook/dl.h"
#include "hook/hook.h"
#include "base/utils.h"
#include "action/action.h"

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

extern "C"
JNIEXPORT void JNICALL
Java_com_wyc_qhook_JNI_nativeDlInit(JNIEnv *env, jclass clazz, jint mode) {
    ALOGD("start DL init ...");
    init_DL();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_wyc_qhook_JNI_nativeAction(JNIEnv *env, jclass clazz, jint mode) {
    ALOGD("native action ...");
    do_action();
}
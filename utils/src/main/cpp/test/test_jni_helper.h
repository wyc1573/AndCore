//
// Created by wyc on 2022/12/27.
//

#pragma once
#include <jni.h>
#include "jni-helper/core_jni_helpers.h"

inline void test_jni_helper(JNIEnv* env) {
    jclass cls = android::FindClassOrDie(env, "com/wyc/utils/UtilsMainActivity");
//    jfieldID  generic_test = android::GetFieldIDOrDie(env, cls, "generic_test", "Landroid/widget/Button;");
//    jmethodID onCreate = android::GetMethodIDOrDie(env, cls, "onCreate", "(Landroid/os/Bundle;)V");
//    jclass clsRef = android::MakeGlobalRefOrDie(env, cls);
}
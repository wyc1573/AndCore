//
// Created by wyc on 2022/8/6.
//

#include "hook.h"
#include "proxy.h"
#include "base/utils.h"
#include "base/sym.h"
#include "shadowhook.h"

#define W(TYPE, SYM, VAL)

//for shadow hook. eg. static void* stub_kAddressOf_ = nullptr;
#define V(TYPE, LIB, SYM, VAL, RET, PARAM) static void* stub_##SYM##_ = nullptr;
#include "hook_sym"

void start_hook(JNIEnv *env) {
    init(env);
#define V(TYPE, LIB, SYM, VAL, RET, PARAM)  stub_##SYM##_ = shadowhook_hook_sym_name(LIB, REAL_NAME(SYM), \
    (void*) PROXY_FUNC(SYM), (void**) &ORIGIN_FUNC(SYM));                                     \
    if (stub_##SYM##_ == nullptr) {                                                           \
        ALOG_ALWAYS("Hook " #SYM " failed! %s", shadowhook_to_errmsg(shadowhook_get_errno()));                                         \
        stop_hook();                                                                          \
        return;                                                                               \
    }
#include "hook_sym"

    ALOG_ALWAYS("start hook success!");
}

void stop_hook() {
#define V(TYPE, LIB, SYM, VAL, RET, PARAM) if (stub_##SYM##_ != nullptr) { shadowhook_unhook(stub_##SYM##_); \
    stub_##SYM##_ = nullptr; }
#include "hook_sym"

    ALOG_ALWAYS("stop hook!");
}


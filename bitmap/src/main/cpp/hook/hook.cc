//
// Created by wyc on 2022/8/6.
//

#include "hook.h"
#include "proxy.h"
#include "helper-inl.h"
#include "base/utils.h"
#include "base/sym.h"
#include "shadowhook.h"

#define W(TYPE, SYM, VAL)

//for shadow hook. eg. static void* stub_kAllocateJavaPixelRef_ = nullptr;
#define V(TYPE, LIB, SYM, VAL, RET, PARAM) static void* stub_##SYM##_ = nullptr;
#include "symbols"

void start_bitmap_opt(JNIEnv *env) {
    init(env);
#define V(TYPE, LIB, SYM, VAL, RET, PARAM)  stub_##SYM##_ = shadowhook_hook_sym_name(LIB, REAL_NAME(SYM), \
    (void*) PROXY_FUNC(SYM), (void**) &ORIGIN_FUNC(SYM));                                     \
    if (stub_##SYM##_ == nullptr) {                                                           \
        ALOG_ALWAYS("Hook" #SYM "failed! return!");                                           \
        stop_bitmap_opt();                                                                    \
        return;                                                                               \
    }
#include "symbols"
    ALOG_ALWAYS("native start bitmap opt!");
}

void stop_bitmap_opt() {
#define V(TYPE, LIB, SYM, VAL, RET, PARAM) if (stub_##SYM##_ != nullptr) { shadowhook_unhook(stub_##SYM##_); \
    stub_##SYM##_ = nullptr; }
#include "symbols"
    ALOG_ALWAYS("native stop bitmap opt!");
}


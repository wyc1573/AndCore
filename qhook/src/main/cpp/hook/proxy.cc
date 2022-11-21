//
// Created by wyc on 2022/8/6.
//

#include "proxy.h"
#include "base/utils.h"
#include "base/macro.h"


void init(JNIEnv* env) {

}

// 定义待hook方法的origin方法(eg. void* originkAddressOf = nullptr;)
// 以及其函数指针类型(eg. using kAddressOfFuncType = )
#define W(TYPE, SYM, VAL)
#define V(TYPE, LIB, SYM, VAL, RET, PARAM) void* ORIGIN_FUNC(SYM) = nullptr;      \
    using  SYM##FuncType = RET (*) PARAM;
#include "symbols"

#define CALL_ORIGIN(kSym, param) ((kSym##FuncType)ORIGIN_FUNC(kSym)) param
#define CHECK_ORIGIN_NULL_ABORT(kSym) if(UNLIKELY(ORIGIN_FUNC(kSym) == nullptr)) { \
     Abort(#kSym"Proxy is NULL");                                                  \
}

jlong PROXY_FUNC(kAddressOf)(JNIEnv* env, jobject obj, jobject objArr) {
    CHECK_ORIGIN_NULL_ABORT(kAddressOf);
    return CALL_ORIGIN(kAddressOf, (env, obj, objArr));
}

#undef W
#undef V
//
// Created by wyc on 2022/8/6.
//

#include "proxy.h"
#include "helper-inl.h"
#include "base/utils.h"
#include "base/macro.h"

static jobject   gVMRuntime_;
static jclass    gVMRuntime_class_;
static jmethodID gVMRuntime_addressOf_;
static jmethodID gVMRuntime_registerNativeAllocation_;

void init(JNIEnv* env) {
    jmethodID mGetRuntime;
    gVMRuntime_class_ = make_globalref(env, "dalvik/system/VMRuntime");
    mGetRuntime = env->GetStaticMethodID(gVMRuntime_class_, "getRuntime", "()Ldalvik/system/VMRuntime;");
    gVMRuntime_ = env->NewGlobalRef(env->CallStaticObjectMethod(gVMRuntime_class_, mGetRuntime));
    gVMRuntime_addressOf_ = env->GetMethodID(gVMRuntime_class_, "addressOf", "(Ljava/lang/Object;)J");
    gVMRuntime_registerNativeAllocation_ = env->GetMethodID(gVMRuntime_class_, "registerNativeAllocation", "(I)V");
}

// 定义待hook方法的origin方法(eg. void* originkAllocateJavaPixelRef = nullptr;)
// 以及其函数指针类型(eg. using kAllocateJavaPixelRefFuncType = )
#define W(TYPE, SYM, VAL)
#define V(TYPE, LIB, SYM, VAL, RET, PARAM) void* ORIGIN_FUNC(SYM) = nullptr; \
    using  SYM##FuncType = RET (*) PARAM;
#include "symbols"


#define CALL_ORIGIN(kSym, param) ((kSym##FuncType)ORIGIN_FUNC(kSym)) param
#define CHECK_ORIGIN_IF_NULL_ABORT(kSym) if(UNLIKELY(ORIGIN_FUNC(kSym) == nullptr)) { \
     Abort(#kSym"Proxy is NULL");                               \
}

void* PROXY_FUNC(kAllocateJavaPixelRef)(JNIEnv* env, void* bitmap, void* ctable) {
    LOCAL_SCOPED(kAllocateJavaPixelRef);
    CHECK_ORIGIN_IF_NULL_ABORT(kAllocateJavaPixelRef);
    return CALL_ORIGIN(kAllocateJavaPixelRef, (env, bitmap, ctable));
}

#define COND_CALL_ORIGIN_RET(cond, func, param) if (cond) { \
    return CALL_ORIGIN(func, param);                        \
}

#define COND_CALL_ORIGIN_LOG_RET(cond, func, param, msg) if (cond) {  \
    ALOG_ALWAYS(msg);                                                 \
    return CALL_ORIGIN(func, param);                                  \
}

jobject PROXY_FUNC(kNewNonMovableArray)(JNIEnv* env, jobject obj, jclass jcls, jint length) {
    CHECK_ORIGIN_IF_NULL_ABORT(kNewNonMovableArray);
    bool cond = (false == IS_EXE_ING(kAllocateJavaPixelRef));
    COND_CALL_ORIGIN_RET(cond, kNewNonMovableArray, (env, obj, jcls, length));

    jbyteArray fake_array = (jbyteArray) CALL_ORIGIN(kNewNonMovableArray, (env, obj, jcls, kFakeSize));
    kArrAddrType array_addr = env->CallLongMethod(gVMRuntime_, gVMRuntime_addressOf_, fake_array);

    set_mock_length(array_addr, length);
    set_magic(array_addr);
    set_global_ref(env, fake_array, array_addr);
    env->CallVoidMethod(gVMRuntime_, gVMRuntime_registerNativeAllocation_, length);

    //double check: 如果addressOf hook失败，或者length，magic不一致，则调用原来的函数；否则返回fake_array
    cond = (ORIGIN_FUNC(kAddressOf) == nullptr);
    cond = cond || (false == check_length(env, fake_array, length));
    cond = cond || (false == check_magic(env, fake_array));
    COND_CALL_ORIGIN_LOG_RET(cond, kNewNonMovableArray, (env, obj, jcls, length), "CHECK FAILED, CALL ORIGIN")
    return fake_array;
}

jlong PROXY_FUNC(kAddressOf)(JNIEnv* env, jobject obj, jobject javaArray) {
    CHECK_ORIGIN_IF_NULL_ABORT(kAddressOf);
    bool condition = (false == IS_EXE_ING(kAllocateJavaPixelRef));
    condition = condition || IS_EXE_ING(kAddressOf) ;
    condition = condition || (false == check_magic(env, (jbyteArray)javaArray));
    COND_CALL_ORIGIN_RET(condition, kAddressOf, (env, obj, javaArray));

    LOCAL_SCOPED(kAddressOf);
    jsize retrieved_size = env->GetArrayLength((jbyteArray)javaArray);
    void* ret = calloc(retrieved_size, 1);
    jlong array_addr = env->CallLongMethod(gVMRuntime_, gVMRuntime_addressOf_, javaArray);
    set_bitmap(array_addr, ret);
    return reinterpret_cast<jlong>(ret);
}

void PROXY_FUNC(kDeleteWeakGlobalRef)(JNIEnv* env, jobject buffer) {
    CHECK_ORIGIN_IF_NULL_ABORT(kDeleteWeakGlobalRef);
    bool condition = (false == IS_EXE_ING(kDoFreePixels));
    COND_CALL_ORIGIN_RET(condition, kDeleteWeakGlobalRef, (env, buffer));
    jlong addr = env->CallLongMethod(gVMRuntime_, gVMRuntime_addressOf_, buffer);
    check_and_free(env, addr);
}

void PROXY_FUNC(kDoFreePixels)(void* thiz) {
    LOCAL_SCOPED(kDoFreePixels);
    CHECK_ORIGIN_IF_NULL_ABORT(kDoFreePixels);
    CALL_ORIGIN(kDoFreePixels, (thiz));
}

#undef W
#undef V
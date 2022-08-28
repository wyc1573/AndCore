//
// Created by wyc on 2022/8/6.
//

#pragma once

#include "base/utils.h"

using kArrAddrType = jlong;
using kArrLenType = int32_t;
using kMagicType = int32_t;
using kGlobalRefType = jobject;
using kFakeSizeType = uint8_t ;

constexpr kMagicType kMagic = 0x13572468;
constexpr kMagicType kMagicLowest = 0x68;
constexpr kMagicType kMagicHighest = 0x13;
constexpr kFakeSizeType kFakeSize = sizeof(int) + sizeof(jobject) + sizeof(void*);

struct TLSFuncExecuting {
#define W(TYPE, SYM, VAL)
#define V(TYPE, LIB, SYM, VAL, RET, PARAM) bool SYM = false;
#include "symbols"
};

static thread_local TLSFuncExecuting g_tls_fe_;
#define SET_EXE_ING(kSym)  g_tls_fe_.kSym = true
#define UNSET_EXE_ING(kSym) g_tls_fe_.kSym = false
#define IS_EXE_ING(kSym)  (g_tls_fe_.kSym)

#define LOCAL_SCOPED(kSym) Scoped##kSym scoped##kSym
#define DEFINE_SCOPED(kSym) class Scoped##kSym { \
public:                                   \
    Scoped##kSym() {                      \
        SET_EXE_ING(kSym);                \
    }                                     \
    ~Scoped##kSym() {                     \
         UNSET_EXE_ING(kSym);             \
    }                                     \
}

// 自动管理对thread local变量的修改
// 目的是使proxy方法知道自己的调用环境
// 比如，让kAddressOf方法知道自己是kAllocateJavaPixelRef方法调用的
// 只有在这种条件下，kAddressOf proxy方法才能执行对应的逻辑
DEFINE_SCOPED(kAllocateJavaPixelRef);
DEFINE_SCOPED(kAddressOf);
DEFINE_SCOPED(kDoFreePixels);

ALWAYS_INLINE void set_mock_length(kArrAddrType addr, jint length) {
    *reinterpret_cast<kArrLenType*>(addr - sizeof(kArrLenType)) = static_cast<kArrLenType>(length);
}

ALWAYS_INLINE void set_real_size(kArrAddrType addr, kFakeSizeType length) {
    *reinterpret_cast<kArrLenType*>(addr - sizeof(kArrLenType)) = static_cast<kArrLenType>(length);
}

ALWAYS_INLINE void set_magic(kArrAddrType addr) {
    *reinterpret_cast<kMagicType*>(addr) = reinterpret_cast<kMagicType>(kMagic);
}

ALWAYS_INLINE kMagicType get_magic(kArrAddrType addr) {
    return *reinterpret_cast<kMagicType*>(addr);
}

ALWAYS_INLINE void set_global_ref(JNIEnv* env, jbyteArray byte_array, kArrAddrType addr) {
    jobject g_ref = env->NewGlobalRef(byte_array);
    *reinterpret_cast<kGlobalRefType*>(addr + sizeof(kMagicType)) = reinterpret_cast<kGlobalRefType>(g_ref);
}

ALWAYS_INLINE kGlobalRefType get_global_ref(kArrAddrType addr) {
    return *(kGlobalRefType*)(addr + sizeof(kMagicType));
}

ALWAYS_INLINE void set_bitmap(kArrAddrType addr, void* ret) {
    *reinterpret_cast<void**>(addr + sizeof(kMagicType) + sizeof(kGlobalRefType)) =
            reinterpret_cast<void*>(ret);
}

ALWAYS_INLINE void* get_bitmap(kArrAddrType addr) {
    void* ret = *reinterpret_cast<void**>(addr + sizeof(kMagicType) + sizeof(kGlobalRefType));
    return ret;
}

ALWAYS_INLINE bool check_magic(JNIEnv* env, jbyteArray fake_byte_arr) {
    jbyte buff[4];
    bool little_endian = is_little_endian();
    env->GetByteArrayRegion(fake_byte_arr, 0, 4, buff);
    if (little_endian && buff[0] == kMagicLowest) { return true; }
    if (!little_endian && buff[0] == kMagicHighest) { return true; }
    return false;
}

ALWAYS_INLINE bool check_length(JNIEnv* env, jbyteArray fake_byte_arr, jint length) {
    jsize retrieved_size = env->GetArrayLength(fake_byte_arr);
    if (retrieved_size == length) { return true; }
    return false;
}

ALWAYS_INLINE void check_and_free(JNIEnv* env, kArrAddrType addr) {
    if (get_magic(addr) == kMagic) {
        ALOGD("free native bitmap!");
        void* bitmap = get_bitmap(addr);
        free(bitmap);
        set_real_size(addr, kFakeSize);
        jobject global_ref = get_global_ref(addr);
        env->DeleteGlobalRef(global_ref);
    }
}

#undef W
#undef V
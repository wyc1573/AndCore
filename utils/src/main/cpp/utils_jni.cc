//
// Created by wyc on 2022/8/20.
//

#include "utils_jni.h"
#include "env.h"
#include "test/test_log.h"
#include "test/test_string.h"
#include "test/test_parseint.h"
#include "test/test_file.h"
#include "test/test_atomic.h"
#include "test/test_bit_utils.h"
#include "test/test_casts.h"
#include "test/test_hash.h"
#include "test/test_dcheck_vector.h"
#include "test/test_debug_stack.h"
#include "test/test_hashmap.h"
#include "test/test_stl-util.h"
#include "test/test_iter_range.h"
#include "test/test_util.h"
#include "test/test_assemble.h"
#include "test/test_enum.h"
#include "test/test_unix_file.h"
#include "test/test_mem_map.h"
#include "test/test_mutex.h"
#include "test/test_time_utils.h"
#include "test/test_thread_pool.h"
#include "test/test_bit_table.h"
#include "test/test_safe_copy.h"
#include "test/test_atomic_stack.h"
#include "test/test_bitmap.h"
#include "test/test_space_bitmap.h"
#include "test/test_card_table.h"
#include "test/test_jni_helper.h"
#include "jni-helper/core_jni_helpers.h"
#include "test/socket/socket_sample.h"
#include "test/cpp/cpp_sample.h"
#include "test/test_rand.h"
#include "test/test_statistics.h"
#include "test/test_futex.h"
#include "test/test_mem_model.h"
#include "apue/signal_test.h"

#define CASE(n) LOG(INFO) << "============" << #n << "==============";

const char* env_test_path = nullptr;
volatile bool init_ = false;

void initLog() {
    if (!init_) {
        const char*  argv[] = {"AndCore"};
        android::base::InitLogging(const_cast<char**>(argv));
        init_ = true;
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_wyc_utils_JNI_setPath(JNIEnv *env, jclass clazz, jstring path) {
    initLog();
    env_test_path = env->GetStringUTFChars(path, NULL);
    LOG(INFO) << "env_test_path: " << env_test_path;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_wyc_utils_JNI_test(JNIEnv *env, jclass clazz, jint mode) {
    initLog();

    CASE(logging);
    test_logging();

    CASE(string);
    test_string();

    CASE(parseint);
    test_parseint();

    CASE(StringPrintf);
    test_string_printf();

    CASE(file)
    test_file();

    CASE(Atomic)
    test_atomic();

    CASE(BitUtils);
    test_bitutils();

    CASE(Casts)
    test_casts();

    CASE(DataHash)
    test_hash();

    CASE(DCheckedVector)
    test_dcheck_vector();

    CASE(DebugStack)
    test_debug_stack();

    CASE(HashMap)
    test_hashmap();

    CASE(Stl-Utils)
    test_stl_util();

    CASE(IteratorRange)
    test_iter_range();

    CASE(Unix_File)
    test_unix_file();

    CASE(MemMap)
    test_mem_map();

    CASE(Time-Utils)
    test_time_utils();

    CASE(Mutex)
    test_mutex();

    CASE(ThreadPool)
    test_thread_pool();

    CASE(BitTable)
    test_bit_table();

    CASE(SafeCopy)
    test_safe_copy();

    CASE(AtomicStack)
    test_atomic_stack();

    CASE(Bitmap)
    test_bitmap();

    CASE(SpaceBitmap)
    test_space_bitmap();

    CASE(CardTable)
    test_card_table();

    CASE(JniHelper)
    test_jni_helper(env);

    CASE(TestAssemble)
    test_assemble();

    CASE(RAND)
    test_random();

    CASE(STATISTICS)
    test_statistics();

    CASE(FUTEX)
    test_futex();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_wyc_utils_JNI_socketNormal(JNIEnv *env, jclass clazz) {
    initLog();
    LOG(INFO) << __FUNCTION__ ;
    test_normal_socket();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_wyc_utils_JNI_socketDiagram(JNIEnv *env, jclass clazz) {
    initLog();
    LOG(INFO) << __FUNCTION__;
    test_diagram_socket();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_wyc_utils_JNI_socketMsg(JNIEnv *env, jclass clazz) {
    initLog();
    LOG(INFO) << __FUNCTION__;
    test_msg_socket();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_wyc_utils_JNI_socketPair(JNIEnv *env, jclass clazz) {
    initLog();
    LOG(INFO) << __FUNCTION__;
    test_socket_pair();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_wyc_utils_JNI_socketPipe(JNIEnv *env, jclass clazz) {
    initLog();
    LOG(INFO) << __FUNCTION__;
    test_socket_pipe();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_wyc_utils_JNI_eventFd(JNIEnv *env, jclass clazz) {
    initLog();
    LOG(INFO) << __FUNCTION__ ;
    test_eventfd();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_wyc_utils_JNI_epoll(JNIEnv *env, jclass clazz) {
    initLog();
    LOG(INFO) << __FUNCTION__ ;
    test_epoll();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_wyc_utils_JNI_cppObjModel(JNIEnv *env, jclass clazz) {
    initLog();
    LOG(INFO) << __FUNCTION__ ;
    cpp_obj_model();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_wyc_utils_JNI_cppMisc(JNIEnv *env, jclass clazz) {
    initLog();
    LOG(INFO) << __FUNCTION__ ;
    cpp_misc();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_wyc_utils_JNI_statistics(JNIEnv *env, jclass clazz) {
    initLog();
    LOG(INFO) << __FUNCTION__ ;
    test_statistics();
}

#include "test/elf_utils.h"
extern "C"
JNIEXPORT void JNICALL
Java_com_wyc_utils_JNI_cppConcurrency(JNIEnv *env, jclass clazz) {
    initLog();
    LOG(INFO) << __FUNCTION__ ;
    test_cpp_concurrency();
}


extern "C"
JNIEXPORT void JNICALL
Java_com_wyc_utils_JNI_mmapThenDel(JNIEnv *env, jclass clazz) {
    char* begin = nullptr;
    int size = 4 * 1024 * 10;

    const char * const str = "a";
    int fd = open("/data/local/tmp/mmap", O_RDWR | O_CREAT, S_IRWXU|S_IRWXG|S_IRWXO);
    if (fd == -1) {
        LOG(INFO) << "open failed!";
        return;
    }

    LOG(INFO) << "Write Start (10 PAGE).";
    for (int i = 0; i < size; i += 1024) {
        write(fd, str, 1);
    }
    LOG(INFO) << "write End.";

    begin = static_cast<char *>(mmap(nullptr, 1024, PROT_READ, MAP_PRIVATE, fd, 0));
    LOG(INFO) << "begin_ = " << android::base::StringPrintf("%p", begin);
    unlink("/data/local/tmp/mmap");

    LOG(INFO) << "Sleep 15 Seconds ...";
    sleep(15);

    LOG(INFO) << "Read MemMap Address ...";
    int sum = 0;
    for (int i  = 0; i < size; i += 1024) {
    sum += begin[i];
    }
    LOG(INFO) << "Read End.(" + sum << ")";
}


extern "C"
JNIEXPORT void JNICALL
Java_com_wyc_utils_JNI_testElf(JNIEnv *env, jclass clazz) {
    initLog();
    LOG(INFO) << __FUNCTION__ ;
    test_elf();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_wyc_utils_JNI_tempTest(JNIEnv *env, jclass clazz) {
    initLog();
    LOG(INFO) << __FUNCTION__ ;
    signal_test();
}

#undef CASE
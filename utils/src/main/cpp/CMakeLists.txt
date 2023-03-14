cmake_minimum_required(VERSION 3.4.1)

set(CMAKE_CXX_STANDARD 17)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(Utils
        SHARED
        utils_jni.cc
        android-base/strings.cc
        android-base/logging.cc
        android-base/stringprintf.cc
        android-base/file.cc
        base/mem_map.cc
        base/mem_map_unix.cc
        base/safe_copy.cc
        base/utils.cc
        base/os_linux.cc
        base/logging.cc
        base/mutex.cc
        base/locks.cc
        base/time_utils.cc
        base/barrier.cc
        base/thread_pool.cc
        base/arena_allocator.cc
        base/malloc_arena_pool.cc
        base/rand_util.cc
        gc/bitmap.cc
        gc/space_bitmap.cc
        gc/card_table.cc
        base/scoped_arena_allocator.cc
        unix_file/random_access_file_utils.cc
        unix_file/fd_file.cc
        test/socket/socket_sample.cc
        test/cpp/cpp_sample.cc
        test/cpp/cpp_misc.cc)

find_library(log-lib
        log)

target_link_libraries(Utils
        ${log-lib})

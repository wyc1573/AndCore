cmake_minimum_required(VERSION 3.4.1)

set(CMAKE_CXX_STANDARD 17)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libziparchive/include)

add_library(Utils
        SHARED
        utils_jni.cc
        android-base/strings.cc
        android-base/logging.cc
        android-base/stringprintf.cc
        android-base/file.cc
        android-base/mapped_file.cc
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
        base/utf8.cc
        gc/bitmap.cc
        gc/space_bitmap.cc
        gc/card_table.cc
        base/scoped_arena_allocator.cc
        unix_file/random_access_file_utils.cc
        unix_file/fd_file.cc
        dexdump/dexdump.cc
        dexdump/dexdump_cfg.cc
        dexdump/dexdump_main.cc
        libdexfile/compact_offset_table.cc
        libdexfile/dex_instruction.cc
        libdexfile/signature.cc
        libdexfile/standard_dex_file.cc
        libdexfile/compact_dex_file.cc
        libdexfile/utf.cc
        libdexfile/dex_file_loader.cc
        libdexfile/dex_file_verifier.cc
        libdexfile/descriptors_names.cc
        libdexfile/primitive.cc
        libdexfile/dex_file.cc
        libdexfile/dex_file_exception_helpers.cc
        libdexfile/modifiers.cc
        libziparchive/zip_archive_stream_entry.cc
        libziparchive/zip_archive.cc
        libziparchive/zip_writer.cc
        effective-cpp/effect-1.cc
        test/socket/socket_sample.cc
        test/cpp/cpp_sample.cc
        test/elf_utils.cc
        test/cpp/cpp_misc.cc)

find_library(log-lib
        log)

find_library(log-z
        z)

target_link_libraries(Utils
        ${log-lib}
        ${log-z})

add_library(ElfDemo
        SHARED
        test/test_elf.cc
        )

target_link_libraries(ElfDemo
        ${log-lib})
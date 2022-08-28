cmake_minimum_required(VERSION 3.4.1)
project("NativeBitmap")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_FLAGS "-Wmacro-redefined")
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
find_package(shadowhook REQUIRED CONFIG)

add_library(NativeBitmap
        SHARED
        hook/hook.cc
        hook/proxy.cc
        bitmap_jni.cc)

find_library(log-lib
        log)

target_link_libraries(NativeBitmap
        ${log-lib}
        shadowhook::shadowhook)

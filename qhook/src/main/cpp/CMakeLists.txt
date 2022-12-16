cmake_minimum_required(VERSION 3.4.1)
project("qhook")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_FLAGS "-Wmacro-redefined")
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
find_package(shadowhook REQUIRED CONFIG)
find_package(xdl REQUIRED CONFIG)

add_library(qhook
        SHARED
        hook/proxy.cc
        hook/hook.cc
        hook/dl.cc
        action/action.cc
        qhook.cc)

find_library(log-lib
        log)

target_link_libraries(qhook
        ${log-lib}
        xdl::xdl
        shadowhook::shadowhook)

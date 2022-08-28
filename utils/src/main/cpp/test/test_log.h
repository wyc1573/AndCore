//
// Created by wyc on 2022/8/6.
//

#pragma once
#include "android-base/logging.h"

inline void test_logging() {
    LOG(INFO) << "This is printed by LOG(INFO).";
    LOG(INFO) << "With LOG(XX), u can print like use a printf.";
    PLOG(INFO) << "This is print by PLOG(INFO), witch 'P' as perror(3).";  //output: test PLOG(INFO): Success
    LOG(INFO) << "LOG(FATAL) will abort with this msg.";
    LOG(INFO) << "PLOG(FATAL) will abort with this msg, and print line number eg. test_log.h:16";
    LOG(INFO) << "logging.h also provide assertions, eg. CHECK(must_be_true), CHECK_EQ(a, b) << z_is_interesting_too;";
//    LOG(FATAL) << "LOG(FATAL) will abort with this msg";
//    PLOG(FATAL) << "PLOG(FATAL) will abort with this msg, and print line number eg test_log.h:16";  //output: test_log.h:16] test LOG(FATAL)
}

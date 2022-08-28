//
// Created by wyc on 2022/8/6.
//

#pragma once

#include "android-base/strings.h"
#include "android-base/logging.h"
#include "android-base/stringprintf.h"

inline void test_string() {
    std::string str("  hello world  ");
    std::string deli(" ");
    LOG(INFO) << "s = " << str;
    std::string trim_str = android::base::Trim(str);
    LOG(INFO) << "trim str: " << trim_str;
    auto vec = android::base::Split(trim_str, deli);
    LOG(INFO) << "After Split: vec[0]=" << vec[0] << " vec[1]=" << vec[1];
    if (android::base::StartsWith(trim_str, "hello")) {
        LOG(INFO) << trim_str << " StartsWith " << "hello";
    }
    if (android::base::EndsWith(trim_str, "world")) {
        LOG(INFO) << trim_str << " EndsWith " << "world";
    }
    if (android::base::EndsWithIgnoreCase(trim_str, "WORLD")) {
        LOG(INFO) << trim_str << " EndsWithIgnoreCase " << "WORLD";
    }
    if (android::base::EqualsIgnoreCase(trim_str, "HELLO WORLD")) {
        LOG(INFO) << trim_str << " EqualsIgnoreCase " << "HELLO WORLD";
    }
    std::vector join_vec{"hello", "world"};
    std::string joined_str = android::base::Join(join_vec, ",");
    LOG(INFO) << "Join {\"hello\", \"world\"}" << " ==> " << joined_str;
}

inline void test_string_printf() {
    using namespace android::base;
    LOG(INFO) << StringPrintf("With StringPrintf, u can easily construct a string");
    LOG(INFO) << StringPrintf("join int eg.%d, pointer eg.%p, str eg.%s and so on, just like what u use in printf", 1, 12345, "hello world");
}

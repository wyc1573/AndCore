//
// Created by wyc on 2022/8/27.
//

#pragma once

#include "base/safe_copy.h"

inline void test_safe_copy() {
    char buff[1024];
    art::SafeCopy(buff, 0x0, 1024);
    LOG(INFO) << "With SafeCopy, u can safely access any mem address.";
    LOG(INFO) << "not usable for MIN SDK < 23";
}



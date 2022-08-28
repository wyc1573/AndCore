//
// Created by wyc on 2022/8/6.
//

#pragma once

#include "android-base/parseint.h"
#include "android-base/logging.h"
inline void test_parseint() {
    uint64_t resU64;
    using namespace android::base;
    LOG(INFO) << "ParseUint(" << "\"" << 12345 << "\", &resu64) return " << ParseUint("12345", &resU64) << ", resU64: " << resU64;
    uint8_t resU8;
    LOG(INFO) << "ParseUint(" << "\"" << 12345 << "\", &resU8) return " << ParseUint("12345", &resU8) << ", resU8: " << resU8;
    LOG(INFO) << "ParseUint(" << "\"0x" << 12345 << "\", &resu64) return " << ParseUint("0x12345", &resU64) << ", resU64: " << resU64;

    int32_t res32;
    LOG(INFO) << "ParseInt(" << "\"-" << 12345 << "\", &res32) return " << ParseInt("-12345", &res32) << ", res32: " << res32;

    std::string snum("12345");
    LOG(INFO) << "ParseInt(" << "snum(\"12345\")" << ", &res32) return " << ParseInt(snum, &res32) << ", res32: " << res32;
    LOG(INFO) << "ParseByteCount(\"12345\", &resU64) return " << ParseByteCount("12345", &resU64) << ", res32: " << resU64;
}


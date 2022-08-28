//
// Created by wyc on 2022/8/20.
//

#pragma once

#include "base/dchecked_vector.h"

#define TEST(func) LOG(INFO) << #func << " -> " << func
#define TESTV(func) LOG(INFO) << #func; func
#define TEST_LOG(func ,msg) LOG(INFO) << #func << msg

inline void test_dcheck_vector() {
    using namespace art;
    LOG(INFO) << "[serving as a replacement for std::vector<> but adding\n"
                 "DCHECK()s for the subscript operator, front(), back(), pop_back(),\n"
                 "and for insert()/emplace()/erase() positions]";

    dchecked_vector<int> dvec = {1,2,3};
    LOG(INFO) << "dchecked_vector<int> dvec = {1,2,3};";

    TEST(dvec.size());
    TEST_LOG(dvec[3], " will abort: Check failed: n < size() (n=3, size()=3)");
    TESTV(dvec.clear());
    TEST(dvec.size());
    TEST_LOG(dvec.front(), " Abort message: Check failed: !empty()");
    TEST_LOG(dvec.back(), " Abort message: Check failed: !empty()");
    TEST_LOG(dvec.pop_back(), " Abort message: Check failed: !empty()");
    TEST_LOG(dvec.insert(++dvec.end(), 1)," Abort message: 'Check failed: cbegin() <= position && position <= cend() '");
    LOG(INFO) << "others ...";
}

#undef TEST
#undef TESTV
#undef TEST_LOG
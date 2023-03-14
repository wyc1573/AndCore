//
// Created by wyc on 2023/3/14.
//

#pragma once
#include <vector>
#include "base/rand_utils.h"
#include "android-base/logging.h"
#include "android-base/stringprintf.h"

inline void test_random() {
    std::vector<int> vec_uniform;
    std::vector<double> vec_normal;
    UniformRand(0, 100, vec_uniform, 10);
    NormalRand(0, 1, vec_normal, 10);
    LOG(INFO) << "均匀分布[0-100]:";
    std::string uniform_str;
    bool flag = false;
    for (auto i : vec_uniform) {
        if (flag == false) {
            uniform_str = android::base::StringPrintf("%d", i);
            flag = true;
        } else {
            uniform_str = android::base::StringPrintf("%s, %d", uniform_str.c_str(), i);
        }
    }
    LOG(INFO) << "{ " << uniform_str << " }";
    LOG(INFO) << "正态分布[0,1]:";
    std::string normal_str;
    flag = false;
    for (auto i : vec_normal) {
        if (flag) {
            normal_str = android::base::StringPrintf("%s, %f", normal_str.c_str(), i);
        } else {
            flag = true;
            normal_str = android::base::StringPrintf("%f", i);
        }
    }
    LOG(INFO) << "{ " << normal_str << " }";
}

//
// Created by wyc on 2023/3/14.
//

#pragma once
#include "base/statistics-inl.h"
#include "base/rand_utils.h"

void test_statistics() {
    int N = 10000;
    int uniform_start = 0, uniform_end = 100, mean = 0, sd = 1;
    std::vector<int> vec_uniform;
    UniformRand(0, 100, vec_uniform, N);
    LOG(INFO) << "生成均匀分布的[" << uniform_start << "-" << uniform_end << "]的" << N << "个整数, 检测{平均值="
    << average(vec_uniform) << ", 标准差=" << standard_deviation(vec_uniform) << "}";
    std::vector<double> vec_normal;
    NormalRand(0, 1, vec_normal, N);
    LOG(INFO) << "生成{均值="<< mean <<",标准差=" << sd << "}的"<< N << "个数, 检测{均值=" << average(vec_normal)
    << ", 标准差= " << standard_deviation(vec_normal) << "}";

    std::vector<int> vec_uniform2;
    UniformRand(0, 10, vec_uniform2, N);
    LOG(INFO) << "生成均匀分布的[0-10]的" << N << "个整数, 检测{平均值="
              << average(vec_uniform2) << ", 标准差=" << standard_deviation(vec_uniform2) << "}";
}

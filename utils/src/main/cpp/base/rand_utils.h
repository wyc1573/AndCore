//
// Created by wyc on 2023/3/14.
//

#pragma once
#include <vector>

void UniformRand(int begin, int end, std::vector<int>& vec, int n);

// 平均值和标准差
void NormalRand(int mean, int standard_deviation, std::vector<double> &vec, int n);
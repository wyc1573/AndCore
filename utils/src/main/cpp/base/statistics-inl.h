//
// Created by wyc on 2023/3/14.
//

#pragma once
#include "statistics.h"

template <typename T,
        typename /*= typename std::enable_if_t<std::is_arithmetic_v<T>>*/>
T average(std::vector<T>& vec) {
    T sum = 0;
    for (int i = 0; i < vec.size(); i++) {
        sum += vec[i];
    }
    return (sum/vec.size());
}

template<typename T,
        typename /*= typename std::enable_if_t<std::is_arithmetic_v<T>>*/>
T variance(std::vector<T>& vec) {
    T mean = average(vec);
    T sum = 0;
    for (int i = 0; i < vec.size(); i++) {
        sum += (vec[i] - mean) * (vec[i] - mean);
    }
    return sum / vec.size();
}

template<typename T,
        typename/* = typename std::enable_if_t<std::is_arithmetic_v<T>>*/>
double standard_deviation(std::vector<T>& vec) {
    T var = variance(vec);
    return sqrt(var);
}

//
// Created by wyc on 2023/3/14.
//

#pragma once
#include <vector>

template <typename T,
        typename = typename std::enable_if_t<std::is_arithmetic_v<T>>>
T average(std::vector<T>& vec);

template<typename T,
        typename = typename std::enable_if_t<std::is_arithmetic_v<T>>>
T variance(std::vector<T>& vec);

template<typename T,
        typename = typename std::enable_if_t<std::is_arithmetic_v<T>>>
double standard_deviation(std::vector<T>& vec);

//
// Created by wyc on 2022/8/20.
//

#pragma once

#include "base/iteration_range.h"
#include <vector>

template <typename Iter>
inline void print(art::IterationRange<Iter> range) {
    Iter iter = range.begin();
    Iter end = range.end();
    std::string log;
    LOG(INFO) << "traversal begin >>>";
    while(iter != end) {
        LOG(INFO) << *iter++;
    }
    LOG(INFO) << "<<< traversal end";
}

inline void test_iter_range() {
    LOG(INFO) << "Helper class that acts as a container for range-based loops, given an iteration"
    " \n range [first, last) defined by two iterators.";
    LOG(INFO) << "print(art::MakeIterationRange(vec.begin(), vec.end()))";
    std::vector<int> vec{1,2,3};
    print(art::MakeIterationRange(vec.begin(), vec.end()));
    LOG(INFO) << "print(art::ReverseRange(vec))";
    print(art::ReverseRange(vec));
    int arr[] = {1,2,3};
    LOG(INFO) << "int arr[] = {1,2,3}; print(art::ReverseRange(arr))";
    print(art::ReverseRange(arr));
}

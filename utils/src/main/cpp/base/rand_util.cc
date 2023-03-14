//
// Created by wyc on 2023/3/14.
//
#include "rand_utils.h"
#include <random>
#include "base/logging.h"


void UniformRand(int begin, int end, std::vector<int>& vec, int n) {
    std::random_device rd;
    std::mt19937 uniform_gen(rd());
    std::uniform_int_distribution<int> distrib(begin, end);
    for (int i = 0; i < n; i++) {
        vec.push_back(distrib(uniform_gen));
    }
}

void NormalRand(int mean, int standard_deviation, std::vector<double> &vec, int n) {
    std::default_random_engine normal_gen(std::chrono::system_clock::now().time_since_epoch().count());
    std::normal_distribution<double> distrib(mean, standard_deviation);
    for (int i = 0; i < n; i++) {
        vec.push_back(distrib(normal_gen));
    }
}
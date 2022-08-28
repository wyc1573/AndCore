//
// Created by wyc on 2022/8/20.
//

#pragma once

#include "base/data_hash.h"
#include<vector>
void inline test_hash() {
    art::DataHash data_hash;
    std::vector<int> vec{1,2,3,4,5,6,7,8,9,0};
    auto hash = data_hash(vec);
    LOG(INFO) << "DataHash vector {1,2,3,4,5,6,7,8,9,0} -> " << hash;
}

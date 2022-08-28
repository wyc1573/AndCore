//
// Created by wyc on 2022/8/20.
//

#pragma once

#include "base/stl_util.h"
#include <vector>
#include <map>

class  Elem {
public:
    Elem(int i) {
        LOG(INFO) << "Elem Construct: " << i;
        cnt = i;
    }

    ~Elem() {
        LOG(INFO) << "Elem Destructor: " << cnt;
    }

private:
    int cnt;
};

inline void fill_vector(std::vector<Elem*>& vec) {
    vec.clear();
    for (int i = 0; i < 2; i++) { vec.push_back(new Elem(i)); }
}

inline void test_stl_util() {
    std::vector<Elem*> vec;
    fill_vector(vec);
    LOG(INFO) << "STLDeleteContainerPointers(delete elem)";
    art::STLDeleteContainerPointers(vec.begin(), vec.end());

    fill_vector(vec);
    art::STLDeleteElements(&vec);
    LOG(INFO) << "STLDeleteElements(delete elem, then clear!)";

    std::map<int, Elem*> map;
    map[0] = new Elem(0);
    map[1] = new Elem(1);
    art::STLDeleteValues(&map);
    LOG(INFO) << "STLDeleteValues(delete iter->second, then clear!)";

    std::unique_ptr<int, art::FreeDelete> uptr((int*) malloc(1024));

    std::vector<int> vec3{1,2,3};
    LOG(INFO) << "For vec3{1,2,3}; IndexOfElement(vec3, 2): " << art::IndexOfElement(vec3, 2);
    art::RemoveElement(vec3, 2);
    LOG(INFO) << "After RemoveElement(vec3, 2), ContainsElement(vec3, 2): "
            << (art::ContainsElement(vec3, 2) ? "true" : "false");

    art::ReplaceElement(vec3, 1 , 2);
    LOG(INFO) << "After ReplaceElement(vec3, 1, 2), ContainsElement(vec3, 2): "
            << (art::ContainsElement(vec3, 2) ? "true" : "false");

    art::FNVHash<std::vector<int>> hash;
    LOG(INFO) << "FNVHash(vec3): " << hash(vec3);

}

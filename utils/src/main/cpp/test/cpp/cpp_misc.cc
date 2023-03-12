//
// Created by wyc on 2023/3/12.
//

#include "cpp_sample.h"
#include <stdarg.h>
#include "base/utils.h"

void test_va(int num, ...) {
    va_list  ap;
    va_start(ap, num);
    LOG(INFO) << "test_va: args = {";
    for (int i = 0; i < num; i++) {
        LOG(INFO) << va_arg(ap, int);
    }
    LOG(INFO) << "}";
}

void cpp_misc() {
    test_va(3, 1, 2, 3);
}
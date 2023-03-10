//
// Created by wyc on 2023/3/10.
//

#pragma once

#include "base/utils.h"

void funcA() {
    try {
        unsigned long pc;
        __asm__("adr %0, pc_plus_4_1\n\t"
                "pc_plus_4_1:\n\t"
                "nop\n\t"
                : "=r" (pc));
        LOG(INFO) << __FUNCTION__ << " {pc: " << android::base::StringPrintf("0x%x", pc)
                  << ", __builtin_return_address(0): " << android::base::StringPrintf("0x%x", __builtin_return_address(0)) <<"}";;
        throw "test";
    } catch (...) {
        unsigned long lr;
        __asm__("mov %0, x30\n\t"
                : "=r" (lr));
        LOG(INFO) << __FUNCTION__ << " In Catch: {assemble manual LR: " << android::base::StringPrintf("0x%x", lr)
                  << ", __builtin_return_address(0): " << android::base::StringPrintf("0x%x", __builtin_return_address(0)) <<"}";
    }
}

void funcB() {
    unsigned long pc;
    __asm__("adr %0, pc_plus_4_2\n\t"
            "pc_plus_4_2:\n\t"
            "nop\n\t"
            : "=r" (pc));
    LOG(INFO) << __FUNCTION__ << " {pc: " << android::base::StringPrintf("0x%x", pc)
              << ", __builtin_return_address(0): " << android::base::StringPrintf("0x%x", __builtin_return_address(0)) <<"}";
    funcA();
}

void funcC() {
    unsigned long pc;
    __asm__("adr %0, pc_plus_4_3\n\t"
            "pc_plus_4_3:\n\t"
            "nop\n\t"
            : "=r" (pc));
    LOG(INFO) << __FUNCTION__ << " {pc: " << android::base::StringPrintf("0x%x", pc)
              << ", __builtin_return_address(0): " << android::base::StringPrintf("0x%x", __builtin_return_address(0)) <<"}";
    funcB();
}

void test_assemble() {
    LOG(INFO) << __FUNCTION__ ;
    funcC();
}
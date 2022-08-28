//
// Created by wyc on 2022/8/20.
//

#pragma once
#include "base/debug_stack.h"

using StackRefCounter = art::DebugStackRefCounterImpl<true>;
using StackReference = art::DebugStackReferenceImpl<true>;
using StackIndirectTopRef = art::DebugStackIndirectTopRefImpl<true>;

inline void test_debug_stack() {
    StackRefCounter* counter = new StackRefCounter();
    StackReference ref1(counter);
    StackReference ref2 = ref1;
    LOG(INFO) << "StackReference aimed to use reference like a stack! eg.";
    LOG(INFO) << "Now, ref1.CheckTop() will Abort";
    LOG(INFO) << "Must use like a stack: first Destruct ref2, then ref1";
    ref2.~StackReference();
    LOG(INFO) << "Now, re1.CheckTop() is normal!";
    ref1.CheckTop();
    LOG(INFO) <<  "StackIndirectTopRef is similar!";
}

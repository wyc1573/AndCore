//
// Created by wyc on 2022/8/27.
//

#pragma once

#include "base/atomic_stack.h"

inline void test_atomic_stack() {
    using namespace art::gc::accounting;
    AtomicStack<int>* stack = AtomicStack<int>::Create("Atomic", 10, 10);
    LOG(INFO) << "begin_: " << stack->Begin() << ", capacity_: " << stack->Capacity()
    << ", end_: " << stack->End() << ", size: " << stack->Size();
    int one = 1, two = 2;
    stack->PushBack(one);
    stack->PushBack(two);
    LOG(INFO) << "PopFront: " << stack->PopFront() << ", PopBack: "
    << stack->PopBack() << ",  IsEmpty: " <<  stack->IsEmpty();
    stack->PushBack(one);
    stack->PushBack(two);
    LOG(INFO) << "Contains(one): " << stack->Contains(one);
    stack->PopBackCount(2);
    LOG(INFO) << "After PopBackCount(2), IsEmpty: " <<  stack->IsEmpty();

    int **begin, **end;
    stack->AtomicPushBack(1);
    stack->AtomicPushBackIgnoreGrowthLimit(2);
    stack->AtomicBumpBack(2, begin, end);
    LOG(INFO) << "AtomicBumpBack: begin: " << begin << ", end: " << end;
}
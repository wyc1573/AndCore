//
// Created by wyc on 2022/8/27.
//

#pragma once

#include <string>
#include "gc/space_bitmap.h"
#include "gc/space_bitmap-inl.h"
#include "base/globals.h"
#include "base/utils.h"

inline void test_space_bitmap() {
    using namespace art::gc::accounting;
    size_t MB = 1024 * 1024;
    size_t GB = 1024 * MB;
    SpaceBitmap<art::kObjectAlignment>* space_bitmap = SpaceBitmap<art::kObjectAlignment>::Create("space-bitmap", 0, 4*GB);
    LOG(INFO) << "Begin: " << space_bitmap->Begin();
    LOG(INFO) << "Size: " << space_bitmap->Size();
    LOG(INFO) << "ComputeBitmapSize(4*GB): " << art::PrettySize(space_bitmap->ComputeBitmapSize(4*GB));
    LOG(INFO) << "ComputeHeapSize(10*MB): " << art::PrettySize(space_bitmap->ComputeHeapSize(10*MB));
    space_bitmap->Set(0);
    space_bitmap->Set((void*)0xffffffff);
    LOG(INFO) << "After Set(0), AtomicTestAndSet(0): " << space_bitmap->AtomicTestAndSet(0);
    space_bitmap->SetHeapLimit(1024);
    //  space_bitmap->AtomicTestAndSet((void*)0xffffffff); //Abort
    LOG(INFO) << "HeapBegin: " << space_bitmap->HeapBegin() << ", HeapLimit: " << space_bitmap->HeapLimit()
    << ", HeapSize: " << space_bitmap->HeapSize();
    LOG(INFO) << "Test(0): " << space_bitmap->Test(0);
    space_bitmap->Set((void*) 0x120);
    LOG(INFO) << "After Set((void*) 0x120), HasAddress((void*)0x120)" << space_bitmap->HasAddress((void*)0x120);
    space_bitmap->VisitAllMarked([](void* ptr) {
        LOG(INFO) << "VisitAllMarked: " << ptr;
    });
    space_bitmap->VisitRange(0, 24, [](void* ptr) {
        LOG(INFO) << "VisitRange [0-24]: " << ptr;
    });
    LOG(INFO) << "IndexToOffset(10): " << space_bitmap->IndexToOffset(10);
    LOG(INFO) << "OffsetToIndex(16): " << space_bitmap->OffsetToIndex(16);
    LOG(INFO) << "OffsetBitIndex(16): " << space_bitmap->OffsetBitIndex(16);
    space_bitmap->Clear(0);
    space_bitmap->ClearRange(0, (void*)8);
    space_bitmap->Clear();
    LOG(INFO) << "After Clear, Test(0): " << space_bitmap->Test(0);
}


//
// Created by wyc on 2022/8/27.
//

#pragma once

#include "gc/bitmap.h"
#include "gc/bitmap-inl.h"

inline void test_bitmap() {
    using namespace art::gc::accounting;
    Bitmap* bitmap = Bitmap::Create("live-bitmap", 1024);
    LOG(INFO) << "begin_: " << bitmap->Begin();
    bitmap->SetBit(1);
    LOG(INFO) << "SetBit(1), then TestBit(1): " << bitmap->TestBit(1);
    bitmap->ClearBit(1);
    LOG(INFO) << "ClearBit(1), then TestBit(1): " << bitmap->TestBit(1);

    bitmap->SetBit(1);
    bitmap->SetBit(3);
//    bitmap->CheckValidBitIndex(1024);
    bitmap->AtomicTestAndSetBit(1023);
    bitmap->VisitSetBits(0, 1024, [](uintptr_t val) {
        LOG(INFO) << "VisitSetBits- " << val;
    });

}

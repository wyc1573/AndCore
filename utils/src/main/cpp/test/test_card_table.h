//
// Created by wyc on 2022/8/28.
//

#pragma once

#include "gc/card_table.h"
#include "base/utils.h"
#include "base/globals.h"
#include "gc/card_table-inl.h"
#include "gc/space_bitmap.h"
#include "gc/space_bitmap-inl.h"
#include "android-base/stringprintf.h"

inline void test_card_table() {
    using namespace art::gc::accounting;
    using namespace android::base;
    size_t MB = 1024 * 1024;
    size_t GB = 1024 * MB;
    CardTable* card_table = CardTable::Create((uint8_t*)GB, 2*GB);
    LOG(INFO) << "GetBiasedBegin: " << StringPrintf("%p", card_table->GetBiasedBegin());
    LOG(INFO) << "MemMapBegin: " << StringPrintf("%p", card_table->MemMapBegin());
    LOG(INFO) << "MemMapSize: " << art::PrettySize(card_table->MemMapSize());
    LOG(INFO) << "AddrIsInCardTable((void*)GB): " << card_table->AddrIsInCardTable((void*)GB);
    auto ptr = card_table->CardFromAddr((void*)GB);
    LOG(INFO) << "CardFromAddr((void*)GB): " << StringPrintf("%p", ptr);
    LOG(INFO) << "CardFromAddr((void*)(GB+1024)): " << StringPrintf("%p", card_table->CardFromAddr((void*)(GB + 1024)));
    LOG(INFO) << "AddrFromCard(ptr): " << StringPrintf("%p", card_table->AddrFromCard(ptr));
    LOG(INFO) << "IsClean(GB): " << card_table->IsClean((void*) GB);
    card_table->MarkCard((void*) GB);
    LOG(INFO) << "MarkCard((void*)GB): ";
    LOG(INFO) << "IsClean(GB): " << card_table->IsClean((void*) GB);
    LOG(INFO) << "IsClean((void*)(GB+128): " << card_table->IsClean((void*)(GB+128));
    LOG(INFO) << "GetCard((void*)GB): " << StringPrintf("0x%x, aka kDirty", card_table->GetCard((void*)GB));
    art::gc::AgeCardVisitor age_card_visitor;
    card_table->ModifyCardsAtomic((uint8_t*)GB, (uint8_t*)(2 * GB), age_card_visitor, [](uint8_t*, uint8_t, uint8_t) {});

    SpaceBitmap<8>* space_bitmap = SpaceBitmap<8>::Create("space-bitmap", (uint8_t*)GB, GB);
    card_table->Scan<false>(space_bitmap, (uint8_t*)GB,  (uint8_t*)(GB + 10 * MB), [](void*) {});
    card_table->ClearCardTable();
}

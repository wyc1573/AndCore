//
// Created by wyc on 2022/8/20.
//

#pragma once

#include "base/bit_table.h"
#include "base/malloc_arena_pool.h"

class InlineInfo : public art::BitTableAccessor<3> {
public:
    BIT_TABLE_HEADER(InlineInfo)
    BIT_TABLE_COLUMN(0, IsLast)  // Determines if there are further rows for further depths.
    BIT_TABLE_COLUMN(1, DexPc)
    BIT_TABLE_COLUMN(2, MethodInfoIndex)
    // ............
};

class StackMap : public art::BitTableAccessor<3> {
public:
    BIT_TABLE_HEADER(StackMap)
    BIT_TABLE_COLUMN(0, Kind)
    BIT_TABLE_COLUMN(1, PackedNativePc)
    BIT_TABLE_COLUMN(2, DexPc)
    // ....
};

inline void test_bit_table() {
    using namespace art;

    MallocArenaPool pool;
    ArenaStack arena_stack(&pool);
    ScopedArenaAllocator allocator(&arena_stack);
    BitTableBuilder<InlineInfo> bit_table_builder(&allocator);

    BitTableBuilder<InlineInfo>::Entry entry1{1,2,3};
    BitTableBuilder<InlineInfo>::Entry entry2{3,2,1};
    BitTableBuilder<InlineInfo>::Entry entry3{1,2,3};
    BitTableBuilder<InlineInfo>::Entry entry4{1,2,3};
    bit_table_builder.Add(entry1);
    bit_table_builder.Add(entry2);
    bit_table_builder.Dedup(entry3);
    bit_table_builder.Dedup(entry4);

    ScopedArenaVector<uint8_t> buffer(allocator.Adapter(kArenaAllocStackMapStream));
    BitMemoryWriter<ScopedArenaVector<uint8_t>> out(&buffer);
    bit_table_builder.Encode(out);

    BitMemoryReader reader(out.data());
    BitTable<InlineInfo> bit_table;
    bit_table.Decode(reader);

    LOG(INFO) << "====== BitTable-1 ======";
    LOG(INFO) << "GetName: " << bit_table.GetName();
    const char* const * col_names = bit_table.GetColumnNames();
    LOG(INFO) << "Column: " << col_names[0] << " " << col_names[1] << " " << col_names[2];
    LOG(INFO) << "NumColumns: " << bit_table.NumColumns();
    LOG(INFO) << "NumRows: " << bit_table.NumRows();
    LOG(INFO) << "NumRowBits: " << bit_table.NumRowBits();
    auto begin = bit_table.begin();
    LOG(INFO) << "Row-0: " << begin[0].GetIsLast() << " " << begin[0].GetDexPc() << " " << begin[0].GetMethodInfoIndex();
    LOG(INFO) << "Row-1: " << begin[1].GetIsLast() << " " << begin[1].GetDexPc() << " " << begin[1].GetMethodInfoIndex();

    BitTableBuilder<StackMap> bit_table_builder2(&allocator);
    BitTableBuilder<StackMap>::Entry entry21{100, 101, 102};
    bit_table_builder2.Add(entry21);
    ScopedArenaVector<uint8_t> buffer2(allocator.Adapter(kArenaAllocStackMapStream));
    BitMemoryWriter<ScopedArenaVector<uint8_t>> out2(&buffer);
    bit_table_builder2.Encode(out2);

    BitMemoryReader reader2(out2.data());
    BitTable<StackMap> bit_table2;
    bit_table2.Decode(reader2);
    LOG(INFO) << "====== BitTable-2 ======";
    LOG(INFO) << "GetName: " << bit_table2.GetName();
    const char* const * col_names2 = bit_table2.GetColumnNames();
    LOG(INFO) << "Column: " << col_names2[0] << " " << col_names2[1] << " " << col_names2[2];
    LOG(INFO) << "NumColumns: " << bit_table2.NumColumns();
    LOG(INFO) << "NumRows: " << bit_table2.NumRows();
    LOG(INFO) << "NumRowBits: " << bit_table2.NumRowBits();
    LOG(INFO) << "Row-0: " << bit_table2.Get(0,0) << " " << bit_table2.Get(0,1) << " " << bit_table2.Get(0, 2);
}

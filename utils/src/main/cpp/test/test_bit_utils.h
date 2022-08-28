//
// Created by wyc on 2022/8/20.
//

#pragma once
#include "base/bit_utils.h"
#include "android-base/stringprintf.h"

#define TEST(func) LOG(INFO) << #func << " -> "
#define TEST_F(fmt, func) LOG(INFO) << #func << " -> " << StringPrintf(fmt, func)

inline void test_bitutils() {
    using namespace art;
    using namespace android::base;

    TEST(BitSizeOf<int>());
    TEST(BitSizeOf<uint8_t>());
    TEST(BitSizeOf<int64_t>());

    uint64_t u64_var = 0;
    TEST(BitSizeOf(u64_var));

    uint64_t u64_0x00010 = 0x00010;
    uint8_t u8_0x10 = 0x10;
    TEST(CLZ(u64_0x00010));
    TEST(CLZ(u8_0x10));
    TEST(CTZ(u64_0x00010));
    TEST(CTZ(u8_0x10));

    TEST(POPCOUNT(u8_0x10));

    uint32_t u32_0x01020304 = 0x01020304;
    TEST_F("0x%0x", BSWAP(u32_0x01020304));

    u32_0x01020304 = 0x01020304;
    TEST(MostSignificantBit(u32_0x01020304));
    TEST(LeastSignificantBit(u32_0x01020304));

    TEST(MinimumBitsToStore(7u));

    TEST(RoundUpToPowerOfTwo(7u));
    TEST(RoundUpToPowerOfTwo(1u));
    TEST(TruncToPowerOfTwo(9u));

    TEST(IsPowerOfTwo(-2));
    TEST(IsPowerOfTwo(3));

    TEST(WhichPowerOf2(1));
    TEST(WhichPowerOf2(8));

    TEST(RoundDown(2000, 1024));
    TEST(RoundDown(1023, 1024));

    TEST(RoundUp(0, 1024));
    TEST(RoundUp(1, 1024));
    TEST(RoundUp(1025, 1024));

    TEST(AlignDown((void*)0x12345678, 1024));

    TEST(IsAligned<512>(1024));
    TEST(IsAligned<1024>(1024));
    TEST(IsAligned<2048>(1024));

    TEST(IsAlignedParam(1024, 512));
    TEST(IsAlignedParam((void*)1024, 512));

    uint32_t u32_0x12345678 = 0x12345678;
    TEST_F("0x%0x (overflow)", Low16Bits(u32_0x12345678));
    TEST_F("0x%0x", High16Bits(u32_0x12345678));

    uint64_t u64_0x0102030405 = 0x0102030405;
    TEST_F("0x%0x", Low32Bits(u64_0x0102030405));
    TEST_F("0x%0x", High32Bits(u64_0x0102030405));

    TEST(IsInt(10, 1024));
    TEST(IsInt(10, 512));
    TEST(IsInt<10>(256));
    TEST(IsUint<10>(1023));

    TEST(IsAbsoluteUint<10>(-256));
    TEST(IsAbsoluteUint<10>(-512));

    TEST(GetIntLimit<int>(8));

    TEST(MaxInt<int>(10));
    TEST(MaxInt<unsigned>(10));

    TEST(MinInt<int>(10));
    TEST(MinInt<unsigned>(10));

    TEST_F("0x%0x", LowestOneBitValue<int>(0x04030201));
    TEST_F("0x%0x", HighestOneBitValue(0x01020304));

    TEST_F("0x%0x", (Rot<int , true>(0x01020304, 8)));
    TEST_F("0x%0x", (Rot<int , false>(0x01020304, 8)));

    TEST_F("0x%0x", ReverseBits32(0xffff0001u));
    TEST_F("0x%0lx", ReverseBits64(0xffffffff00000001u));

    TEST_F("0x%0lx", MaskLeastSignificant(6));

    TEST_F("0x%0x", BitFieldClear(0xffffffff, 4, 8));
//    LOG(INFO) << "BitFieldInsert(0, 0xffff, 4, 6) -> %0x" << BitFieldInsert(0, 0xffff, 4, 16);

    TEST_F("0x%0x (sign-extended)", BitFieldExtract(0x01020f04, 4, 8));
    TEST_F("0x%0x", BitFieldExtract(0x01020f04u, 4, 8));

    TEST(BitsToBytesRoundUp(15));
}
#undef TEST
#undef TEST_F

//
// Created by wyc on 2022/8/20.
//

#pragma once

#include "base/casts.h"
#include "android-base/logging.h"
#include "android-base/stringprintf.h"

class SuperFoo {};
class Foo : public SuperFoo {};
class Foo2 : public SuperFoo {};
class Bar {};

enum FooEnum : uint8_t {
    ONE,
    TWO,
    THREE
};

#define TEST(func) LOG(INFO) << #func << " -> " << func;

inline void test_casts() {
    using namespace art;
    using namespace android::base;
    Foo* pointer_to_foo = new Foo();
    Foo2* pointer_to_foo2 = new Foo2();
    SuperFoo* pointer_to_super_foo = new SuperFoo();
    Bar* pointer_to_bar;
    TEST(static_cast<Foo*>(pointer_to_super_foo));
    TEST(static_cast<Foo*>(pointer_to_super_foo));
    TEST(static_cast<Foo*>(pointer_to_super_foo));
    TEST(static_cast<Foo2*>(pointer_to_super_foo));
    TEST(static_cast<SuperFoo*>(pointer_to_foo));
    TEST(static_cast<SuperFoo*>(pointer_to_foo2));

    LOG(INFO) << "static_cast<Bar*>(pointer_to_foo) -> compile error!";
    LOG(INFO) << "(Bar*)(pointer_to_super_foo) -> compile OK! but dangerous";
    LOG(INFO) << "reinterpret_cast<Bar*>(pointer_to_foo) -> compile OK";
//    static_cast<Bar*>(pointer_to_foo);
    pointer_to_bar = reinterpret_cast<Bar*>(pointer_to_foo);
    pointer_to_bar = (Bar*)(pointer_to_super_foo);

    LOG(INFO) << "pointer_to_super_foo = implicit_cast<SuperFoo*>(pointer_to_foo) OK";
    pointer_to_super_foo = implicit_cast<SuperFoo*>(pointer_to_foo);
    LOG(INFO) << "pointer_to_foo = implicit_cast<Foo*>(pointer_to_super_foo) -> compile error!";
//    pointer_to_foo = implicit_cast<Foo*>(pointer_to_super_foo);

    TEST(down_cast<Foo*>(pointer_to_super_foo));
    TEST(down_cast<Foo2*>(pointer_to_super_foo));
    LOG(INFO) << "down_cast<void*>(pointer_to_super_foo) -> compile error!";
//    down_cast<void*>(pointer_to_super_foo);

    Foo foo;
    SuperFoo super_foo = foo;
    down_cast<Foo&>(super_foo);

    int64_t i64_neg_one = -1;
    TEST(bit_cast<uint64_t>(i64_neg_one));

    int64_t i64_0xffffff = 0xffffff;
//    dchecked_integral_cast<int8_t>(0xffffff);
    LOG(INFO) << "dchecked_integral_cast<int8_t>(0xffffff) -> runtime error!";

    TEST(enum_cast<FooEnum>(1));
    TEST(enum_cast<int>(FooEnum::ONE));
    LOG(INFO) << "enum_cast<FooEnum>(256) -> runtime error! because underlying type is uint8_t (overflow)";
    // enum_cast<FooEnum>(256);

    LOG(INFO) << "reinterpret_cast<uint32_t>(i32) is not allowed (signed -> unsigned)";
    LOG(INFO) << "reinterpret_cast<int64_t>(i32) is not allowed!";
#ifdef __aarch64__
    TEST(reinterpret_cast64<void*>(i64_neg_one));
#endif
    TEST(reinterpret_cast64<int64_t>((void*)0xffffffffffffffff));
    int32_t i32_neg_one = -1;
    TEST(reinterpret_cast32<void*>(i32_neg_one));
    TEST(reinterpret_cast32<int32_t>((void*)0xffffffff));

}
#undef TEST

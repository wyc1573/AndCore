//
// Created by wyc on 2023/3/12.
//

#include "cpp_sample.h"
#include <stdarg.h>
#include <new>
#include "base/utils.h"
#include "base/obj_ptr.h"
#include "base/obj_ptr-inl.h"
#include "android-base/stringprintf.h"

void test_va(int num, ...) {
    va_list  ap;
    va_start(ap, num);
    LOG(INFO) << __FUNCTION__  << "args = {";
    for (int i = 0; i < num; i++) {
        LOG(INFO) << va_arg(ap, int);
    }
    LOG(INFO) << "}";
}

class Base {
public:
    int m = 0;
protected:
    void testUse() {
        LOG(INFO) << "Base::testUse";
    }
};

class Derived : public Base {
public:
    using Base::Base;
    using Base::testUse;
};

void test_objPtr() {
    Derived* derived = new Derived();
    art::ObjPtr<Base> objPtr(derived);
    LOG(INFO) << "objPtr.PtrUnchecked = " << android::base::StringPrintf("0x%lx", objPtr.Ptr());
    LOG(INFO) << "objPtr->m = " << objPtr->m;
    objPtr.Assign(0);
    LOG(INFO) << "objPtr.IsNull = " << objPtr.IsNull();

    Base* base = new Derived();
    objPtr.DownCast(base);
}

class A {
    int mem;
public:
    A() {
        LOG(INFO) << "A::A()";
    }

    A(const A& a) {
        LOG(INFO) << "A::A(A&)";
    }

    A(const A&& a) {
        LOG(INFO) << "A::A(A&&)";
    }

    ~A() {
        LOG(INFO) << "~A::A()";
    }

    void* operator new(std::size_t sz)
    {
        LOG(INFO) << "custom new for size " << sz;
        return ::operator new(sz);
    }

    void* operator new(std::size_t size, std::align_val_t align) {
        LOG(INFO) << "custom new for size = " << size << ", align = " << static_cast<int>(align);
        return ::operator new(size, align);
    }

    void* operator new(std::size_t sz, void* p) {
        LOG(INFO) << "custom new placement sz = " << sz << ", p = " << p;
        return ::operator new(sz, p);
    }

    void* operator new[](std::size_t sz) {
        LOG(INFO) << "custom new[] for size " << sz;
        return ::operator new[](sz);
    }

    void operator delete(void* ptr) noexcept
    {
        LOG(INFO) << "custom delete ptr = " << ptr;
        std::free(ptr);
    }

    void operator delete(void* ptr, std::size_t size) noexcept
    {
        LOG(INFO) <<  "custom delete ptr = " << ptr << ", size = " << size;
        std::free(ptr);
    }

    void operator delete[](void* ptr) noexcept
    {
        LOG(INFO) << "custom delete[]  ptr = " << ptr;
        std::free(ptr);
    }

    void operator delete[](void* ptr, std::size_t size) noexcept
    {
        LOG(INFO) << "custom delete[] ptr = " << ptr << ", size = " << size;
        std::free(ptr);
    }
};


void test_construct() {
    LOG(INFO) << __FUNCTION__ ;
    A* newA = new A;
    LOG(INFO) << "new A return " << newA;
    A* alignA = new (std::align_val_t(16)) A();
    LOG(INFO) << "align new return " << alignA;
    delete alignA;
    void* buff = malloc(128);
    LOG(INFO) << "buff = " << buff;
    A* aPlacement = new (buff) A();
    A* aNewArr = new A[10];
    LOG(INFO) << "placement new return " << aPlacement;
    delete aPlacement;
    delete[] aNewArr;
}


void cpp_misc() {
    test_va(3, 1, 2, 3);
    test_objPtr();
    test_construct();
}

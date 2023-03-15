//
// Created by wyc on 2023/3/12.
//

#include "cpp_sample.h"
#include <stdarg.h>
#include <new>
#include <thread>
#include <sys/syscall.h>
#include <linux/futex.h>
#include <unistd.h>
#include <time.h>
#include "base/utils.h"
#include "base/obj_ptr.h"
#include "base/obj_ptr-inl.h"
#include "android-base/stringprintf.h"
#include "base/futextest.h"


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

int futex_var_ = 0;
int futex_var2_ = 0;

void test_futex_wait() {
    syscall(SYS_futex, &futex_var_, FUTEX_WAIT_PRIVATE, 0, nullptr, nullptr, 0);
    LOG(INFO) << "test_futex_wait end";
    struct timespec timespec;
    timespec.tv_nsec = 0;
    timespec.tv_sec = 2;
    syscall(SYS_futex, &futex_var_, FUTEX_WAIT_PRIVATE, 0, &timespec, nullptr, 0);
    LOG(INFO) << "test_futex_wait timeout relative end";
    clock_gettime(CLOCK_REALTIME , &timespec);
    timespec.tv_sec += 1;
    syscall(SYS_futex, &futex_var_, FUTEX_WAIT_BITSET | FUTEX_CLOCK_REALTIME , 0, &timespec, nullptr, FUTEX_BITSET_MATCH_ANY);
    LOG(INFO) << "test_futex_wait timeout absolute end";
}

void test_futex_wake() {
    sleep(1);
    syscall(SYS_futex, &futex_var_, FUTEX_WAKE_PRIVATE, 1, nullptr, nullptr, 0);
    LOG(INFO) << "test_futex_wake end";
}


void test_futex_cmp_requeue() {
    LOG(INFO) << "start wait futex_var_ ...";
    syscall(SYS_futex, &futex_var_, FUTEX_WAIT_PRIVATE, 0, nullptr, nullptr, 0);
}

void cpp_misc() {
    test_va(3, 1, 2, 3);
    test_objPtr();
    test_construct();
    std::thread t(test_futex_wait);
    std::thread t2(test_futex_wake);
    t.join();
    t2.join();

    std::thread tArr[10];
    for (int i = 0; i < 10; i++) {
        tArr[i] = std::thread(test_futex_cmp_requeue);
    }
    sleep(1);

//  防止惊群效应, FUTEX_CMP_REQUEUE_PRIVATE必须相对应
//    syscall(SYS_futex, &futex_var_, FUTEX_CMP_REQUEUE_PRIVATE, 1, INT_MAX, &futex_var_, 0);
    for (int i = 0; i < 10; i++) {
        tArr[i].join();
    }

}

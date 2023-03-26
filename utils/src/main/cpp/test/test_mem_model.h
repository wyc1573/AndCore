//
// Created by wyc on 2023/3/20.
//
#pragma once

#include <atomic>
#include <future>
#include <thread>

//===============================================================

int computation(int);
std::atomic<int> arr[3] = { -1, -1, -1 };
int data[1000] = {0}; // 非原子数据

void WorkerA( int v0, int v1, int v2 )
{
    //assert( 0 <= v0, v1, v2 < 1000 );
    data[v0] = computation(v0);
    data[v1] = computation(v1);
    data[v2] = computation(v2);
//    std::atomic_thread_fence(std::memory_order_release);

    arr[0].store(v0, std::memory_order_relaxed);
    arr[1].store(v1, std::memory_order_relaxed);
    arr[2].store(v2, std::memory_order_relaxed);
}

void WorkerB()
{
    int v0 = arr[0].load(std::memory_order_relaxed);
    int v1 = arr[1].load(std::memory_order_relaxed);
    int v2 = arr[2].load(std::memory_order_relaxed);
//    std::atomic_thread_fence(std::memory_order_acquire);
    // v0 、 v1 、 v2 可能全部或部分结果为 -1 。
    // 其他情况下读取非原子数据是安全的，因为栅栏：
    if (v0 != -1 || v1 != -1 || v2 != -1) {
        assert(data[v0] != 0 && data[v1] != 0 && data[v2] != 0);
    }
}

int computation(int v) {
    return v;
}
//=================Release-Acquire ordering=======================
std::atomic<std::string*> ptr;
int var;

void producer()
{
    std::string* p  = new std::string("Hello");
    var = 42;
    ptr.store(p, std::memory_order_release);
}

void consumer()
{
    std::string* p2;
    while (!(p2 = ptr.load(std::memory_order_acquire)));
    assert(*p2 == "Hello"); // never fires
    assert(var == 42); // never fires
}
//===========================================================//
inline void test_vis() {
    while (true) {
        LOG(INFO) << "start test visibility...";
        std::thread threadA(WorkerA, 1, 2 ,3);
        std::thread threadB(WorkerB);
        usleep(1000);
        threadA.join();
        threadB.join();
    }
}
//============================================================
std::atomic<int> a = 0;
std::atomic<int> b = 0;
// atomic_signal_fence针对同一个线程
extern "C" void handler(int) {
    if (1 == a.load(std::memory_order_relaxed)) {
        std::atomic_signal_fence(std::memory_order_acquire);
        assert(1 == b.load(std::memory_order_relaxed));
    }
}

void test_atomic_signal_fence() {
    signal(SIGTERM, &handler);
    b.store(1, std::memory_order_relaxed);
    std::atomic_signal_fence(std::memory_order_release);
    a.store(1, std::memory_order_relaxed);
}
//=============================================================

/**
 * The specification of release-consume ordering is being revised, and the use of memory_order_consume is temporarily discouraged.
 * (since C++17)
 * */

std::atomic<std::string*> ptr2;
int data2;

void producer2()
{
    std::string* p  = new std::string("Hello");
    data2 = 42;
    ptr.store(p, std::memory_order_release);
}

void consumer2()
{
    std::string* p2;
    while (!(p2 = ptr.load(std::memory_order_consume)));
    assert(*p2 == "Hello"); // never fires: *p2 carries dependency from ptr
    assert(data2 == 42);    // may or may not fire: data does not carry dependency from ptr
}
//====================Sequentially-consistent ordering=================================
std::atomic<bool> x = {false};
std::atomic<bool> y = {false};
std::atomic<int> z = {0};

void write_x()
{
    x.store(true, std::memory_order_seq_cst);
}

void write_y()
{
    y.store(true, std::memory_order_seq_cst);
}

void read_x_then_y()
{
    while (!x.load(std::memory_order_seq_cst))
        ;
    if (y.load(std::memory_order_seq_cst)) {
        ++z;
    }
}

void read_y_then_x()
{
    while (!y.load(std::memory_order_seq_cst))
        ;
    if (x.load(std::memory_order_seq_cst)) {
        ++z;
    }
}

int test_cst()
{
    std::thread a(write_x);
    std::thread b(write_y);
    std::thread c(read_x_then_y);
    std::thread d(read_y_then_x);
    a.join(); b.join(); c.join(); d.join();
    assert(z.load() != 0);  // will never happen
}
//============================================================================

constexpr bool FALSE_SHARING = false;
constexpr size_t CACHE_LINE_SIZE = 64,
        SECOND_ALIGN_SIZE = FALSE_SHARING ? sizeof(int) : CACHE_LINE_SIZE;
//设置线程A B分别运行在哪个core上
constexpr int A_THREAD_CORE = 0, B_THREAD_CORE = 7;
constexpr size_t CPU_NUM = 8;

struct AlignStruct
{
    std::atomic<int> a alignas(CACHE_LINE_SIZE);
    std::atomic<int> b alignas(SECOND_ALIGN_SIZE);
};

struct AlignStructNormal
{
    int a alignas(CACHE_LINE_SIZE);
    int b alignas(SECOND_ALIGN_SIZE);
};

void set_affinity(int nr);

inline void test_false_sharing() {
    AlignStruct alignStruct;
    LOG(INFO) << "size of a " << sizeof(std::atomic<int>);
    LOG(INFO) << "size of b " << sizeof(alignStruct.b);
    auto runnable = [](std::atomic<int> *val, int nr, const char* tag)
    {
        set_affinity(nr); // 设置线程运行在哪个core上
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
        for(size_t r = 100'000'000; r--;)
            ++*val;
        std::chrono::steady_clock::time_point last = std::chrono::steady_clock::now();
        auto dt = last - start;
        LOG(INFO) << tag << " - spent: " << dt.count() << " ns";
    };
    std::thread threadA(runnable, &alignStruct.a, A_THREAD_CORE, "thread-a");
    std::thread threadB(runnable, &alignStruct.b, B_THREAD_CORE, "thread-b");
    threadA.join();
    threadB.join();
}


void set_affinity(int nr) {
    cpu_set_t *cpusetp;
    size_t size;

    cpusetp = CPU_ALLOC(CPU_NUM);
    if (cpusetp == NULL) {
        perror("CPU_ALLOC");
        exit(EXIT_FAILURE);
    }

    size = CPU_ALLOC_SIZE(CPU_NUM);

    if (nr >= size) {
        perror("nr >= size");
        exit(EXIT_FAILURE);
    }

    CPU_ZERO_S(size, cpusetp);
    CPU_SET_S(nr, size, cpusetp);

    LOG(INFO) << "CPU_COUNT() of set: " << CPU_COUNT_S(size, cpusetp) << ", nr = " << nr;

    sched_setaffinity(gettid(), size, cpusetp);
    CPU_FREE(cpusetp);
}

void test_cpp_concurrency() {
    test_vis();
}


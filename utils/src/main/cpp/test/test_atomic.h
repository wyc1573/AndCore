//
// Created by wyc on 2022/8/6.
//

#pragma once

#include "base/atomic.h"

template <class T>
struct ListNode {
    T val_;
    ListNode* next_;
    ListNode() = default;
    ListNode(T val, ListNode* next) : val_(val), next_(next){}
};

template <class T>
class List {
public:
    List() : head_(nullptr) {}

    void Insert(T val) {
        ListNode<T>* old_head = head_;
        ListNode<T>* new_node = new ListNode(val, old_head);
        while (!std::atomic_compare_exchange_weak(old_head, new_node)) {
            new_node->next_ = old_head;
        }
    }

    void Dump() {

    }
private:
    ListNode<T>* head_;
};

void test_atomic() {
    using namespace art;
    AtomicInteger atomic_integer;
    atomic_integer.StoreJavaData(1);
    LOG(INFO) << "AtomicInteger: call StoreJavaData(1), and LoadJavaData = " << atomic_integer.LoadJavaData();
    atomic_integer.CompareAndSet(1, 2, CASMode::kStrong, std::memory_order_relaxed);
    LOG(INFO) << "After CompareAndSet(1, 2, CASMode::kStrong, std::memory_order_relaxed).";
    LOG(INFO) << "Now, load(std::memory_order_relaxed) = " << atomic_integer.load(std::memory_order_relaxed);
    int32_t expected = 1;
    bool ret = atomic_integer.CompareAndSetWeakRelaxed(expected, 3);
    LOG(INFO) << "Call CompareAndSetWeakRelaxed(expected = 1, 3) return " << (ret ? "true" : "false.");
    LOG(INFO) << "Now, load(std::memory_order_relaxed) = " << atomic_integer.load(std::memory_order_relaxed) << ", expected = " << expected;
    std::atomic<int32_t> atomic32;

    atomic32.store(2);
    ret = atomic32.compare_exchange_weak(expected, 3);
    LOG(INFO) << "Test Raw std::atomic, witch init value = 2, expected = 1.";
    LOG(INFO) << "Call compare_exchange_weak(expected = 1, 3) return " << (ret ? "true" : "false") << ", expected = " << expected;
    LOG(INFO) << "So, std::atomic modify expected, but AtomicInteger not.";

    LOG(INFO) << "AtomicInteger与std::atomic不等价! 实现无锁链表时必须注意这个区别";
}

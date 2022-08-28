//
// Created by wyc on 2022/8/20.
//

#pragma once
#include "base/thread_pool.h"
class AndCoreTask : public art::SelfDeletingTask {
public:
    void Run() {
        LOG(INFO) << "Task Running ...";
    }
};

inline void test_thread_pool() {
    using namespace art;
    ThreadPool thread_pool("AndCore-Thread-Pool", 3, 16*1024);
    auto workers = thread_pool.GetWorkers();
    LOG(INFO) << "workers size: " << workers.size();
    thread_pool.AddTask(new AndCoreTask());
    LOG(INFO) << "GetTaskCount: " << thread_pool.GetTaskCount();
    thread_pool.SetPthreadPriority(9);  // set nice
    thread_pool.SetMaxActiveWorkers(1);
    thread_pool.StartWorkers();
    // 等待所有任务完成
    thread_pool.Wait(false, false);
}

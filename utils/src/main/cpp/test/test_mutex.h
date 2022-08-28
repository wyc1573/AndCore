//
// Created by wyc on 2022/8/20.
//

#pragma once

#include <thread>
#include "base/mutex.h"
#include "base/mutex-inl.h"
#include "base/time_utils.h"


inline void test_mutex() {
    using namespace art;
    Mutex mutex("AndCore-Mutex");
    Mutex mutex_recursive("AndCore-Recur-Mutex", art::LockLevel::kDefaultMutexLevel, true);

    mutex.Lock();
    LOG(INFO) << "GetName: " << mutex.GetName();
    LOG(INFO) << "GetDepth: " << mutex.GetDepth();
    LOG(INFO) << "TryLock: " << mutex.TryLock();
    LOG(INFO) << "ExclusiveTryLock: " << mutex.ExclusiveTryLock();
    LOG(INFO) << "IsExclusiveHeld: " << mutex.IsExclusiveHeld();
    LOG(INFO) << "Mutex Dump: ";
    mutex.Dump(LOG_STREAM(INFO));
    mutex.Unlock();

    mutex_recursive.ExclusiveLock();
    mutex_recursive.ExclusiveLock();
    LOG(INFO) << "GetName: " << mutex_recursive.GetName();
    LOG(INFO) << "GetDepth： " << mutex_recursive.GetDepth();
    LOG(INFO) << "IsExclusiveHeld: " << mutex_recursive.IsExclusiveHeld();
    LOG(INFO) << "TryLock: " << mutex_recursive.TryLock();
    mutex_recursive.ExclusiveUnlock();
    mutex_recursive.ExclusiveUnlock();

    ReaderWriterMutex rw_mutex("AndCore-RW-Mutex");
    rw_mutex.ReaderLock();
    rw_mutex.SharedLock(); // same as ReaderLock()

    LOG(INFO) << "GetName: " << rw_mutex.GetName();
    LOG(INFO) << "IsReaderWriterMutex: " << rw_mutex.IsReaderWriterMutex();
    LOG(INFO) << "SharedTryLock: " << rw_mutex.SharedTryLock();
    //对应释放3次
    rw_mutex.ReaderUnlock();
    rw_mutex.ReaderUnlock();
    rw_mutex.ReaderUnlock();
    // 死锁
//    rw_mutex.ExclusiveLock();
//    rw_mutex.SharedLock();
    LOG(INFO) << "Dump ReaderWriterMutex:";
    rw_mutex.Dump(LOG_STREAM(INFO));
}

//
// Created by wyc on 2022/8/20.
//

#pragma once
#include "base/time_utils.h"

inline void test_time_utils() {
    using namespace art;
    LOG(INFO) << "PrettyDuration(1000): " << PrettyDuration(1000);
    LOG(INFO) << "PrettyDuration(1000010): " << PrettyDuration(1100);
    LOG(INFO) << "FormatDuration(1000, TimeUnit::kTimeUnitMicrosecond, 1): "
    << FormatDuration(1000, TimeUnit::kTimeUnitMicrosecond, 1);
    LOG(INFO) << "FormatDuration(1000, TimeUnit::kTimeUnitMillisecond, 1): "
    << FormatDuration(1000, TimeUnit::kTimeUnitMillisecond, 1);
    LOG(INFO) << "GetAppropriateTimeUnit(1100) == TimeUnit::kTimeUnitMicrosecond: "
    << (GetAppropriateTimeUnit(1100) == TimeUnit::kTimeUnitMicrosecond ? "true" : "false");
    LOG(INFO) << "GetNsToTimeUnitDivisor(TimeUnit::kTimeUnitMicrosecond): "
    << GetNsToTimeUnitDivisor(TimeUnit::kTimeUnitMicrosecond);
    LOG(INFO) << "GetIsoDate(): " << GetIsoDate();
    LOG(INFO) << "MilliTime(): " << MilliTime();
    LOG(INFO) << "MicroTime(): " << MicroTime();
    LOG(INFO) << "NanoTime(): " << NanoTime();
    LOG(INFO) << "ThreadCpuNanoTime(): " << ThreadCpuNanoTime();
    LOG(INFO) << "ProcessCpuNanoTime(): " << ProcessCpuNanoTime();
    LOG(INFO) << "NsToMs(1000000000): " << NsToMs(1000000000);
    LOG(INFO) << "MsToUs(1100): " << MsToUs(1100);
    LOG(INFO) << "UsToNs(1): " << UsToNs(1);
    timespec tm;
    InitTimeSpec(true, CLOCK_MONOTONIC, 60000, 0, &tm);
    LOG(INFO) << "InitTimeSpec(true, CLOCK_MONOTONIC, 60000, 0, &tm): "
    << ", tm.tv_sec: " << tm.tv_sec << ", tm.tv_nsec: " << tm.tv_nsec;

    timespec tm2;
    InitTimeSpec(false, CLOCK_MONOTONIC, 60000, 0, &tm2);
    LOG(INFO) << "InitTimeSpec(false, CLOCK_REALTIME, 60000, 0, &tm2): "
              << ", tm2.tv_sec: " << tm2.tv_sec << ", tm2.tv_nsec: " << tm2.tv_nsec;

    NanoSleep(1000000);
}

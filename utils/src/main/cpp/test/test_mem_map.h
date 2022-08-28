//
// Created by wyc on 2022/8/20.
//

#pragma once
#include <sys/mman.h>
#include "env.h"
#include "base/mem_map.h"
#include "android-base/stringprintf.h"
#include "unix_file/random_access_file.h"
#include "unix_file/random_access_file_utils.h"

inline void test_mem_map() {
    using namespace art;
    MemMap::Init();
    MemMap mem_map = MemMap::MapAnonymous("AndCore-1", nullptr,
                              1024, MAP_ANONYMOUS, true, false,
                              nullptr, nullptr, true);
    // /proc/self/maps中会用"AndCore-1"命名相应的区域
    LOG(INFO)
    << "After MapAnonymous, Name: " << mem_map.GetName()
    << ", Begin: " << android::base::StringPrintf("%p", mem_map.Begin())
    << ", size: " << mem_map.Size()
    << ", End: " << android::base::StringPrintf("%p", mem_map.End())
    << ", Protect: " << mem_map.GetProtect()
    << ", valid: "<< mem_map.IsValid();

    std::string test_file_path(env_test_path);
    TemporaryFile tmp_file(test_file_path);
    FdFile file(tmp_file.path, O_RDWR, false);
    const char* buff = "hello, world!";
    auto res = file.WriteFully(buff, strlen(buff));


    MemMap file_map = MemMap::MapFileAtAddress(nullptr, 1024, PROT_READ, MAP_SHARED,
                             file.Fd(), 0, true, file.GetPath().c_str(),
                             false, nullptr, nullptr);

    char* begin = (char*) file_map.BaseBegin();
    LOG(INFO) << "After MapFileAtAddress, Read: " << begin;
    res = file.FlushClose();
    UNUSED(res); // suppress compiler warning
}
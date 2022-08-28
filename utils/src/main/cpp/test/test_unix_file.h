//
// Created by wyc on 2022/8/20.
//

#pragma once
#include <string>
#include <unistd.h>
#include "env.h"
#include "unix_file/fd_file.h"
#include "android-base/file.h"
#include "unix_file/random_access_file.h"
#include "unix_file/random_access_file_utils.h"

using namespace unix_file;

inline void test_fd_file() {
    std::string test_file_path(env_test_path);
    TemporaryFile tmp1(test_file_path);
    LOG(INFO) << "TemporaryFile path: " << tmp1.path;
// 方式一
//    int fd = open(tmp_file.path, O_RDWR, 0644);
//    FdFile fd_file(fd, false);
// 方式二
    FdFile file1(tmp1.path, O_RDWR, false);
    LOG(INFO) << "GetPath: " << file1.GetPath() << ", CheckUsage: " << file1.CheckUsage()
        << ", IsOpened: " << file1.IsOpened() << ", Fd: " << file1.Fd();

    const char* buff = "hello, world!";
//    file1.Write(buff, strlen(buff), 0);
    auto res = file1.WriteFully(buff, strlen(buff));
    LOG(INFO) << "After Write [" << buff << "], GetLength: " << file1.GetLength();
    res = file1.Flush();

    char rbuff[128] = {'\0'};
//    lseek(fd_file.Fd(), 0, SEEK_SET);
//    fd_file.ReadFully(rbuff, fd_file.GetLength());
    res = file1.Read(rbuff, file1.GetLength(), 0);
    LOG(INFO) << "ReadFully: " << rbuff;

    res = file1.PreadFully(rbuff, sizeof(rbuff), 0);
    LOG(INFO) << "PreadFully: " << rbuff;
    res = file1.PwriteFully(buff, strlen(buff), strlen(buff));
    LOG(INFO) << "GetLength: " << file1.GetLength();

    TemporaryFile tmp2(test_file_path);
    FdFile file2(tmp2.path, O_RDWR, false);
    bool bres = CopyFile(file1, &file2);
    LOG(INFO) << "After CopyFile(file1, &file2), file2.GetLength: " << file2.GetLength();
    bres = file1.ClearContent();
    res = file1.Close();
    UNUSED(bres); // suppress compiler warning
    UNUSED(res); // suppress compiler warning
}

inline void test_unix_file() {
    test_fd_file();
}

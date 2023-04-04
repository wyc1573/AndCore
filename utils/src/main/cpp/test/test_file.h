//
// Created by wyc on 2022/8/6.
//

#pragma once

#include <fcntl.h>
#include "android-base/logging.h"
#include "android-base/file.h"
#include "android-base/unique_fd.h"

#define LOG_CALL(func, param) LOG(INFO) << "function call: " << #func#param " returns "<< func param <<"\n";

inline void test_file() {
    using namespace android::base;
    LOG_CALL(GetExecutableDirectory, ());
    LOG_CALL(GetExecutablePath, ());

    std::string content;
    unique_fd fd_status(open("/proc/self/status",  O_RDONLY));
    LOG_CALL(ReadFdToString, (fd_status.release(), &content));
    LOG(INFO) << "/proc/self/status: \n" << content;

    LOG_CALL(ReadFileToString, ("/proc/self/limits", &content));
    LOG(INFO) << "/proc/self/limits: \n" << content;

    unique_fd  fd_dev_null(open("/dev/null", O_RDWR));
    LOG_CALL(WriteStringToFd, (content, fd_dev_null.release()));
    LOG_CALL(WriteStringToFile, (content, "/dev/null"));

    LOG_CALL(Basename, ("/data/local/tmp/core.dump"));
    LOG_CALL(Dirname, ("/data/local/tmp/core.dump"));

    char buff[512];
    fd_status.reset(open("/proc/self/cmdline",  O_RDONLY));
    LOG_CALL(ReadFully, (fd_status, buff, 512));
    LOG(INFO) << "/proc/self/cmdline:\n" << buff;

    fd_dev_null.reset(open("/dev/null", O_RDWR));
    LOG_CALL(WriteFully, (fd_dev_null.release(), buff, 512));

    std::string err_msg;
    unique_fd proc_self_cmdline(open("/proc/self/cmdline",  O_RDONLY));
    LOG_CALL(ForEachLine, ([](auto str) { LOG(INFO) << str; }, proc_self_cmdline.release(), err_msg));
    LOG(INFO) << err_msg;
}
#undef LOG_CALL
//
// Created by Alone on 2022-9-21.
//

#include "my-logger/processinfo.h"

using namespace lblog;

platform::PidType ProcessInfo::GetPid()
{
    thread_local platform::PidType pid = platform::GetPid();
    return pid;
}

platform::TidType ProcessInfo::GetTid()
{
    thread_local platform::TidType tid = platform::GetTid();
    return tid;
}

const char* ProcessInfo::GetHostname()
{
    thread_local char buf[256]{};
    if (buf[0] == -1)
    {
        return buf + 1;
    }
    if (platform::GetHostname(buf + 1, sizeof(buf) - 1) == 0)
    {
        buf[0] = -1;
        return buf + 1;
    }
    return "unknownhost";
}
//
// Created by Alone on 2022-9-21.
//

#include "my-logger/processinfo.h"

using namespace lblog;

sys::PidType ProcessInfo::GetPid()
{
	thread_local sys::PidType pid = sys::GetPid();
	return pid;
}

sys::TidType ProcessInfo::GetTid()
{
	thread_local sys::TidType tid = sys::GetTid();
	return tid;
}

std::string ProcessInfo::GetHostname()
{
	thread_local char buf[256]{};
    if(buf[0] == 5){
        return buf+1;
    }
	if (sys::GetHostname(buf+1, sizeof(buf) - 1) == 0)
	{
        buf[0] = 5;
		return buf+1;
	}
	return "unknownhost";
}
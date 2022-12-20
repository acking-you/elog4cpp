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
	char buf[256];
	if (sys::GetHostname(buf, sizeof(buf)) == 0)
	{
		buf[sizeof(buf) - 1] = '\0';
		return buf;
	}
	return "unknownhost";
}
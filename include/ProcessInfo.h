//
// Created by Alone on 2022-9-21.
//

#ifndef MY_LOGGER_PROCESSINFO_H
#define MY_LOGGER_PROCESSINFO_H
#include<string>

#include <unistd.h>
#include <sys/types.h>
namespace lblog
{
	namespace ProcessInfo
	{
		pid_t GetPid();
		pid_t GetTid();
		uid_t GetUid();
		std::string GetHostname();
	}
}

#endif //MY_LOGGER_PROCESSINFO_H

//
// Created by Alone on 2022-9-21.
//

#ifndef MY_LOGGER_LOGGERUTIL_H
#define MY_LOGGER_LOGGERUTIL_H

#include <string>

#include<ctime>

namespace lblog
{
	namespace detail
	{
		struct Util
		{
			static const char* getCurDateTime(bool isTime, time_t* now = nullptr);
			static const char* getErrorInfo(int error_code);
			static std::string getLogFileName(const std::string& basename, time_t& now);
		};
	}
}
#endif //MY_LOGGER_LOGGERUTIL_H

#pragma once
#include "common.h"
#include "systemcall_wrapper.h"

LBLOG_NAMESPACE_BEGIN
	struct Util
	{
		static const char* getCurDateTime(bool isTime, time_t* now = nullptr);
		static const char* getErrorInfo(int error_code);
		static std::string getLogFileName(const std::string& basename, time_t& now);
	};
LBLOG_NAMESPACE_END

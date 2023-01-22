#pragma once
#include <ctime>

#include "string_view.h"
#include "systemcall_wrapper.h"

LBLOG_NAMESPACE_BEGIN
struct Util
{
   static const char* getCurDateTime(bool isTime, std::time_t* now = nullptr);
   static const char* getErrorInfo(int error_code);
   static const char* getLogFileName(const char* basename, time_t& now);
};
LBLOG_NAMESPACE_END

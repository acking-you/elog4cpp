#pragma once
#include <ctime>

namespace elog {
struct Util
{
   static const char* getCurDateTime(bool isTime, std::time_t* now = nullptr);
   static const char* getErrorInfo(int error_code);
   static const char* getLogFileName(const char* basename, time_t& now);
};
}   // namespace elog
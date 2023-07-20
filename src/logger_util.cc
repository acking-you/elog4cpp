//
// Created by Alone on 2022-9-21.
//

#include "elog/logger_util.h"

#include <cassert>
#include <cstdio>
#include <cstring>
#include <ctime>

#include "elog/processinfo.h"

using namespace elog;

constexpr int kSumLength = 1024;

thread_local char           t_filename[kSumLength];
thread_local char           t_errnobuf[512];
thread_local char           t_time[64];
thread_local int            t_timezone = -1;
thread_local struct std::tm t_tm;
thread_local struct std::tm t_gmtm;
thread_local time_t         t_lastSecond;

using namespace std;

const char* Util::getCurDateTime(bool isTime, time_t* now)
{
   time_t timer = time(nullptr);
   if (now != nullptr)
   {   // to reduce system call
      *now = timer;
   }

   if (t_lastSecond != timer)
   {
      t_lastSecond = timer;
#if defined(_WIN32)
      ::localtime_s(&t_tm, &timer);
#else
      ::localtime_r(&timer,&t_tm);
#endif
   }
   // to subtract gmtime and localtime for obtain timezone
   if (t_timezone == -1)
   {
#if defined(_WIN32)
      ::gmtime_s(&t_gmtm, &timer);
      time_t gm_time = std::mktime(&t_gmtm);
#else
      time_t gm_time = std::mktime(::gmtime_r(&timer, &t_gmtm));
#endif
      t_timezone = static_cast<int>(timer - gm_time) / 3600;
   }
   int len;
   if (isTime)
   {
      len = std::snprintf(t_time, sizeof(t_time),
                          "%4d-%02d-%02d %02d:%02d:%02d +%02d",
                          t_tm.tm_year + 1900, t_tm.tm_mon + 1, t_tm.tm_mday,
                          t_tm.tm_hour, t_tm.tm_min, t_tm.tm_sec, t_timezone);
      assert(len == 23);
   }
   else
   {
      len = std::snprintf(t_time, sizeof(t_time), "%4d-%02d-%02d",
                          t_tm.tm_year + 1900, t_tm.tm_mon + 1, t_tm.tm_mday);
      assert(len == 10);
   }
   (void)len;
   return t_time;
}

const char* Util::getErrorInfo(int error_code)
{
#if defined(_WIN32)
   ::strerror_s(t_errnobuf, sizeof(t_errnobuf), error_code);
#else
   auto p = ::strerror_r(error_code, t_errnobuf, sizeof(t_errnobuf));
   (void)p;
#endif
   return t_errnobuf;
}

// Don't have same name If it is less than one
// second since the last refresh time
const char* Util::getLogFileName(const char* basename, time_t& now)
{
   auto baseSz = std::strlen(basename);
   assert(baseSz < kSumLength - 200);
   // prevent strcpy no copy if `basename == t_filename[0:baseSz]`
   t_filename[baseSz] = '\0';
   char* filename     = t_filename;
#if defined(_WIN32)
   ::strncpy_s(t_filename, basename, baseSz);
#else
   ::strncpy(t_filename, basename, baseSz);
#endif
   assert(filename && filename[baseSz] == '\0');

   size_t  remainLen = kSumLength - baseSz;
   std::tm tm{};
#if defined(_WIN32)
   ::localtime_s(&tm, &now);
#else
   ::localtime_r(&now, &tm);
#endif
   size_t tsz =
     std::strftime(filename + baseSz, remainLen, ".%Y%m%d-%H%M%S.", &tm);
   size_t nsz = std::snprintf(filename + baseSz + tsz, remainLen - tsz,
                              "%s.%d.log", ProcessInfo::GetHostname(),
                              static_cast<int>(ProcessInfo::GetPid()));
   assert(filename[baseSz + tsz + nsz] == '\0');
   return filename;
}

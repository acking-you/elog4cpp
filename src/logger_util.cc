//
// Created by Alone on 2022-9-21.
//

#include "elog/logger_util.h"

#include <cassert>
#include <cstdio>
#include <cstring>
#include <ctime>

#include "elog/processinfo.h"

USING_LBLOG
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
   time_t timer = time(0);
   if (now != nullptr)
   {   // to reduce system call
      *now = timer;
   }

   if (t_lastSecond != timer)
   {
      t_lastSecond = timer;
      platform::GetLocalTime_r(&timer, &t_tm);
   }
   // to subtract gmtime and localtime for obtain timezone
   if (t_timezone == -1)
   {
      time_t time_utc = mktime(platform::GetGmTime_r(&timer, &t_gmtm));
      t_timezone      = static_cast<int>(timer - time_utc) / 3600;
   }
   int len;
   if (isTime)
   {
      len =
        snprintf(t_time, sizeof(t_time), "%4d-%02d-%02d %02d:%02d:%02d +%02d",
                 t_tm.tm_year + 1900, t_tm.tm_mon + 1, t_tm.tm_mday,
                 t_tm.tm_hour, t_tm.tm_min, t_tm.tm_sec, t_timezone);
      assert(len == 23);
   }
   else
   {
      len = snprintf(t_time, sizeof(t_time), "%4d-%02d-%02d",
                     t_tm.tm_year + 1900, t_tm.tm_mon + 1, t_tm.tm_mday);
      assert(len == 10);
   }
   return t_time;
}

const char* Util::getErrorInfo(int error_code)
{
   return platform::GetStrError_r(error_code, t_errnobuf, sizeof(t_errnobuf));
}

//disable waring in msvc
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4996)
#endif
// Don't have same name If it is less than one
// second since the last refresh time
const char* Util::getLogFileName(const char* basename, time_t& now)
{
   auto baseSz = std::strlen(basename);
   assert(baseSz < kSumLength - 200);
   // prevent strcpy no copy if `basename == t_filename[0:baseSz]`
   t_filename[baseSz] = '\0';
   auto* filename     = std::strncpy(t_filename, basename, baseSz);
   assert(filename && filename[baseSz] == '\0');

   size_t  remainLen = kSumLength - baseSz;
   std::tm tm{};
   platform::GetLocalTime_r(&now, &tm);
   size_t tsz =
     std::strftime(filename + baseSz, remainLen, ".%Y%m%d-%H%M%S.", &tm);
   size_t nsz = std::snprintf(filename + baseSz + tsz, remainLen - tsz,
                              "%s.%d.log", ProcessInfo::GetHostname(),
                              static_cast<int>(ProcessInfo::GetPid()));
   assert(filename[baseSz + tsz + nsz] == '\0');
   return filename;
}
#ifdef _MSC_VER
#pragma warning(pop)
#endif

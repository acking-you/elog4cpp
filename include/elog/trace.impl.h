#pragma once
#include "logger.h"

#define LOG_NAMESPACE        lblog::
#define LOG_DETAIL_NAMESPACE lblog::detail::
#define LOG_CONTEXT_NAME     _ctx
#define LTRACE               0

#define INIT_LOG_(level_)                                                      \
   _ctx.level          = level_;                                               \
   _ctx.line           = __LINE__;                                             \
   _ctx.long_filename  = __FILE__;                                             \
   _ctx.short_filename = lblog::detail::GetShortName(__FILE__);                \
   _ctx.func_name      = __FUNCTION__;

#ifdef TRACE_
// LB_TRACE_用于跟踪logger日志系统内部情况
#define LB_TRACE_(fmt_, ...)                                                   \
   do {                                                                        \
      LOG_NAMESPACE context LOG_CONTEXT_NAME;                                  \
      LOG_CONTEXT_NAME.text = fmt::format(fmt_, ##__VA_ARGS__);                \
      INIT_LOG_(LTRACE)                                                        \
      LOG_DETAIL_NAMESPACE DoInternalLog(LOG_CONTEXT_NAME);                    \
   } while (false)

#else
#define LB_TRACE_(format, ...)
#endif
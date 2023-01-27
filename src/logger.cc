#include "elog/logger.h"

#include <cassert>

#if defined(_WIN32)
#include <Windows.h>
#endif

USING_LBLOG
USING_LBLOG_DETAIL

#define GLOB_CONFIG elog::GlobalConfig::Get()

LoggerImpl::LoggerImpl() { init_data(); }

LoggerImpl::~LoggerImpl() = default;

LoggerImpl& LoggerImpl::GetInstance()
{
   static LoggerImpl logger;
   return logger;
}

void LoggerImpl::init_data()
{
   // Determine whether to output logs to a file based on the config
   if (GLOB_CONFIG.log_filepath != nullptr)
   {
      m_logging = elog::make_unique<AsyncLogging>(
        GLOB_CONFIG.log_filepath, GLOB_CONFIG.log_rollSize,
        GLOB_CONFIG.log_flushInterval);
   }
}

void LoggerImpl::waitForDone()
{
   if (m_logging) m_logging->waitDone();
}

void LoggerImpl::LogFile(Config* config, context const& ctx)
{
   assert(config != nullptr);
   inner_message msg;
   msg.config = config;
   msg.source = inner_logsource::fromContext(ctx);
   m_logging->pushMsg(msg);
}

void LoggerImpl::LogConsole(Config* config, const context& ctx)
{
   assert(config != nullptr);

   auto buffer = buffer_t{};
   config->log_formatter(config, ctx, buffer, Appenders::kConsole);

   {
      std::lock_guard<std::mutex> lk(m_mutex);   // Lock the I/O device
#if defined(_WIN32)
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4244)
#endif
      DWORD dwBytesWritten{};
      ::WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), buffer.data(),
                      buffer.size(), &dwBytesWritten, nullptr);
      assert(dwBytesWritten == buffer.size());
#ifdef _MSC_VER
#pragma warning(pop)
#endif
#else
      fwrite_unlocked(buffer.data(), 1, buffer.size(), stdout);
#endif
   }
}

void LoggerImpl::LogConsoleUnsafe(Config* config, const context& ctx)
{
   assert(config != nullptr);

   auto buffer = buffer_t{};
   config->log_formatter(config, ctx, buffer, Appenders::kConsole);
#if defined(_WIN32)
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4244)
#endif
   DWORD dwBytesWritten{};
   ::WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), buffer.data(),
                   buffer.size(), &dwBytesWritten, nullptr);
   assert(dwBytesWritten == buffer.size());
#ifdef _MSC_VER
#pragma warning(pop)
#endif
#else
   fwrite_unlocked(buffer.data(), 1, buffer.size(), stdout);
#endif
}

void LoggerImpl::DoInternalLog(const context& ctx)
{
   if (GLOB_CONFIG.log_level != Levels::kTrace) { return; }
   if (GLOB_CONFIG.log_console) { GetInstance().LogConsole(&GLOB_CONFIG, ctx); }
}

// 全局config logger输出
void LoggerImpl::DoLog(context const& ctx) { DoConfigLog(&GLOB_CONFIG, ctx); }

// 自定义config logger输出
void LoggerImpl::DoConfigLog(Config* config, const context& ctx)
{
   assert(config != nullptr);
   // FIXME
   // 输出到控制台没有进行任何性能优化，输出控制台方便开发时调试，输出到文件可以方便后续问题的跟踪
   if (ctx.level < static_cast<int>(config->log_level)) return;
   if (m_logging) LogFile(config, ctx);
   if (GLOB_CONFIG.log_console) LogConsole(config, ctx);

   if (ctx.level == static_cast<int>(Levels::kFatal))
   {                           // 若为fatal则发送abort信号退出进程
      m_logging->waitDone();   // 等待数据成功刷盘
      abort();
   }
}

Log& Log::instance()
{
   thread_local Log t_log;
   return t_log;
}

logger_helper elog::Check(bool cond, source_location const& location)
{
   if (cond) { return logger_helper{location}; }
   return {};
}

void elog::CheckIfFatal(bool cond, source_location const& location,
                        const char* text)
{
   if (cond) { Check(false, location).fatal(text); }
}

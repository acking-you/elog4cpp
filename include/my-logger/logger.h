#pragma once
#include "common.h"
#include "async_logging.h"
#include "config.h"
#include "formatter.h"

#include <functional>

#include <cstdio>
#include <cstdlib>
#include <ctime>

LBLOG_NAMESPACE_BEGIN
/**
 * @debug
 * 默认release模式下会屏蔽debug的输出
 *
 * @LOG_LIMIT_WARN
 * 设置warn以上等级输出
 *
 * @LOG_LIMT_ERROR
 * 设置error以上等级输出
 *
 * @LOG_CUSTOM_ONLY
 * 取消所有预定义的宏
 */
// 定义后可在控制台上得到日志系统的运行情况
#ifdef LOG_TRACE
#define TRACE_
#endif
// 默认debug模式下
#ifndef NDEBUG

#define DEBUG_
#define INFO_
#define WARN_
#define ERROR_
#define FATAL_

#endif

// 默认release模式下
#ifdef NDEBUG

#undef DEBUG_
#define INFO_
#define WARN_
#define ERROR_
#define FATAL_

#endif

#ifdef LOG_LIMIT_WARN // 设置warn及以上才能记录

#undef DEBUG_
#undef INFO_
#define WARN_
#define ERROR_
#define FATAL_

#endif

#ifdef LOG_LIMIT_ERROR // error及以上才记录

#undef DEBUG_
#undef INFO_
#undef WARN_
#define ERROR_
#define FATAL_

#endif
	using std::string;

	struct context;

	namespace detail
	{

		class Logger : noncopyable
		{
		 private:
			Logger();

		 public:
			~Logger();

			static Logger& GetInstance();

			void waitForDone();

			void DoLog(context const& ctx);

			//提供给外界的局部config接口
			void DoConfigLog(Config* config, context const& ctx);

			//打印Logger内部情况
			static void DoInternalLog(context const& ctx);

			void LogFile(Config* config, context const& ctx);

			void LogConsole(Config* config, context const& ctx);

			static void LogConsoleUnsafe(Config* config, context const& ctx);

		 private:
			void init_data();

		 private:
			std::unique_ptr<async_logging> m_logging;
			std::mutex m_mutex; //locked console
		};

		inline void DoLog(context& ctx)
		{
			detail::Logger::GetInstance().DoLog(ctx);
		}

		inline void DoConfigLog(Config* config, context& ctx)
		{
			detail::Logger::GetInstance().DoConfigLog(config, ctx);
		}

		inline void DoInternalLog(context& ctx)
		{
			detail::Logger::DoInternalLog(ctx);
		}

		inline const char* GetShortName(const char* filename)
		{
			int len = strlen(filename);
			int pos = 0;
			for (int i = len - 1; i >= 0; i--)
			{
				if (filename[i] == '/' || filename[i] == '\\')
				{
					pos = i + 1;
					break;
				}
			}
			return filename + pos;
		}

		// A small helper for CHECK_NOTNULL().
		template<typename T>
		T* CheckNotNull(const char* file, int line, const char* text, T* ptr)
		{
			if (ptr == NULL)
			{
				context ctx;
				ctx.level = static_cast<int>(Levels::kFatal);
				ctx.short_filename = GetShortName(file);
				ctx.long_filename = file;
				ctx.text = text;
				ctx.line = line;
				Logger::GetInstance().DoLog(ctx);
			}
			return ptr;
		}

		// A small helper for PTR().
		template<typename T>
		void* CastToVoidPtr(T* ptr)
		{
			return ptr;
		}
	} // namespace detail
	//wait for async logging done
	inline void WaitForDone()
	{
		detail::Logger::GetInstance().waitForDone();
	}
LBLOG_NAMESPACE_END

#define LOG_NAMESPACE lblog::
#define LOG_DETAIL_NAMESPACE lblog::detail::
#define LOG_CONTEXT_NAME _ctx
#define LTRACE 0
#define LDEBUG 1
#define LINFO 2
#define LWARN 3
#define LERROR 4
#define LFATAL 5

#define INIT_LOG_(level_)          \
    LOG_CONTEXT_NAME.level          = level_;   \
    LOG_CONTEXT_NAME.line           = __LINE__; \
    LOG_CONTEXT_NAME.long_filename  = __FILE__; \
    LOG_CONTEXT_NAME.short_filename = LOG_DETAIL_NAMESPACE GetShortName(__FILE__);

#ifdef TRACE_
// _trace用于跟踪logger日志系统内部情况
#define trace_(fmt_, args_...) \
   do                                         \
    {                                          \
        LOG_NAMESPACE context LOG_CONTEXT_NAME;           \
        LOG_CONTEXT_NAME.text = fmt::format(fmt_, ##args_); \
        INIT_LOG_(LTRACE)                           \
        LOG_DETAIL_NAMESPACE DoInternalLog(LOG_CONTEXT_NAME);              \
    } while (false)

#define trace(fmt_, args_...)                  \
    do                                         \
    {                                          \
        LOG_NAMESPACE context LOG_CONTEXT_NAME;           \
        LOG_CONTEXT_NAME.text = fmt::format(fmt_, ##args_); \
        INIT_LOG_(LTRACE)                           \
        LOG_DETAIL_NAMESPACE DoLog(LOG_CONTEXT_NAME);              \
    } while (false)
#else
#define trace(format, args...)
#define trace_(format,args...)
#endif

#ifdef DEBUG_
#define debug(fmt_, args_...)                  \
	do                                         \
	{                                          \
		LOG_NAMESPACE context LOG_CONTEXT_NAME;           \
		LOG_CONTEXT_NAME.text = fmt::format(fmt_, ##args_); \
		INIT_LOG_(LDEBUG)                           \
		LOG_DETAIL_NAMESPACE DoLog(LOG_CONTEXT_NAME);                    \
	} while (false)

#else
#define debug(format, args...)
#endif

#ifdef INFO_
#define info(fmt_, args_...)                   \
    do                                         \
    {                                          \
        LOG_NAMESPACE context LOG_CONTEXT_NAME;           \
        LOG_CONTEXT_NAME.text = fmt::format(fmt_, ##args_); \
        INIT_LOG_(LINFO)                           \
        LOG_DETAIL_NAMESPACE DoLog(LOG_CONTEXT_NAME);                    \
    } while (false)
#else
#define info(format, args...)
#endif

#ifdef WARN_
#define warn(fmt_, args_...)                   \
    do                                         \
    {                                          \
        LOG_NAMESPACE context LOG_CONTEXT_NAME;           \
        LOG_CONTEXT_NAME.text = fmt::format(fmt_, ##args_); \
        INIT_LOG_(LWARN)                           \
        LOG_DETAIL_NAMESPACE DoLog(LOG_CONTEXT_NAME);                    \
    } while (false)
#else
#define warn(format, args...)
#endif

#ifdef ERROR_
#define error(fmt_, args_...)                  \
    do                                         \
    {                                          \
        LOG_NAMESPACE context LOG_CONTEXT_NAME;           \
        LOG_CONTEXT_NAME.text = fmt::format(fmt_, ##args_); \
        INIT_LOG_(LERROR)                           \
        LOG_DETAIL_NAMESPACE DoLog(LOG_CONTEXT_NAME);                    \
    } while (false)
#else
#define error(format, args...)
#endif

#ifdef FATAL_
#define fatal(fmt_, args_...)                  \
    do                                         \
    {                                          \
        LOG_NAMESPACE context LOG_CONTEXT_NAME;           \
        LOG_CONTEXT_NAME.text = fmt::format(fmt_, ##args_); \
        INIT_LOG_(LFATAL)                           \
        LOG_DETAIL_NAMESPACE DoLog(LOG_CONTEXT_NAME);                    \
    } while (false)
#else
#define fatal(format, args...)
#endif

#define CHECK_NOTNULL(val) lblog::detail::CheckNotNull(__FILE__, __LINE__, "'" #val "' Must be non NULL", (val))
#define PTR(p) lblog::detail::CastToVoidPtr(p)

#ifdef LOG_CUSTOM_ONLY
#undef trace
#undef debug
#undef info
#undef warn
#undef error
#undef fatal
#endif


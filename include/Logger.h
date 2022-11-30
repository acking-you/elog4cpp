#ifndef LOGGER_H
#define LOGGER_H

#include "AsyncLogging.h"
#include "Config.h"
#include "fmt/color.h"
#include "fmt/core.h"
#include "fmt/ranges.h"
#include "noncopyable.h"

#include <functional>

#include <cstdio>
#include <cstdlib>
#include <ctime>

namespace lblog
{
/**
 * @debug
 * 默认release模式下会屏蔽debug的输出
 *
 * @LOG_LIMIT_WARN
 * 设置warn以上等级输出
 *
 * @LOG_LIMT_ERROR
 * 设置error以上等级输出
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

	struct Config
	{
		int print_flag = LstdFlags;
		int print_level = L_DEBUG;
		const char* output_prefix = nullptr;
		const char* output_basedir = nullptr;
		bool is_console = true;
		detail::callback_t before;
		detail::callback_t after;

		Config& setFlag(int flag)
		{
			print_flag = flag;
			return *this;
		}
		Config& setLevel(int level)
		{
			print_level = level;
			return *this;
		}

		Config& setOutputPrefix(const char* prefix)
		{
			output_prefix = prefix;
			return *this;
		}
		Config& setOutPutBaseDir(const char* basedir)
		{
			output_basedir = basedir;
			return *this;
		}
		Config& enableConsole(bool s)
		{
			is_console = s;
			return *this;
		}
		Config& setBeforeCallback(detail::callback_t function)
		{
			this->before = std::move(function);
			return *this;
		}
		Config& setAfterCallback(detail::callback_t function)
		{
			this->after = std::move(function);
			return *this;
		}

		static int Level();
		static void Set(const Config& config);
	};

	namespace detail
	{

		struct context;

		class Logger : noncopyable
		{
		 private:
			Logger();

		 public:
			~Logger();

			static Logger& GetInstance();

			void waitForDone();

			void DoLog(context const& ctx);

			//打印Logger内部情况
			static void DoInternalLog(context const& ctx);

			void LogFile(context const& ctx);

			static void LogConsole(context const& ctx);

		 private:
			void init_data();

		 private:
			std::unique_ptr<AsyncLogging> m_logging;
		};

		struct context
		{ // 用于传递每次输出内容的上下文
			int level;
			int line;
			const char* short_filename{};
			const char* long_filename{};
			string text;
		};

		inline void DoLog(context& ctx)
		{
			detail::Logger::GetInstance().DoLog(ctx);
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
	} // namespace detail
	//wait for async logging done
	inline void waitForDone()
	{
		detail::Logger::GetInstance().waitForDone();
	}
} // namespace lblog

#define LOG_NAMESPACE lblog::
#define LOG_DETAIL_NAMESPACE lblog::detail::

#define INIT_LOG_(level_)          \
    ctx.level          = level_;   \
    ctx.line           = __LINE__; \
    ctx.long_filename  = __FILE__; \
    ctx.short_filename = LOG_DETAIL_NAMESPACE GetShortName(__FILE__);

#ifdef TRACE_
// _trace用于跟踪logger日志系统内部情况
#define trace_(fmt_, args_...) \
   do                                         \
    {                                          \
        LOG_DETAIL_NAMESPACE context ctx;           \
        ctx.text = fmt::format(fmt_, ##args_); \
        INIT_LOG_(LOG_NAMESPACE L_TRACE)                           \
        LOG_DETAIL_NAMESPACE DoInternalLog(ctx);              \
    } while (false)

#define trace(fmt_, args_...)                  \
    do                                         \
    {                                          \
        LOG_DETAIL_NAMESPACE context ctx;           \
        ctx.text = fmt::format(fmt_, ##args_); \
        INIT_LOG_(LOG_NAMESPACE L_TRACE)                           \
        LOG_DETAIL_NAMESPACE DoLog(ctx);              \
    } while (false)
#else
#define trace(format, args...)
#define trace_(format,args...)
#endif

#ifdef DEBUG_
#define debug(fmt_, args_...)                  \
	do                                         \
	{                                          \
		LOG_DETAIL_NAMESPACE context ctx;           \
		ctx.text = fmt::format(fmt_, ##args_); \
		INIT_LOG_(LOG_NAMESPACE L_DEBUG)                           \
		LOG_DETAIL_NAMESPACE DoLog(ctx);                    \
	} while (false)

#else
#define debug(format, args...)
#endif

#ifdef INFO_
#define info(fmt_, args_...)                   \
    do                                         \
    {                                          \
        LOG_DETAIL_NAMESPACE context ctx;           \
        ctx.text = fmt::format(fmt_, ##args_); \
        INIT_LOG_(LOG_NAMESPACE L_INFO)                           \
        LOG_DETAIL_NAMESPACE DoLog(ctx);                    \
    } while (false)
#else
#define info(format, args...)
#endif

#ifdef WARN_
#define warn(fmt_, args_...)                   \
    do                                         \
    {                                          \
        LOG_DETAIL_NAMESPACE context ctx;           \
        ctx.text = fmt::format(fmt_, ##args_); \
        INIT_LOG_(LOG_NAMESPACE L_WARN)                           \
        LOG_DETAIL_NAMESPACE DoLog(ctx);                    \
    } while (false)
#else
#define warn(format, args...)
#endif

#ifdef ERROR_
#define error(fmt_, args_...)                  \
    do                                         \
    {                                          \
        LOG_DETAIL_NAMESPACE context ctx;           \
        ctx.text = fmt::format(fmt_, ##args_); \
        INIT_LOG_(LOG_NAMESPACE L_ERROR)                           \
        LOG_DETAIL_NAMESPACE DoLog(ctx);                    \
    } while (false)
#else
#define error(format, args...)
#endif

#ifdef FATAL_
#define fatal(fmt_, args_...)                  \
    do                                         \
    {                                          \
        LOG_DETAIL_NAMESPACE context ctx;           \
        ctx.text = fmt::format(fmt_, ##args_); \
        INIT_LOG_(LOG_NAMESPACE L_FATAL)                           \
        LOG_DETAIL_NAMESPACE DoLog(ctx);                    \
    } while (false)
#else
#define fatal(format, args...)
#endif

#endif
#pragma once
#include "async_logging.h"
#include "config.h"

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
 * 取消所有预定义的宏，自定义实现自己的logger
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

#ifdef LOG_LIMIT_ERROR // error及以上才被记录

#undef DEBUG_
#undef INFO_
#undef WARN_
#define ERROR_
#define FATAL_

#endif
using std::string;

namespace detail {

class Logger : noncopyable
{
private:
    Logger();

public:
    ~Logger();

    static Logger& GetInstance();

    void waitForDone();

    void DoLog(context const& ctx);

    // 提供给外界的局部config接口
    void DoConfigLog(Config* config, context const& ctx);

    // 打印Logger内部情况
    static void DoInternalLog(context const& ctx);

    void LogFile(Config* config, context const& ctx);

    void LogConsole(Config* config, context const& ctx);

    static void LogConsoleUnsafe(Config* config, context const& ctx);

private:
    void init_data();

private:
    std::unique_ptr<AsyncLogging> m_logging;
    std::mutex                    m_mutex; // locked console
};

inline void DoLog(context& ctx) { detail::Logger::GetInstance().DoLog(ctx); }

/**
 * @brief
 * 提供给外界自定义config输出的接口，默认提供的接口只支持全局config，当你的项目需要多个config配置的输出时可以用到这个接口
 * @param config
 * @param ctx
 */
inline void DoConfigLog(Config* config, context& ctx)
{
    detail::Logger::GetInstance().DoConfigLog(config, ctx);
}

inline void DoInternalLog(context& ctx) { detail::Logger::DoInternalLog(ctx); }

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
template <typename T>
T* CheckNotNull(const char* file, int line, const char* text, T* ptr)
{
    if (ptr == NULL)
    {
        context ctx;
        ctx.level          = static_cast<int>(Levels::kFatal);
        ctx.short_filename = GetShortName(file);
        ctx.long_filename  = file;
        ctx.text           = text;
        ctx.line           = line;
        Logger::GetInstance().DoLog(ctx);
    }
    return ptr;
}

// A small helper for PTR().
template <typename T>
void* CastToVoidPtr(T* ptr)
{
    return ptr;
}
} // namespace detail
// wait for async logging done
inline void WaitForDone() { detail::Logger::GetInstance().waitForDone(); }
LBLOG_NAMESPACE_END

#define LOG_NAMESPACE        lblog::
#define LOG_DETAIL_NAMESPACE lblog::detail::
#define LOG_CONTEXT_NAME     _ctx
#define LTRACE               0
#define LDEBUG               1
#define LINFO                2
#define LWARN                3
#define LERROR               4
#define LFATAL               5

#define INIT_LOG_(level_)                            \
    LOG_CONTEXT_NAME.level         = level_;         \
    LOG_CONTEXT_NAME.line          = __LINE__;       \
    LOG_CONTEXT_NAME.long_filename = __FILE__;       \
    LOG_CONTEXT_NAME.short_filename =                \
        LOG_DETAIL_NAMESPACE GetShortName(__FILE__); \
    LOG_CONTEXT_NAME.func_name = __FUNCTION__;

#ifdef TRACE_
// LB_TRACE_用于跟踪logger日志系统内部情况
#define LB_TRACE_(fmt_, ...)                                      \
    do                                                            \
    {                                                             \
        LOG_NAMESPACE context LOG_CONTEXT_NAME;                   \
        LOG_CONTEXT_NAME.text = fmt::format(fmt_, ##__VA_ARGS__); \
        INIT_LOG_(LTRACE)                                         \
        LOG_DETAIL_NAMESPACE DoInternalLog(LOG_CONTEXT_NAME);     \
    } while (false)

#define LB_TRACE(fmt_, ...)                                       \
    do                                                            \
    {                                                             \
        LOG_NAMESPACE context LOG_CONTEXT_NAME;                   \
        LOG_CONTEXT_NAME.text = fmt::format(fmt_, ##__VA_ARGS__); \
        INIT_LOG_(LTRACE)                                         \
        LOG_DETAIL_NAMESPACE DoLog(LOG_CONTEXT_NAME);             \
    } while (false)
#else
#define LB_TRACE(format, ...)
#define LB_TRACE_(format, ...)
#endif

#ifdef DEBUG_
#define LB_DEBUG(fmt_, ...)                                       \
    do                                                            \
    {                                                             \
        LOG_NAMESPACE context LOG_CONTEXT_NAME;                   \
        LOG_CONTEXT_NAME.text = fmt::format(fmt_, ##__VA_ARGS__); \
        INIT_LOG_(LDEBUG)                                         \
        LOG_DETAIL_NAMESPACE DoLog(LOG_CONTEXT_NAME);             \
    } while (false)

#else
#define LB_DEBUG(format, ...)
#endif

#ifdef INFO_
#define LB_INFO(fmt_, ...)                                        \
    do                                                            \
    {                                                             \
        LOG_NAMESPACE context LOG_CONTEXT_NAME;                   \
        LOG_CONTEXT_NAME.text = fmt::format(fmt_, ##__VA_ARGS__); \
        INIT_LOG_(LINFO)                                          \
        LOG_DETAIL_NAMESPACE DoLog(LOG_CONTEXT_NAME);             \
    } while (false)
#else
#define LB_INFO(format, ...)
#endif

#ifdef WARN_
#define LB_WARN(fmt_, ...)                                        \
    do                                                            \
    {                                                             \
        LOG_NAMESPACE context LOG_CONTEXT_NAME;                   \
        LOG_CONTEXT_NAME.text = fmt::format(fmt_, ##__VA_ARGS__); \
        INIT_LOG_(LWARN)                                          \
        LOG_DETAIL_NAMESPACE DoLog(LOG_CONTEXT_NAME);             \
    } while (false)
#else
#define LB_WARN(format, ...)
#endif

#ifdef ERROR_
#define LB_ERROR(fmt_, ...)                                       \
    do                                                            \
    {                                                             \
        LOG_NAMESPACE context LOG_CONTEXT_NAME;                   \
        LOG_CONTEXT_NAME.text = fmt::format(fmt_, ##__VA_ARGS__); \
        INIT_LOG_(LERROR)                                         \
        LOG_DETAIL_NAMESPACE DoLog(LOG_CONTEXT_NAME);             \
    } while (false)
#else
#define LB_ERROR(format, ...)
#endif

#ifdef FATAL_
#define LB_FATAL(fmt_, ...)                                       \
    do                                                            \
    {                                                             \
        LOG_NAMESPACE context LOG_CONTEXT_NAME;                   \
        LOG_CONTEXT_NAME.text = fmt::format(fmt_, ##__VA_ARGS__); \
        INIT_LOG_(LFATAL)                                         \
        LOG_DETAIL_NAMESPACE DoLog(LOG_CONTEXT_NAME);             \
    } while (false)
#else
#define LB_FATAL(format, ...)
#endif

#define CHECK_NOTNULL(val)                          \
    lblog::detail::CheckNotNull(__FILE__, __LINE__, \
                                "'" #val "' Must be non NULL", (val))
#define PTR(p) lblog::detail::CastToVoidPtr(p)

#ifdef LOG_CUSTOM_ONLY
#undef LB_TRACE
#undef LB_DEBUG
#undef LB_INFO
#undef LB_WARN
#undef LB_ERROR
#undef LB_FATAL
#endif

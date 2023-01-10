#pragma once
#include "async_logging.h"
#include "config.h"
#include "processinfo.h"
#include "source_location.h"

LBLOG_NAMESPACE_BEGIN

using std::string;

namespace detail {
class LoggerImpl : noncopyable
{
private:
    LoggerImpl();

public:
    ~LoggerImpl();

    static LoggerImpl& GetInstance();

    void waitForDone();

    void DoLog(context const& ctx);

    // 自定义化的config打印
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

inline void DoLog(context& ctx)
{
    detail::LoggerImpl::GetInstance().DoLog(ctx);
}

inline void DoInternalLog(context& ctx)
{
    detail::LoggerImpl::DoInternalLog(ctx);
}

inline const char* GetShortName(const char* filename)
{
    int len = std::strlen(filename);
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

/**
 * @brief
 * 提供给外界自定义config输出的接口，默认提供的接口只支持全局config，当你的项目需要多个config配置的输出时可以用到这个接口
 * @param config
 * @param ctx
 */
inline void DoConfigLog(Config* config, context& ctx)
{
    detail::LoggerImpl::GetInstance().DoConfigLog(config, ctx);
}

// A small helper for PTR().
template <typename T>
void* Ptr(T* ptr)
{
    return ptr;
}

// A small helper for CHECK_NOTNULL().
template <typename T>
T* CheckNotNull(const char* file, int line, const char* text, T* ptr)
{
    if (ptr == NULL)
    {
        context ctx;
        ctx.level          = static_cast<int>(Levels::kFatal);
        ctx.short_filename = detail::GetShortName(file);
        ctx.long_filename  = file;
        ctx.text           = text;
        ctx.line           = line;
        detail::DoLog(ctx);
    }
    return ptr;
}
// wait for async logging done
inline void WaitForDone() { detail::LoggerImpl::GetInstance().waitForDone(); }

// only use LogStorage::Get
struct LogStorage : noncopyable
{
    explicit LogStorage(Levels level) : id_(platform::GetTid()) {}

public:
    friend class Log;

    static LogStorage& Get(Levels level)
    {
        thread_local LogStorage instance(level);
        instance.ctx_.level = level;
        return instance;
    }

private:
    platform::TidType id_;
    fmt_buffer_t      buffer_;
    context           ctx_;
};

// user can use that
class Log : noncopyable
{
public:
    explicit Log(Levels level) { m_log = &LogStorage::Get(level); }

    Log(Log&& log) noexcept { threadSafeMove(std::forward<Log>(log)); }
    Log& operator=(Log&& log) noexcept
    {
        threadSafeMove(std::forward<Log>(log));
        return *this;
    }

    template <typename T = source_location>
    inline const Log& with(T const& location = T::current()) const
    {
        assert(m_log != nullptr);
        m_log->ctx_.line          = location.line();
        m_log->ctx_.func_name     = location.function_name();
        m_log->ctx_.long_filename = location.file_name();
        m_log->ctx_.short_filename =
            detail::GetShortName(m_log->ctx_.long_filename);
        return *this;
    }

    template <typename T, typename... Args>
    void println(T&& first, Args&&... args) const
    {
        assert(m_log != nullptr);
        buffer_helper{&m_log->buffer_}.formatTo("{}, ", first);
        println(std::forward<Args>(args)...);
    }

    template <typename T>
    void println(T&& first) const
    {
        assert(m_log != nullptr);
        buffer_helper{&m_log->buffer_}.formatTo("{}", first);
        m_log->ctx_.text = fmt::to_string(m_log->buffer_);
        detail::DoLog(m_log->ctx_);
        clear();
    }

    template <typename... Args>
    void printf(const char* format, Args&&... args) const
    {
        assert(m_log != nullptr);
        m_log->ctx_.text = fmt::format(format, std::forward<Args>(args)...);
        detail::DoLog(m_log->ctx_);
        clear();
    }

private:
    void threadSafeMove(Log&& log)
    {
        if (!log.m_log)
        {
            return;
        }
        if (log.m_log->id_ == platform::GetTid())
        {
            m_log     = log.m_log;
            log.m_log = nullptr;
        }
        else
        {
            m_log =
                &LogStorage::Get(static_cast<Levels>(log.m_log->ctx_.level));
            log.m_log = nullptr;
        }
    }

    void clear() const
    {
        assert(m_log != nullptr);
        m_log->ctx_.line           = 0;
        m_log->ctx_.func_name      = 0;
        m_log->ctx_.long_filename  = 0;
        m_log->ctx_.short_filename = 0;
        m_log->buffer_.clear();
    }

private:
    LogStorage* m_log{};
};

LBLOG_NAMESPACE_END
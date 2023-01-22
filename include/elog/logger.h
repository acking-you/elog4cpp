#pragma once
#include <sstream>

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
   std::mutex                    m_mutex;   // locked console
};

inline void DoLog(context& ctx)
{
   detail::LoggerImpl::GetInstance().DoLog(ctx);
}

inline void DoInternalLog(context& ctx)
{
   detail::LoggerImpl::DoInternalLog(ctx);
}

/**
 * Get the string length at compile time
 * @param str
 * @param len
 * @return
 */
inline constexpr size_t GetStrLen(const char* str, size_t len = 0)
{
   return str[len] ? GetStrLen(str, len + 1) + 1 : 0;
}

/**
 * Get the file name based on the path at compile time
 * @param filename
 * @param len
 * @return
 */
inline constexpr const char* GetShortName(const char* filename,
                                          size_t      len = string::npos)
{
   return len == string::npos
            ? (len = GetStrLen(filename), GetShortName(filename, len))
          : (len > 0 && (filename[len - 1] == '/' || filename[len - 1] == '\\'))
            ? filename + len
          : (len > 0) ? GetShortName(filename, len - 1)
                      : filename;
}

}   // namespace detail

/**
 * @brief
 * The interface provided to the outside to customize the config output,
 * the default provided interface only supports the global config,
 * when your project needs multiple config configuration output,
 * you should need this interface
 * @param config
 * @param ctx
 */
inline void DoConfigLog(Config* config, context& ctx)
{
   detail::LoggerImpl::GetInstance().DoConfigLog(config, ctx);
}

// A small helper for print any pointer.
template <typename T>
void* Ptr(T* ptr)
{
   return ptr;
}

// A small helper for CHECK_NOTNULL().
template <typename T>
T* CheckNotNull(const char* file, const char* shortFile, int line,
                const char* text, T* ptr)
{
   if (ptr == NULL)
   {
      context ctx;
      ctx.level          = static_cast<int>(Levels::kFatal);
      ctx.short_filename = shortFile;
      ctx.long_filename  = file;
      ctx.text           = text;
      ctx.line           = line;
      detail::DoLog(ctx);
   }
   return ptr;
}

#define CHECK_NOTNULL(ptr)                                                     \
   elog::CheckNotNull(__FILE__, elog::detail::GetShortName(__FILE__),          \
                      __LINE__, "nullptr error", ptr)
// wait for async logging done
inline void WaitForDone() { detail::LoggerImpl::GetInstance().waitForDone(); }

// only use LogStorage::Get
struct LogStorage : noncopyable
{
   explicit LogStorage() = default;

public:
   friend class Log;
   static std::unique_ptr<LogStorage> Get();

private:
   context      ctx_;
   fmt_buffer_t buffer_;
};

// log by class
class Log : noncopyable
{
public:
   using ConfigPtr     = std::unique_ptr<Config>;
   using LogStoragePtr = std::unique_ptr<LogStorage>;

   explicit Log(Levels level) : m_storage(LogStorage::Get()) {}

   explicit Log(Levels level, ConfigPtr config)
     : m_storage(LogStorage::Get()), m_config(std::move(config))
   {
   }

   Log(Log&& log) noexcept
     : m_storage(std::move(log.m_storage)), m_config(std::move(log.m_config))
   {
   }

   Log& operator=(Log&& log) noexcept
   {
      this->m_config  = std::move(log.m_config);
      this->m_storage = std::move(log.m_storage);
      return *this;
   }

   template <typename T                          = source_location,
             typename std::enable_if<std::is_same<typename std::decay<T>::type,
                                                  source_location>::value,
                                     bool>::type = true>
   inline const Log& with(T const& location = T::current()) const
   {
      assert(m_storage != nullptr);
      initLocation(location);
      return *this;
   }

   template <typename T, typename... Args>
   void println(T&& first, Args&&... args) const
   {
      assert(m_storage != nullptr);
      buffer_helper{&m_storage->buffer_}.formatTo("{}, ", first);
      println(std::forward<Args>(args)...);
   }

   template <typename T>
   void println(T&& first) const
   {
      assert(m_storage != nullptr);

      buffer_helper{&m_storage->buffer_}.formatTo("{}", first);
      m_storage->ctx_.text = fmt::to_string(m_storage->buffer_);
      doLog();
      clearLocation();
   }

   template <typename... Args>
   void printf(const char* format, Args&&... args) const
   {
      assert(m_storage != nullptr);

      m_storage->ctx_.text = fmt::format(format, std::forward<Args>(args)...);
      doLog();
      clearLocation();
   }

   template <typename... Args>
   static void trace(const char* format, Args&&... args)
   {
      auto& log = instance();
      assert(log.m_storage != nullptr);
      log.m_storage->ctx_.level = kTrace;
      log.printf(format, std::forward<Args>(args)...);
   }

   template <typename... Args>
   static void debug(const char* format, Args&&... args)
   {
      auto& log = instance();
      assert(log.m_storage != nullptr);

      log.m_storage->ctx_.level = kDebug;
      log.printf(format, std::forward<Args>(args)...);
   }

   template <typename... Args>
   static void info(const char* format, Args&&... args)
   {
      auto& log = instance();
      assert(log.m_storage != nullptr);

      log.m_storage->ctx_.level = kInfo;
      log.printf(format, std::forward<Args>(args)...);
   }

   template <typename... Args>
   static void warn(const char* format, Args&&... args)
   {
      auto& log = instance();
      assert(log.m_storage != nullptr);

      log.m_storage->ctx_.level = kWarn;
      log.printf(format, std::forward<Args>(args)...);
   }

   template <typename... Args>
   static void error(const char* format, Args&&... args)
   {
      auto& log = instance();
      assert(log.m_storage != nullptr);

      log.m_storage->ctx_.level = kError;
      log.printf(format, std::forward<Args>(args)...);
   }

   template <typename... Args>
   static void fatal(const char* format, Args&&... args)
   {
      auto& log = instance();
      assert(log.m_storage != nullptr);

      log.m_storage->ctx_.level = kFatal;
      log.printf(format, std::forward<Args>(args)...);
   }

private:
   static Log& instance();

   void doLog() const
   {
      detail::LoggerImpl::GetInstance().DoConfigLog(
        m_config ? m_config.get() : &GlobalConfig::Get(), m_storage->ctx_);
   }

   void initLocation(source_location const& location) const
   {
      m_storage->ctx_.line          = location.line();
      m_storage->ctx_.func_name     = location.function_name();
      m_storage->ctx_.long_filename = location.file_name();
      m_storage->ctx_.short_filename =
        detail::GetShortName(m_storage->ctx_.long_filename);
   }

   void clearLocation() const
   {
      std::memset((void*)&m_storage->ctx_.line, 0,
                  context::GetNoTextAndLevelLength(m_storage->ctx_));
      m_storage->buffer_.clear();
   }

private:
   LogStoragePtr m_storage;
   ConfigPtr     m_config;
};

LBLOG_NAMESPACE_END
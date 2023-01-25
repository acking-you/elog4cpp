#pragma once
#include <array>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <functional>
#include <memory>
#include <string>
#include <type_traits>

#include "any.h"
#include "fmt/core.h"
#include "fmt/ranges.h"
#include "micros.h"
#include "string_view.h"

LBLOG_NAMESPACE_BEGIN
#if __cplusplus == 201103l
template <typename T, typename... Args>
auto make_unique(Args&&... args) -> ::std::unique_ptr<T>
{
   return ::std::unique_ptr<T>(new T(::std::forward<Args>(args)...));
}
#else
using std::make_unique;
#endif

enum Flags {
   kDate      = 1 << 0,
   kTime      = 1 << 1,
   kLongname  = 1 << 2,
   kShortname = 1 << 3,
   kLine      = 1 << 4,
   kFuncName  = 1 << 5,
   kThreadId  = 1 << 6,
   kStdFlags  = kDate | kTime | kShortname | kLine | kFuncName
};

enum Levels { kTrace, kDebug, kInfo, kWarn, kError, kFatal, kLevelCount };

enum Appenders { kConsole, kFile };

inline Flags operator+(Flags lhs, Flags rhs)
{
   return static_cast<Flags>(lhs | rhs);
}

inline bool operator<=(Levels lhs, Levels rhs)
{
   return static_cast<int>(lhs) <= static_cast<int>(rhs);
}

inline bool operator>=(Levels lhs, Levels rhs)
{
   return static_cast<int>(lhs) >= static_cast<int>(rhs);
}

inline bool operator<(Levels lhs, Levels rhs)
{
   return static_cast<int>(lhs) < static_cast<int>(rhs);
}

inline bool operator>(Levels lhs, Levels rhs)
{
   return static_cast<int>(lhs) > static_cast<int>(rhs);
}

struct Config;
struct context;
class buffer_helper;
class OutputBuffer;

using buffer_t     = fmt::memory_buffer;
using output_buf_t = OutputBuffer;
using callback_t   = std::function<void(output_buf_t&)>;
using formatter_t  = std::function<void(Config*, context const&, buffer_t&,
                                       Appenders appenderType)>;

template <bool B, typename T = void>
using enable_if_t = typename std::enable_if<B, T>::type;
template <typename... Args>
using format_string =
  fmt::basic_format_string<char, fmt::type_identity_t<Args>...>;

inline constexpr int OP_INT(const StringView& sv)
{
   return sv.empty() ? 0
                     : sv[sv.size() - 1] + OP_INT({sv.data(), sv.size() - 1});
}

inline constexpr int operator""_i(const char* op, size_t len)
{
   return OP_INT({op, len});
}

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4244)
#endif

struct context
{   // 用于传递每次输出内容的上下文
   int                 level;
   int                 line{};
   const char*         short_filename{};
   const char*         long_filename{};
   const char*         func_name{};
   StringView          text;
   // 计算中间经常可变的位置信息长度，可用于通过memset优化清零
   static unsigned int GetNoTextAndLevelLength(context& ctx)
   {
      static const unsigned int s_ctx_len = (char*)&ctx.text - (char*)&ctx.line;
      return s_ctx_len;
   }
};

class buffer_helper
{
public:
   buffer_t* buf{};
   buffer_helper() = default;
   explicit buffer_helper(buffer_t* fmt_buf) : buf(fmt_buf) {}
   auto startWith(const StringView& sv) -> bool
   {
      auto buf_sv = make_view();
      if (sv.size() > buf_sv.size()) { return false; }
      return buf_sv.substr(0, sv.size()) == sv;
   }
   auto endWith(const StringView& sv) -> bool
   {
      auto buf_sv = make_view();
      if (sv.size() > buf_sv.size()) { return false; }
      return buf_sv.substr(buf_sv.size() - sv.size(), sv.size()) == sv;
   }
   auto find(const StringView& sv) -> size_t { return make_view().find(sv); }
   auto rfind(const StringView& sv) -> size_t { return make_view().rfind(sv); }
   auto equal(const StringView& sv) -> bool { return make_view() == sv; }
   void append(const StringView& sv) const
   {
      assert(buf != nullptr);
      if (sv.empty()) { return; }
      buf->append(sv);
   }

   void push_back(char ch) const
   {
      assert(buf != nullptr);
      buf->push_back(ch);
   }

   template <typename... T>
   void formatTo(format_string<T...> fmt, T&&... args)
   {
      assert(buf != nullptr);
      format_to(std::back_inserter(*buf), fmt, std::forward<T>(args)...);
   }

private:
   [[nodiscard]] auto make_view() const -> StringView
   {
      assert(buf != nullptr);
      return StringView{buf->data(), buf->size()};
   }
};

class OutputBuffer : public buffer_helper
{
private:
   Any_t m_ctx;

public:
   OutputBuffer() = default;
   explicit OutputBuffer(buffer_t* buf) : buffer_helper(buf) {}

   void setContext(Any_t v) { m_ctx = std::move(v); }
   [[nodiscard]] auto getContext() const -> const Any_t& { return m_ctx; }
   auto getMutableContext() -> Any_t& { return m_ctx; }
};

// RAII for trigger before and after
struct trigger_helper
{
   output_buf_t* buffer{};
   callback_t*   after{};
   trigger_helper(output_buf_t* buffer_, callback_t* before_,
                  callback_t* after_)
     : buffer(buffer_), after(after_)
   {
      assert(buffer_ != nullptr);
      if (before_ && *before_) { before_->operator()(*buffer_); }
   }
   ~trigger_helper()
   {
      assert(buffer != nullptr);
      if (after && *after) { after->operator()(*buffer); }
      buffer->push_back('\n');
   }
};

#ifdef _MSC_VER
#pragma warning(pop)
#endif

LBLOG_NAMESPACE_END

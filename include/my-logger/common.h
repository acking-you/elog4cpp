#pragma once
#include "fmt/format.h"
#include "fmt/color.h"
#include "fmt/core.h"
#include "fmt/ranges.h"
#include "any.h"
#include "string_view.h"
#include <string>
#include <functional>
#include <ctime>
#include <cstdint>

#define LBLOG_NAMESPACE lblog
#define LBLOG_NAMESPACE_BEGIN namespace LBLOG_NAMESPACE {
#define LBLOG_NAMESPACE_END  }
#define USING_LBLOG using namespace LBLOG_NAMESPACE;
#define USING_LBLOG_DETAIL  using namespace LBLOG_NAMESPACE::detail;

LBLOG_NAMESPACE_BEGIN

enum class Flags : int
{
    kDate = 1 << 0,
    kTime = 1 << 1,
    kLongname = 1 << 2,
    kShortname = 1 << 3,
    kLine = 1 << 4,
    kThreadId = 1 << 5,
    kStdFlags = kDate | kTime | kShortname | kLine
};

enum class Levels : int
{
    kTrace,
    kDebug,
    kInfo,
    kWarn,
    kError,
    kFatal,
    kLevelCount
};

enum class Appenders : int
{
    kConsole,
    kFile
};
struct Config;
struct context;
class buffer_helper;
class OutputBuffer;

using fmt_buffer_t = fmt::memory_buffer;
using buffer_t = OutputBuffer;
using callback_t = std::function<void(buffer_t&)>;
using formatter_t = std::function<void(Config*, context const&, fmt_buffer_t &,Appenders appenderType)>;

template <bool B, typename T = void>
using enable_if_t = typename std::enable_if<B, T>::type;
template <typename... Args>
using format_string = fmt::basic_format_string<char, fmt::type_identity_t<Args>...>;




class buffer_helper{
public:
    fmt_buffer_t* buf;
    buffer_helper() = default;
    buffer_helper(fmt_buffer_t* fmt_buf):buf(fmt_buf){}
    auto startWith(StringView sv) -> bool{
        return make_view().starts_with(sv);
    }
    auto endWith(StringView sv) -> bool{
        return make_view().ends_with(sv);
    }
    auto find(StringView sv) -> size_t{
        return make_view().find(sv);
    }
    auto rfind(StringView sv) -> size_t{
        return make_view().rfind(sv);
    }
    auto equal(StringView sv) -> bool{
        return make_view() == sv;
    }
    template <typename... T>
    void formatTo(format_string<T...> fmt,T&&... args){
        assert(buf!=nullptr);
        format_to(std::back_inserter(*buf),fmt,std::forward<T>(args)...);
    }
private:
    auto make_view()->StringView{
        assert(buf!=nullptr);
       return StringView{buf->data(),buf->size()};
    }
};

class OutputBuffer:public buffer_helper{
private:
    Any_t m_ctx;
public:

   OutputBuffer() = default;
   OutputBuffer(fmt_buffer_t* fmt_buf): buffer_helper(fmt_buf){}

   void setContext(Any_t v){
       m_ctx = std::move(v);
   }
   auto getContext() const ->const Any_t& {
      return m_ctx;
   }
   auto getMutableContext() -> Any_t &{
      return m_ctx;
   }
};

class noncopyable
{
public:
    noncopyable(const noncopyable&) = delete;
    void operator=(const noncopyable&) = delete;

protected:
    noncopyable() = default;
    ~noncopyable() = default;
};
LBLOG_NAMESPACE_END

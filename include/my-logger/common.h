#pragma once
#include "fmt/format.h"
#include "fmt/color.h"
#include "fmt/core.h"
#include "fmt/ranges.h"
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

using fmt_buffer_t = fmt::memory_buffer;
using buffer_t = fmt_buffer_t;
using callback_t = std::function<void(buffer_t&)>;
using formatter_t = std::function<void(Config*, context const&, buffer_t&,Appenders appenderType)>;

template <bool B, typename T = void>
using enable_if_t = typename std::enable_if<B, T>::type;
template <typename... Args>
using format_string = fmt::basic_format_string<char, fmt::type_identity_t<Args>...>;

class buffer_helper{
public:
    fmt_buffer_t& buf;
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
        format_to(std::back_inserter(buf),fmt,std::forward<T>(args)...);
    }
private:
    auto make_view()->StringView{
       return StringView{buf.data(),buf.size()};
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

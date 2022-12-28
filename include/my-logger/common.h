#pragma once
#include <cstdint>
#include <ctime>
#include <functional>
#include <string>

#include "any.h"
#include "fmt/color.h"
#include "fmt/core.h"
#include "fmt/format.h"
#include "fmt/ranges.h"
#include "micros.h"
#include "string_view.h"

LBLOG_NAMESPACE_BEGIN

enum class Flags : int
{
    kDate      = 1 << 0,
    kTime      = 1 << 1,
    kLongname  = 1 << 2,
    kShortname = 1 << 3,
    kLine      = 1 << 4,
    kFuncName  = 1 << 5,
    kThreadId  = 1 << 6,
    kStdFlags  = kDate | kTime | kShortname | kLine | kFuncName
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
using buffer_t     = OutputBuffer;
using callback_t   = std::function<void(buffer_t&)>;
using formatter_t  = std::function<void(Config*, context const&, fmt_buffer_t&,
                                       Appenders appenderType)>;

template <bool B, typename T = void>
using enable_if_t = typename std::enable_if<B, T>::type;
template <typename... Args>
using format_string =
    fmt::basic_format_string<char, fmt::type_identity_t<Args>...>;

struct context
{ // 用于传递每次输出内容的上下文
    int         level;
    int         line;
    const char* short_filename{};
    const char* long_filename{};
    const char* func_name{};
    std::string text;
};

class buffer_helper
{
public:
    fmt_buffer_t* buf{};
    buffer_helper() = default;
    explicit buffer_helper(fmt_buffer_t* fmt_buf) : buf(fmt_buf) {}
    auto startWith(const StringView& sv) -> bool
    {
        return make_view().starts_with(sv);
    }
    auto endWith(const StringView& sv) -> bool
    {
        return make_view().ends_with(sv);
    }
    auto find(const StringView& sv) -> size_t { return make_view().find(sv); }
    auto rfind(const StringView& sv) -> size_t { return make_view().rfind(sv); }
    auto equal(const StringView& sv) -> bool { return make_view() == sv; }
    void append(const StringView& sv) const
    {
        assert(buf != nullptr);
        if (sv.empty())
        {
            return;
        }
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
    auto make_view() const -> StringView
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
    explicit OutputBuffer(fmt_buffer_t* fmt_buf) : buffer_helper(fmt_buf) {}

    void setContext(Any_t v) { m_ctx = std::move(v); }
    auto getContext() const -> const Any_t& { return m_ctx; }
    auto getMutableContext() -> Any_t& { return m_ctx; }
};

// RAII for trigger before and after
struct trigger_helper
{
    buffer_t*   buffer{};
    callback_t* after{};
    trigger_helper(buffer_t* buffer_, callback_t* before_, callback_t* after_)
        : buffer(buffer_), after(after_)
    {
        assert(buffer_ != nullptr);
        if (before_ && *before_)
        {
            before_->operator()(*buffer_);
        }
    }
    ~trigger_helper()
    {
        assert(buffer != nullptr);
        if (after && *after)
        {
            after->operator()(*buffer);
        }
        buffer->push_back('\n');
    }
};

LBLOG_NAMESPACE_END

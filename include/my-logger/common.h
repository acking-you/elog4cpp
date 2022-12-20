#pragma once
#include "fmt/format.h"
#include "fmt/color.h"
#include "fmt/core.h"
#include "fmt/ranges.h"
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

using buffer_t = fmt::memory_buffer;
using callback_t = std::function<void(buffer_t&)>;
using formatter_t = std::function<void(Config*, context const&, buffer_t&,Appenders appenderType)>;

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

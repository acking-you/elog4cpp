#pragma once

#if __cplusplus >= 201703L
#include <string_view>
namespace lblog{
using StringView = std::string_view;
}
#else
#include "string_vew_lite.h"
namespace lblog{
using StringView = nonstd::string_view;
}

#endif
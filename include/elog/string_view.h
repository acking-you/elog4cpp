#pragma once

#if __cplusplus >= 201703L
#include <string_view>
namespace elog {
using StringView = std::string_view;
}
#else
#include "dependencies/string_vew_lite.h"
namespace elog {
using StringView = nonstd::string_view;
}

#endif
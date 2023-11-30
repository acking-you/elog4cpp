#pragma once

#if (__cplusplus && __cplusplus >= 201703L) ||                                 \
  (_MSC_VER && _MSVC_LANG >= 201703L)
#include <any>
namespace elog {
using Any_t = std::any;
using std::any_cast;
}   // namespace elog
#else
#include "dependencies/any_lite.h"
namespace elog {
using Any_t = linb::any;
using linb::any_cast;
}   // namespace elog
#endif

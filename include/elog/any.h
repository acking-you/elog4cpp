#pragma once

#if __cplusplus >= 201703l
#include <any>
namespace elog {
using Any_t = std::any;
using std::any_cast;
} // namespace lblog
#else
#include "any_lite.h"
namespace elog {
using Any_t = linb::any;
using linb::any_cast;
} // namespace lblog
#endif

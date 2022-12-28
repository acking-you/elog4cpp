#pragma once

#if __cplusplus >= 201703l
#include <any>
namespace lblog {
using Any_t = std::any;
using std::any_cast;
} // namespace lblog
#else
#include "any_lite.h"
namespace lblog {
using Any_t = linb::any;
using linb::any_cast;
} // namespace lblog
#endif

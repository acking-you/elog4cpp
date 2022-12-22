#pragma once


#if __cplusplus>=201703l
#include <any>
namespace lblog{
    using Any_t = std::any;
}
#else
#include "any_lite.h"
namespace lblog{
using Any_t = nonstd::any;
}
#endif

#pragma once
#include "common.h"
#include "systemcall_wrapper.h"

LBLOG_NAMESPACE_BEGIN
namespace ProcessInfo
{
    sys::PidType GetPid();
    sys::TidType GetTid();
    std::string GetHostname();
}
LBLOG_NAMESPACE_END

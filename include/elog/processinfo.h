#pragma once
#include "systemcall_wrapper.h"

LBLOG_NAMESPACE_BEGIN
namespace ProcessInfo {
platform::PidType GetPid();
platform::TidType GetTid();
const char*       GetHostname();
}   // namespace ProcessInfo
LBLOG_NAMESPACE_END

#pragma once

namespace elog {
using tid_t = unsigned int;
using pid_t = unsigned int;
namespace ProcessInfo {
pid_t GetPid();
tid_t GetTid();
const char*       GetHostname();
}   // namespace ProcessInfo
}
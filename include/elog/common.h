#pragma once
#include <memory>

namespace elog {
#if __cplusplus == 201103l
template <typename T, typename... Args>
auto make_unique(Args&&... args) -> ::std::unique_ptr<T>
{
   return ::std::unique_ptr<T>(new T(::std::forward<Args>(args)...));
}
#else
using std::make_unique;
#endif

enum Flags {
   kDate      = 1 << 0,
   kTime      = 1 << 1,
   kLongname  = 1 << 2,
   kShortname = 1 << 3,
   kLine      = 1 << 4,
   kFuncName  = 1 << 5,
   kThreadId  = 1 << 6,
   kStdFlags  = kDate | kTime | kShortname | kLine | kFuncName
};

enum Levels { kTrace, kDebug, kInfo, kWarn, kError, kFatal, kLevelCount };

enum Appenders { kConsole, kFile };

inline Flags operator+(Flags lhs, Flags rhs)
{
   return static_cast<Flags>(lhs | rhs);
}

inline bool operator<=(Levels lhs, Levels rhs)
{
   return static_cast<int>(lhs) <= static_cast<int>(rhs);
}

inline bool operator>=(Levels lhs, Levels rhs)
{
   return static_cast<int>(lhs) >= static_cast<int>(rhs);
}

inline bool operator<(Levels lhs, Levels rhs)
{
   return static_cast<int>(lhs) < static_cast<int>(rhs);
}

inline bool operator>(Levels lhs, Levels rhs)
{
   return static_cast<int>(lhs) > static_cast<int>(rhs);
}
}   // namespace elog
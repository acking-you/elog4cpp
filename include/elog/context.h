#pragma once
#include "string_view.h"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4244)
#endif
#ifdef __APPLE__
#include <thread>
#endif

namespace elog {
// The context used to pass each output
struct context
{
   int          level;
   unsigned int tid;
   int          line{};
   const char*  short_filename{};
   const char*  long_filename{};
   const char*  func_name{};
   StringView   text;
};
}   // namespace elog

#ifdef _MSC_VER
#pragma warning(pop)
#endif
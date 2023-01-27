#pragma once
#include "string_view.h"
namespace elog {

inline constexpr int OP_INT(const StringView& sv)
{
   return sv.empty() ? 0
                     : sv[sv.size() - 1] + OP_INT({sv.data(), sv.size() - 1});
}

inline constexpr int operator""_i(const char* op, size_t len)
{
   return OP_INT({op, len});
}
}   // namespace elog
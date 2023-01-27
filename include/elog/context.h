#pragma once
#include "string_view.h"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4244)
#endif
namespace elog {
struct context
{
   // 用于传递每次输出内容的上下文
   int                 level;
   int                 line{};
   const char*         short_filename{};
   const char*         long_filename{};
   const char*         func_name{};
   StringView          text;
   // 计算中间经常可变的位置信息长度，可用于通过memset优化清零
   static unsigned int GetNoTextAndLevelLength(context& ctx)
   {
      static const unsigned int s_ctx_len = (char*)&ctx.text - (char*)&ctx.line;
      return s_ctx_len;
   }
};
}   // namespace elog

#ifdef _MSC_VER
#pragma warning(pop)
#endif
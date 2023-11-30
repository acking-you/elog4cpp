//
// Created by Alone on 2022-12-14.
//

#pragma once
#include <functional>

#include "common.h"
#include "context.h"
#include "output_buffer.h"

namespace elog {
struct Config;
using formatter_t = std::function<void(Config*, SharedContext const&, buffer_t&,
                                       Appenders appenderType)>;
class formatter
{
public:
   static void defaultFormatter(Config* config, SharedContext const& ctx,
                                buffer_t& buf, Appenders appenderType);
   static void colorfulFormatter(Config* config, SharedContext const& ctx,
                                 buffer_t& buf, Appenders appenderType);
   static void jsonFormatter(Config* config, SharedContext const& ctx,
                             buffer_t& buf, Appenders appenderType);
   // use %T:time,%t:tid,%F:filepath,%f:func, %e:error info
   //  %L:long levelText,%l:short levelText,%v:message ,%c color start %C color
   //  end
   static inline formatter_t customFromString(const char* formatString)
   {
      return [formatString](Config* config, SharedContext const& context,
                            buffer_t& buffer, Appenders appender) {
         return customStringFormatter(
           formatString, std::forward<decltype(config)>(config),
           std::forward<decltype(context)>(context),
           std::forward<decltype(buffer)>(buffer),
           std::forward<decltype(appender)>(appender));
      };
   }

private:
   static void customStringFormatter(const char* format_str, Config* config,
                                     SharedContext const& ctx, buffer_t& buf,
                                     Appenders appenderType);
};   // namespace formatter
}   // namespace elog

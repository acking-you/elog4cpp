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
using formatter_t = std::function<void(Config*, context const&, buffer_t&,
                                       Appenders appenderType)>;
namespace formatter {
void defaultFormatter(Config* config, context const& ctx, buffer_t& buf,
                      Appenders appenderType);
void colorfulFormatter(Config* config, context const& ctx, buffer_t& buf,
                       Appenders appenderType);
void jsonFormatter(Config* config, context const& ctx, buffer_t& buf,
                   Appenders appenderType);

void customStringFormatter(const StringView& format_str, Config* config,
                           context const& ctx, buffer_t& buf,
                           Appenders appenderType);
// use %T:time,%t:tid,%F:filepath,%f:func, %e:error info
//  %L:long levelText,%l:short levelText,%v:message ,%c color start %C color end
inline formatter_t customFromString(const StringView& formatString)
{
   return [formatString](Config* config, context const& context,
                         buffer_t& buffer, Appenders appender) {
      return customStringFormatter(formatString,
                                   std::forward<decltype(config)>(config),
                                   std::forward<decltype(context)>(context),
                                   std::forward<decltype(buffer)>(buffer),
                                   std::forward<decltype(appender)>(appender));
   };
}

}   // namespace formatter
}   // namespace elog

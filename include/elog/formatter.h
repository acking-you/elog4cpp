//
// Created by Alone on 2022-12-14.
//

#pragma once
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
// use %T:time,%t:tid,%F:filepath,%f:func, %e:error info
//  %L:long levelText,%l:short levelText,%v:message ,%c color start %C color end
auto customFromString(StringView formatString) -> formatter_t;
}   // namespace formatter
}   // namespace elog
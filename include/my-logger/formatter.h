//
// Created by Alone on 2022-12-14.
//

#pragma once
#include "common.h"

LBLOG_NAMESPACE_BEGIN
namespace formatter {
void defaultFormatter(Config* config, context const& ctx, fmt_buffer_t& buf,
                      Appenders appenderType);
void colorfulFormatter(Config* config, context const& ctx, fmt_buffer_t& buf,
                       Appenders appenderType);
void jsonFormatter(Config* config, context const& ctx, fmt_buffer_t& buf,
                   Appenders appenderType);
auto customFromString(StringView formatString) -> formatter_t;
} // namespace formatter
LBLOG_NAMESPACE_END

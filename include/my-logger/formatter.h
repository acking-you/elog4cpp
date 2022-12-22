//
// Created by Alone on 2022-12-14.
//

#pragma once
#include"common.h"

#include <functional>
#include<string>

LBLOG_NAMESPACE_BEGIN
struct context
{ // 用于传递每次输出内容的上下文
    int level;
    int line;
    const char* short_filename{};
    const char* long_filename{};
    std::string text;
};
struct Config;


namespace formatter
{
    void defaultFormatter(Config* config, context const& ctx, fmt_buffer_t & buf,Appenders appenderType);
    void colorfulFormatter(Config* config, context const& ctx, fmt_buffer_t & buf,Appenders appenderType);
    void jsonFormatter(Config* config, context const& ctx, fmt_buffer_t& buf,Appenders appenderType);
}
LBLOG_NAMESPACE_END

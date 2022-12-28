//
// Created by Alone on 2022-9-22.
//
#pragma once
#include "formatter.h"

LBLOG_NAMESPACE_BEGIN

struct Config
{
    callback_t log_before;
    callback_t log_after;
    formatter_t log_formatter = formatter::defaultFormatter; // 默认的formatter
    Flags  log_flag  = Flags::kStdFlags;
    Levels log_level = Levels::kDebug;

    auto setFlag(Flags flag) -> Config&;

    auto setLevel(Levels level) -> Config&;

    auto setBefore(callback_t function) -> Config&;

    auto setAfter(callback_t function) -> Config&;

    auto setFormatter(formatter_t formatter) -> Config&;
};

struct GlobalConfig : Config
{
    int         log_rollSize      = 20 * 1024 * 1024; // 默认rollSize为20m
    int         log_flushInterval = 3; // 默认3s刷新一次日志
    const char* log_filepath      = nullptr;
    bool        log_console       = true;

    static auto instance() -> GlobalConfig&;

    auto setFilepath(const char* basedir) -> GlobalConfig&;

    auto setRollSize(int size) -> GlobalConfig&;

    auto setFlushInterval(int flushInterval) -> GlobalConfig&;

    auto enableConsole(bool s) -> GlobalConfig&;
};
LBLOG_NAMESPACE_END

//
// Created by Alone on 2022-9-22.
//
#pragma once

#include "common.h"
#include "formatter.h"

#include <functional>

#include <cstdio>

LBLOG_NAMESPACE_BEGIN

#define F_EXPAND(x) x
#define F_GET_MICRO(v1,v2,v3,v4,v5,v6,v7,v8,NAME,...) NAME
#define F_PASTE(...) F_EXPAND(F_GET_MICRO(__VA_ARGS__, \
        F_SET7,                                        \
		F_SET6,                                              \
		F_SET5,                                              \
		F_SET4,                                              \
		F_SET3,                                              \
		F_SET2,                                              \
		F_SET1,                                              \
		F_SET0\
)(__VA_ARGS__))

#define F_SET1(func,v1) func(v1)
#define F_SET2(func,v1,v2) F_SET1(func,v1) F_SET1(func,v2)
#define F_SET3(func,v1,v2,v3) F_SET1(func,v1) F_SET2(func,v2,v3)
#define F_SET4(func,v1,v2,v3,v4) F_SET1(func,v1) F_SET3(func,v2,v3,v4)
#define F_SET5(func,v1,v2,v3,v4,v5) F_SET1(func,v1) F_SET4(func,v2,v3,v4,v5)
#define F_SET6(func,v1,v2,v3,v4,v5,v6) F_SET1(func,v1) F_SET5(func,v2,v3,v4,v5,v6)
#define F_SET7(func,v1,v2,v3,v4,v5,v6,v7) F_SET1(func,v1) F_SET6(func,v2,v3,v4,v5,v6,v7)
#define CAST_INT(x) static_cast<int>(x)
#define CAST_FLAG(x) static_cast<LBLOG_NAMESPACE::Flags>(x)

#define FLAG_ADD(x) |CAST_INT(x)
#define FLAGS(v1,...) CAST_FLAG(CAST_INT(v1) F_EXPAND(F_PASTE(FLAG_ADD,##__VA_ARGS__)))



struct Config
{
    callback_t log_before;
    callback_t log_after;
    formatter_t log_formatter = formatter::defaultFormatter; //默认的formatter
    Flags log_flag = Flags::kStdFlags;
    Levels log_level = Levels::kDebug;

    auto setFlag(Flags flag) -> Config&;

    auto setLevel(Levels level) -> Config&;

    auto setBefore(callback_t function) -> Config&;

    auto setAfter(callback_t function) -> Config&;

    auto setFormatter(formatter_t formatter) -> Config&;
};

struct GlobalConfig : Config
{
    int log_rollSize = 20 * 1024 * 1024; //默认rollSize为20m
    int log_flushInterval = 3; //默认3s刷新一次日志
    const char* log_filepath = nullptr;
    bool log_console = true;

    static auto instance() -> GlobalConfig&;

    auto setFilepath(const char* basedir) -> GlobalConfig&;

    auto setRollSize(int size) -> GlobalConfig&;

    auto setFlushInterval(int flushInterval) -> GlobalConfig&;

    auto enableConsole(bool s) -> GlobalConfig&;
};
LBLOG_NAMESPACE_END

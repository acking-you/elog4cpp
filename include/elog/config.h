//
// Created by Alone on 2022-9-22.
//
#pragma once
#include "formatter.h"

namespace elog {
// Config must have static lifetime
struct Config
{
   Flags       log_flag  = Flags::kStdFlags;
   Levels      log_level = Levels::kDebug;
   const char* log_name  = nullptr;
   callback_t  log_before;
   callback_t  log_after;
   formatter_t log_formatter = formatter::defaultFormatter;   // 默认的formatter
   auto setFlag(Flags flag) -> Config&;

   auto setName(const char* name) -> Config&;

   auto setLevel(Levels level) -> Config&;

   auto setBefore(callback_t const& function) -> Config&;

   auto setAfter(callback_t const& function) -> Config&;

   auto setFormatter(formatter_t const& formatter) -> Config&;

   [[nodiscard]] auto level() const -> Levels { return log_level; }
};

struct GlobalConfig : Config
{
   int         log_rollSize      = 20 * 1024 * 1024;   // 默认rollSize为20m
   int         log_flushInterval = 3;   // 默认3s刷新一次日志
   const char* log_filepath      = nullptr;
   bool        log_console       = true;

   static auto Get() -> GlobalConfig&;

   auto setFilepath(const char* basedir) -> GlobalConfig&;

   auto setRollSize(int size) -> GlobalConfig&;

   auto setFlushInterval(int flushInterval) -> GlobalConfig&;

   auto enableConsole(bool s) -> GlobalConfig&;

   auto loadToJSON(const char* filename) -> GlobalConfig&;
   auto loadFromJSON(const char* filename) -> GlobalConfig&;
};

}   // namespace elog
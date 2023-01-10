//
// Created by Alone on 2022-9-23.
//
#include "elog/config.h"

#include <utility>

USING_LBLOG

auto GlobalConfig::instance() -> GlobalConfig&
{
    static GlobalConfig instance;
    return instance;
}
auto GlobalConfig::setRollSize(int size) -> GlobalConfig&
{
    log_rollSize = size;
    return *this;
}
auto GlobalConfig::setFlushInterval(int flushInterval) -> GlobalConfig&
{
    log_flushInterval = flushInterval;
    return *this;
}
auto GlobalConfig::setFilepath(const char* basedir) -> GlobalConfig&
{
    this->log_filepath = basedir;
    return *this;
}
auto GlobalConfig::enableConsole(bool s) -> GlobalConfig&
{
    this->log_console = s;
    return *this;
}

auto Config::setFlag(Flags flag) -> Config&
{
    this->log_flag = flag;
    return *this;
}
auto Config::setLevel(Levels level) -> Config&
{
    this->log_level = level;
    return *this;
}

auto Config::setBefore(std::function<void(buffer_t&)> function) -> Config&
{
    this->log_before = std::move(function);
    return *this;
}
auto Config::setAfter(std::function<void(buffer_t&)> function) -> Config&
{
    this->log_after = std::move(function);
    return *this;
}

auto Config::setFormatter(formatter_t formatter) -> Config&
{
    log_formatter = std::move(formatter);
    return *this;
}
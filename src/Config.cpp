//
// Created by Alone on 2022-9-23.
//
#include "Config.h"

using namespace lblog::detail;


Config &Config::instance() {
    static Config instance;
    return instance;
}
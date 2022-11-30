//
// Created by Alone on 2022-9-23.
//

#include "AsyncLogging.h"
#include "Config.h"


int main(){

    lblog::detail::AsyncLogging logging("../../log",lblog::detail::Config::instance().rollSize());
    std::string s("fddkfjdskafsadfsdafdsfsdafasffdsafdsaf");
    logging.append(s.data(),s.size());
}
#include <memory>
#include "common_func.h"
#include "gtest/gtest.h"
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/async.h"
#include "my-logger/logger.h"

auto get_spdlog_sync(){
    auto logger = spdlog::basic_logger_mt("sync_log", "../../log/test.log");
    logger->set_pattern("[%Y-%m-%d %H:%M:%S.%f %z][tid%t][%@,%!][%n][%l]: %v");
    return logger;
}
inline auto get_spdlog_async(){
    auto logger = spdlog::basic_logger_mt<spdlog::async_factory>("async_log", "../../log/test.log");
    logger->set_pattern("[%Y-%m-%d %H:%M:%S.%f %z][thread %t][%n][%l]: %v");
    return logger;
}

extern const char* test_line;
extern void set_config();


TEST(bench,one_thread_spd_async){
    const int n = 1e4;
    auto logger = get_spdlog_async();
    bench_start("spdlog",[&](){
        test_one_thread(n,[&](){
           logger->info(test_line);
        });
    });
    set_config();
    bench_start("my-logger",[&](){
        test_one_thread(n,[&](){
            LB_INFO(test_line);
        });
    });
}

TEST(bench,one_thread_spd_sync){
    const int n = 1e4;
    auto logger = get_spdlog_sync();
    bench_start("spdlog",[&](){
        test_one_thread(n,[&](){
            logger->info(test_line);
        });
    });
    set_config();
    bench_start("my-logger",[&](){
        test_one_thread(n,[&](){
            LB_INFO(test_line);
        });
    });
}

TEST(bench,multi_thread_spd_async){
    const int n = 1e4;
    auto logger = get_spdlog_async();
    bench_start("spdlog",[&](){
        test_multi_thread(n,[&](){
            logger->info(test_line);
        });
    });
    set_config();
    bench_start("my-logger",[&](){
        test_multi_thread(n,[&](){
            LB_INFO(test_line);
        });
    });
}

TEST(bench,multi_thread_spd_sync){
    const int n = 1e5;
    auto logger = get_spdlog_sync();
    bench_start("spdlog",[&](){
        test_multi_thread(n,[&](){
            logger->info(test_line);
        });
    });
    set_config();
    bench_start("my-logger",[&](){
        test_multi_thread(n,[&](){
            LB_INFO(test_line);
        });
    });
}
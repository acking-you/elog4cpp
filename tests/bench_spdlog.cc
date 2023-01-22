#include "bench_interface.h"
#include "common_func.h"
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include "spdlog/async.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"

std::shared_ptr<spdlog::logger> s_log_sync;
std::shared_ptr<spdlog::logger> s_log_async;

void before_third_part_bench()
{
   s_log_sync = spdlog::basic_logger_mt("sync_log", "../../log/test.log");
   s_log_sync->set_pattern(
     "[%Y-%m-%d %H:%M:%S.%f %z][tid%t][%@,%!][%n][%l]: %v");
   s_log_async = spdlog::basic_logger_mt<spdlog::async_factory>(
     "async_log", "../../log/test.log");
   s_log_async->set_pattern("[%Y-%m-%d %H:%M:%S.%f %z][tid:%t][%n][%l]: %v");
}

void one_thread_async_third_part(int bench_n, int test_n)
{
   bench_start(
     "spdlog:one_thread_async",
     [&]() {
        test_one_thread(test_n, [&]() { s_log_async->info(OUTPUT_TEXT); });
     },
     bench_n);
}

void one_thread_sync_third_part(int bench_n, int test_n)
{
   bench_start(
     "spdlog:one_thread_sync",
     [&]() {
        test_one_thread(test_n, [&]() { s_log_sync->info(OUTPUT_TEXT); });
     },
     bench_n);
}

void multi_thread_sync_third_part(int bench_n, int test_n)
{
   bench_start(
     "spdlog:multi_thread_sync",
     [&]() {
        test_multi_thread(test_n, [&]() { s_log_sync->info(OUTPUT_TEXT); });
     },
     bench_n);
}

void multi_thread_async_third_part(int bench_n, int test_n)
{
   bench_start(
     "spdlog:multi_thread_async",
     [&]() {
        test_multi_thread(test_n, [&]() { s_log_async->info(OUTPUT_TEXT); });
     },
     bench_n);
}
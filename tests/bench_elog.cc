#include "bench_interface.h"
#include "common_func.h"
#define ENABLE_ELG_LOG
#include "elog/logger.h"

using namespace elog;

void before_bench()
{
   GlobalConfig::Get()
     .loadFromJSON("../../config.json")
     .setFilepath(PROJECT_ROOT "tests/test_log/")
     .enableConsole(false);
}

void one_thread_async(int bench_n, int test_n)
{
   bench_start(
     "elog:one_thread",
     [&]() { test_one_thread(test_n, []() { ELG_INFO(OUTPUT_TEXT); }); },
     bench_n);
}

void one_thread_sync(int bench_n, int test_n)
{
   bench_start(
     "elog:one_thread",
     [&]() { test_one_thread(test_n, []() { ELG_INFO(OUTPUT_TEXT); }); },
     bench_n);
}

void multi_thread_sync(int bench_n, int test_n)
{
   bench_start(
     "elog:multi_thread",
     [&]() { test_multi_thread(test_n, []() { ELG_INFO(OUTPUT_TEXT); }); },
     bench_n);
}

void multi_thread_async(int bench_n, int test_n)
{
   bench_start(
     "elog:multi_thread",
     [&]() { test_multi_thread(test_n, []() { ELG_INFO(OUTPUT_TEXT); }); },
     bench_n);
}

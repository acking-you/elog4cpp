#include "bench_interface.h"
#include "common_func.h"
#include "elog/logger.h"

using namespace elog;

Log* s_log{};

void before_bench()
{
   GlobalConfig::Get().loadFromJSON("../../config.json");
   static Log instance = Log(elog::kInfo);
   s_log               = &instance;
}

void one_thread_async(int bench_n, int test_n)
{
   bench_start(
     "elog:one_thread",
     [&]() { test_one_thread(test_n, []() { s_log->println(OUTPUT_TEXT); }); },
     bench_n);
}

void one_thread_sync(int bench_n, int test_n)
{
   bench_start(
     "elog:one_thread",
     [&]() { test_one_thread(test_n, []() { s_log->println(OUTPUT_TEXT); }); },
     bench_n);
}

void multi_thread_sync(int bench_n, int test_n)
{
   bench_start(
     "elog:multi_thread",
     [&]() { test_multi_thread(test_n, []() { Log::info(OUTPUT_TEXT); }); },
     bench_n);
}

void multi_thread_async(int bench_n, int test_n)
{
   bench_start(
     "elog:multi_thread",
     [&]() { test_multi_thread(test_n, []() { Log::warn(OUTPUT_TEXT); }); },
     bench_n);
}

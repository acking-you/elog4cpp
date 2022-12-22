#include <chrono>
#include <thread>
#include <vector>

#include "gtest/gtest.h"
#include "my-logger/logger.h"
#include "nanobench.h"

using namespace lblog;
const char* log_dir = "../../log/";
// 打印多少行
int         test_n  = 2e5;
const char* test_line =
    "abcdefeajalfmdafijkjsfasfdsfdnfdsaf,dasfd,smfd,fafdsfdsfaf";

struct Timer
{
    std::chrono::time_point<std::chrono::high_resolution_clock> start_point;
    void    start() { start_point = std::chrono::high_resolution_clock::now(); }
    int64_t end() const
    {
        auto end_point = std::chrono::high_resolution_clock::now();
        return std::chrono::time_point_cast<std::chrono::nanoseconds>(end_point)
                   .time_since_epoch()
                   .count() -
               std::chrono::time_point_cast<std::chrono::nanoseconds>(
                   start_point)
                   .time_since_epoch()
                   .count();
    }
};

void set_config()
{
    GlobalConfig::instance()
        .enableConsole(false)
        .setFilepath(log_dir)
        .setFormatter(formatter::defaultFormatter)
        .setFlag(FLAGS(Flags::kStdFlags, Flags::kShortname, Flags::kThreadId));
}

void set_timer_config()
{
    GlobalConfig::instance()
        .enableConsole(false)
        .setFilepath(log_dir)
        .setFormatter(formatter::defaultFormatter)
        .setFlag(FLAGS(Flags::kStdFlags, Flags::kShortname, Flags::kThreadId))
        .setBefore([](auto& bf) {
            bf.setContext(Timer{});
            auto& tm = nonstd::any_cast<Timer>(bf.getMutableContext());
            tm.start();
        })
        .setAfter([](auto& bf) {
            auto& tm      = nonstd::any_cast<Timer>(bf.getMutableContext());
            int   consume = tm.end();
            bf.formatTo("-----format takes:{}ns", consume);
        });
}

TEST(test, timer_logger)
{
    set_timer_config();
    for (int i = 0; i < 10; i++)
    {
        Timer tm;
        tm.start();
        LB_INFO("test1");
        LB_WARN("sum of time:{}ns", tm.end());
    }
}

void bench_start_(const char*                  bench_name,
                  const std::function<void()>& bench_func)
{
    ankerl::nanobench::Bench().run(bench_name, bench_func);
}

void test_multi_thread_performance()
{
    set_config();
    std::thread th1{[]() {
        for (int i = 0; i < test_n; i++) LB_WARN("data:{},thread1", test_line);
    }};
    std::thread th2{[]() {
        for (int i = 0; i < test_n; i++) LB_INFO("data:{},thread2", test_line);
    }};
    std::thread th3{[]() {
        for (int i = 0; i < test_n; i++) LB_ERROR("data:{},thread3", test_line);
    }};
    std::thread th4{[]() {
        for (int i = 0; i < test_n; i++) LB_INFO("data:{},thread4", test_line);
    }};
    std::thread th5{[]() {
        for (int i = 0; i < test_n; i++) LB_INFO("data:{},thread5", test_line);
    }};
    th1.join();
    th2.join();
    th3.join();
    th4.join();
    th5.join();
}

void test_one_thread_performance()
{
    set_config();
    for(int i=0;i<test_n*5;i++) LB_INFO("data:{}",test_line);
}

TEST(bench,nanobench)
{
    bench_start_("multi_thread",test_multi_thread_performance);
    bench_start_("single_thread",test_one_thread_performance);
}
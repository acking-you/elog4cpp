#include <chrono>
#include <thread>
#include <vector>

#include "common_func.h"
#include "elog/logger.h"
#include "gtest/gtest.h"
#include "nanobench.h"

using namespace elog;
const char* log_dir = "../../log/";
// 打印多少行
int         test_n  = 2e4;
const char* test_line =
  "abcdefeajalfmdafijkjsfasfdsfdnfdsaf,dasfd,smfd,fafdsfdsfaf";

#define LB_INFO Log(elog::kInfo).printf

struct Timer
{
   std::chrono::time_point<std::chrono::high_resolution_clock> startPoint;
   void    start() { startPoint = std::chrono::high_resolution_clock::now(); }
   int64_t end() const
   {
      auto endPoint = std::chrono::high_resolution_clock::now();
      return std::chrono::time_point_cast<std::chrono::nanoseconds>(endPoint)
               .time_since_epoch()
               .count() -
             std::chrono::time_point_cast<std::chrono::nanoseconds>(startPoint)
               .time_since_epoch()
               .count();
   }
};

void set_config()
{
   GlobalConfig::Get()
     .enableConsole(false)
     .setFilepath(log_dir)
     .setFormatter(formatter::jsonFormatter)
     .setFlag(kStdFlags + kThreadId);
}

void set_timer_config()
{
   auto customFormatter =
     formatter::customFromString("[%T][%t][\"%F\"][%f][%c%l%C]: %c[%v]%C");
   GlobalConfig::Get()
     .enableConsole(true)
     .setFilepath("../../log/")
     .setFormatter(customFormatter)
     .setFlag(Flags::kStdFlags + Flags::kShortname + Flags::kThreadId)
     .setBefore([](buffer_t& bf) {
        bf.setContext(Timer{});
        auto& tm = any_cast<Timer&>(bf.getMutableContext());
        tm.start();
     })
     .setAfter([](buffer_t& bf) {
        auto& tm      = any_cast<Timer&>(bf.getMutableContext());
        int   consume = tm.end();
        bf.formatTo("-----format takes:{}ns", consume);
     });
}

TEST(test, timer_logger)
{
   set_timer_config();
   auto info = Log(elog::kInfo);
   auto warn = Log(elog::kWarn);
   for (int i = 0; i < 10; i++)
   {
      Timer tm;
      tm.start();
      info.with().println("test1", "test2", "test3",
                          std::vector<int>{32, 32, 23, 3});
      warn.printf("sum of time:{}ns", tm.end());
   }
}

void test_multi_thread_performance()
{
   auto        warn = Log(elog::kWarn);
   std::thread th1{[a = std::move(warn)]() {
      for (int i = 0; i < test_n; i++)
         a.with().printf("data:{},thread1", test_line);
   }};
   std::thread th2{[]() {
      auto info = Log(elog::kInfo);
      for (int i = 0; i < test_n; i++)
         info.with().printf("data:{},thread2", test_line);
   }};
   std::thread th3{[]() {
      auto error = Log(elog::kError);
      for (int i = 0; i < test_n; i++)
         error.with().printf("data:{},thread3", test_line);
   }};
   std::thread th4{[&]() {
      auto info = Log(elog::kInfo);
      for (int i = 0; i < test_n; i++)
         info.with().printf("data:{},thread4", test_line);
   }};
   std::thread th5{[&]() {
      auto info = Log(elog::kInfo);
      for (int i = 0; i < test_n; i++)
         info.with().printf("data:{},thread5", test_line);
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
   for (int i = 0; i < test_n * 5; i++) LB_INFO("data:{}", test_line);
}

TEST(bench, nanobench)
{
   GlobalConfig::Get().loadFromJSON("../../config.json");
   bench_start(
     "bench multi", []() { test_multi_thread_performance(); }, 10);
}
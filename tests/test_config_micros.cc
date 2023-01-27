#include <chrono>
#include <vector>

#include "elog/logger.h"
#include "doctest/doctest.h"
#include "nanobench.h"

using namespace elog;

struct Timer
{
   std::chrono::time_point<std::chrono::high_resolution_clock> startPoint;
   void    start() { startPoint = std::chrono::high_resolution_clock::now(); }
   [[nodiscard]] int64_t end() const
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

void set_console_json_config()
{
   GlobalConfig::Get()
     .enableConsole(false)
     .setFilepath(PROJECT_ROOT "log/")
     .setLevel(elog::kInfo)
     .setFormatter(formatter::jsonFormatter)
     .setFlag(kStdFlags + kThreadId);
}

void set_console_colorful_config()
{
   GlobalConfig::Get()
     .enableConsole(false)
     .setFilepath(PROJECT_ROOT "log/")
     .setFormatter(formatter::colorfulFormatter)
     .setFlag(kStdFlags + kThreadId);
}

void set_custom_config()
{
   GlobalConfig::Get()
     .enableConsole(false)
     .setFilepath(PROJECT_ROOT "log/")
     .setFormatter(
       formatter::customFromString("[%n][%T][tid:%t][%L][%F][%f]: %v"))
     .setFlag(kStdFlags + kThreadId);
}

void set_timer_callback_and_load_config()
{
   GlobalConfig::Get()
     .loadFromJSON(PROJECT_ROOT "config.json")
     .setBefore([](output_buf_t& bf) {
        bf.setContext(Timer{});
        auto& tm = any_cast<Timer&>(bf.getMutableContext());
        tm.start();
     })
     .setAfter([](output_buf_t& bf) {
        auto& tm      = any_cast<Timer&>(bf.getMutableContext());
        auto  consume = tm.end();
        bf.formatTo("-----formatter takes:{}ns", consume);
     });
}

void test_timer_callback_and_load_config()
{
   set_timer_callback_and_load_config();
   auto info = Log(elog::kInfo);
   auto warn = Log(elog::kWarn);
   for (int i = 0; i < 10; i++)
   {
      Timer tm;
      tm.start();
      info.println("test1", "test2", "test3", std::vector<int>{3243, 242, 324});
      warn.printf("sum of time:{}ns", tm.end());
   }
}

void test_console_json_config()
{
   set_console_json_config();
   elog::Log::info("hello ejson4cpp");
}

void test_custom_config()
{
   set_custom_config();
   elog::Log::info("hello ejson4cpp");
}

void test_console_colorful_config()
{
   set_console_colorful_config();
   elog::Log::info("hello ejson4cpp");
}

void test_and_bench_CHECK()
{
   const char* data = "n32432432dsfdsafrerm,m,sdfsfsadfjihaodajfladsjfdskaffds";
   GlobalConfig::Get().enableConsole(false).setFilepath(PROJECT_ROOT "log/");
   ankerl::nanobench::Bench()
     .minEpochIterations(1000)
     .run("bench CHECK", [&]() { ELG_CHECK(1 == 1).info("{}", data); })
     .doNotOptimizeAway(data);
}

TEST_CASE("test config")
{
   // test config with callbac
   test_timer_callback_and_load_config();
   // test json formatter
   test_console_json_config();
   // test custom formatter
   test_custom_config();
   // test colorful formatter
   test_console_colorful_config();
   // test CHECK micro
   test_and_bench_CHECK();
}

TEST_CASE("test micros")
{
   int a = 1, b = 1;
   ELG_CHECK_EQ(a, b);
   auto* ptr = new elog::context;
   ptr->text = "abc";
   auto* p   = ELG_CHECK_NOTNULL(ptr);
   REQUIRE_EQ(p->text, "abc");
   ptr = nullptr;
   ELG_CHECK_NE(ptr, p);
   a = 0;
   ELG_CHECK_LT(a, b);
   a = 1;
   ELG_CHECK_LE(a, b);
   ELG_CHECK_GE(a, b);
   a = 2;
   ELG_CHECK_GT(a, b);
   delete p;
}
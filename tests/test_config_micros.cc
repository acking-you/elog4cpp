#include <chrono>
#include <vector>

#define ENABLE_ELG_CHECK
#define ENABLE_ELG_LOG
#include "doctest/doctest.h"
#include "elog/logger.h"
#include "nanobench.h"

using namespace elog;

struct Timer
{
   std::chrono::time_point<std::chrono::high_resolution_clock> startPoint;
   void start() { startPoint = std::chrono::high_resolution_clock::now(); }
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
     .setFilepath(PROJECT_ROOT "tests/test_log/")
     .setLevel(elog::kInfo)
     .setFormatter(formatter::jsonFormatter)
     .setFlag(kStdFlags + kThreadId);
}

void set_console_colorful_config()
{
   GlobalConfig::Get()
     .enableConsole(false)
     .setFilepath(PROJECT_ROOT "tests/test_log/")
     .setFormatter(formatter::colorfulFormatter)
     .setFlag(kStdFlags + kThreadId);
}

void set_custom_config()
{
   GlobalConfig::Get()
     .enableConsole(false)
     .setFilepath(PROJECT_ROOT "tests/test_log/")
     .setFormatter(
       formatter::customFromString("[%n][%T][tid:%t][%L][%F][%f]: %v"))
     .setFlag(kStdFlags + kThreadId);
}

void set_timer_callback_and_load_config()
{
   GlobalConfig::Get()
     .loadFromJSON(PROJECT_ROOT "config.json")
     .setFilepath(PROJECT_ROOT "tests/test_log/")
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

void test_localConfig()
{
   // register local config
   auto config           = elog::make_unique<Config>();
   config->log_before    = [](output_buf_t& out) { out.append("before"); };
   config->log_after     = [](output_buf_t& out) { out.append("after"); };
   config->log_flag      = kStdFlags + kThreadId;
   config->log_name      = "test_local";
   config->log_level     = kInfo;
   config->log_formatter = formatter::colorfulFormatter;
   Log::registerConfigByName("test_local", std::move(config));

   // create Log by localConfig
   auto log = Log(kInfo, "test_local");
   for (int i = 0; i < 10; i++) log.println("hello world");
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

TEST_SUITE_BEGIN("test config&micros");

TEST_CASE("test config")
{
   // test config with callback
   test_timer_callback_and_load_config();
   // test local config
   test_localConfig();
   // test json formatter
   test_console_json_config();
   // test custom formatter
   test_custom_config();
   // test colorful formatter
   test_console_colorful_config();
}

TEST_CASE("test log micros")
{
   GlobalConfig::Get().setLevel(elog::kTrace);
   ELG_TRACE("hello {}", "world");
   ELG_DEBUG("hello {}", "world");
   ELG_INFO("hello {}", "world");
   ELG_WARN("hello {}", "world");
   ELG_ERROR("hello {}", "world");
   CHECK_THROWS_AS(ELG_FATAL("hello {}", "world");, std::runtime_error);
}

TEST_CASE("test check micros")
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

TEST_SUITE_END;
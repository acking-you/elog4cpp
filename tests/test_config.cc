#include <chrono>
#include <vector>

#include "common_func.h"
#include "elog/logger.h"
#include "gtest/gtest.h"

using namespace elog;
const char* log_dir = "../../log/";

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
        auto   consume = tm.end();
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
      //      info.with().println("test1", "test2", "test3",
      //                          std::vector<int>{32, 32, 23, 3});
      warn.printf("sum of time:{}ns", tm.end());
   }
}
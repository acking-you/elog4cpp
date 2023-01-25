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
   GlobalConfig::Get().loadFromJSON("../../config.json")
     .setBefore([](output_buf_t& bf) {
        bf.setContext(Timer{});
        auto& tm = any_cast<Timer&>(bf.getMutableContext());
        tm.start();
     })
     .setAfter([](output_buf_t& bf) {
        auto& tm      = any_cast<Timer&>(bf.getMutableContext());
        auto   consume = tm.end();
        bf.formatTo("-----formatter takes:{}ns", consume);
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
      info.println("test1", "test2", "test3",std::vector<int>{3243,242,324});
      warn.printf("sum of time:{}ns", tm.end());
   }
}

TEST(test,loadFromJSON){
   elog::GlobalConfig::Get().loadFromJSON("../../config.json");
//   elog::Log::info("test1 {}","hello world");
//   elog::Log::Get(elog::kError).with().printf("test2 {}",234);
   elog::Log::info("你好");
}
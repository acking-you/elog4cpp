#include <elog/logger.h>
#include <gtest/gtest.h>
#include <nanobench.h>

TEST(test, micros)
{
   int a = 1, b = 1;
   ELG_CHECK_EQ(a, b);
   auto* ptr = new elog::context;
   ptr->text = "abc";
   auto* p   = ELG_CHECK_NOTNULL(ptr);
   EXPECT_EQ(p->text, "abc");
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
using namespace elog;

TEST(test, CHECK)
{
   const char*        data ="n32432432dsfdsafrerm,m,sdfsfsadfjihaodajfladsjfdskaffds";
   elog::GlobalConfig::Get().loadFromJSON("../../config.json");
   ankerl::nanobench::Bench()
     .minEpochIterations(1000)
     .run("bench CHECK", [&]() { ELG_CHECK(1 == 1).info("{}",data); })
     .doNotOptimizeAway(data);
}

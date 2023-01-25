#include <elog/logger.h>
#include <fmt/ostream.h>

#include <vector>

#include "gtest/gtest.h"

using namespace elog;

TEST(common_test, buffer_helper)
{
   buffer_t buffer;
   buffer.push_back('a');
   elog::buffer_helper helper{&buffer};
   helper.formatTo("{}b{}", "a", 123);

   EXPECT_EQ(to_string(buffer), "aab123");
}

TEST(common_test, OutputBuffer)
{
   {
      buffer_t buffer;
      buffer.push_back('a');
      output_buf_t output(&buffer);
      output.formatTo("{}b{}", "a", 123);
      EXPECT_EQ(to_string(buffer), "aab123");
   }
   {
      buffer_t buffer;
      output_buf_t out(&buffer);
      out.setContext("String");
      EXPECT_EQ(any_cast<const char*>(out.getContext()), "String");
      out.setContext(324);
      EXPECT_EQ(any_cast<int>(out.getContext()), 324);
   }
}

TEST(common_test, const_util)
{
   EXPECT_EQ(elog::detail::GetStrLen("abc"), 3);
   EXPECT_EQ(elog::detail::GetStrLen("a"), 1);
   EXPECT_EQ(elog::detail::GetStrLen("ab"), 2);

   EXPECT_EQ(elog::detail::GetShortName("a/b/c"), std::string("c"));
   EXPECT_EQ(elog::detail::GetShortName("/"), std::string(""));
   EXPECT_EQ(elog::detail::GetShortName("asfds/afsa/saf"), std::string("saf"));
   EXPECT_EQ(elog::detail::GetShortName("asfds\\afsa\\sdf"), std::string("sdf"));
}
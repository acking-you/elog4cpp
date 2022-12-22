#include<vector>
#include "gtest/gtest.h"
#include "my-logger/common.h"





using namespace lblog;

TEST(common_test,buffer_helper){
    buffer_t buffer;
    buffer.push_back('a');
    lblog::buffer_helper helper{buffer};
    helper.formatTo("{}b{}","a",123);

    EXPECT_EQ(to_string(buffer),"aab123");
}

#include<vector>
#include "gtest/gtest.h"
#include "elog/common.h"





using namespace elog;

TEST(common_test,buffer_helper){
    fmt_buffer_t buffer;
    buffer.push_back('a');
    elog::buffer_helper helper{&buffer};
    helper.formatTo("{}b{}","a",123);

    EXPECT_EQ(to_string(buffer),"aab123");
}

TEST(common_test,OutputBuffer){
    {
        fmt_buffer_t buffer;
        buffer.push_back('a');
        buffer_t output(&buffer);
        output.formatTo("{}b{}","a",123);
        EXPECT_EQ(to_string(buffer),"aab123");
    }
    {
        fmt_buffer_t buffer;
        buffer_t out(&buffer);
        out.setContext("String");
        EXPECT_EQ(any_cast<const char*>(out.getContext()),"String");
        out.setContext(324);
        EXPECT_EQ(any_cast<int>(out.getContext()),324);
    }
}
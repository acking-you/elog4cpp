#include "common_func.h"
#include <sstream>
#include <nanobench.h>
#include <fmt/format.h>
#include <gtest/gtest.h>


const int bench_n = 10000;
const char* str="abcdefghi1234567890";

constexpr size_t const_str_size(const char* s,size_t i = 0){
    return s[i]?const_str_size(s,i+1)+1:0;
}

void bench_membuffer(){
    fmt::memory_buffer buffer;
    for(int i=0;i<bench_n;i++){
        buffer.append(str,str+ const_str_size(str));
    }
//    auto out = to_string(buffer);
//    (void)out;
}

void bench_string(){
    std::string buffer;
    for(int i=0;i<bench_n;i++){
        buffer.append(str,const_str_size(str));
    }
    (void)buffer;
}

void bench_ostream(){
    std::ostringstream buffer;
    for(int i=0;i<bench_n;i++){
        buffer.write(str,const_str_size(str));
    }
//    auto out = buffer.str();
//    (void)out;
}




TEST(bench,membuffer_string_stream){
    bench_start("mem_buffer",bench_membuffer);
    bench_start("std::string",bench_string);
    bench_start("ostream",bench_ostream);
}
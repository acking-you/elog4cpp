#pragma once
#include "nanobench.h"
#include <thread>
#include <functional>

inline void bench_start(const char* bench_name,const std::function<void()>&bench_func,int num = 10){
    ankerl::nanobench::Bench().minEpochIterations(num).run(bench_name,bench_func);
}

inline void test_multi_thread(const int test_n,const std::function<void()>&logger_func)
{
    std::thread th1{ [&](){for (int i = 0; i < test_n; i++)logger_func();}};
    std::thread th2{ [&](){for (int i = 0; i < test_n; i++)logger_func();}};
    std::thread th3{ [&](){for (int i = 0; i < test_n; i++)logger_func();}};
    std::thread th4{ [&](){for (int i = 0; i < test_n; i++) logger_func();}};
    std::thread th5{ [&](){for (int i = 0; i < test_n; i++)logger_func();}};
    th1.join();
    th2.join();
    th3.join();
    th4.join();
    th5.join();
}

inline void test_one_thread(const int test_n,const std::function<void()>&logger_func)
{
    for(int i=0;i<test_n;i++) logger_func();
}
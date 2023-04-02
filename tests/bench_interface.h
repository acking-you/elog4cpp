#pragma once


#define OUTPUT_TEXT "abcdefasdjfkdjfjdsaafdsa123432432543"

void before_bench();
//void before_third_part_bench();

void one_thread_async(int bench_n,int test_n);
//void one_thread_async_third_part(int bench_n,int test_n);

void one_thread_sync(int bench_n,int test_n);
//void one_thread_sync_third_part(int bench_n,int test_n);

void multi_thread_async(int bench_n,int test_n);
//void multi_thread_async_third_part(int bench_n,int test_n);

void multi_thread_sync(int bench_n,int test_n);
//void multi_thread_sync_third_part(int bench_n,int test_n);

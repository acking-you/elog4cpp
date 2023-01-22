#include<gtest/gtest.h>
#include <memory>
#include "bench_interface.h"


struct before_hook{
   explicit before_hook(){
      before_bench();
      before_third_part_bench();
   }
};

before_hook s_h;

TEST(bench, bench_start)
{
   one_thread_async(10,1e4);
   one_thread_async_third_part(10,1e4);

   one_thread_sync(10,1e3);
   one_thread_sync_third_part(10,1e3);

   multi_thread_sync(10,1e3);
   multi_thread_sync_third_part(10,1e3);

   multi_thread_async(10,1e3);
   multi_thread_async_third_part(10,1e3);
}
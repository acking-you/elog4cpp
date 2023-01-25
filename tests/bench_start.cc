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

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4244)
#endif

TEST(bench, bench_start)
{
   one_thread_async(1e4,1);
   one_thread_async_third_part(1e4,1);

   one_thread_sync(1e6,1);
   one_thread_sync_third_part(1e6,1);

   multi_thread_sync(1000,1);
   multi_thread_sync_third_part(1000,1);

   multi_thread_async(1000,1);
   multi_thread_async_third_part(1000,1);
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif
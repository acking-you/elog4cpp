#include <doctest/doctest.h>

#include "bench_interface.h"

struct before_hook
{
   explicit before_hook()
   {
      before_bench();
      //      before_third_part_bench();
   }
};

before_hook s_h;

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4244)
#endif

TEST_CASE("bench start")
{
   one_thread_sync(1e3, 100);
   //   one_thread_sync_third_part(1e5, 1);

   //   one_thread_async(1e5, 5);
   //   one_thread_async_third_part(1e5, 5);

   multi_thread_sync(1e3, 100);
   //   multi_thread_sync_third_part(1e4, 10);

   //   multi_thread_async(1e3, 100);
   //   multi_thread_async_third_part(1e3, 100);
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif
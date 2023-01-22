//
// Created by Alone on 2022-9-23.
//

#pragma once
#include <condition_variable>
#include <mutex>

#include "micros.h"

LBLOG_NAMESPACE_BEGIN
namespace detail {
class CountDownLatch
{
public:
   explicit CountDownLatch(int count);
   void wait();
   void countDown();
   int  getCount();

private:
   std::mutex              m_mtx;
   std::condition_variable m_cv;
   int                     m_count;
};
}   // namespace detail
LBLOG_NAMESPACE_END

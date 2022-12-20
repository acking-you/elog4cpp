//
// Created by Alone on 2022-9-23.
//

#pragma once
#include "common.h"
#include <condition_variable>
#include <mutex>

LBLOG_NAMESPACE_BEGIN
namespace detail
{
    class CountDownLatch
    {
     public:
        explicit CountDownLatch(int count);
        void wait();
        void countDown();
        int getCount();
     private:
        std::condition_variable m_cv;
        std::mutex m_mtx;
        int m_count;
    };
}
LBLOG_NAMESPACE_END


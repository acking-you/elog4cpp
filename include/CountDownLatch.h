//
// Created by Alone on 2022-9-23.
//

#ifndef MY_LOGGER_COUNTDOWNLATCH_H
#define MY_LOGGER_COUNTDOWNLATCH_H
#include <condition_variable>
#include <mutex>

namespace lblog
{
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
}

#endif //MY_LOGGER_COUNTDOWNLATCH_H

//
// Created by Alone on 2022-9-21.
//

#ifndef MY_LOGGER_ASYNCLOGGING_H
#define MY_LOGGER_ASYNCLOGGING_H
#include "FixedBuffer.hpp"
#include "CountDownLatch.h"
#include "noncopyable.h"

#include <vector>
#include<mutex>
#include <atomic>
#include <thread>
#include <memory>
#include <condition_variable>

namespace lblog
{
	namespace detail
	{
		class AsyncLogging : noncopyable
		{
		 public:
			explicit AsyncLogging(std::string basename, off64_t rollSize, int flushInterval = 3);

			~AsyncLogging();

			void append(const char* line, int len);

			void waitDone();

		 private:
			void do_done();

			void thread_worker();

		 private:
			using Buffer = FixedBuffer<kLargeBuffer>;
			using BufferPtr = std::unique_ptr<Buffer>;
			using BufferVectorPtr = std::vector<BufferPtr>;

			const int m_flushInterval;
			const off64_t m_rollSize;
			std::atomic<bool> m_done;
			const std::string m_basename;
			std::unique_ptr<std::thread> m_thread;
			CountDownLatch m_latch; //用于保证负责刷盘的线程资源初始化完成
			std::mutex m_mtx;
			std::condition_variable m_cv;
			BufferPtr m_curBuffer;
			BufferPtr m_nextBuffer;
			BufferVectorPtr m_buffers;
		};
	}
}

#endif //MY_LOGGER_ASYNCLOGGING_H

//
// Created by Alone on 2022-9-21.
//

#pragma once

#include "common.h"
#include "fixed_buffer.h"
#include "count_down_latch.h"

#include <vector>
#include<mutex>
#include <atomic>
#include <thread>
#include <memory>
#include <condition_variable>

LBLOG_NAMESPACE_BEGIN
	namespace detail
	{
		class async_logging : noncopyable
		{
		 public:
			explicit async_logging(std::string basename, off64_t rollSize, int flushInterval = 3);

			~async_logging();

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
LBLOG_NAMESPACE_END
//
// Created by Alone on 2022-9-21.
//

#pragma once
#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include "count_down_latch.h"
#include "fixed_buffer.h"

LBLOG_NAMESPACE_BEGIN
namespace detail {
class AsyncLogging : noncopyable
{
public:
    explicit AsyncLogging(const char* basename, int rollSize,
                          int flushInterval = 3);

    ~AsyncLogging();

    void append(const char* line, int len);

    void waitDone();

private:
    void do_done();

    void thread_worker();

private:
    using Buffer          = FixedBuffer<kLargeBuffer>;
    using BufferPtr       = std::unique_ptr<Buffer>;
    using BufferVectorPtr = std::vector<BufferPtr>;

    const int                    m_flushInterval;
    const int                    m_rollSize;
    std::atomic<bool>            m_done;
    const char*                  m_basename;
    std::unique_ptr<std::thread> m_thread;
    CountDownLatch m_latch; // 用于确保刷盘的线程资源初始化完成
    std::mutex     m_mtx;
    std::condition_variable m_cv;
    BufferPtr               m_curBuffer;
    BufferPtr               m_nextBuffer;
    BufferVectorPtr         m_buffers;
};
} // namespace detail
LBLOG_NAMESPACE_END
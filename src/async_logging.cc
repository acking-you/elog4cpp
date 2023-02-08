//
// Created by Alone on 2022-9-21.
//

#include "elog/async_logging.h"

#include <array>
#include <cassert>
#include <chrono>
#include <memory>

#include "elog/config.h"
#include "elog/log_file.h"
#include "elog/logger_util.h"

using namespace elog::detail;

inner_logsource inner_logsource::fromContext(const context& ctx)
{
   inner_logsource source;
   source.line           = ctx.line;
   source.tid            = ctx.tid;
   source.level          = ctx.level;
   source.func_name      = ctx.func_name;
   source.short_filename = ctx.short_filename;
   source.long_filename  = ctx.long_filename;
   source.text           = std::string(ctx.text.data(), ctx.text.size());
   return source;
}
elog::context inner_logsource::toContext() const
{
   context ctx;
   ctx.line           = line;
   ctx.tid            = tid;
   ctx.level          = level;
   ctx.func_name      = func_name;
   ctx.short_filename = short_filename;
   ctx.long_filename  = long_filename;
   ctx.text           = text;
   return ctx;
}

AsyncLogging::AsyncLogging(const char* basename, int rollSize,
                           int flushInterval)

  : m_flushInterval(flushInterval),
    m_rollSize(rollSize),
    m_done(false),
    m_basename(basename),
    m_latch(1)
{
   // 由于构造函数抛出异常不会调用析构，所以需要做异常安全处理
   try
   {
#if __cplusplus >= 201403L
      m_thread = std::make_unique<std::thread>([this]() { thread_worker(); });
#else
      m_thread = std::unique_ptr<std::thread>(
        new std::thread([this]() { thread_worker(); }));
#endif
      m_latch.wait();   // 等待线程任务的资源初始化完成
   }
   catch (...)
   {
      do_done();   // 需要做的额外安全处理
      throw std::runtime_error(
        "AsyncLogging create thread or buffer alloc error");
   }
}

AsyncLogging::~AsyncLogging()
{
   if (m_done.load(std::memory_order::memory_order_acquire)) { return; }
   do_done();
}

// 如果发生异常，则需要维护最后的资源安全退出
void AsyncLogging::do_done()
{
   m_done.store(true, std::memory_order::memory_order_release);

   m_cv.notify_one();   // 由于只控制一个线程

   if (m_thread && m_thread->joinable())
   {
      m_thread->join();
   }
}

void AsyncLogging::waitDone()
{
   if (m_done.load(std::memory_order::memory_order_acquire)) { return; }
   do_done();
}

// 双缓冲关键代码
void AsyncLogging::pushMsg(inner_message const& msg)
{
   // 下面为关键逻辑，采取双缓冲机制，如果缓存足够则push进去，否则将缓存转移到vector中待flush到磁盘
   // 虽然是这样说双缓冲，实际上不存在两个层级的缓存，第二个层级只是存储待push缓存的指针，并不会有拷贝
   // 为了减少重复申请内存的性能开销，又增加了备用内存nextBuffer，如果缓冲区需要新的内存，则直接使用它
   assert(!m_done);
   std::lock_guard<std::mutex> lock(m_mtx);
   if (m_curBuffer.avail() > 0)
   {
      // 缓存足够
      m_curBuffer.push(msg);
      return;
   }

   // 缓存满了，需要flush
   m_buffers.push_back(std::move(m_curBuffer));   // 转移指针所有权
   if (!m_nextBuffer.valid())
   {
      // 如果备用内存还存在，则直接使用备用内存
      m_curBuffer = std::move(m_nextBuffer);
   }
   else
   {
      // 否则重新申请
      m_curBuffer = Buffer();
   }
   m_curBuffer.push(msg);
   m_cv.notify_one();   // 通知消费
}

struct call_once
{
   explicit call_once(const std::function<void()>& func) { func(); }
};

// 异步写入和内存复用
void AsyncLogging::thread_worker()
{
   try
   {
      // 不需要线程安全的写入，因本身使用它就会加锁
      LogFile      output(m_basename, m_rollSize, false);
      // 防止重复内存申请的内存
      Buffer       newBuffer1;
      Buffer       newBuffer2;
      // 用于帮助m_buffers写入磁盘的vector，每次通过swap将m_buffers的内容重置，目的是缩小临界区
      BufferVector buffersToWrite;
      buffersToWrite.reserve(16);   // 提前预分配

      while (!m_done.load(std::memory_order::memory_order_acquire))
      {
         {
            std::unique_lock<std::mutex> lock(m_mtx);
            if (m_buffers.empty())
            {
               thread_local call_once once([&]() {
                  // 只执行一次，外界会等待thread任务执行到这
                  m_latch.countDown();
               });

               m_cv.wait_for(lock, std::chrono::seconds(m_flushInterval));
            }
            // 等待flush_interval后，不管是否有任务，都把cur_buffer刷入buffers，然后再交换到buffer减小临界区
            m_buffers.push_back(std::move(m_curBuffer));
            m_curBuffer = std::move(newBuffer1);   // 换上新的内存
            buffersToWrite.swap(m_buffers);
            if (!m_nextBuffer.valid())
            {
               // 顺便更新下nextBuffer的内存
               m_nextBuffer = std::move(newBuffer2);
            }
         }

         if (buffersToWrite.empty()) { continue; }

         if (buffersToWrite.size() > 100)
         {   // FIXME
             // 如果刷入的内存块超过100个，则丢弃到只剩下两个,后续可以加入告警通知异常
            char buf[256];
            snprintf(buf, sizeof buf,
                     "Dropped log messages at %s, %zd larger buffers\n",
                     Util::getCurDateTime(true), buffersToWrite.size() - 2);
            fputs(buf, stderr);
            output.append(buf, static_cast<int>(strlen(buf)));
            buffersToWrite.erase(buffersToWrite.begin() + 2,
                                 buffersToWrite.end());
         }

         // 开始格式化并写入
         for (auto& buffer : buffersToWrite)
         {
            for (auto& it : buffer)
            {
               buffer_t formatBuffer;
               // 格式化
               it.config->log_formatter(it.config, it.source.toContext(),
                                        formatBuffer, kFile);
               // 开始写入文件
               output.append(formatBuffer.data(),
                             static_cast<int>(formatBuffer.size()));
            }
         }

         if (buffersToWrite.size() < 2)
         {
            // 保留两个，可以进行资源的利用，即使没有2个，resize也会为你开两个空的
            buffersToWrite.resize(2);
         }

         if (!newBuffer1.valid())
         {
            // 由于前面必定存留两个，则可以直接利用
            assert(!buffersToWrite.empty());
            newBuffer1 = std::move(buffersToWrite.back());
            buffersToWrite.pop_back();
            newBuffer1.reset();   // 记得重新设定内部读指针
         }

         if (!newBuffer2.valid())
         {   // 由于前面必定存留两个，则可以直接利用
            assert(!buffersToWrite.empty());
            newBuffer2 = std::move(buffersToWrite.back());
            buffersToWrite.pop_back();
            newBuffer2.reset();
         }

         buffersToWrite.clear();
         output.flush();
      }
      output.flush();
   }
   catch (std::exception const& e)
   {
      fprintf(stderr, "log thread exit: %s\n", e.what());
      m_thread.reset();
   }
}
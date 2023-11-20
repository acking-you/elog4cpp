//
// Created by Alone on 2022-9-21.
//

#pragma once
#include <atomic>
#include <cstring>
#include <thread>
#include <vector>
#include <string>

#include "count_down_latch.h"
#include "noncopyable.h"

LBLOG_NAMESPACE_BEGIN

struct context;
struct Config;

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4018)
#endif
namespace detail {
enum { kSmallBuffer = 4096, kLargeBuffer = 65536 };

struct inner_logsource
{
   int          level{};
   unsigned int tid{};
   int          line{};
   const char*  short_filename{};
   const char*  long_filename{};
   const char*  func_name{};
   std::string  text;

   static inner_logsource fromContext(const context& ctx);
   [[nodiscard]] context  toContext() const;
};

struct inner_message
{
   Config*         config{};
   inner_logsource source;
};

template <int SIZE>
class FixedBuffer : noncopyable
{
public:
   FixedBuffer() : m_data(new inner_message[SIZE]), m_current(m_data) {}

   ~FixedBuffer() { delete[] m_data; }

   FixedBuffer(FixedBuffer&& other) noexcept
     : m_data(other.m_data), m_current(other.m_current)
   {
      other.m_data    = nullptr;
      other.m_current = nullptr;
   }

   FixedBuffer& operator=(FixedBuffer&& other) noexcept
   {
      m_data          = other.m_data;
      m_current       = other.m_current;
      other.m_data    = nullptr;
      other.m_current = nullptr;
      return *this;
   }

   bool valid() { return m_data != nullptr; }

   int avail() { return static_cast<int>(end_addr() - m_current); }

   void push(const inner_message& msg) { *(m_current++) = msg; }

   inner_message* begin() { return m_data; }

   inner_message* end() { return m_current; }

   void reset() { m_current = m_data; }

private:
   inner_message* end_addr() { return m_data + SIZE; }

   inner_message* m_data;
   inner_message* m_current;
};

#ifdef _MSC_VER
#pragma warning(pop)
#endif

class AsyncLogging : noncopyable
{
public:
   explicit AsyncLogging(const char* basename, int rollSize,
                         int flushInterval = 3);

   ~AsyncLogging();

   void pushMsg(inner_message const& msg);

   void waitDone();

private:
   void do_done();

   void thread_worker();

private:
   using Buffer       = FixedBuffer<kLargeBuffer>;
   using BufferVector = std::vector<Buffer>;

   const int                    m_flushInterval;
   const int                    m_rollSize;
   std::atomic<bool>            m_done;
   const char*                  m_basename;
   std::unique_ptr<std::thread> m_thread;
   // 用于确保刷盘的线程资源初始化完成
   CountDownLatch               m_latch;
   std::mutex                   m_mtx;
   std::condition_variable      m_cv;
   Buffer                       m_curBuffer;
   Buffer                       m_nextBuffer;
   BufferVector                 m_buffers;
};
}   // namespace detail
LBLOG_NAMESPACE_END
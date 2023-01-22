//
// Created by Alone on 2022-9-21.
//

#pragma once
#include <cstring>

#include "noncopyable.h"

LBLOG_NAMESPACE_BEGIN
namespace detail {
enum { kSmallBuffer = 4096, kLargeBuffer = 4096 * 1024 };
template <int SIZE>

#pragma warning(disable: 4018)

class FixedBuffer : noncopyable
{
public:
   FixedBuffer() : m_cur_buf(m_buffer) {}
   int   avail() { return static_cast<int>(end() - m_cur_buf); }
   int   size() { return static_cast<int>(m_cur_buf - m_buffer); }
   char* data() { return m_buffer; }
   void  append(const char* line, size_t len)
   {
      if (avail() > len)
      {
         memcpy(m_cur_buf, line, len);
         m_cur_buf += len;
      }
   }
   void bzero() { memset(m_buffer, 0, sizeof(m_buffer)); };
   void reset() { m_cur_buf = m_buffer; }

private:
   char* end() { return m_buffer + sizeof(m_buffer); }

private:
   char  m_buffer[SIZE]{};   // 固定缓冲区
   char* m_cur_buf{};        // 目前写入的位置
};
}   // namespace detail

#pragma warning(default:4018)

LBLOG_NAMESPACE_END
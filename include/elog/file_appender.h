//
// Created by Alone on 2022-9-21.
//
#pragma once

#include <cstdio>

#include "noncopyable.h"
#include "string_view.h"

LBLOG_NAMESPACE_BEGIN
namespace detail {
class FileAppender : noncopyable
{
public:
    // 'e' 代表O_CLOEXEC，可以防止多进程中文件描述符暴露给子进程
    explicit FileAppender(const char* filename);
    ~FileAppender();
    void   append(const char* line, size_t len);
    void   flush();
    size_t writtenBytes() const { return m_writenBytes; }
    void   resetWritten() { m_writenBytes = 0; }

private:
    size_t write(const char* line, size_t len);
    void   init(const char*);

private:
    char   m_buffer[64 * 1024]{};
    FILE*  m_file{};
    size_t m_writenBytes{};
};
} // namespace detail
LBLOG_NAMESPACE_END
//
// Created by Alone on 2022-9-21.
//
#include "my-logger/log_file.h"

#include <memory>

#include "my-logger/logger.h"
#include "my-logger/logger_util.h"

using namespace lblog::detail;

LogFile::LogFile(const char* basename, int rollSize, bool threadSafe,
                 int flushInterval, int checkEveryN)
    : m_basename(basename),
      m_rollSize(rollSize),
      m_flushInterval(flushInterval),
      m_checkEveryN(checkEveryN),
      m_mtx(threadSafe ? std::make_unique<std::mutex>() : nullptr)
{
    rollFile();
}

void LogFile::append(const char* line, int len)
{
    // 如果有线程安全标记，则加锁，否则不加
    if (m_mtx)
    {
        std::lock_guard<std::mutex> lock(*m_mtx);
        append_unlocked(line, len);
    }
    else
    {
        append_unlocked(line, len);
    }
}

void LogFile::flush()
{
    // 如果有线程安全标记，则加锁，否则不加
    if (m_mtx)
    {
        std::lock_guard<std::mutex> lock(*m_mtx);
        m_file->flush();
    }
    else
    {
        m_file->flush();
    }
}

void LogFile::append_unlocked(const char* line, int len)
{
    m_file->append(line, len);
    if (m_file->writtenBytes() > m_rollSize)
    { // 单个文件超出规定大小，滚动日志（重新创建新文件写入
        LB_TRACE_("文件大小超过限定大小 {:d}kb，开始创建新文件进行写入",
                  m_rollSize / 1024);
        rollFile();
        m_file->resetWritten();
    }
    else
    {
        ++m_count;
        if (m_count >= m_checkEveryN)
        { // 开始检查
            LB_TRACE_("开始检查flushInterval和curPeriod");
            m_count          = 0;
            time_t now       = ::time(nullptr);
            time_t curPeriod = now / kRollPerSeconds * kRollPerSeconds;
            if (curPeriod != m_lastPeriod)
            {
                rollFile(&now);
            }
            else if (now - m_lastFlush > m_flushInterval)
            {
                m_lastFlush = now;
                m_file->flush();
            }
        }
    }
}

void LogFile::rollFile(const time_t* cache_now)
{
    time_t now;
    if (cache_now != nullptr)
    {
        now = *cache_now;
    }
    else
    {
        now = time(nullptr);
    }

    auto filename = Util::getLogFileName(m_basename, now);
    auto start = now / kRollPerSeconds * kRollPerSeconds; // 更新天的数据

    if (now > m_lastRoll)
    {
        LB_TRACE_("开始执行roll操作，filename：{}", filename);

        m_lastRoll   = now;
        m_lastFlush  = now;
        m_lastPeriod = start;
        m_file = std::make_unique<FileAppender>(filename); // 创建新的文件
    }
}
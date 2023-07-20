//
// Created by Alone on 2022-9-21.
//
#include "elog/file_appender.h"

#include <cassert>
#include <cstdio>
#include <stdexcept>

#include "elog/logger_util.h"

#if defined(_WIN32)
#include <io.h>
#else
#include <unistd.h>

#include <cstring>
#include <string>

#endif

USING_LBLOG_DETAIL

//'e' 代表O_CLOEXEC 防止fork多进程文件描述符未关闭
FileAppender::FileAppender(const char* filename) { init(filename); }

void FileAppender::init(const char* filename)
{
   // 判断写入文件的文件夹是否存在，以及最终文件是否正常打开
   auto filepos = ::strrchr(filename, '/');
   if (!filepos)
   {
      //      LB_TRACE_("无效的文件路径 {}", filename);
      throw std::runtime_error(std::string("invalid filepath ") +
                               std::string(filename));
   }
   const_cast<char*>(filepos)[0] = '\0';
#if defined(_WIN32)
   int ret = _access(filename, 0);
#else
   int ret = ::access(filename, F_OK);
#endif
   if (ret == -1)
   {
      throw std::runtime_error(std::string("file directory not exist: ") +
                               filename);
   }
   const_cast<char*>(filepos)[0] = '/';

   assert(const_cast<char*>(filepos)[0] != '\0');

#if defined(_WIN32)
   ::fopen_s(&m_file, filename, "a");
#else
   m_file  = ::fopen(filename, "ae");
#endif
   if (m_file == nullptr)
   {
      int   err       = ferror(m_file);
      auto* errorInfo = Util::getErrorInfo(err);
      //      LB_TRACE_("FileAppender 初始化失败 error:{}", errorInfo);
      fprintf(stderr, "FileAppender error in open file:%s erron:%s \r\n",
              filename, errorInfo);
      throw std::runtime_error("panic:FILE* is null");
   }
   std::setvbuf(m_file, m_buffer, _IOFBF, sizeof(m_buffer));
}
#pragma warning(default : 4996)

FileAppender::~FileAppender()
{
   if (m_file != nullptr)
   {
      ::fflush(m_file);
      ::fclose(m_file);
   }
}

void FileAppender::append(const char* line, size_t len)
{
   size_t written = 0;

   while (written != len)
   {
      size_t remain = len - written;
      size_t n      = write(line + written, remain);
      if (n != remain)
      {
         int err = ferror(m_file);
         if (err)
         {
            //            LB_TRACE_("write写入预期内容失败且发生错误，error:{}",
            //                      Util::getErrorInfo(err));
            fprintf(stderr, "AppendFile::append() failed %s\n",
                    Util::getErrorInfo(err));
            break;
         }
         if (n == 0) { throw std::runtime_error("write 出错，FILE*为空"); }
      }
      written += n;
   }

   m_writenBytes += written;
}

void FileAppender::flush()
{
   if (m_file)
   {
      //      LB_TRACE_("底层flush执行");
      ::fflush(m_file);
   }
}

size_t FileAppender::write(const char* line, size_t len)
{
   size_t sz = 0;
   if (m_file)
   {
#if !defined(__linux__)
      sz = ::fwrite(line, 1, len, m_file);
#else
      sz = ::fwrite_unlocked(line, 1, len, m_file);
#endif
   }
   return sz;
}

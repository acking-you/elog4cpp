//
// Created by Alone on 2022-9-21.
//

#ifndef MY_LOGGER_FILEAPPENDER_H
#define MY_LOGGER_FILEAPPENDER_H

#include "noncopyable.h"

#include <string>

#include<cstdio>
#include<sys/types.h>

namespace lblog
{
	namespace detail
	{
		class FileAppender : noncopyable
		{
		 public:
			// 'e' 代表O_CLOEXEC，可以防止多进程中文件描述符暴露给子进程
			explicit FileAppender(std::string const& filename);
			~FileAppender();
			void append(const char* line, size_t len);
			void flush();
			off64_t writtenBytes() const
			{
				return m_writenBytes;
			}
			void resetWritten()
			{
				m_writenBytes = 0;
			}
		 private:
			size_t write(const char* line, size_t len);
			void init(std::string const&);
		 private:
			FILE* m_file{};
			char m_buffer[64 * 1024]{};
			off64_t m_writenBytes{};
		};
	}
}
#endif //MY_LOGGER_FILEAPPENDER_H

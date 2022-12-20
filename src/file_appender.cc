//
// Created by Alone on 2022-9-21.
//
#include"my-logger/file_appender.h"
#include "my-logger/logger_util.h"
#include "my-logger/logger.h"

#include <cstdio>

USING_LBLOG_DETAIL

//'e' 代表O_CLOEXEC 防止fork多进程文件描述符未关闭
FileAppender::FileAppender(std::string const& filename)
{
	init(filename);
}

void FileAppender::init(std::string const& filename)
{
	//判断写入文件的文件夹是否存在，以及最终文件是否正常打开
	std::string filedir;
	size_t pos;
	if ((pos = filename.rfind('/')) == string::npos)
	{
		trace_("无效的文件路径 {}", filename);
		throw std::runtime_error("invalid filepath");
	}
	filedir = filename.substr(0, pos + 1);
	if (sys::CallAccess(filedir.c_str(), F_OK) == -1)
	{
		trace_("文件夹路径不存在：{}", filename);
		throw std::runtime_error(fmt::format("filedir not exist! {}", filedir));
	}

	m_file = fopen(filename.c_str(), "a");
	if (m_file == nullptr)
	{
		int err = ferror(m_file);
		auto* errorInfo = Util::getErrorInfo(err);
		trace_("FileAppender 初始化失败 error:{}", errorInfo);
		fprintf(stderr, "FileAppender error in open file:%s erron:%s \r\n", filename.c_str(), errorInfo);
        throw std::runtime_error("panic:FILE* is null");
	}
	trace_("设置 FILE* 缓冲区大小为{}", sizeof(m_buffer));
	sys::CallSetBuffer(m_file, m_buffer, sizeof(m_buffer));
}

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
		size_t n = write(line + written, remain);
		if (n != remain)
		{
			int err = ferror(m_file);
			if (err)
			{
				trace_("write写入预期内容失败且发生错误，error:{}", Util::getErrorInfo(err));
				fprintf(stderr, "AppendFile::append() failed %s\n", Util::getErrorInfo(err));
				break;
			}
			if (n == 0)
			{
				throw std::runtime_error("write 出错，FILE*为空");
			}
		}
		written += n;
	}

	m_writenBytes += written;
}

void FileAppender::flush()
{
	if (m_file)
	{
		trace_("底层flush执行");
		::fflush(m_file);
	}
}

size_t FileAppender::write(const char* line, size_t len)
{
	size_t sz = 0;
	if (m_file)
	{
		trace_("底层write执行");
		sz = sys::CallUnlockedWrite(line, 1, len, m_file);
	}
	return sz;
}
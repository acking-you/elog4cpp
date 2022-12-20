#include "my-logger/logger.h"
#include "my-logger/logger_util.h"

#include<cassert>

USING_LBLOG
USING_LBLOG_DETAIL

#define GLOB_CONFIG lblog::GlobalConfig::instance()

Logger::Logger()
{
	init_data();
}

Logger::~Logger() = default;

Logger& Logger::GetInstance()
{
	static Logger logger;
	return logger;
}

void Logger::init_data()
{
	// Determine whether to output logs to a file based on the config
	if (GLOB_CONFIG.log_filepath != nullptr)
	{
		m_logging = std::make_unique<AsyncLogging>(GLOB_CONFIG.log_filepath,
			GLOB_CONFIG.log_rollSize,
			GLOB_CONFIG.log_flushInterval);
	}
}

void Logger::waitForDone()
{
	if (m_logging) m_logging->waitDone();
}

void Logger::LogFile(Config* config, context const& ctx)
{
	assert(config != nullptr);

	fmt::memory_buffer buffer;
	config->log_formatter(config, ctx, buffer,Appenders::kFile);
	//将数据写入Async缓冲区
	m_logging->append(buffer.data(), static_cast<int>(buffer.size()));
}

void Logger::LogConsole(Config* config, const context& ctx)
{
	assert(config != nullptr);

	fmt::memory_buffer buffer;
	config->log_formatter(config, ctx, buffer,Appenders::kConsole);
	buffer.push_back('\0'); //with c-style

	{
		std::lock_guard<std::mutex> lk(m_mutex); // Lock the I/O device
		sys::CallFPutsUnlocked(buffer.data(), stdout);
	}
	std::fflush(stdout);
}

void Logger::LogConsoleUnsafe(Config* config, const context& ctx)
{
	assert(config != nullptr);

	fmt::memory_buffer buffer;
	config->log_formatter(config, ctx, buffer,Appenders::kConsole);
	buffer.push_back('\0'); //with c-style
	sys::CallFPutsUnlocked(buffer.data(), stdout);
	std::fflush(stdout);
}

void Logger::DoInternalLog(const context& ctx)
{
	if (GLOB_CONFIG.log_level != Levels::kTrace)
	{
		return;
	}
	if (GLOB_CONFIG.log_console)
	{
		GetInstance().LogConsole(&GLOB_CONFIG, ctx);
	}
}

//全局config logger输出
void Logger::DoLog(context const& ctx)
{
	DoConfigLog(&GLOB_CONFIG, ctx);
}

//自定义config logger输出
void Logger::DoConfigLog(Config* config, const context& ctx)
{
	assert(config != nullptr);
	//FIXME 输出到控制台没有进行任何性能优化，输出控制台方便开发时调试，输出到文件可以方便后续问题的跟踪
	if (ctx.level < static_cast<int>(config->log_level)) return;
	if (m_logging) LogFile(config, ctx);
	if (GLOB_CONFIG.log_console) LogConsole(config, ctx);

	if (ctx.level == static_cast<int>(Levels::kFatal))
	{ // 若为fatal则发送abort信号退出进程
		m_logging->waitDone(); //等待数据成功刷盘
		abort();
	}
}

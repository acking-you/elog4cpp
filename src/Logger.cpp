#include "Logger.h"

#include "LoggerUtil.h"
#include "fmt/chrono.h"
#include "ProcessInfo.h"

#include <cerrno>

using namespace lblog;

#define LOG_CONFIG detail::Config::instance()

void lblog::Config::Set(const lblog::Config& config)
{
	LOG_CONFIG.basename() = config.output_basedir;
	LOG_CONFIG.flags() = config.print_flag;
	LOG_CONFIG.level() = config.print_level;
	LOG_CONFIG.console() = config.is_console;
	LOG_CONFIG.prefix() = config.output_prefix;
	LOG_CONFIG.before() = config.before;
	LOG_CONFIG.end() = config.after;
}

int Config::Level()
{
	return LOG_CONFIG.level();
}

const char* s_level_text[LEVEL_COUNT + 1] = {
	"[TRACE]  ", "[DEBUG]  ", "[INFO]   ",
	"[WARN]   ", "[ERROR]  ",
	"[FATAL]  ", "[UNKNOWN]" };

fmt::color s_color[LEVEL_COUNT + 1] = {
	fmt::color::light_blue, fmt::color::teal, fmt::color::green, fmt::color::yellow,
	fmt::color::red, fmt::color::purple,
	fmt::color::orange_red, // 如果不在所有的范围内
};

inline const char* GET_LEVEL_TEXT(int level)
{
	if (level >= LEVEL_COUNT || level < 0)
		return s_level_text[LEVEL_COUNT]; // level不在范围内
	return s_level_text[level];
}

inline fmt::color GET_COLOR_BY_LEVEL(int level)
{
	if (level >= LEVEL_COUNT || level < 0)
		return s_color[LEVEL_COUNT]; // level不在范围内
	return s_color[level];
}

detail::Logger::Logger()
{
	init_data();
}

detail::Logger::~Logger() = default;

detail::Logger& detail::Logger::GetInstance()
{
	static Logger logger;
	return logger;
}

void detail::Logger::init_data()
{
	// 根据config判断是否需要输出日志到文件
	if (LOG_CONFIG.basename() != nullptr)
	{
		m_logging = std::make_unique<AsyncLogging>(LOG_CONFIG.basename(),
			LOG_CONFIG.rollSize(),
			LOG_CONFIG.flushInterval());
	}
}

void detail::Logger::waitForDone()
{
	if (m_logging)
		m_logging->waitDone();
}

void detail::Logger::LogFile(detail::context const& ctx)
{
	int tid = ProcessInfo::GetTid();
	auto* level_text = GET_LEVEL_TEXT(ctx.level);
	const char* filename =
		LOG_CONFIG.flags() & Llongname
		? ctx.long_filename
		: ((LOG_CONFIG.flags() & Lshortname) ? ctx.short_filename
											 : nullptr);
	fmt::memory_buffer buffer;

	if (LOG_CONFIG.flags() & Ldata) // 打印年月日
	{
		fmt::format_to(std::back_inserter(buffer), "{} ",
			Util::getCurDateTime(LOG_CONFIG.flags() & Ltime));
	}

	if (LOG_CONFIG.prefix()) // 打印前缀
	{
		fmt::format_to(std::back_inserter(buffer), "{} ", LOG_CONFIG.prefix());
	}

	if (LOG_CONFIG.before())
	{ // before回调
		LOG_CONFIG.before()(buffer);
	}

	fmt::format_to(std::back_inserter(buffer), " {}",
		level_text); // 打印日志等级标志

	if (LOG_CONFIG.flags() & LthreadId)
	{ // 打印线程id
		fmt::format_to(std::back_inserter(buffer), " [tid:{:d}] ", tid);
	}

	if (filename != nullptr)
	{
		fmt::format_to(std::back_inserter(buffer), " [{}:{:d}] ", filename,
			ctx.line); // 打印文件和行号
	}

	if (LOG_CONFIG.end())
	{ // end回调
		LOG_CONFIG.end()(buffer);
	}

	if (ctx.level == L_FATAL || ctx.level == L_ERROR)
	{ // 如果是fatal等级，则获取errno产生的错误
		fmt::format_to(std::back_inserter(buffer), " {}: {} \r\n", ctx.text,
			Util::getErrorInfo(errno)); // 打印提示信息
	}
	else
	{
		fmt::format_to(std::back_inserter(buffer), " {} \r\n",
			ctx.text); // 打印提示信息
	}

	//将数据写入Async缓冲区
	m_logging->append(buffer.data(), static_cast<int>(buffer.size()));
}

void detail::Logger::LogConsole(const lblog::detail::context& ctx)
{
	flockfile(stdout); // 给IO设备上锁
	int tid = ProcessInfo::GetTid();
	auto* level_text = GET_LEVEL_TEXT(ctx.level);
	const char* filename =
		LOG_CONFIG.flags() & Llongname
		? ctx.long_filename
		: ((LOG_CONFIG.flags() & Lshortname) ? ctx.short_filename
											 : nullptr);
	fmt::memory_buffer buffer;

	if (LOG_CONFIG.flags() & Ldata) // 首先打印年月日，方便直接按事件定位日志
	{
		fmt::format_to(std::back_inserter(buffer), "{} ",
			Util::getCurDateTime(LOG_CONFIG.flags() & Ltime));
	}

	if (LOG_CONFIG.prefix()) // 打印前缀
	{
		fmt::format_to(std::back_inserter(buffer), "{} ", LOG_CONFIG.prefix());
	}

	if (LOG_CONFIG.before())
	{ // before回调

		LOG_CONFIG.before()(buffer);
	}

	fmt::format_to(std::back_inserter(buffer),
		fg(GET_COLOR_BY_LEVEL(ctx.level)), " {}",
		level_text); // 打印带颜色如果为控制台

	if (LOG_CONFIG.flags() & LthreadId)
	{ // 打印线程id
		fmt::format_to(std::back_inserter(buffer), " [tid:{:d}] ", tid);
	}

	if (filename != nullptr)
	{
		fmt::format_to(std::back_inserter(buffer), " [{}:{:d}] ", filename,
			ctx.line); // 打印文件和行号
	}

	if (LOG_CONFIG.end())
	{ // end回调
		LOG_CONFIG.end()(buffer);
	}

	if ((ctx.level == L_FATAL || ctx.level == L_ERROR) && errno)
	{ // 如果是fatal等级，则获取errno产生的错误
		fmt::format_to(std::back_inserter(buffer),
			fg(GET_COLOR_BY_LEVEL(ctx.level)), "{}: {} \r\n",
			ctx.text, Util::getErrorInfo(errno)); // 打印提示信息
	}
	else
	{
		fmt::format_to(std::back_inserter(buffer),
			fg(GET_COLOR_BY_LEVEL(ctx.level)), " {} \r\n",
			ctx.text); // 打印提示信息
	}

	fmt::print(stdout, fmt::runtime(to_string(buffer))); // 最终输出到控制台

	fflush(stdout);
	funlockfile(stdout); // 解锁
}

void detail::Logger::DoInternalLog(const lblog::detail::context& ctx)
{
	if (ctx.level != L_TRACE)
	{
		return;
	}
	if (LOG_CONFIG.console())
	{
		LogConsole(ctx);
	}
}

void detail::Logger::DoLog(detail::context const& ctx)
{
	// 输出到控制台没有进行任何性能优化，输出控制台方便开发时调试，输出到文件可以方便后续问题的跟踪
	if (ctx.level < LOG_CONFIG.level()) return;
	if (m_logging)
	{
		LogFile(ctx);
	}
	if (LOG_CONFIG.console())
	{
		LogConsole(ctx);
	}
	if (ctx.level == L_FATAL)
	{ // 如果为fatal则发送abort信号退出进程
		m_logging->waitDone(); //等待数据成功刷盘
		abort();
	}
}
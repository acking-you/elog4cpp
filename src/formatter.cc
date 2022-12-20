#include"my-logger/formatter.h"
#include "my-logger/config.h"
#include "my-logger/processinfo.h"
#include "my-logger/logger_util.h"
#include<cassert>
#include<cstring>

LBLOG_NAMESPACE_BEGIN
#define LEVEL_COUNT  static_cast<int>(Levels::kLevelCount)

thread_local char t_filename_buf[1024];

	const char* s_level_text[LEVEL_COUNT + 1] = {
		"[TRACE]  ",
		"[DEBUG]  ",
		"[INFO]   ",
		"[WARN]   ",
		"[ERROR]  ",
		"[FATAL]  ",
		"[UNKNOWN]"
	};
	const char* s_simple_text[LEVEL_COUNT + 1] = {
		"TRC",
		"DEB",
		"INF",
		"WAR",
		"ERR",
		"FAL",
		"UNK"
	};

	fmt::color s_color[LEVEL_COUNT + 1] = {
		fmt::color::light_blue, fmt::color::teal, fmt::color::green, fmt::color::yellow,
		fmt::color::red, fmt::color::purple,
		fmt::color::orange_red, // if not in range
	};

	inline const char* GET_LEVEL_TEXT(int level, bool simple = false)
	{
		if (simple)
		{
			if (level >= LEVEL_COUNT || level < 0)
				return s_simple_text[LEVEL_COUNT]; // level not in range
			return s_simple_text[level];
		}
		if (level >= LEVEL_COUNT || level < 0)
			return s_level_text[LEVEL_COUNT]; // level not in range
		return s_level_text[level];
	}

	inline fmt::color GET_COLOR_BY_LEVEL(int level)
	{
		if (level >= LEVEL_COUNT || level < 0)
			return s_color[LEVEL_COUNT]; // level not in range
		return s_color[level];
	}

	enum
	{
		kDate,
		kLevel,
		kTid,
		kFilename,
		kText,
		kContentCount
	};

	struct JsonMap
	{
		const char* text[kContentCount]{};
		auto toJson() -> std::string
		{
			std::string json = "{";
			for (int i = 0; i < kContentCount; i++)
			{
				if (!text[i])
				{
					continue;
				}
				switch (i)
				{
				case kDate:
					json.append("\"time\":");
					json.push_back('\"');
					json.append(text[kDate]);
					json.append("\", ");
					break;
				case kLevel:
					json.append("\"level\":");
					json.push_back('\"');
					json.append(text[kLevel]);
					json.append("\", ");
					break;
				case kTid:
					json.append("\"tid\":");
					json.push_back('\"');
					json.append(text[kTid]);
					json.append("\", ");
					break;
				case kFilename:
					json.append("\"file\":");
					json.push_back('\"');
					json.append(text[kFilename]);
					json.append("\", ");
					break;
				case kText:
					json.append("\"message\":");
					json.push_back('\"');
					json.append(text[kText]);
					json.push_back('\"');
					break;
				default:
					continue;
				}
			}
			json.push_back('}');
			return json;
		}
		void clear()
		{
			memset(text, 0, sizeof(text));
		}
	};
	thread_local JsonMap s_json;

#define INT(x) static_cast<int>(x)
#define IS_SET(log_flag_, flags_)  (INT(log_flag_) & INT(flags_))
	namespace formatter
	{
		void defaultFormatter(Config* config, context const& ctx, buffer_t& buffer,Appenders outType)
		{
			assert(config != nullptr);
			auto tid = ProcessInfo::GetTid();
			auto* levelText = GET_LEVEL_TEXT(ctx.level);
			const char* filename =
				IS_SET(config->log_flag, Flags::kLongname)
				? ctx.long_filename
				: (IS_SET(config->log_flag, Flags::kShortname) ? ctx.short_filename
															   : nullptr);
			if (config->log_before)
			{ //before call
				config->log_before(buffer);
			}

			Flags logFlag = config->log_flag;

			if (IS_SET(logFlag, Flags::kDate)) // y-m-d
			{
				fmt::format_to(std::back_inserter(buffer), "{} ",
					Util::getCurDateTime(IS_SET(logFlag, Flags::kTime)));
			}

			// log level
			if (outType == Appenders::kConsole) //colorful
			{
				fmt::format_to(std::back_inserter(buffer), fg(GET_COLOR_BY_LEVEL(ctx.level)), " {}", levelText);
			}
			else
			{ //nocolor
				fmt::format_to(std::back_inserter(buffer), " {}", levelText);
			}

			if (IS_SET(logFlag, Flags::kThreadId))
			{ // log thread id
				fmt::format_to(std::back_inserter(buffer), " [tid:{:d}] ", tid);
			}

			if (filename != nullptr)
			{
				fmt::format_to(std::back_inserter(buffer), " [{}:{:d}] ", filename,
					ctx.line); // log file-line
			}

			if (outType == Appenders::kConsole) //colorful
			{
				if (ctx.level >= INT(Levels::kError))
				{ // if level >= Error,get the error info
					fmt::format_to(std::back_inserter(buffer),
						fg(GET_COLOR_BY_LEVEL(ctx.level)),
						" {} ERR:{} ",
						ctx.text,
						Util::getErrorInfo(errno)); // 打印提示信息
				}
				else
				{
					fmt::format_to(std::back_inserter(buffer), fg(GET_COLOR_BY_LEVEL(ctx.level)), " {} ",
						ctx.text); // log info
				}
			}
			else
			{ //nocolor
				if (ctx.level >= INT(Levels::kError))
				{ // if level >= Error,get the error info
					fmt::format_to(std::back_inserter(buffer), " {} ERR:{} ", ctx.text,
						Util::getErrorInfo(errno)); // 打印提示信息
				}
				else
				{
					fmt::format_to(std::back_inserter(buffer), " {} ",
						ctx.text); // log info
				}
			}

			if (config->log_after)
			{ // after callback
				config->log_after(buffer);
			}

			buffer.push_back('\n');
		}

		void jsonFormatter(Config* config, context const& ctx, buffer_t& buffer,Appenders outType)
		{
			assert(config != nullptr);
			char tid[10];
			auto* levelText = GET_LEVEL_TEXT(ctx.level, true);
			auto* filename =
				IS_SET(config->log_flag, Flags::kLongname)
				? ctx.long_filename
				: (IS_SET(config->log_flag, Flags::kShortname) ? ctx.short_filename
															   : nullptr);
			auto* dateText = Util::getCurDateTime(IS_SET(config->log_flag, Flags::kTime));
			auto* text = ctx.text.c_str();
			sprintf(t_filename_buf, "%s:%d", filename, ctx.line);

			s_json.text[kFilename] = t_filename_buf;
			s_json.text[kDate] = dateText;
			s_json.text[kLevel] = levelText;
			s_json.text[kText] = text;
			if (IS_SET(config->log_flag, Flags::kThreadId))
			{
				sprintf(tid, "%d", CAST_INT(ProcessInfo::GetTid()));
				s_json.text[kTid] = tid;
			}

			if (config->log_before)
			{ //before call
				config->log_before(buffer);
			}

			buffer.append(s_json.toJson());

			if (config->log_after)
			{ //after call
				config->log_after(buffer);
			}

			buffer.push_back('\n');
			s_json.clear();
		}
	}

LBLOG_NAMESPACE_END


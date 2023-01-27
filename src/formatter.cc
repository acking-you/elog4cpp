#include "elog/formatter.h"

#include "elog/config.h"
#include "elog/logger_util.h"
#include "elog/micros.h"
#include "elog/switch_helper.h"
#include "fmt/color.h"

LBLOG_NAMESPACE_BEGIN
namespace {

#define LEVEL_COUNT static_cast<int>(Levels::kLevelCount)

const char* s_level_text[LEVEL_COUNT + 1]  = {"TRACE", "DEBUG", "INFO", "WARN",
                                              "ERROR", "FATAL", "UNKOW"};
const char* s_simple_text[LEVEL_COUNT + 1] = {"TRC", "DEB", "INF", "WAR",
                                              "ERR", "FAL", "UNK"};

fmt::color s_color[LEVEL_COUNT + 1] = {
  fmt::color::light_blue, fmt::color::teal, fmt::color::green,
  fmt::color::yellow,     fmt::color::red,  fmt::color::purple,
  fmt::color::orange_red,   // if not in range
};

const char* s_ansi_color[LEVEL_COUNT + 1] = {
  "\033[36m",     // light_blue
  "\033[34m",     // teal
  "\033[32m",     // green
  "\033[33m",     // yellow
  "\033[31m",     // red
  "\033[35m",     // purple
  "\033[4;31m",   // red and underline
};

inline const char* GET_LEVEL_TEXT(int level, bool simple = false)
{
   if (simple)
   {
      if (level >= LEVEL_COUNT || level < 0)
         return s_simple_text[LEVEL_COUNT];   // level not in range
      return s_simple_text[level];
   }
   if (level >= LEVEL_COUNT || level < 0)
      return s_level_text[LEVEL_COUNT];   // level not in range
   return s_level_text[level];
}

inline fmt::color GET_COLOR_BY_LEVEL(int level)
{
   if (level >= LEVEL_COUNT || level < 0)
      return s_color[LEVEL_COUNT];   // level not in range
   return s_color[level];
}
inline const char* GET_ANSI_COLOR_BY_LEVEL(int level)
{
   if (level >= LEVEL_COUNT || level < 0)
      return s_ansi_color[LEVEL_COUNT];   // level not in range
   return s_ansi_color[level];
}

#define INT(x)                    static_cast<int>(x)
#define IS_SET(log_flag_, flags_) (INT(log_flag_) & INT(flags_))
}   // namespace

void formatter::defaultFormatter(Config* config, context const& ctx,
                                 buffer_t& buffer, Appenders outType)
{
   assert(config != nullptr);
   output_buf_t t_outputBuffer;
   t_outputBuffer.buf = &buffer;
   // RAII before after call
   auto helper =
     trigger_helper(&t_outputBuffer, &config->log_before, &config->log_after);

   // prepare data
   auto* levelText = GET_LEVEL_TEXT(ctx.level);
   auto* funcName =
     IS_SET(config->log_flag, Flags::kFuncName) ? ctx.func_name : nullptr;
   auto* filename =
     IS_SET(config->log_flag, Flags::kLongname)
       ? ctx.long_filename
       : (IS_SET(config->log_flag, Flags::kShortname) ? ctx.short_filename
                                                      : nullptr);
   Flags logFlag = config->log_flag;
   // log name
   if (config->log_name)
   {
      fmt::format_to(fmt::appender(buffer), "[{}]", config->log_name);
   }

   // log date
   if (IS_SET(logFlag, Flags::kDate))   // y-m-d
   {
      fmt::format_to(fmt::appender(buffer), "[{}]",
                     Util::getCurDateTime(IS_SET(logFlag, Flags::kTime)));
   }
   // log level
   fmt::format_to(fmt::appender(buffer), "[{}]", levelText);

   if (IS_SET(logFlag, Flags::kThreadId))
   {   // log thread id
      fmt::format_to(fmt::appender(buffer), "[tid:{:d}]", ctx.tid);
   }
   if (filename)
   {
      fmt::format_to(fmt::appender(buffer), "[{}:{:d}]", filename,
                     ctx.line);   // log file-line
   }
   // log func_name
   if (funcName)
   {
      fmt::format_to(fmt::appender(buffer), "[func:{}]",
                     funcName);   // log file-line
   }

   fmt::string_view text{ctx.text.data(), ctx.text.size()};

   if (ctx.level >= INT(Levels::kError) && errno)
   {   // if level >= Error,get the error info
      fmt::format_to(fmt::appender(buffer), ": {} system error:{}", text,
                     Util::getErrorInfo(errno));   // 打印系统错误提示信息
   }
   else
   {
      fmt::format_to(fmt::appender(buffer), ": {}",
                     text);   // log info
   }
}

void formatter::colorfulFormatter(Config* config, context const& ctx,
                                  buffer_t& buffer, Appenders outType)
{
   assert(config != nullptr);
   output_buf_t t_outputBuffer;
   t_outputBuffer.buf = &buffer;
   // RAII before after call
   auto helper =
     trigger_helper(&t_outputBuffer, &config->log_before, &config->log_after);

   // prepare data
   auto* levelText = GET_LEVEL_TEXT(ctx.level);
   auto* funcName =
     IS_SET(config->log_flag, Flags::kFuncName) ? ctx.func_name : nullptr;
   auto* filename =
     IS_SET(config->log_flag, Flags::kLongname)
       ? ctx.long_filename
       : (IS_SET(config->log_flag, Flags::kShortname) ? ctx.short_filename
                                                      : nullptr);
   Flags logFlag = config->log_flag;
   // log name
   if (config->log_name)
   {
      if (outType == kConsole)
      {
         fmt::format_to(std::back_inserter(buffer),
                        fg(GET_COLOR_BY_LEVEL(ctx.level)), "[{}]",
                        config->log_name);
      }
      else
      {
         fmt::format_to(std::back_inserter(buffer), "[{}]", config->log_name);
      }
   }

   // log date
   if (IS_SET(logFlag, Flags::kDate))   // y-m-d
   {
      fmt::format_to(std::back_inserter(buffer), "[{}]",
                     Util::getCurDateTime(IS_SET(logFlag, Flags::kTime)));
   }
   // log level
   if (outType == Appenders::kConsole)   // colorful
   {
      fmt::format_to(std::back_inserter(buffer),
                     fg(GET_COLOR_BY_LEVEL(ctx.level)), "[{}]", levelText);
   }
   else
   {   // nocolor
      fmt::format_to(std::back_inserter(buffer), "[{}]", levelText);
   }
   if (IS_SET(logFlag, Flags::kThreadId))
   {   // log thread id
      fmt::format_to(std::back_inserter(buffer), "[tid:{:d}]", ctx.tid);
   }
   if (filename)
   {
      fmt::format_to(std::back_inserter(buffer), "[{}:{:d}]", filename,
                     ctx.line);   // log file-line
   }
   // log func_name
   if (funcName)
   {
      fmt::format_to(std::back_inserter(buffer), "[func:{}]",
                     funcName);   // log file-line
   }
   fmt::string_view text{ctx.text.data(), ctx.text.size()};
   if (outType == Appenders::kConsole)   // colorful
   {
      if (ctx.level >= INT(Levels::kError) && errno)
      {   // if level >= Error,get the error info
         fmt::format_to(std::back_inserter(buffer),
                        fg(GET_COLOR_BY_LEVEL(ctx.level)),
                        ": {} system error:{}", text,
                        Util::getErrorInfo(errno));   // 打印系统错误提示信息
      }
      else
      {
         fmt::format_to(std::back_inserter(buffer),
                        fg(GET_COLOR_BY_LEVEL(ctx.level)), ": {}",
                        text);   // log info
      }
   }
   else
   {   // nocolor
      if (ctx.level >= INT(Levels::kError) && errno)
      {   // if level >= Error,get the error info
         fmt::format_to(std::back_inserter(buffer), ": {} system error:{}",
                        text,
                        Util::getErrorInfo(errno));   // 打印提示信息
      }
      else
      {
         fmt::format_to(std::back_inserter(buffer), ": {}",
                        text);   // log info
      }
   }
}

void formatter::jsonFormatter(Config* config, context const& ctx,
                              buffer_t& buffer, Appenders outType)
{
   assert(config != nullptr);
   output_buf_t t_outputBuffer;
   t_outputBuffer.buf = &buffer;
   // RAII before after call
   auto helper =
     trigger_helper(&t_outputBuffer, &config->log_before, &config->log_after);

   // prepare data
   auto* dateText =
     Util::getCurDateTime(IS_SET(config->log_flag, Flags::kTime));
   auto* levelText = GET_LEVEL_TEXT(ctx.level, true);
   auto* funcName =
     IS_SET(config->log_flag, Flags::kFuncName) ? ctx.func_name : nullptr;
   auto* filename =
     IS_SET(config->log_flag, Flags::kLongname)
       ? ctx.long_filename
       : (IS_SET(config->log_flag, Flags::kShortname) ? ctx.short_filename
                                                      : nullptr);
   t_outputBuffer.push_back('{');
   if (config->log_name)
   {
      t_outputBuffer.formatTo(R"("name":"{}", )", config->log_name);
   }
   if (dateText) { t_outputBuffer.formatTo(R"("time":"{}")", dateText); }
   if (levelText) { t_outputBuffer.formatTo(R"(, "level":"{}")", levelText); }
   if (IS_SET(config->log_flag, Flags::kThreadId))
   {
      t_outputBuffer.formatTo(R"(, "tid":"{:d}")", ctx.tid);
   }
   if (filename)
   {
      if (IS_SET(config->log_flag, Flags::kLine))
      {
         t_outputBuffer.formatTo(R"(, "file":"{}:{:d}")", filename, ctx.line);
      }
      else { t_outputBuffer.formatTo(R"(, "file":"{}")", filename); }
   }
   if (funcName) { t_outputBuffer.formatTo(R"(, "func":"{}")", funcName); }

   t_outputBuffer.formatTo(R"(, "message":"{}")",
                           fmt::string_view{ctx.text.data(), ctx.text.size()});
   t_outputBuffer.push_back('}');
}

// use %T:time,%t:tid,%F:filepath,%f:func, %e:error info
//  %L:long levelText,%l:short levelText,%n:name,%v:message ,%c color start %C
//  color end
void customStringFormatter(const StringView& format_str, Config* config,
                           context const& ctx, buffer_t& buffer,
                           Appenders outType)
{
   assert(config != nullptr);
   output_buf_t outputBuffer;
   outputBuffer.buf = &buffer;
   // RAII before after call
   auto helper =
     trigger_helper(&outputBuffer, &config->log_before, &config->log_after);

   size_t index = format_str.find('%');
   // not find format flag
   if (index == std::string::npos)
   {
      outputBuffer.append(format_str);
      return;
   }
   // start custom format
   outputBuffer.append(format_str.substr(0, index));
   for (;;)
   {
      auto op = format_str.substr(index, 2);
      switch (OP_INT(op))
      {
         case "%T"_i: {
            auto* dateText =
              Util::getCurDateTime(IS_SET(config->log_flag, Flags::kTime));
            outputBuffer.append(dateText);
            break;
         }
         case "%t"_i: {
            outputBuffer.formatTo("{:d}", ctx.tid);
            break;
         }
         case "%F"_i: {
            auto* filepath = IS_SET(config->log_flag, Flags::kLongname)
                               ? ctx.long_filename
                               : (IS_SET(config->log_flag, Flags::kShortname)
                                    ? ctx.short_filename
                                    : nullptr);
            if (filepath)
            {
               if (IS_SET(config->log_flag, Flags::kLine))
                  outputBuffer.formatTo("{}:{:d}", filepath, ctx.line);

               else outputBuffer.append(filepath);
            }
            break;
         }
         case "%f"_i: {
            auto* funcName = IS_SET(config->log_flag, Flags::kFuncName)
                               ? ctx.func_name
                               : nullptr;
            if (funcName) outputBuffer.append(funcName);
            break;
         }
         case "%e"_i: {
            if (ctx.level < INT(Levels::kError) && errno == 0) break;
            outputBuffer.append(Util::getErrorInfo(errno));
            break;
         }
         case "%L"_i: {
            auto* longLevelText = GET_LEVEL_TEXT(ctx.level);
            if (longLevelText) outputBuffer.append(longLevelText);
            break;
         }
         case "%l"_i: {
            auto* shortLevelText = GET_LEVEL_TEXT(ctx.level, true);
            if (shortLevelText) outputBuffer.append(shortLevelText);
            break;
         }
         case "%v"_i: {
            outputBuffer.append(ctx.text);
            break;
         }
         case "%n"_i: {
            outputBuffer.append(config->log_name);
            break;
         }
         case "%c"_i: {
            // only console support
            if (outType != Appenders::kConsole) break;
            // color start
            outputBuffer.append(GET_ANSI_COLOR_BY_LEVEL(ctx.level));
            break;
         }
         case "%C"_i: {
            // only console support
            if (outType != Appenders::kConsole) break;
            // color end
            outputBuffer.append("\033[0m");
            break;
         }
         case "%%"_i: {
            outputBuffer.push_back('%');
            break;
         }
         default: {
            outputBuffer.append(op);
         }
      }
      // if the op str is last one
      if (index + op.size() >= format_str.size()) { break; }
      index += op.size();
      // push_back until ch is '%'
      while (index < format_str.size() && format_str[index] != '%')
         outputBuffer.push_back(format_str[index++]);
      if (index == format_str.size()) break;
   }
}

using namespace std::placeholders;
auto formatter::customFromString(StringView formatString) -> formatter_t
{
   return std::bind(customStringFormatter, formatString, _1, _2, _3, _4);
}

LBLOG_NAMESPACE_END

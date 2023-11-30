//
// Created by Alone on 2022-9-23.
//
#include "elog/config.h"

#include <ejson/parser.h>

#include <iostream>
#include <utility>

#include "elog/switch_helper.h"

using namespace elog;

auto GlobalConfig::Get() -> GlobalConfig&
{
   static GlobalConfig instance;
   return instance;
}

auto GlobalConfig::setRollSize(int size) -> GlobalConfig&
{
   log_rollSize = size;
   return *this;
}
auto GlobalConfig::setFlushInterval(int flushInterval) -> GlobalConfig&
{
   log_flushInterval = flushInterval;
   return *this;
}
auto GlobalConfig::setFilepath(const char* basedir) -> GlobalConfig&
{
   this->log_filepath = basedir;
   return *this;
}
auto GlobalConfig::enableConsole(bool s) -> GlobalConfig&
{
   this->log_console = s;
   return *this;
}

auto Config::setFlag(Flags flag) -> Config&
{
   this->log_flag = flag;
   return *this;
}
auto Config::setLevel(Levels level) -> Config&
{
   this->log_level = level;
   return *this;
}

auto Config::setBefore(callback_t const& function) -> Config&
{
   this->log_before = function;
   return *this;
}
auto Config::setAfter(callback_t const& function) -> Config&
{
   this->log_after = function;
   return *this;
}

auto Config::setFormatter(formatter_t const& formatter) -> Config&
{
   log_formatter = formatter;
   return *this;
}
auto Config::setName(const char* name) -> Config&
{
   log_name = name;
   return *this;
}

namespace detail {
struct config
{
   AUTO_GEN_INTRUSIVE(detail::config, roll_size, flush_interval, out_console,
                      out_file, flag, level, formatter)
   int        roll_size{};
   int        flush_interval{};
   bool       out_console{};
   StringView out_file;
   StringView flag;
   StringView level;
   StringView formatter;
   StringView fmt_string;

   static const char* to_formatter(const formatter_t& formatter)
   {
      auto f = formatter.target<void (*)(Config*, context const&, buffer_t&,
                                         Appenders appenderType)>();
      if (!f) return nullptr;
      if (*f == formatter::defaultFormatter) { return "default"; }
      if (*f == formatter::colorfulFormatter) { return "colorful"; }
      if (*f == formatter::jsonFormatter) { return "json"; }
      return "custom";
   }
   static formatter_t from_formatter(const StringView& formatter,
                                     const char*       format_string = nullptr)
   {
      switch (OP_INT(formatter))
      {
         case "default"_i: return formatter::defaultFormatter;
         case "json"_i: return formatter::jsonFormatter;
         case "colorful"_i: return formatter::colorfulFormatter;
         case "custom"_i:
            if (format_string)
               return formatter::customFromString(format_string);
            else
               std::cerr << "you use custom formatter but not define,"
                            "format_string.default formatter will be used.";
         default:
            std::cerr << "not valid formatter.default formatter will be used.";
      }
      return formatter::defaultFormatter;
   }

   static void add_flag(int& flag, StringView op)
   {
      switch (OP_INT(op))
      {
         case "date"_i: flag |= kDate; return;
         case "time"_i: flag |= kTime; return;
         case "file"_i: flag |= kLongname; return;
         case "short_file"_i: flag |= kShortname; return;
         case "tid"_i: flag |= kThreadId; return;
         case "line"_i: flag |= kLine; return;
         case "func"_i: flag |= kFuncName; return;
         case "default"_i: flag |= kStdFlags; return;
         default: std::cerr << "invalid format_flag:" << op;
      }
   }

   static int from_flags(const StringView& flags)
   {
      size_t start = 0, i = 0;
      int    ret = 0;
      // trim left
      while (i < flags.size() && std::isspace(flags[i])) ++i;
      start = i;
      while (i < flags.size())
      {
         if (std::isspace(flags[i]) || flags[i] == '+')
         {
            add_flag(ret, flags.substr(start, i - start));
            // skip to next start
            ++i;
            while (i < flags.size() &&
                   (std::isspace(flags[i]) || flags[i] == '+'))
               ++i;
            if (i >= flags.size()) { return ret; }
            start = i;
            continue;
         }
         ++i;
      }
      add_flag(ret, flags.substr(start, i - start));
      return ret;
   }

   static void flags_append(char* flags, const char* data)
   {
      size_t i = 0;
      while (data[i])
      {
         flags[i] = data[i];
         ++i;
      }
   }

   static const char* to_flags(int flags)
   {
      thread_local char buf[8 * 12];
      std::memset(buf, 0, sizeof(buf));
      size_t size{};
      if (flags | kDate)
      {
         flags_append(buf + size, "date+");
         size += 5;
      }
      if (flags | kTime)
      {
         flags_append(buf + size, "time+");
         size += 5;
      }
      if (flags | kLine)
      {
         flags_append(buf + size, "line+");
         size += 5;
      }
      if (flags | kThreadId)
      {
         flags_append(buf + size, "tid+");
         size += 4;
      }
      if (flags | kFuncName)
      {
         flags_append(buf + size, "func+");
         size += 5;
      }
      if (flags | kLongname)
      {
         flags_append(buf + size, "file+");
         size += 5;
      }
      if (flags | kShortname)
      {
         flags_append(buf + size, "short_file+");
         size += 11;
      }
      buf[size - 1] = '\0';
      return buf;
   }
   static int from_level(const StringView& level)
   {
      switch (OP_INT(level))
      {
         case "trace"_i: return kTrace;
         case "debug"_i: return kDebug;
         case "info"_i: return kInfo;
         case "warn"_i: return kWarn;
         case "error"_i: return kError;
         case "fatal"_i: return kFatal;
         default: std::cerr << "not valid level,default use debug level.";
      }
      return kDebug;
   }
   static const char* to_level(int level)
   {
      switch (level)
      {
         case kTrace: return "trace";
         case kDebug: return "debug";
         case kInfo: return "info";
         case kWarn: return "warn";
         case kError: return "error";
         case kFatal: return "fatal";
         default:
            std::cerr
              << "error in to_level:not valid level,default use debug level";
      }
      return "debug";
   }
};
}   // namespace detail

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4996)
#endif

auto GlobalConfig::loadFromJSON(const char* filename) -> GlobalConfig&
{
   thread_local detail::config t_config;
   thread_local char           t_filepath[1024];
   thread_local char           t_log_name[1024];
   thread_local char           t_fmt_string[1024];

   auto& object = ejson::Parser::FromFile(filename).at("elog").ref;
   from_json(object, t_config);

   // str copy
   if (t_config.out_file.empty() || t_config.out_file.compare("null") == 0)
   {
      log_filepath = nullptr;
   }
   else
   {
      if (t_config.out_file.size() > 1024)
      {
         EJSON_THROW_ERROR_POS("`filepath` length must be less than 1024");
      }
      std::strncpy(t_filepath, t_config.out_file.data(),
                   t_config.out_file.size());
      log_filepath = t_filepath;
   }
   if (object.has_key("fmt_string"))
   {
      auto data = object.at("fmt_string").ref.cast<ejson::str_t>();
      if (data.size() > 1024)
      {
         EJSON_THROW_ERROR_POS("`fmt_string` length must be less than 1024");
      }
      std::strncpy(t_fmt_string, data.data(), data.size());
   }

   if (object.has_key("name"))
   {
      auto data = object.at("name").ref.cast<ejson::str_t>();
      if (data.size() > 1024)
      {
         EJSON_THROW_ERROR_POS("`name` length must be less than 1024");
      }
      std::strncpy(t_log_name, data.data(), data.size());
   }

   // 以mb为单位
   if (t_config.roll_size > 128)
   {
      std::cerr << "roll_size must be less than 128mb,now you set to "
                << t_config.roll_size << ".use default roll_size = 4mb";
      t_config.roll_size = 4;
   }
   log_rollSize      = t_config.roll_size * (1024 * 1024);
   log_flushInterval = t_config.flush_interval;
   log_console       = t_config.out_console;
   log_flag  = static_cast<Flags>(detail::config::from_flags(t_config.flag));
   log_level = static_cast<Levels>(detail::config::from_level(t_config.level));
   log_formatter = detail::config::from_formatter(
     t_config.formatter, object.has_key("fmt_string") ? t_fmt_string : nullptr);
   log_name = object.has_key("name") ? t_log_name : nullptr;

   return *this;
}

#pragma warning(default : 4996)

auto GlobalConfig::loadToJSON(const char* filename) -> GlobalConfig&
{
   thread_local detail::config t_config;
   // 以mb为单位
   t_config.roll_size      = log_rollSize / (1024 * 1024);
   t_config.flush_interval = log_flushInterval;
   t_config.out_console    = log_console;
   t_config.out_file       = log_filepath ? log_filepath : "null";
   t_config.flag           = detail::config::to_flags(log_flag);
   t_config.level          = detail::config::to_level(log_level);
   t_config.formatter      = detail::config::to_formatter(log_formatter);
   auto object             = ejson::JObject::Dict();
   object.at("elog").get_from(t_config);
   auto list = ejson::JObject::List();
   list.push_back(
     "The following values are default-generated comments intended to explain "
     "considerations for filling in the parameters:");
   list.push_back("name: optional parameter, if left empty by default, the log "
                  "output will lack a name.");
   list.push_back(
     "roll_size:Threshold for rolling logs, measured in megabytes (MB)");
   list.push_back("flush_interval:The time interval for the log backend to "
                  "flush, measured in seconds.");
   list.push_back("out_console:Whether to enable console output, represented "
                  "as a boolean value.");
   list.push_back(
     "out_file:"
     "是否开启输出日志文件，不开启请使用null值，开启请用一个文件夹目录");
   list.push_back(
     "flag:The data content used to enable corresponding log output has seven "
     "options: `date`, `time`, `line`, `file`, `short_file`, `tid`, `func`. "
     "You can activate them simultaneously using the plus sign (+), or simply "
     "use 'default' to represent all options except 'tid'.");
   list.push_back(
     "level:\n"
     "It is used to define the global minimum output level, with options "
     "including `trace`, `debug`, `info`, `warn`, `error`, `fatal`. By "
     "default, it uses `debug` as the minimum output level.");
   list.push_back("formatter:\n"
                  "It's used to specify the global log formatting style, "
                  "offering three options: default, colorful, custom. By "
                  "default, it uses the default formatting style. If 'custom' "
                  "is chosen, you'll need to add 'fmt_string'.");
   list.push_back(
     "fmt_string:"
     "This is the format used exclusively when the 'custom' formatter is "
     "selected. The corresponding data representation is as follows: %T for "
     "time, %t for thread ID, %F for file path, %f for function, %e for error "
     "info, %L for long level text, %l for short level text, %v for message, "
     "%c for color start, and %C for color end.");
   object.at("comments").ref = std::move(list);
   ejson::Parser::ToFile(filename, object, 2);

   return *this;
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif
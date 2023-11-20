#include <elog/logger.h>
using namespace elog;

int main()
{
   GlobalConfig::Get()
     .setLevel(Levels::kTrace)
     .setFormatter(formatter::colorfulFormatter);
   Log::trace(loc::current(), "hello elog4cpp");
   Log::debug(loc::current(), "hello elog4cpp");
   Log::info(loc::current(), "hello elog4cpp");
   Log::warn(loc::current(), "hello elog4cpp");
   Log::error(loc::current(), "hello elog4cpp");
}
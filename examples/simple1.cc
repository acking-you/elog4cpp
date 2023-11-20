#include <elog/logger.h>
using namespace elog;

int main()
{
   Log::trace("hello elog4cpp");
   Log::debug("hello elog4cpp");
   Log::info("hello elog4cpp");
   Log::warn("hello elog4cpp");
   Log::error("hello elog4cpp");
   Log::fatal("hello elog4cpp");
}
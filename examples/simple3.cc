#define ENABLE_ELG_LOG
#include <elog/logger.h>
using namespace elog;

int main()
{
   GlobalConfig::Get()
     .setLevel(Levels::kTrace)
     .setFormatter(formatter::colorfulFormatter);
   ELG_TRACE("hello elog4cpp");
   ELG_DEBUG("hello elog4cpp");
   ELG_INFO("hello elog4cpp");
   ELG_WARN("hello elog4cpp");
   ELG_ERROR("hello elog4cpp");
}
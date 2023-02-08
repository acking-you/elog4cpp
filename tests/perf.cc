#include <elog/logger.h>

#include <thread>

using namespace elog;

void log_n_times(int n)
{
   for (int i = 0; i < n; i++) { Log::info("hello elog4cpp"); }
}

int main()
{
   GlobalConfig::Get().loadFromJSON(PROJECT_ROOT "config.json");
   std::thread th1(log_n_times, 1);
   std::thread th2(log_n_times, 1);
   std::thread th3(log_n_times, 1);
   std::thread th4(log_n_times, 1);

   th1.join();
   th2.join();
   th3.join();
   th4.join();
}

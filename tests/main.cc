#include "../dependencies/Timer.hpp"

#include "my-logger/logger.h"
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>
#include<sys/types.h>
#include<unistd.h>

using namespace lblog;
using namespace std::chrono_literals;

// 建议不要使用多进程+多线程模型，具体原因可以看下面的链接
// https://www.quora.com/What-happens-when-a-mutex-owner-is-forked
// 一旦用多线程就不要用fork创建多进程，除非马上使用exec函数族切断父子进程联系

const char* log_dir = "../../log/";

void test_multithreadAndProcess()
{
	int id = fork();
	if (id == -1)
	{
		fatal("error:{}", errno);
		exit(-1);
	}
GlobalConfig::instance()
.setFilepath(log_dir)
.setFlag(FLAGS(Flags::kStdFlags,Flags::kLongname,Flags::kThreadId));

	if (id == 0) // 设置父进程的debug配置
        GlobalConfig::instance()
            .setFilepath(log_dir)
            .setFlag(FLAGS(Flags::kStdFlags,Flags::kShortname,Flags::kThreadId));

	std::thread th1{ []()
					 {
					   for (int i = 0; i < 10; i++)
					   {
						   std::this_thread::sleep_for(1s);
						   info("线程1 {}", std::vector<int>{
							   32,
							   234,
							   32423,
							   234,
						   });
					   }
					 }};

	std::thread th2{ []()
					 {
					   for (int i = 0; i < 10; i++)
					   {
						   std::this_thread::sleep_for(1s);
						   warn("线程2");
					   }
					 }};

	std::thread th3{ []()
					 {
					   for (int i = 0; i < 10; i++)
					   {
						   std::this_thread::sleep_for(1s);
						   debug("线程3");
					   }
					 }};

	std::thread th4{ []()
					 {
					   for (int i = 0; i < 10; i++)
					   {
						   std::this_thread::sleep_for(1s);
						   error("线程4");
					   }
					 }};
	std::thread th5{ []()
					 {
					   for (int i = 0; i < 10; i++)
					   {
						   std::this_thread::sleep_for(1s);
						   warn("线程5");
					   }
					 }};
	th1.join();
	th2.join();
	th3.join();
	th4.join();
	th5.join();
}

void test_multithread()
{
    GlobalConfig::instance()
        .setFilepath(log_dir)
        .setFlag(FLAGS(Flags::kStdFlags,Flags::kLongname,Flags::kThreadId))
		.setAfter([](auto& f)
		{
		  fmt::format_to(back_inserter(f), "你好我是end回调");
		});

	std::thread th1{ []()
					 {
					   for (int i = 0; i < 10; i++)
					   {
						   std::this_thread::sleep_for(1s);
						   info("线程1 {}", std::vector<int>{
							   32,
							   234,
							   32423,
							   234,
						   });
					   }
					 }};

	std::thread th2{ []()
					 {
					   for (int i = 0; i < 10; i++)
					   {
						   std::this_thread::sleep_for(1s);
						   warn("线程2");
					   }
					 }};

	std::thread th3{ []()
					 {
					   for (int i = 0; i < 10; i++)
					   {
						   std::this_thread::sleep_for(1s);
						   debug("线程3");
					   }
					 }};

	std::thread th4{ []()
					 {
					   for (int i = 0; i < 10; i++)
					   {
						   std::this_thread::sleep_for(1s);
						   error("线程4");
					   }
					 }};
	std::thread th5{ []()
					 {
					   for (int i = 0; i < 10; i++)
					   {
						   std::this_thread::sleep_for(1s);
						   fatal("线程5");
					   }
					 }};
	th1.join();
	th2.join();
	th3.join();
	th4.join();
	th5.join();
}



// 测试100w行写入
int test_lines = 1000000;

void test_performance()
{
    GlobalConfig::instance().enableConsole(false)
        .setFilepath(log_dir)
        .setFlag(FLAGS(Flags::kStdFlags,Flags::kLongname,Flags::kThreadId))
        .setAfter([](auto& f)
                  {
                      fmt::format_to(back_inserter(f), "你好我是end回调");
                  });
	for (int i = 0; i < test_lines; i++)
	{
		info("abcdefghiksfasdajfksdajfkljdsakfjdksjfdksajfdsajdfsfasdfsdafsdafu"
			 "roewpewm,xcv,sf{}",
			rand());
	}
}

//void test_buffered_performance()
//{
//	auto out = fmt::output_file("../../log/test_bufferd.txt");
//	for (int i = 0; i < test_lines * 5; i++)
//	{
//		out.print("[INFO] {} [tid:{}] [{}:{:d}] {}",
//			detail::Util::getCurDateTime(true), ProcessInfo::GetTid(),
//			lblog::detail::GetShortName(__FILE__), __LINE__,
//			"abcdefghiksfasdajfksdajfkljdsakfjdksjfdksajfdsajdfsfasdfsdaf"
//			"sdafuroewpewm,xcv,sf\r\n");
//	}
//}

//void test_stl_performance()
//{
//	auto out = std::ofstream("../../log/test_stl.txt");
//	for (int i = 0; i < test_lines; i++)
//		out << "[INFO] " << detail::Util::getCurDateTime(true)
//			<< "[tid:" << ProcessInfo::GetTid() << "] ["
//			<< lblog::detail::GetShortName(__FILE__) << ":" << __LINE__ << "]"
//			<< "abcdefghiksfasdajfksdajfkljdsakfjdksjfdksajfdsajdfsfasdfsdafsda"
//			   "furoewpewm,xcv,sf\r\n";
//}

//void test_c_performance()
//{
//	FILE* file = fopen("../../log/test_c.txt", "a");
//	const char* data = "abcdefghiksfasdajfksdajfkljdsakfjdksjfdksajfdsajdfsfasd"
//					   "fsdafsdafuroewpewm,xcv,sf\r\n";
//	for (int i = 0; i < test_lines; i++)
//	{
//		fprintf(file, "[INFO] %s [tid:%d] [%s:%d] %s",
//			detail::Util::getCurDateTime(true), ProcessInfo::GetTid(),
//			lblog::detail::GetShortName(__FILE__), __LINE__,
//			"abcdefghiksfasdajfksdajfkljdsakfjdksjfdksajfdsajdfsfasdfsdafsd"
//			"afuroewpewm,xcv,sf\r\n");
//	}
//	fclose(file);
//}

//void test_sys_performance()
//{
//	int fd = ::open("../../log/test_sys.txt", O_RDWR | O_APPEND);
//	const char* data = "abcdefghiksfasdajfksdajfkljdsakfjdksjfdksajfdsajdfsfasd"
//					   "fsdafsdafuroewpewm,xcv,sf\r\n";
//	int n;
//	for (int i = 0; i < test_lines; i++)
//		n = ::write(fd, (char*)data, strlen(data));
//	close(fd);
//}

void test_multiPerformance()
{
	GlobalConfig::instance().enableConsole(false).setFilepath("../../").setFlag(FLAGS(Flags::kStdFlags,Flags::kLongname,Flags::kThreadId)).setBefore([](buffer_t& bf){
           fmt::format_to(std::back_inserter(bf),"{}","我是before");
        });

	std::thread th1([&]()
	{
	  for (int i = 0; i < test_lines; i++)
		  warn("helloworld hello world heoolwdsfsddsfsadfsdfsdfgdsajyfgsdayfgdsfgdsfgyuasgfysgfydsghfyusdfadsf");
	});
	std::thread th2([&]()
	{
	  for (int i = 0; i < test_lines; i++)
		  info("helloworld hello world heoolwdsfsddsfsadfsdfsdfgdsajyfgsdayfgdsfgdsfgyuasgfysgfydsghfyusdfadsf");
	});
	std::thread th3([&]()
	{
	  for (int i = 0; i < test_lines; i++)
		  error("helloworld hello world heoolwdsfsddsfsadfsdfsdfgdsajyfgsdayfgdsfgdsfgyuasgfysgfydsghfyusdfadsf");
	});
	std::thread th4([&]()
	{
	  for (int i = 0; i < test_lines; i++)
		  fatal("helloworld hello world heoolwdsfsddsfsadfsdfsdfgdsajyfgsdayfgdsfgdsfgyuasgfysgfydsghfyusdfadsf");
	});
	std::thread th5([&]()
	{
	  for (int i = 0; i < test_lines; i++)
		  info("helloworld hello world heoolwdsfsddsfsadfsdfsdfgdsajyfgsdayfgdsfgdsfgyuasgfysgfydsghfyusdfadsf");
	});

	th1.join();
	th2.join();
	th3.join();
	th4.join();
	th5.join();
}

int main()
{
    Timer tm;
	test_multiPerformance();
}
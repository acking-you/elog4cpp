//
// Created by Alone on 2022-9-22.
//

#ifndef MY_LOGGER_CONFIG_H
#define MY_LOGGER_CONFIG_H
#include "fmt/format.h"

#include <functional>

#include <cstdio>

namespace lblog{

      enum PRINT_FLAG {
        Ldata = 1 << 0,
        Ltime = 1 << 1,
        Llongname = 1 << 2,
        Lshortname = 1 << 3,
        Lline = 1 << 4,
        LthreadId = 1 << 5,
        LstdFlags = Ldata | Ltime | Lshortname | Lline
    };

    enum LEVEL
	{
		L_TRACE,
		L_DEBUG,
		L_INFO,
		L_WARN,
		L_ERROR,
		L_FATAL,
		LEVEL_COUNT
	};

    namespace detail{

        using callback_t = std::function<void(fmt::memory_buffer &)>;

        class Config {

        public:
            static Config &instance();

            const char* &basename() {
                return m_log_basename;
            }

            int &flags() {
                return m_log_flags;
            }

            int& level(){
                return m_log_level;
            }

            bool &console() {
                return m_log_console;
            }

            int& rollSize(){
                return m_log_rollSize;
            }

            int& flushInterval(){
                return m_log_flushInterval;
            }

            const char* &prefix() {
                return m_log_prefix;
            }

            callback_t& before(){
               return m_before;
            }

            callback_t& end(){
                return m_end;
            }

        private:
            int m_log_flags = LstdFlags;
            int m_log_level = L_DEBUG;
            bool m_log_console = true;
            int m_log_rollSize = 20*1024*1024; //默认rollSize为20m
            int m_log_flushInterval = 3; //默认3s刷新一次日志
            const char *m_log_basename = nullptr;
            const char *m_log_prefix = nullptr;
            callback_t m_before;
            callback_t m_end;
        };
    }
}

#endif //MY_LOGGER_CONFIG_H

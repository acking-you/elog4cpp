#test_main
include_directories(../include)
include_directories(../dependencies/fmt/include)

link_directories(../lib)
add_definitions(-DTRACE_)
add_executable(test_main main.cc)
target_link_libraries(test_main logger fmt)

#add_definitions(-DLOG_TRACE)
add_executable(test_async test_asyncWrite.cpp)
target_link_libraries(test_async fmt)

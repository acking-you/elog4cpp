cmake_minimum_required(VERSION 3.14)
project(elog)

#防止依赖源码的情况下产生重复的目标
if (TARGET elog)
    message(WARNING "Target elog already exists!")
    return()
endif ()

# 检测C++版本，并给出提示
set(CMAKE_CXX_STANDARD  11)
if (CMAKE_CXX_STANDARD LESS 11)
    message(FATAL_ERROR "CMAKE_CXX_STANDARD must be at least 11")
elseif (CMAKE_CXX_STANDARD EQUAL 11)
    message(STATUS "Using C++11")
elseif (CMAKE_CXX_STANDARD EQUAL 14)
    message(STATUS "Using C++14")
else ()
    message(STATUS "Using C++17")
    set(CMAKE_CXX_STANDARD 17)
endif ()

if (TARGET fmt)
    message(WARNING "Target fmt already exists!")
else ()
    add_subdirectory(dependencies/fmt EXCLUDE_FROM_ALL)
endif ()
if (TARGET ejson)
    message(WARNING "Target ejson already exists!")
else ()
    add_subdirectory(dependencies/ejson4cpp EXCLUDE_FROM_ALL)
endif ()

option(ENABLE_TEST "Build unittest." OFF)

file(GLOB SRC ${PROJECT_SOURCE_DIR}/src/*.cc)

add_library(${PROJECT_NAME} ${SRC})
target_include_directories(
        ${PROJECT_NAME}
        PUBLIC
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/include
)

if (UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} PUBLIC ejson fmt pthread)
    MESSAGE(STATUS "Now is Linux")
elseif(APPLE)
    target_link_libraries(${PROJECT_NAME} PUBLIC ejson fmt pthread)
    MESSAGE(STATUS "Now is MacOS")
elseif (WIN32)
    target_link_libraries(${PROJECT_NAME} PUBLIC ejson fmt ws2_32)
    MESSAGE(STATUS "Now is windows")
endif ()

if (ENABLE_TEST)
    enable_testing()
    message("BUILD_UNITTEST")
    add_subdirectory(tests)
endif ()

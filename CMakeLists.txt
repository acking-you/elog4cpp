cmake_minimum_required(VERSION 3.14)
project(my_logger)
# 检测C++版本，并给出提示
if(CMAKE_CXX_STANDARD LESS 11)
    message(FATAL_ERROR "CMAKE_CXX_STANDARD must be at least 11")
elseif( CMAKE_CXX_STANDARD EQUAL 11)
    message(STATUS "Using C++11")
elseif(CMAKE_CXX_STANDARD EQUAL 14)
    message(STATUS "Using C++14")
else()
    message(STATUS "Using C++17")
    set(CMAKE_CXX_STANDARD 17)
endif()

add_subdirectory(dependencies/fmt EXCLUDE_ALL)

option(ENABLE_TEST "Build unittest." OFF)

file(GLOB SRC ./src/*.cc)

add_library(my-logger ${SRC})
target_include_directories(my-logger PUBLIC ./include ./dependencies ./dependencies/fmt/include)

IF (CMAKE_SYSTEM_NAME MATCHES "Linux")
    target_link_libraries(my-logger PUBLIC fmt-header-only pthread)
    MESSAGE(STATUS "Now is Linux")
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Windows")
    target_link_libraries(my-logger PUBLIC fmt-header-only ws2_32)
    MESSAGE(STATUS "Now is windows")
ENDIF ()

if (ENABLE_TEST)
    message("BUILD_UNITTEST")
    add_subdirectory(tests)
endif ()

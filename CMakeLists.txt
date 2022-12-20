cmake_minimum_required(VERSION 3.00)
project(my_logger)

set(CMAKE_CXX_STANDARD 14)

add_definitions(-DTRACE_ -DTEST_MAIN)
set(TEST_MAIN ON)

add_subdirectory(dependencies/fmt EXCLUDE_ALL)

include_directories(./include)
include_directories(./dependencies/fmt/include)


if (DEFINED TEST_MAIN)
    add_subdirectory(tests)
endif ()

file(GLOB SRC ./src/*.cc)

add_library(logger SHARED ${SRC})
target_link_libraries(logger fmt-header-only pthread)

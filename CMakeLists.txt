cmake_minimum_required(VERSION 3.14)
project(my_logger)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(dependencies/fmt EXCLUDE_ALL)

option(ENABLE_TEST "Build unittest." OFF)

file(GLOB SRC ./src/*.cc)

add_library(my-logger  ${SRC})
target_include_directories(my-logger PUBLIC ./include ./dependencies ./dependencies/fmt/include)

IF(CMAKE_SYSTEM_NAME MATCHES "Linux")
    target_link_libraries(my-logger PUBLIC fmt-header-only pthread)
    MESSAGE(STATUS "Now is Linux")
ELSEIF(CMAKE_SYSTEM_NAME MATCHES "Windows")
    target_link_libraries(my-logger PUBLIC fmt-header-only ws2_32)
    MESSAGE(STATUS "Now is windows")
ENDIF()

if (ENABLE_TEST)
    message("BUILD_UNITTEST")
    add_subdirectory(tests)
endif ()

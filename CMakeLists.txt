cmake_minimum_required(VERSION 3.14)
project(elog)
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

include_directories(
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/dependencies
        ${PROJECT_SOURCE_DIR}/dependencies/fmt/include)

option(ENABLE_TEST "Build unittest." OFF)

file(GLOB SRC ${PROJECT_SOURCE_DIR}/src/*.cc)

add_library(${PROJECT_NAME} ${SRC})

IF (CMAKE_SYSTEM_NAME MATCHES "Linux")
    target_link_libraries(${PROJECT_NAME} PUBLIC fmt-header-only pthread)
    MESSAGE(STATUS "Now is Linux")
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Windows")
    target_link_libraries(${PROJECT_NAME} PUBLIC fmt-header-only ws2_32)
    MESSAGE(STATUS "Now is windows")
ENDIF ()

if (ENABLE_TEST)
    enable_testing()
    message("BUILD_UNITTEST")
    add_subdirectory(tests)
endif ()

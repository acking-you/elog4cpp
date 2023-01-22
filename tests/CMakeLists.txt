# copied from https://github.com/google/googletest/blob/main/googletest/README.md
include(FetchContent)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        release-1.12.1
        GIT_SHALLOW TRUE
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

FetchContent_Declare(
        nanobench
        GIT_REPOSITORY https://github.com/martinus/nanobench.git
        GIT_TAG v4.1.0
        GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(nanobench)

FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.11.0
        GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(spdlog)

file(GLOB TEST_FILES
        "${PROJECT_SOURCE_DIR}/tests/test_*.cc"
        )
file(GLOB BENCH_FILES
        "${PROJECT_SOURCE_DIR}/tests/bench_*.cc"
        )

message(STATUS "TEST_FILES:${TEST_FILES}\n BENCH_FILES:${BENCH_FILES}")


add_executable(unittest ${TEST_FILES})
target_link_libraries(unittest PRIVATE gtest_main elog nanobench)
add_test(NAME ${PROJECT_NAME}-unittest COMMAND unittest)

add_executable(benchtest ${BENCH_FILES})
target_link_libraries(benchtest PRIVATE gtest_main elog nanobench spdlog_header_only)
add_test(NAME ${PROJECT_NAME}-benchtest COMMAND benchtest)
CPMAddPackage(
        doctest
        GIT_REPOSITORY https://github.com/doctest/doctest.git
        GIT_TAG v2.4.9
        GIT_SHALLOW TRUE
)
CPMAddPackage(
        nanobench
        GIT_REPOSITORY https://github.com/martinus/nanobench.git
        GIT_TAG v4.1.0
        GIT_SHALLOW TRUE)

add_definitions(-DPROJECT_ROOT="${PROJECT_SOURCE_DIR}/")

file(GLOB TEST_FILES
        "${PROJECT_SOURCE_DIR}/tests/test_*.cc"
)
file(GLOB BENCH_FILES
        "${PROJECT_SOURCE_DIR}/tests/bench_*.cc"
)

message(STATUS "TEST_FILES:${TEST_FILES}\n BENCH_FILES:${BENCH_FILES}")


add_executable(unittest ${TEST_FILES})
target_link_libraries(unittest PRIVATE doctest_with_main elog nanobench)
add_test(NAME ${PROJECT_NAME}-unittest COMMAND unittest)

add_executable(benchtest ${BENCH_FILES})
target_link_libraries(benchtest PRIVATE doctest_with_main elog nanobench)

add_executable(perf perf.cc)
target_link_libraries(perf elog)
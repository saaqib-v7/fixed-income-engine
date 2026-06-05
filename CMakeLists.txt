cmake_minimum_required(VERSION 3.20)
project(FixedIncomeEngine VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(FIE_BUILD_TESTS "Build unit tests" ON)
option(FIE_BUILD_BENCHMARKS "Build benchmarks" OFF)

# Library
add_library(fie
    src/curve.cpp
    src/interpolation.cpp
    src/bond.cpp
    src/swap.cpp
    src/risk.cpp
    src/scenario.cpp
)

target_include_directories(fie PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_options(fie PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -O2 -march=native>
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /O2>
)

# Tests
if(FIE_BUILD_TESTS)
    enable_testing()
    find_package(GTest QUIET)
    if(NOT GTest_FOUND)
        include(FetchContent)
        FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG v1.14.0
        )
        FetchContent_MakeAvailable(googletest)
    endif()

    add_executable(fie_tests
        tests/test_interpolation.cpp
        tests/test_curve.cpp
        tests/test_bond.cpp
        tests/test_swap.cpp
        tests/test_risk.cpp
        tests/test_scenario.cpp
    )
    target_link_libraries(fie_tests PRIVATE fie GTest::gtest_main)
    include(GoogleTest)
    gtest_discover_tests(fie_tests)
endif()

# Benchmarks
if(FIE_BUILD_BENCHMARKS)
    find_package(benchmark QUIET)
    if(benchmark_FOUND)
        add_executable(fie_bench benchmarks/bench_curve.cpp)
        target_link_libraries(fie_bench PRIVATE fie benchmark::benchmark)
    endif()
endif()

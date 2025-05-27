cmake_minimum_required(VERSION 3.20)
project(cal_clone LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

# Use FetchContent to download GTest (using your specified version and URL)
include(FetchContent)
FetchContent_Declare(
    GTest
    VERSION v1.16.0
    URL https://github.com/google/googletest/archive/refs/tags/v1.16.0.zip
)
FetchContent_MakeAvailable(GTest)
# or use find_package to find gtest: YOU need to config VCPKG package manager
# find_package(GTest CONFIG REQUIRED)


# 如果不支持c++20及以上，需要配置fmt库的FetchContent
# FetchContent_Declare(
#     fmt
#     GIT_REPOSITORY https://github.com/fmtlib/fmt.git
#     GIT_TAG 11.2.0
#     DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/third_party/fmt
# )
# FetchContent_MakeAvailable(fmt)

# # 如果需要链接fmt库（某些编译器需要手动加上）
# find_package(fmt REQUIRED)
# target_link_libraries(cal PRIVATE fmt::fmt)


set(CALENDAR_LIB_SRCS
    src/option.cpp
    src/dateUtils.cpp
    src/calendar.cpp
)

add_library(calendar_logic STATIC ${CALENDAR_LIB_SRCS})

target_include_directories(calendar_logic PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
add_executable(cal src/main.cpp)
target_link_libraries(cal PRIVATE calendar_logic)

file(GLOB TEST_FILES test/*.cpp)

add_executable(run_tests ${TEST_FILES})
target_link_libraries(run_tests PRIVATE 
    calendar_logic
    GTest::gtest
    GTest::gtest_main
    # GTest::gmock
    # GTest::gmock_main
)

add_test(NAME unit_tests COMMAND run_tests)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
# cmake_minimum_required(VERSION 3.20)
# project(cal_clone LANGUAGES CXX)

# set(CMAKE_CXX_STANDARD 23)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

# # 输出目录
# # set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
# # set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

# # 如果不支持c++20及以上，需要配置fmt库的FetchContent
# # FetchContent_Declare(
# #     fmt
# #     GIT_REPOSITORY https://github.com/fmtlib/fmt.git
# #     GIT_TAG 11.2.0
# #     DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/third_party/fmt
# # )
# # FetchContent_MakeAvailable(fmt)

# # # 如果需要链接fmt库（某些编译器需要手动加上）
# # find_package(fmt REQUIRED)
# # target_link_libraries(cal PRIVATE fmt::fmt)

# # for Gtest:
# # add gtest lib for testing
# enable_testing()

# # use fechContent to download gtest
# include(FetchContent)
# FetchContent_Declare(
#     GTest
#     VERSION v1.16.0
#     URL https://github.com/google/googletest/archive/refs/tags/v1.16.0.zip
# )
# FetchContent_MakeAvailable(GTest)

# # or use find_package to find gtest: YOU need to config VCPKG package manager
# # find_package(GTest CONFIG REQUIRED)

# # 编译src目录下所有源文件
# file(GLOB SRC_FILES src/*.cpp)
# file(GLOB TEST_FILES test/*.cpp)

# # 如果仅仅想得到一个cal，请用以下的命令
# # add_executable(cal ${SRC_FILES})
# add_executable(run_tests ${TEST_FILES} ${SRC_FILES})

# target_link_libraries(run_tests PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)

# add_test(NAME unit_tests COMMAND run_tests)

cmake_minimum_required(VERSION 3.20) # [cite: 1]
project(cal_clone LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23) # [cite: 1]
set(CMAKE_CXX_STANDARD_REQUIRED ON) # [cite: 1]

enable_testing() # [cite: 1]

# Use FetchContent to download GTest (using your specified version and URL) [cite: 1]
include(FetchContent)
FetchContent_Declare(
    GTest
    VERSION v1.16.0 # [cite: 1]
    URL https://github.com/google/googletest/archive/refs/tags/v1.16.0.zip # [cite: 1]
)
FetchContent_MakeAvailable(GTest) # [cite: 1]

# --- 1. Define a library for the calendar logic ---
# List the source files for your calendar library (excluding main.cpp)
set(CALENDAR_LIB_SRCS
    src/option.cpp
    src/dateUtils.cpp
    src/calendar.cpp
)

# List the header files (optional for library definition itself, but good for clarity)
# Headers will be found via include_directories
# file(GLOB CALENDAR_LIB_HDRS src/*.h) # If all .h in src are part of the lib interface

add_library(calendar_logic STATIC ${CALENDAR_LIB_SRCS})

# Make headers in 'src' directory available to targets linking 'calendar_logic'
# and also for the library itself.
target_include_directories(calendar_logic PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)


# --- 2. Define the main 'cal' executable ---
# This executable will use main.cpp and link against the calendar_logic library
add_executable(cal src/main.cpp)
target_link_libraries(cal PRIVATE calendar_logic)


# --- 3. Define the 'run_tests' executable for GTest ---
# Glob for your test files located in the 'test' directory
file(GLOB TEST_FILES test/*.cpp) # [cite: 1]

add_executable(run_tests ${TEST_FILES}) # Only test source files for the test executable
target_link_libraries(run_tests PRIVATE 
    calendar_logic      # Link your library to be tested
    GTest::gtest        # Link GTest library [cite: 1]
    GTest::gtest_main   # Link GTest main (provides a main function for tests) [cite: 1]
    # GTest::gmock        # Uncomment if you use Google Mock features [cite: 1]
    # GTest::gmock_main   # Uncomment if you use Google Mock features with its own main [cite: 1]
)

# Add the test to CTest so it can be run by 'ctest' command
add_test(NAME unit_tests COMMAND run_tests) # [cite: 1]

# Optional: Set a common output directory for executables for convenience
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
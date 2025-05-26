cmake_minimum_required(VERSION 3.20)
project(cal_clone LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 编译src目录下所有源文件
file(GLOB SRC_FILES src/*.cpp)

add_executable(cal ${SRC_FILES})


# # 配置 fmt 库的 FetchContent
# FetchContent_Declare(
#     fmt
#     GIT_REPOSITORY https://github.com/fmtlib/fmt.git
#     GIT_TAG 11.2.0
#     DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/third_party/fmt
# )
# FetchContent_MakeAvailable(fmt)

# # 如果需要链接 fmt 库（某些编译器需要手动加上）
# find_package(fmt REQUIRED)
# target_link_libraries(cal PRIVATE fmt::fmt)
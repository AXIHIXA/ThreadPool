cmake_minimum_required(VERSION 3.28)
project(ThreadPool CXX)
set(CMAKE_CXX_STANDARD 17)

set(THREAD_POOL "pool")
add_executable(${THREAD_POOL}
        "include/ThreadPool.h"
        "src/ThreadPool.cpp"
        "src/main.cpp"
)
target_include_directories(${THREAD_POOL} PRIVATE "include/")
target_compile_options(${THREAD_POOL} PRIVATE $<$<CONFIG:Debug>:-O0>)

cmake_minimum_required(VERSION 3.28)
project(ThreadPool CXX)
set(CMAKE_CXX_STANDARD 17)

set(DEMO "demo")
add_executable(${DEMO}
        "include/ThreadPool.h"
        "src/ThreadPool.cpp"
        "src/main.cpp"
)
target_include_directories(${DEMO} PRIVATE "include/")

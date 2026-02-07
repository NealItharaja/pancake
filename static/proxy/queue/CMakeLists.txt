cmake_minimum_required(VERSION 3.16)

project(pancake_queue LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(pancake_queue STATIC
        blocking_queue.cpp
)

target_include_directories(pancake_queue
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

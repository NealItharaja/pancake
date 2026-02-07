cmake_minimum_required(VERSION 3.16)

project(pancake_util LANGUAGES CXX)

add_library(pancake_util STATIC
        util.cpp
)

target_include_directories(pancake_util
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_features(pancake_util
        PUBLIC
        cxx_std_17
)

cmake_minimum_required(VERSION 3.16)

add_library(pancake_distribution STATIC
        distribution.cpp
)

target_include_directories(pancake_distribution
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_features(pancake_distribution
        PUBLIC
        cxx_std_17
)

cmake_minimum_required(VERSION 3.16)

project(pancake_update_cache LANGUAGES CXX)

add_library(pancake_update_cache STATIC
        update_cache.cpp
)

target_include_directories(pancake_update_cache
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/third_party/parallel-hashmap
)

target_compile_features(pancake_update_cache
        PUBLIC
        cxx_std_17
)

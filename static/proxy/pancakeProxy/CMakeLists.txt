cmake_minimum_required(VERSION 3.16)

project(pancake_proxy LANGUAGES CXX)

add_library(pancake_proxy STATIC
        pancake_proxy.cpp
)

target_compile_features(pancake_proxy PUBLIC cxx_std_17)

target_include_directories(pancake_proxy
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/proxy
        ${PROJECT_SOURCE_DIR}/third_party/parallel-hashmap
)

target_link_libraries(pancake_proxy
        PUBLIC
        pancake_distribution
        pancake_update_cache
        pancake_queue
        pancake_util
        pancake_redis
        pancake_storage_interface
        pancake_encryption
        pancake_thrift
)

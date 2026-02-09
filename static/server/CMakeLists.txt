cmake_minimum_required(VERSION 3.16)
project(pancake_proxy_server LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
find_package(Threads REQUIRED)
find_package(Thrift REQUIRED)

include_directories(
        ${PROJECT_SOURCE_DIR}/../third_party/parallel_hashmap
)

add_executable(pancake_proxy_server
        proxy_server_main.cpp
)

target_include_directories(pancake_proxy_server PRIVATE
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/distribution
        ${PROJECT_SOURCE_DIR}/storage
        ${PROJECT_SOURCE_DIR}/thrift
        ${PROJECT_SOURCE_DIR}/util
)

target_link_libraries(pancake_proxy_server
        PRIVATE
        Threads::Threads
        thrift
)

target_compile_options(pancake_proxy_server PRIVATE
        -Wall
        -Wextra
        -Wno-unused-parameter
)

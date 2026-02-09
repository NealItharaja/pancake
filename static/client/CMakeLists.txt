cmake_minimum_required(VERSION 3.12)
project(pancake_client LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)
find_package(Thrift REQUIRED)

add_subdirectory(proxyClient)
add_subdirectory(asyncClient)
add_subdirectory(commandReader)

add_library(pancake_client STATIC
        client.h
)

target_include_directories(pancake_client
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(pancake_client
        PUBLIC
        proxy_client
        async_proxy_client
        command_response_reader
        thrift
        Threads::Threads
)

cmake_minimum_required(VERSION 3.16)

project(pancake_proxy_stack LANGUAGES CXX)

add_subdirectory(util)
add_subdirectory(queue)
add_subdirectory(distribution)
add_subdirectory(update_cache)
add_subdirectory(storage/redis)
add_subdirectory(storage)
add_subdirectory(encryption)
add_subdirectory(thrift)
add_subdirectory(pancake_proxy)

# This file handles building each task as sub-project.
#
# Each task becomes an independent project in which you can experiment.
#
# Tasks are added as separate sub-projects so as compilation errors in
# one task don't affect another task.

cmake_minimum_required(VERSION 3.16)
project(pancake_redis LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(HIREDIS REQUIRED hiredis)

add_library(pancake_redis STATIC
        redis.cpp
)

target_include_directories(pancake_redis
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${HIREDIS_INCLUDE_DIRS}
)

target_link_libraries(pancake_redis
        PUBLIC
        ${HIREDIS_LIBRARIES}
)

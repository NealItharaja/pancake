# This file handles building each task as sub-project.
#
# Each task becomes an independent project in which you can experiment.
#
# Tasks are added as separate sub-projects so as compilation errors in
# one task don't affect another task.

cmake_minimum_required(VERSION 3.16)

project(pancake_redis_test LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(
        main.cpp
        redis.cpp
)

target_include_directories(redis_test PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

find_library(HIREDIS_LIB hiredis)

if (NOT HIREDIS_LIB)
    message(FATAL_ERROR "hiredis not fond; install libhiredis-dev")
endif()

target_link_libraries(redis_test PRIVATE
        ${HIREDIS_LIB}
)

# Add tasks subprojects
include(cmake/utils.cmake)
add_subprojects(${CMAKE_SOURCE_DIR})
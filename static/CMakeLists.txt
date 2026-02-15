cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(pancake LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "In-source builds are not allowed. Use: mkdir cmakebuild && cd cmakebuild && cmake ..")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_check_modules(THRIFT REQUIRED thrift)
pkg_check_modules(HIREDIS REQUIRED hiredis)

find_library(CRYPTOPP_LIB cryptopp)

include_directories(
        ${CMAKE_SOURCE_DIR}/proxy/updateCache
        ${CMAKE_SOURCE_DIR}/proxy/queue
        ${CMAKE_SOURCE_DIR}/proxy/util
        ${CMAKE_SOURCE_DIR}/proxy/distribution
        ${CMAKE_SOURCE_DIR}/proxy/encryption
        ${CMAKE_SOURCE_DIR}/storage
        ${CMAKE_SOURCE_DIR}/client
        ${CMAKE_SOURCE_DIR}/service
        ${CMAKE_SOURCE_DIR}/thrift
)

file(GLOB_RECURSE PROXY_SOURCES
        proxy/*.cpp
)

file(GLOB_RECURSE CLIENT_SOURCES
        client/*.cpp
)

file(GLOB_RECURSE SERVICE_SOURCES
        service/*.cpp
)

file(GLOB_RECURSE SERVER_SOURCES
        server/*.cpp
)

file(GLOB_RECURSE TEST_SOURCES
        tests/*.cpp
)

add_executable(pancake_proxy_server
        ${PROXY_SOURCES}
        ${SERVICE_SOURCES}
        ${SERVER_SOURCES}
)

target_link_libraries(pancake_proxy_server
        Threads::Threads
        ${THRIFT_LIBRARIES}
        ${HIREDIS_LIBRARIES}
        ${CRYPTOPP_LIB}
)

add_executable(benchmark
        benchmark/benchmark.cpp
        ${CLIENT_SOURCES}
)

target_link_libraries(benchmark
        Threads::Threads
        ${THRIFT_LIBRARIES}
)

add_executable(distribution_test tests/distributionTest.cpp ${PROXY_SOURCES})
add_executable(update_cache_test tests/updateCacheTest.cpp ${PROXY_SOURCES})
add_executable(storage_test tests/storageTest.cpp ${PROXY_SOURCES})

target_link_libraries(distribution_test Threads::Threads)
target_link_libraries(update_cache_test Threads::Threads)
target_link_libraries(storage_test Threads::Threads)

add_compile_options(-Wall -Wextra -Wno-unused-parameter)

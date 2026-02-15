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

find_library(CRYPTOPP_LIB cryptopp REQUIRED)

# --------------------
# Include directories
# --------------------
set(PROXY_INCLUDE
        ${CMAKE_SOURCE_DIR}/proxy/updateCache
        ${CMAKE_SOURCE_DIR}/proxy/queue
        ${CMAKE_SOURCE_DIR}/proxy/util
        ${CMAKE_SOURCE_DIR}/proxy/distribution
        ${CMAKE_SOURCE_DIR}/proxy/encryption
        ${CMAKE_SOURCE_DIR}/proxy/pancakeProxy
        ${CMAKE_SOURCE_DIR}/proxy/parallel-hashmap
)

set(STORAGE_INCLUDE
        ${CMAKE_SOURCE_DIR}/storage
)

set(CLIENT_INCLUDE
        ${CMAKE_SOURCE_DIR}/client
)

set(SERVICE_INCLUDE
        ${CMAKE_SOURCE_DIR}/service
        ${CMAKE_SOURCE_DIR}/thrift
)

set(TEST_INCLUDE
        ${CMAKE_SOURCE_DIR}/tests
)

# --------------------
# Source files
# --------------------
file(GLOB_RECURSE PROXY_SOURCES proxy/*.cpp)
file(GLOB_RECURSE CLIENT_SOURCES client/*.cpp)
file(GLOB_RECURSE SERVICE_SOURCES service/*.cpp)
file(GLOB_RECURSE SERVER_SOURCES server/*.cpp)
file(GLOB_RECURSE TEST_SOURCES tests/*.cpp)

# --------------------
# Server executable
# --------------------
add_executable(pancake_proxy_server
        ${PROXY_SOURCES}
        ${SERVICE_SOURCES}
        ${SERVER_SOURCES}
)

target_include_directories(pancake_proxy_server PRIVATE
        ${PROXY_INCLUDE}
        ${STORAGE_INCLUDE}
        ${CLIENT_INCLUDE}
        ${SERVICE_INCLUDE}
)

target_link_libraries(pancake_proxy_server PRIVATE
        Threads::Threads
        ${THRIFT_LIBRARIES}
        ${HIREDIS_LIBRARIES}
        ${CRYPTOPP_LIB}
)

# --------------------
# Benchmark executable
# --------------------
add_executable(benchmark
        benchmark/benchmark.cpp
        ${CLIENT_SOURCES}
)

target_include_directories(benchmark PRIVATE
        ${CLIENT_INCLUDE}
        ${SERVICE_INCLUDE}
        ${PROXY_INCLUDE}
        ${STORAGE_INCLUDE}
)

target_link_libraries(benchmark PRIVATE
        Threads::Threads
        ${THRIFT_LIBRARIES}
)

# --------------------
# Test macro
# --------------------
macro(add_pancake_test target_name test_source)
    add_executable(${target_name} ${test_source} ${PROXY_SOURCES})
    target_include_directories(${target_name} PRIVATE
            ${PROXY_INCLUDE}
            ${STORAGE_INCLUDE}
            ${CLIENT_INCLUDE}
            ${SERVICE_INCLUDE}
            ${TEST_INCLUDE}
    )
    target_link_libraries(${target_name} PRIVATE Threads::Threads ${CRYPTOPP_LIB})
endmacro()

add_pancake_test(distribution_test tests/distributionTest.cpp)
add_pancake_test(update_cache_test tests/updateCacheTest.cpp)
add_pancake_test(storage_test tests/storageTest.cpp)

# --------------------
# Compile options
# --------------------
add_compile_options(-Wall -Wextra -Wno-unused-parameter)

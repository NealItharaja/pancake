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
find_program(THRIFT_COMPILER thrift REQUIRED)

set(THRIFT_IDL "${CMAKE_SOURCE_DIR}/thrift/pancake.thrift")
set(THRIFT_GEN_DIR "${CMAKE_BINARY_DIR}/gen-cpp")

add_custom_command(
        OUTPUT
            "${THRIFT_GEN_DIR}/pancake_types.h"
            "${THRIFT_GEN_DIR}/pancake_types.cpp"
            "${THRIFT_GEN_DIR}/pancake_constants.h"
            "${THRIFT_GEN_DIR}/pancake_constants.cpp"
            "${THRIFT_GEN_DIR}/pancake_thrift.h"
            "${THRIFT_GEN_DIR}/pancake_thrift.cpp"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${THRIFT_GEN_DIR}"
        COMMAND "${THRIFT_COMPILER}" --gen cpp -out "${THRIFT_GEN_DIR}" "${THRIFT_IDL}"
        DEPENDS "${THRIFT_IDL}"
        COMMENT "Generating C++ Thrift bindings from pancake.thrift"
        VERBATIM
)

add_custom_target(pancake_thrift_codegen ALL
        DEPENDS
            "${THRIFT_GEN_DIR}/pancake_types.h"
            "${THRIFT_GEN_DIR}/pancake_types.cpp"
            "${THRIFT_GEN_DIR}/pancake_constants.h"
            "${THRIFT_GEN_DIR}/pancake_constants.cpp"
            "${THRIFT_GEN_DIR}/pancake_thrift.h"
            "${THRIFT_GEN_DIR}/pancake_thrift.cpp"
)

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
        ${CMAKE_SOURCE_DIR}/client/asyncClient
        ${CMAKE_SOURCE_DIR}/client/proxyClient
        ${CMAKE_SOURCE_DIR}/client/commandReader
)

set(SERVICE_INCLUDE
        ${CMAKE_SOURCE_DIR}/service
        ${CMAKE_SOURCE_DIR}/thrift
)

set(TEST_INCLUDE
        ${CMAKE_SOURCE_DIR}/tests
)

file(GLOB_RECURSE PROXY_SOURCES proxy/*.cpp)
file(GLOB_RECURSE CLIENT_SOURCES client/*.cpp)
file(GLOB_RECURSE SERVICE_SOURCES service/*.cpp)
file(GLOB_RECURSE SERVER_SOURCES server/*.cpp)
file(GLOB_RECURSE STORAGE_SOURCES storage/*.cpp)
file(GLOB_RECURSE TEST_SOURCES tests/*.cpp)

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
        ${THRIFT_GEN_DIR}
)
add_dependencies(pancake_proxy_server pancake_thrift_codegen)

target_link_libraries(pancake_proxy_server PRIVATE
        Threads::Threads
        ${THRIFT_LIBRARIES}
        ${HIREDIS_LIBRARIES}
        ${CRYPTOPP_LIB}
)

option(PANCAKE_BUILD_BENCHMARK "Build benchmark target" OFF)
if (PANCAKE_BUILD_BENCHMARK)
    add_executable(benchmark
            benchmark/benchmark.cpp
            ${CLIENT_SOURCES}
    )

    target_include_directories(benchmark PRIVATE
            ${CLIENT_INCLUDE}
            ${SERVICE_INCLUDE}
            ${PROXY_INCLUDE}
            ${STORAGE_INCLUDE}
            ${THRIFT_GEN_DIR}
    )

    target_link_libraries(benchmark PRIVATE
            Threads::Threads
            ${THRIFT_LIBRARIES}
    )
    target_sources(benchmark PRIVATE
            "${THRIFT_GEN_DIR}/pancake_types.cpp"
            "${THRIFT_GEN_DIR}/pancake_constants.cpp"
            "${THRIFT_GEN_DIR}/pancake_thrift.cpp"
    )
    add_dependencies(benchmark pancake_thrift_codegen)
endif()

macro(add_pancake_test target_name test_source)
    add_executable(${target_name}
            ${test_source}
            ${PROXY_SOURCES}
            ${SERVICE_SOURCES}
            ${STORAGE_SOURCES}
    )
    target_include_directories(${target_name} PRIVATE
            ${PROXY_INCLUDE}
            ${STORAGE_INCLUDE}
            ${CLIENT_INCLUDE}
            ${SERVICE_INCLUDE}
            ${TEST_INCLUDE}
            ${THRIFT_GEN_DIR}
    )
    target_link_libraries(${target_name} PRIVATE
            Threads::Threads
            ${CRYPTOPP_LIB}
            ${THRIFT_LIBRARIES}
            ${HIREDIS_LIBRARIES}
    )
    add_dependencies(${target_name} pancake_thrift_codegen)
endmacro()

add_pancake_test(distribution_test tests/distributionTest.cpp)
add_pancake_test(update_cache_test tests/updateCacheTest.cpp)
add_pancake_test(storage_test tests/storageTest.cpp)

add_compile_options(-Wall -Wextra -Wno-unused-parameter)

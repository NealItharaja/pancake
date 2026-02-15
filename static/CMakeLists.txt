cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(pancake LANGUAGES CXX)

# ---- basic settings ----
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# no in-source builds
if (CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "In-source builds are not allowed. Use: mkdir build && cd build && cmake ..")
endif()

# output dirs (optional, but nice)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# ---- global includes (header-only deps) ----
# Adjust if your phmap folder is somewhere else
include_directories(
        ${CMAKE_SOURCE_DIR}/third_party/parallel-hashmap
)

# ---- add your actual subprojects ----
# Your tree: pancake/static/{client,proxy,server,service,storage,thrift}
add_subdirectory(thrift)
add_subdirectory(storage)
add_subdirectory(service)
add_subdirectory(proxy)
add_subdirectory(client)
add_subdirectory(server)
add_executable(new_target build.sh)

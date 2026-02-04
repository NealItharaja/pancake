cmake_minimum_required(VERSION 3.16)

project(pancake_encryption LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(CryptoPP REQUIRED)

add_library(pancake_encryption STATIC
        encryption.cpp
)

target_include_directories(pancake_encryption
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(pancake_encryption
        PUBLIC
        CryptoPP::CryptoPP
)

cmake_minimum_required(VERSION 3.16)
project(pancake_proxy_service CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(proxy_server
        proxy_server.cpp
        thrift.cpp
        thrift_response_service.cpp
)

target_include_directories(proxy_server PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# ---- Thrift ----
find_package(Thrift REQUIRED)
target_link_libraries(proxy_server PRIVATE Thrift::thrift)

# ---- Crypto++ ----
# On many systems: package name is "cryptopp" and library is "cryptopp"
# If your environment differs, adjust as needed.
find_library(CRYPTOPP_LIB cryptopp)
if (CRYPTOPP_LIB)
    target_link_libraries(proxy_server PRIVATE ${CRYPTOPP_LIB})
    target_compile_definitions(proxy_server PRIVATE HAS_CRYPTOPP=1)
endif()

# ---- parallel-hashmap ----
# If phmap is installed system-wide, includes work automatically.
# If you vendored it, add include path here, e.g.:
# target_include_directories(proxy_server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/third_party/parallel-hashmap)

# pthread (needed on Linux; harmless on macOS if not found)
find_package(Threads REQUIRED)
target_link_libraries(proxy_server PRIVATE Threads::Threads)

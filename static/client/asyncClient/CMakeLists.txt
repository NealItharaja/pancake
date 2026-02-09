add_library(async_proxy_client STATIC
        async_proxy_client.cpp
)

target_include_directories(async_proxy_client
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(async_proxy_client
        PUBLIC
        thrift
        pthread
)

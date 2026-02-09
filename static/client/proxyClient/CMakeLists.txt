add_library(proxy_client STATIC
        proxy_client.cpp
)

target_include_directories(proxy_client
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(proxy_client
        PUBLIC
        thrift
)

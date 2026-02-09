add_library(command_response_reader STATIC
        command_response_reader.cpp
)

target_include_directories(command_response_reader
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(command_response_reader
        PUBLIC
        thrift
)

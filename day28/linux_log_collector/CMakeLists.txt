cmake_minimum_required(VERSION 3.16)

project(linux_log_collector)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(log_collector
    examples/main.cpp
    src/file_descriptor.cpp
    src/log_collector.cpp
)

target_include_directories(log_collector
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

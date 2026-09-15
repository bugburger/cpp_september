cmake_minimum_required(VERSION 3.10)

project(day16_buffer_project)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(
    buffer_lib
    src/buffer.cpp
)

target_include_directories(
    buffer_lib
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(
    buffer_demo
    examples/main.cpp
)

target_link_libraries(
    buffer_demo
    PRIVATE
    buffer_lib
)

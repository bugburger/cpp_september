cmake_minimum_required(VERSION 3.10)

project(day17_library_demo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -------------------------
# 静态库
# -------------------------
add_library(
    calculator_static
    STATIC
    src/calculator.cpp
)

target_include_directories(
    calculator_static
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set_target_properties(
    calculator_static
    PROPERTIES
    OUTPUT_NAME calculator
)

# -------------------------
# 动态库
# -------------------------
add_library(
    calculator_shared
    SHARED
    src/calculator.cpp
)

target_include_directories(
    calculator_shared
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set_target_properties(
    calculator_shared
    PROPERTIES
    OUTPUT_NAME calculator
)

# -------------------------
# 使用静态库的程序
# -------------------------
add_executable(
    static_demo
    examples/main.cpp
)

target_link_libraries(
    static_demo
    PRIVATE
    calculator_static
)

# -------------------------
# 使用动态库的程序
# -------------------------
add_executable(
    shared_demo
    examples/main.cpp
)

target_link_libraries(
    shared_demo
    PRIVATE
    calculator_shared
)

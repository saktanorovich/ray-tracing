cmake_minimum_required(VERSION 3.30)
project(rt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)

# Main
add_executable(rt
    src/main.cpp
    src/math.cpp
)

target_include_directories(rt
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)

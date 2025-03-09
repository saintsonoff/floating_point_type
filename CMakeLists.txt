cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 23)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-fsanitize=address)
    add_link_options(-fsanitize=address)
endif()

set(CMAKE_CXX_FLAGS "-O3 -g -Wall -Werror=unused -Wfloat-conversion")

project(laba1)

add_executable(laba1 main.cpp)
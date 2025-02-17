cmake_minimum_required(VERSION 3.28)

set(CMAKE_C_STANDARD 11)
project(mai-os)

add_library(first SHARED include/first_headers.h src/first_lib.c)
target_include_directories(first PRIVATE include)
target_link_libraries(first PUBLIC m)

add_library(second SHARED include/second_headers.h src/second_lib.c)
target_include_directories(second PRIVATE include)
target_link_libraries(second PUBLIC m)

add_executable(lab4s static.c)
target_include_directories(lab4s PRIVATE include)
target_link_libraries(lab4s first second)

add_executable(lab4d dynamic.c)
target_include_directories(lab4d PRIVATE include)
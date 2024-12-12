cmake_minimum_required(VERSION 3.10)

add_library(prime SHARED prime_count.c)

target_include_directories(prime PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

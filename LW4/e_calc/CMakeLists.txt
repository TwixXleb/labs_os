cmake_minimum_required(VERSION 3.10)

add_library(e SHARED e_calc.c)

target_include_directories(e PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

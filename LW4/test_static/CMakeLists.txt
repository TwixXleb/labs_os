cmake_minimum_required(VERSION 3.10)

add_executable(test_static test_static.c)

target_link_libraries(test_static PRIVATE prime e)

target_include_directories(test_static PRIVATE ../prime_count ../e_calc)

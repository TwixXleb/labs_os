cmake_minimum_required(VERSION 3.10)

add_executable(test_dynamic test_dynamic.c)

target_include_directories(test_dynamic PRIVATE ../prime_count ../e_calc)

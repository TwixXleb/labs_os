cmake_minimum_required(VERSION 3.10)
project(DynamicLibExample)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(prime_count)
add_subdirectory(e_calc)
add_subdirectory(test_static)
add_subdirectory(test_dynamic)

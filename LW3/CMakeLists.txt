set(CMAKE_C_STANDARD 11)
cmake_minimum_required(VERSION 3.16)
project(mai-os)

add_library(LW3_utils include/utils.h src/utils.c)
target_include_directories(LW3_utils PRIVATE include)

add_library(LW3_parent include/parent.h src/parent.c)
target_include_directories(LW3_parent PRIVATE include)
target_link_libraries(LW3_parent LW3_utils)

add_executable(LW3_child1 src/child1.c)
target_include_directories(LW3_child1 PRIVATE include)
target_link_libraries(LW3_child1 LW3_utils)

add_executable(LW3_child2 src/child2.c)
target_include_directories(LW3_child2 PRIVATE include)
target_link_libraries(LW3_child2 LW3_utils)

add_executable(lab3 main.c)
target_include_directories(lab3 PRIVATE include)
target_link_libraries(lab3 LW3_parent)

add_dependencies(lab3 LW3_child1 LW3_child2)
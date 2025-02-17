cmake_minimum_required(VERSION 3.28)

set(CMAKE_C_STANDARD 11)
project(mai-os)

add_library(LW1_utils include/utils.h src/utils.c)
target_include_directories(LW1_utils PRIVATE include)

add_library(LW1_parent include/parent.h src/parent.c)
target_include_directories(LW1_parent PRIVATE include)
target_link_libraries(LW1_parent LW1_utils)

add_executable(LW1_child1 src/child1.c)
target_include_directories(LW1_child1 PRIVATE include)
target_link_libraries(LW1_child1 LW1_utils)

add_executable(LW1_child2 src/child2.c)
target_include_directories(LW1_child2 PRIVATE include)
target_link_libraries(LW1_child2 LW1_utils)

add_executable(lab1 main.c)
target_include_directories(lab1 PRIVATE include)
target_link_libraries(lab1 LW1_parent)

add_dependencies(lab1 LW1_child1 LW1_child2)
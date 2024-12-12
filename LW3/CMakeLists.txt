set(CMAKE_C_STANDARD 11)
project(mai-os)

add_library(LW3_utils src/utils.c)
target_include_directories(LW3_utils PRIVATE include)

add_library(LW3_parent src/parent.c)
target_include_directories(LW3_parent PRIVATE include)
target_link_libraries(LW3_parent LW3_utils)

add_executable(child1 src/child1.c)
target_include_directories(child1 PRIVATE include)
target_link_libraries(child1 LW3_utils)

add_executable(child2 src/child2.c)
target_include_directories(child2 PRIVATE include)
target_link_libraries(child2 LW3_utils)

add_executable(lab3 main.c)
target_include_directories(lab3 PRIVATE include)
target_link_libraries(lab3 LW3_parent)

add_dependencies(lab3 child1 child2)
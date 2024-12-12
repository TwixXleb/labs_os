set(CMAKE_C_STANDARD 11)
project(mai-os)

add_library(LW2_utils src/utils.c)
target_include_directories(LW2_utils PRIVATE include)

add_executable(lab2 main.c)
target_include_directories(lab2 PRIVATE include)
target_link_libraries(lab2 LW2_utils)
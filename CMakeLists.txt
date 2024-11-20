cmake_minimum_required(VERSION 3.16.5)
project(OS_LABS C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)


add_subdirectory(lib/googletest)

add_executable(OS_LABS main.cpp src/parent.cpp)

target_include_directories(OS_LABS PRIVATE include)

add_executable(child src/child.cpp)
target_include_directories(child PRIVATE include)

add_dependencies(OS_LABS child)

add_executable(${CMAKE_PROJECT_NAME}_tests test/test.cpp src/parent.cpp)
target_include_directories(${CMAKE_PROJECT_NAME}_tests PRIVATE ${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
target_link_libraries(${CMAKE_PROJECT_NAME}_tests PRIVATE ${CMAKE_PROJECT_NAME}_lib gtest gtest_main)

add_test(NAME ${CMAKE_PROJECT_NAME}_tests COMMAND ${CMAKE_PROJECT_NAME}_tests)
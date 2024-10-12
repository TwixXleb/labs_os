cmake_minimum_required(VERSION 3.16)

# Название проекта
project(OS_LABS C CXX)

# Устанавливаем стандарт для C и C++
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Добавляем опции компиляции только для Linux
if (UNIX)
    add_compile_options(-g -rdynamic)
else()
    add_compile_options(-g)
endif()

# Подключаем Google Test
add_subdirectory(lib/googletest)

# Основное приложение parent
add_executable(parent_exe src/parent.c)

# Основное приложение child
add_executable(child_exe src/child.c)

# Поиск всех тестовых файлов в директории tests
file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false test/*.cpp)

# Создание тестового приложения
if (TEST_SOURCES)
    add_executable(${CMAKE_PROJECT_NAME}_test ${TEST_SOURCES} ${TEST_LIB_SOURCES})
    target_include_directories(${CMAKE_PROJECT_NAME}_test PRIVATE ${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
    target_link_libraries(${CMAKE_PROJECT_NAME}_test PRIVATE ${CMAKE_PROJECT_NAME}_lib gtest gtest_main)
else()
    message(FATAL_ERROR "No test files found in the test directory")
endif()

cmake_minimum_required(VERSION 3.16)

# Название проекта
project(OS_LABS C CXX)

# Устанавливаем стандарт для C и C++
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Подключаем Google Test
add_subdirectory(lib/googletest)

# Устанавливаем пути к заголовочным файлам и исходникам
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test)

# Добавляем заголовочные файлы
include_directories(${INCLUDE_DIR})

# Находим все исходники в src
file(GLOB SOURCES LIST_DIRECTORIES false ${SOURCE_DIR}/*.c)

# Находим все тестовые файлы в test
file(GLOB TEST_SOURCES LIST_DIRECTORIES false ${TEST_DIR}/*.cpp)

# Создание библиотеки из исходников
add_library(${CMAKE_PROJECT_NAME}_lib ${SOURCES})

# Создание исполняемого файла для родительского процесса (parent.c)
add_executable(parent_exe ${SOURCE_DIR}/parent.c)
target_link_libraries(parent_exe PRIVATE ${CMAKE_PROJECT_NAME}_lib)

# Создание исполняемого файла для дочернего процесса (child.c)
add_executable(child_exe ${SOURCE_DIR}/child.c)
target_link_libraries(child_exe PRIVATE ${CMAKE_PROJECT_NAME}_lib)

# Тестовое приложение
if (TEST_SOURCES)
    add_executable(${CMAKE_PROJECT_NAME}_test ${TEST_SOURCES})
    target_link_libraries(${CMAKE_PROJECT_NAME}_test PRIVATE ${CMAKE_PROJECT_NAME}_lib gtest gtest_main)
else()
    message(FATAL_ERROR "No test files found in the test directory")
endif()

# Включаем поддержку тестов
enable_testing()

# Добавляем тесты
add_test(NAME ${CMAKE_PROJECT_NAME}_test COMMAND ${CMAKE_PROJECT_NAME}_test)

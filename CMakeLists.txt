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

# Отключаем флаг Werror при сборке Google Test
set(gtest_disable_pthreads ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error")

# Указываем директории для заголовков и исходников
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test)

include_directories(${INCLUDE_DIR})

# Поиск всех исходников в src, исключая main.cpp, parent.cpp и child.cpp
file(GLOB_RECURSE LIB_SOURCES LIST_DIRECTORIES false ${SOURCE_DIR}/*.cpp)
list(FILTER LIB_SOURCES EXCLUDE REGEX ".*main\\.cpp$")
list(FILTER LIB_SOURCES EXCLUDE REGEX ".*parent\\.cpp$")
list(FILTER LIB_SOURCES EXCLUDE REGEX ".*child\\.cpp$")

# Поиск всех тестовых файлов в test
file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false ${TEST_DIR}/*.cpp)

# Создание библиотеки из исходников
add_library(${CMAKE_PROJECT_NAME}_lib ${LIB_SOURCES})

# Добавляем parent.cpp в библиотеку
target_sources(${CMAKE_PROJECT_NAME}_lib PRIVATE ${SOURCE_DIR}/parent.cpp)

# Основное приложение
add_executable(${CMAKE_PROJECT_NAME}_exe main.cpp)
target_link_libraries(${CMAKE_PROJECT_NAME}_exe PRIVATE ${CMAKE_PROJECT_NAME}_lib)

# Создаем исполняемый файл для дочернего процесса (child)
add_executable(child_exe ${SOURCE_DIR}/child.cpp)

# Тестовое приложение
if (TEST_SOURCES)
    add_executable(${CMAKE_PROJECT_NAME}_test ${TEST_SOURCES})
    target_include_directories(${CMAKE_PROJECT_NAME}_test PRIVATE ${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
    target_link_libraries(${CMAKE_PROJECT_NAME}_test PRIVATE ${CMAKE_PROJECT_NAME}_lib gtest gtest_main)

    # Копируем child_exe в директорию с тестами после сборки OS_LABS_test
    add_custom_command(TARGET ${CMAKE_PROJECT_NAME}_test POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:child_exe> $<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}_test>
            )
else()
    message(FATAL_ERROR "No test files found in the test directory")
endif()

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

# Поиск всех исходников в src
file(GLOB_RECURSE SOURCES LIST_DIRECTORIES false ${SOURCE_DIR}/*.c)

# Поиск всех тестовых файлов в test
file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false ${TEST_DIR}/*.cpp)

# Добавляем родительский заголовочный файл
set(PARENT_HEADER ${INCLUDE_DIR}/parent.h)

# Создание библиотеки из исходников, включая parent.c и другие исходники
add_library(${CMAKE_PROJECT_NAME}_lib ${SOURCES} ${SOURCE_DIR}/parent.c)

# Создание исполняемого файла для родительского процесса (parent.c) — НЕ НУЖЕН, так как parent вызывается как функция
# add_executable(parent_exe ${SOURCE_DIR}/parent.c)

# Создание исполняемого файла для дочернего процесса (child.c)
add_executable(child_exe ${SOURCE_DIR}/child.c)

# Линковка исполняемого файла child_exe с библиотекой
target_link_libraries(child_exe PRIVATE ${CMAKE_PROJECT_NAME}_lib)

# Основное приложение (main.c)
add_executable(${CMAKE_PROJECT_NAME}_exe main.c)
target_link_libraries(${CMAKE_PROJECT_NAME}_exe PRIVATE ${CMAKE_PROJECT_NAME}_lib)

# Тестовое приложение
if (TEST_SOURCES)
    add_executable(${CMAKE_PROJECT_NAME}_test ${TEST_SOURCES})
    target_include_directories(${CMAKE_PROJECT_NAME}_test PRIVATE ${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

    # Линкуем с библиотекой и Google Test
    target_link_libraries(${CMAKE_PROJECT_NAME}_test PRIVATE ${CMAKE_PROJECT_NAME}_lib gtest gtest_main)
else()
    message(FATAL_ERROR "No test files found in the test directory")
endif()

# Включаем поддержку тестов
enable_testing()

# Добавляем тесты
add_test(NAME ${CMAKE_PROJECT_NAME}_test COMMAND ${CMAKE_PROJECT_NAME}_test)

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

# Поиск вспомогательных исходников (без файлов с main)
file(GLOB_RECURSE LIB_SOURCES LIST_DIRECTORIES false ${SOURCE_DIR}/utils.c)

# Создание библиотеки из вспомогательных исходников
add_library(${CMAKE_PROJECT_NAME}_lib ${LIB_SOURCES})

# Основное приложение parent
add_executable(parent_exe ${SOURCE_DIR}/main_parent.c ${SOURCE_DIR}/parent.c)
target_link_libraries(parent_exe PRIVATE ${CMAKE_PROJECT_NAME}_lib)

# Основное приложение child
add_executable(child_exe ${SOURCE_DIR}/main_child.c ${SOURCE_DIR}/child.c)
target_link_libraries(child_exe PRIVATE ${CMAKE_PROJECT_NAME}_lib)

# Поиск всех тестовых файлов в директории tests
file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false ${TEST_DIR}/*.cpp)

# Добавляем исходники для тестов, включая parent.c для тестирования parent_process
set(TEST_LIB_SOURCES
        ${SOURCE_DIR}/parent.c  # Исходный файл с реализацией parent_process
        ${SOURCE_DIR}/child.c   # Если нужно тестировать и child_process
        )

# Создание тестового приложения
if (TEST_SOURCES)
    add_executable(${CMAKE_PROJECT_NAME}_test ${TEST_SOURCES} ${TEST_LIB_SOURCES})
    target_include_directories(${CMAKE_PROJECT_NAME}_test PRIVATE ${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
    target_link_libraries(${CMAKE_PROJECT_NAME}_test PRIVATE ${CMAKE_PROJECT_NAME}_lib gtest gtest_main)
else()
    message(FATAL_ERROR "No test files found in the test directory")
endif()

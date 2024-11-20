cmake_minimum_required(VERSION 3.10)
project(ParentChildProcess)

# Установка стандарта C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Добавляем флаги компиляции для всех целей
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")

# Указываем директорию заголовочных файлов
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Добавляем исполняемый файл дочернего процесса
add_executable(child src/child.c)

# Добавляем библиотеку с родительским классом
add_library(parent_lib src/parent.c)

# Добавляем тестовый исполняемый файл
add_executable(test_exe test/test.cpp)

# Подключаем заголовочные файлы
target_include_directories(test_exe PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Ссылки на библиотеки
target_link_libraries(test_exe PRIVATE parent_lib)

# Копируем файл child в рабочую директорию тестов после сборки
add_custom_command(TARGET test_exe POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:child> ${CMAKE_CURRENT_BINARY_DIR}
        )

# Копируем input.txt в рабочую директорию сборки
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/input.txt ${CMAKE_CURRENT_BINARY_DIR}/input.txt COPYONLY)

# Добавляем тесты
enable_testing()
add_test(NAME ParentChildTest COMMAND test_exe)

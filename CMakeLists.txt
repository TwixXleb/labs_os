cmake_minimum_required(VERSION 3.16)

# Название проекта
project(OS_LABS C CXX)

# Устанавливаем стандарт для C и C++
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Подключаем Google Test
add_subdirectory(lib/googletest)

# Добавляем исполняемые файлы
add_executable(parent src/parent.c)
add_executable(child src/child.c)

# Добавляем тесты
enable_testing()
add_executable(test_runner test/test.cpp)
target_link_libraries(test_runner gtest_main)
add_test(NAME TestProcess COMMAND test_runner)
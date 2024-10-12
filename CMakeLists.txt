cmake_minimum_required(VERSION 3.10)

# Название проекта
project(ProcessCommunication)

# Стандарт C
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)

# Добавляем исполняемые файлы
add_executable(parent_exe src/parent.c)
add_executable(child_exe src/child.c src/process_functions.c src/process_functions.h)

# Создание библиотеки для тестов
add_library(OS_LABS_lib src/process_functions.c)

# Линкуем библиотеку с основными программами
target_link_libraries(parent_exe OS_LABS_lib)
target_link_libraries(child_exe OS_LABS_lib)

# Тесты
enable_testing()
add_executable(tests test/test.cpp)

# Линкуем тесты с библиотекой и GoogleTest
target_link_libraries(tests gtest gtest_main OS_LABS_lib)
add_test(NAME ProcessTests COMMAND tests)

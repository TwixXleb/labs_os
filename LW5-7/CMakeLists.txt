set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
project(mai-os)

add_library(lab567_tools src/tools.cpp)
target_include_directories(lab567_tools PRIVATE include)
target_link_libraries(lab567_tools PRIVATE /usr/lib/x86_64-linux-gnu/libzmq.so)

add_library(lab567_controller src/controller.cpp)
target_include_directories(lab567_controller PRIVATE include)
target_link_libraries(lab567_controller PRIVATE /usr/lib/x86_64-linux-gnu/libzmq.so lab567_tools lab567_worker)

add_library(lab567_worker src/worker.cpp)
target_include_directories(lab567_worker PRIVATE include)
target_link_libraries(lab567_worker PRIVATE /usr/lib/x86_64-linux-gnu/libzmq.so)

add_executable(lab567 main.cpp)
target_include_directories(lab567 PRIVATE include)
target_link_libraries(lab567 PRIVATE /usr/lib/x86_64-linux-gnu/libzmq.so lab567_controller)
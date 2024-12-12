project(mai-os)
include(FetchContent)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        release-1.11.0
)

FetchContent_MakeAvailable(googletest)
add_library(GTest::GTest INTERFACE IMPORTED)
target_link_libraries(GTest::GTest INTERFACE gtest_main)


set(CMAKE_CXX_STANDARD 17)

add_executable(LW1_test lab1_test.cpp)
target_include_directories(LW1_test PUBLIC ../LW1/include)
target_link_libraries(LW1_test PRIVATE utils parent GTest::GTest)

add_executable(LW2_test lab2_test.cpp)
target_include_directories(LW2_test PUBLIC ../LW2/include)
target_link_libraries(LW2_test PRIVATE LW2_utils GTest::GTest)

add_executable(LW3_test lab3_test.cpp)
target_include_directories(LW3_test PUBLIC ../LW3/include)
target_link_libraries(LW3_test PRIVATE LW3_utils LW3_parent GTest::GTest)

enable_testing()
add_test(NAME LW1_test COMMAND LW1_test)
add_test(NAME LW2_test COMMAND LW2_test)
add_test(NAME LW3_test COMMAND LW3_test)
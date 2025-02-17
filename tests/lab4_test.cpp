#include <gtest/gtest.h>
#include <cmath>

extern "C" {
#include <first_headers.h>
#include <second_headers.h>
}

// Тестируем функции из первой библиотеки
TEST(CompileTimeTest, TestFirstE) {
    EXPECT_FLOAT_EQ(E(10), pow(1 + 1.0/10, 10));
}

TEST(CompileTimeTest, TestFirstPrimeCount) {
    EXPECT_EQ(PrimeCount(1, 10), 4);  // Пример: простые числа на отрезке от 1 до 10: 2, 3, 5, 7
}

// Тестируем функции из второй библиотеки
TEST(CompileTimeTest, TestSecondE) {
    EXPECT_FLOAT_EQ(E_2(10), 2.7182818011463845);  // Погрешность по сравнению с реальным значением e
}

TEST(CompileTimeTest, TestSecondPrimeCount) {
    EXPECT_EQ(PrimeCount_2(1, 10), 4);  // Пример: простые числа на отрезке от 1 до 10: 2, 3, 5, 7
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
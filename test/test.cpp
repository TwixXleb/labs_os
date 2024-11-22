#include <gtest/gtest.h>
#include <cstdlib>

// Тест для проверки, что процессы корректно завершаются
TEST(ProcessTest, TestProcessCompletion) {
    // Запускаем родительский процесс
    int status = system("./parent");
    EXPECT_EQ(status, 0); // Убеждаемся, что процесс завершился без ошибок
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
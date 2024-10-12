#include <gtest/gtest.h>
#include <fstream>
#include <iostream>
#include "../include/parent.h"

std::string removeVowels(const std::string &input) {
    std::string result;
    for (char c : input) {
        if (c != 'a' && c != 'e' && c != 'i' && c != 'o' && c != 'u' && c != 'y') {
            result += c;
        }
    }
    return result;
}

TEST(ProcessTest, ChildProcessCommunication) {
    // Создаём входной файл
    std::ofstream input("input.txt");
    input << "TestStringWithVowels\nAnotherTest\n";
    input.close();

    // Вызываем parent как функцию
    run_parent("input.txt", "output1.txt", "output2.txt");

    // Проверяем, что файлы созданы и содержат ожидаемые данные
    std::ifstream f1("output1.txt"), f2("output2.txt");
    std::string content1, content2;

    ASSERT_TRUE(f1.is_open());
    ASSERT_TRUE(f2.is_open());

    std::getline(f1, content1);
    std::getline(f2, content2);

    std::string expected1 = removeVowels("TestStringWithVowels");
    std::string expected2 = removeVowels("AnotherTest");

    // Проверяем, что в одном из выходных файлов содержатся ожидаемые данные
    ASSERT_TRUE(content1.find(expected1) != std::string::npos || content1.find(expected2) != std::string::npos);
    ASSERT_TRUE(content2.find(expected1) != std::string::npos || content2.find(expected2) != std::string::npos);

    f1.close();
    f2.close();
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

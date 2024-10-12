#include <gtest/gtest.h>
#include <stdio.h>
#include <string>
#include <fstream>
#include <iostream>
#include <unistd.h> // Для получения текущего пути

// Функция, которая будет вызывать процесс parent
int runParent() {
    // Получаем абсолютный путь к исполняемому файлу
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));  // Получаем текущую директорию
    std::string command = std::string(cwd) + "/build/parent_exe";  // Собираем полный путь к parent_exe

    return system(command.c_str());
}

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

    // Вызываем parent процесс
    ASSERT_EQ(runParent(), 0);

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

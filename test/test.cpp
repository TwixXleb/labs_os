#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>

extern "C" {
#include "../include/child.h"  // Функция обработки строк
#include "../include/parent.h"
#include "../include/utils.h"
}

class Lab1Test : public testing::Test {
protected:
    const char *inputFile = "input.txt";
    const char *outputFile1 = "output1.txt";
    const char *outputFile2 = "output2.txt";

    void SetUp() override {
        // Подготовка входного файла с 8 строками
        std::ofstream input(inputFile);
        input << "Hello World\n";
        input << "This is a test\n";
        input << "GoogleTest Example\n";
        input << "Remove vowels please\n";
        input << "Another random string\n";
        input << "Some more text\n";
        input << "Just a string\n";
        input << "Final line\n";
        input.close();
    }

    void TearDown() override {
        auto removeIfExists = [](const char *path) {
            if (std::filesystem::exists(path)) {
                std::filesystem::remove(path);
            }
        };
        removeIfExists(inputFile);
        removeIfExists(outputFile1);
        removeIfExists(outputFile2);
    }
};

TEST_F(Lab1Test, ParentChildProcessTest) {
    // Запускаем родительский процесс, который работает с input.txt и двумя выходными файлами
    parent_process(inputFile, outputFile1, outputFile2);

    // Проверяем, что выходные файлы созданы
    std::ifstream file1Stream(outputFile1);
    std::ifstream file2Stream(outputFile2);
    ASSERT_TRUE(file1Stream.is_open());
    ASSERT_TRUE(file2Stream.is_open());

    // Читаем содержимое файлов
    std::string file1Content((std::istreambuf_iterator<char>(file1Stream)), std::istreambuf_iterator<char>());
    std::string file2Content((std::istreambuf_iterator<char>(file2Stream)), std::istreambuf_iterator<char>());

    // Проверяем, что во всех строках гласные удалены
    ASSERT_EQ(file1Content.find_first_of("AEIOUaeiou"), std::string::npos);
    ASSERT_EQ(file2Content.find_first_of("AEIOUaeiou"), std::string::npos);
}

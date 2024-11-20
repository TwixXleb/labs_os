#include "../include/parent.h"
#include <gtest/gtest.h>
#include <fstream>
#include <iostream>
#include <string>

TEST(ParentChildTest, RemoveVowelsAndOutput) {
    Parent parent("input.txt");
    parent.run();

    // Вывод содержимого выходных файлов
    std::ifstream outfile1("output1.txt");
    std::ifstream outfile2("output2.txt");

    std::cout << "Содержимое файла output1.txt:" << std::endl;
    std::string line;
    while (std::getline(outfile1, line)) {
        std::cout << line << std::endl;
    }
    outfile1.close();

    std::cout << "Содержимое файла output2.txt:" << std::endl;
    while (std::getline(outfile2, line)) {
        std::cout << line << std::endl;
    }
    outfile2.close();
}
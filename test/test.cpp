#include <gtest/gtest.h>
#include <fstream>
#include <string>
#include <vector>

// Функция для чтения файла в вектор строк
std::vector<std::string> read_file(const std::string& filename) {
    std::ifstream file(filename);
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    return lines;
}

// Тест для проверки, что строки без гласных записаны в правильные файлы
TEST(ProcessTest, TestOutputFiles) {
    // Создаем тестовый input.txt
    std::ofstream input_file("input.txt");
    input_file << "Hello\n";
    input_file << "World\n";
    input_file << "Test\n";
    input_file.close();

    // Запускаем родительский процесс
    system("./parent");

    // Читаем output_1.txt и output_2.txt
    auto output_1_lines = read_file("output_1.txt");
    auto output_2_lines = read_file("output_2.txt");

    // Проверяем, что строки без гласных записаны в правильные файлы
    for (const auto& line : output_1_lines) {
        EXPECT_EQ(line.find_first_of("aeiouAEIOU"), std::string::npos);
    }

    for (const auto& line : output_2_lines) {
        EXPECT_EQ(line.find_first_of("aeiouAEIOU"), std::string::npos);
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
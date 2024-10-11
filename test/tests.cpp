
#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>

extern "C" {
#include "../include/child.h"  // Assuming we want to test the removeVowels function
#include "../include/parent.h"
}

template<size_t N>
std::string join(const std::array<std::string, N> &elements, const std::string &delimiter) {
    std::string result;
    for (size_t i = 0; i < elements.size(); ++i) {
        if (i > 0) {
            result += delimiter;
        }
        result += elements[i];
    }
    return result;
}

class Lab1Test : public testing::Test {
protected:
    const char *fileWithInput = "input.txt";
    const char *fileWithOutput = "output.txt";

    void TearDown() override {
        auto removeIfExists = [](const char *path) {
            if (std::filesystem::exists(path)) {
                std::filesystem::remove(path);
            }
        };

        removeIfExists(fileWithInput);
        removeIfExists(fileWithOutput);
    }

    void WriteToFile(const std::string &input) {
        std::ofstream inFile(fileWithInput);
        inFile << input << std::endl;
    }

    std::string ReadFromFile() {
        std::ifstream outFile(fileWithOutput);
        std::string line, result;
        while (std::getline(outFile, line)) {
            result += line + "\n";
        }
        return result;
    }
};

TEST(RemoveVowelsTest, SimpleString) {
    char str[] = "Hello World";
    remove_vowels(str);
    EXPECT_STREQ(str, "Hll Wrld");
}

TEST(RemoveVowelsTest, OnlyVowels) {
    char str[] = "aeiouAEIOU";
    remove_vowels(str);
    EXPECT_STREQ(str, "");
}

TEST(RemoveVowelsTest, NoVowels) {
    char str[] = "bcdfgh";
    remove_vowels(str);
    EXPECT_STREQ(str, "bcdfgh");
}

// Пример теста для процесса (можно использовать заглушки)
TEST(ProcessTest, CreateChildProcesses) {
    // Здесь мы могли бы протестировать создание процессов, но для этого
    // потребуется симуляция процессов и каналов.
    ASSERT_TRUE(true);  // Пример заглушки
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
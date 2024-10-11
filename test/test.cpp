
#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>

extern "C" {
#include "../include/child.h"  // Testing the remove_vowels function and child process logic
#include "../include/parent.h"
#include "../include/utils.h"
}

class Lab1Test : public testing::Test {
protected:
    const char *file1 = "output_child1.txt";
    const char *file2 = "output_child2.txt";

    void TearDown() override {
        auto removeIfExists = [](const char *path) {
            if (std::filesystem::exists(path)) {
                std::filesystem::remove(path);
            }
        };
        removeIfExists(file1);
        removeIfExists(file2);
    }
};

TEST_F(Lab1Test, RemoveVowelsTest) {
    char str[] = "Hello World";
    remove_vowels(str);
    ASSERT_STREQ(str, "Hll Wrld");
}

TEST_F(Lab1Test, ParentChildProcessTest) {
    parent_process(file1, file2);

    // Check output files content
    std::ifstream file1Stream(file1);
    std::ifstream file2Stream(file2);
    ASSERT_TRUE(file1Stream.is_open());
    ASSERT_TRUE(file2Stream.is_open());

    std::string file1Content((std::istreambuf_iterator<char>(file1Stream)), std::istreambuf_iterator<char>());
    std::string file2Content((std::istreambuf_iterator<char>(file2Stream)), std::istreambuf_iterator<char>());

    // Simple check to ensure vowels were removed in both files
    ASSERT_EQ(file1Content.find_first_of("AEIOUaeiou"), std::string::npos);
    ASSERT_EQ(file2Content.find_first_of("AEIOUaeiou"), std::string::npos);
}

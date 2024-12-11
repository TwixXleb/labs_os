#include <gtest/gtest.h>
#include "../LW1/include/utils.h"

TEST(RemoveVowelsTest, BasicTest) {
    const char* input = "Hello World";
    char* output = remove_vowels(input);
    EXPECT_STREQ("Hll Wrld", output);
    free(output);
}

TEST(RemoveVowelsTest, AllVowels) {
    const char* input = "aeiouAEIOU";
    char* output = remove_vowels(input);
    EXPECT_STREQ("", output);
    free(output);
}

TEST(RemoveVowelsTest, NoVowels) {
    const char* input = "bcdfgBCDFG";
    char* output = remove_vowels(input);
    EXPECT_STREQ("bcdfgBCDFG", output);
    free(output);
}

TEST(RemoveVowelsTest, EmptyString) {
    const char* input = "";
    char* output = remove_vowels(input);
    EXPECT_STREQ("", output);
    free(output);
}

TEST(RemoveVowelsTest, NullInput) {
    char* output = remove_vowels(NULL);
    EXPECT_EQ(output, NULL);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
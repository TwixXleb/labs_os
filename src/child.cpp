#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
#include <cstdlib>

bool isVowel(char c) {
    c = std::tolower(static_cast<unsigned char>(c));
    return c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u' || c == 'y';
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Дочернему процессу требуется имя выходного файла в качестве аргумента." << std::endl;
        return EXIT_FAILURE;
    }

    std::string outputFileName = argv[1];
    std::ofstream outfile(outputFileName);
    if (!outfile.is_open()) {
        std::cerr << "Не удалось открыть выходной файл: " << outputFileName << std::endl;
        return EXIT_FAILURE;
    }

    std::string line;
    while (std::getline(std::cin, line)) {
        std::string result;
        for (char c : line) {
            if (!isVowel(c)) {
                result += c;
            }
        }
        outfile << result << std::endl;
    }

    outfile.close();
    return EXIT_SUCCESS;
}

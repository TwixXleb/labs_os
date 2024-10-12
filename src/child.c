#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int is_vowel(char c) {
    c = tolower(c);
    return (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u' || c == 'y');
}

void remove_vowels(char* str) {
    int i, j = 0;
    char result[1024];
    for (i = 0; str[i] != '\0'; i++) {
        if (!is_vowel(str[i])) {
            result[j++] = str[i];
        }
    }
    result[j] = '\0';
    strcpy(str, result);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <output_file>\n", argv[0]);
        return 1;
    }

    char buffer[1024];
    FILE *file;

    // Открытие файла для записи
    file = fopen(argv[1], "w");
    if (!file) {
        perror("Cannot open file for writing");
        return 1;
    }

    // Чтение данных из stdin
    while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        remove_vowels(buffer); // Удаление гласных из строки
        fprintf(file, "Processed string: %s\n", buffer);
    }

    fclose(file);
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void remove_vowels(char *str) {
    char *src = str, *dst = str;
    while (*src) {
        if (*src != 'a' && *src != 'e' && *src != 'i' && *src != 'o' && *src != 'u' &&
            *src != 'A' && *src != 'E' && *src != 'I' && *src != 'O' && *src != 'U') {
            *dst++ = *src;
        }
        src++;
    }
    *dst = '\0';
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <output_file>\n", argv[0]);
        return 1;
    }

    FILE *output_file = fopen(argv[1], "w");
    if (!output_file) {
        perror("Failed to open output file");
        return 1;
    }

    char line[256];
    while (fgets(line, sizeof(line), stdin)) {
        // Убираем символ новой строки, если он есть
        line[strcspn(line, "\n")] = 0;

        // Удаляем гласные
        remove_vowels(line);

        // Записываем строку в файл
        fprintf(output_file, "%s\n", line);
    }

    fclose(output_file);
    return 0;
}
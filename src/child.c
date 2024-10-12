#include <stdio.h>
#include <string.h>
#include <ctype.h>

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

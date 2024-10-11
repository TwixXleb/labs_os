
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "../include/child.h"

void parent_process(const char* inputFile, const char* file1, const char* file2) {
    int pipe1[2], pipe2[2];

    // Создаем каналы
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("Ошибка создания pipe");
        exit(1);
    }

    pid_t child1_pid = fork();
    if (child1_pid == 0) {
        close(pipe1[1]);  // Закрываем конец для записи
        child_process(pipe1[0], file1);
        close(pipe1[0]);
        exit(0);
    }

    pid_t child2_pid = fork();
    if (child2_pid == 0) {
        close(pipe2[1]);  // Закрываем конец для записи
        child_process(pipe2[0], file2);
        close(pipe2[0]);
        exit(0);
    }

    // Родительский процесс: читает строки из input.txt
    close(pipe1[0]);  // Закрываем конец для чтения
    close(pipe2[0]);

    srand(time(NULL));  // Инициализация случайных чисел
    char buffer[256];
    FILE* input = fopen(inputFile, "r");
    if (!input) {
        perror("Не удалось открыть input.txt");
        exit(1);
    }

    while (fgets(buffer, sizeof(buffer), input)) {
        printf("Parent: read line: %s", buffer);
        if (rand() % 100 < 80) {
            printf("Parent: writing to pipe1\n");
            write(pipe1[1], buffer, strlen(buffer));
        } else {
            printf("Parent: writing to pipe2\n");
            write(pipe2[1], buffer, strlen(buffer));
        }
    }

    fclose(input);
    printf("Parent: closing pipe1 and pipe2\n");
    close(pipe1[1]);  // Закрываем каналы после отправки всех данных
    close(pipe2[1]);

    printf("Parent: waiting for child processes to finish\n");
    wait(NULL);  // Ожидаем завершения дочерних процессов
    wait(NULL);
}

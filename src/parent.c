#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include "../include/parent.h"

void parent_process(const char* inputFile, const char* file1, const char* file2) {
    int pipe1[2], pipe2[2];

    // Создаем каналы
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("Ошибка создания pipe");
        exit(1);
    }

    pid_t child1_pid = fork();
    if (child1_pid == -1) {
        perror("Ошибка создания child1");
        exit(1);
    }
    if (child1_pid == 0) {
        // Закрываем конец для записи в дочернем процессе
        close(pipe1[1]);
        dup2(pipe1[0], STDIN_FILENO);  // перенаправляем входной поток
        close(pipe1[0]);

        // Вызов дочернего процесса через exec
        execl("./child", "child", file1, NULL);
        perror("Ошибка exec для child1");
        exit(1);
    }

    pid_t child2_pid = fork();
    if (child2_pid == -1) {
        perror("Ошибка создания child2");
        exit(1);
    }
    if (child2_pid == 0) {
        // Закрываем конец для записи в дочернем процессе
        close(pipe2[1]);
        dup2(pipe2[0], STDIN_FILENO);  // перенаправляем входной поток
        close(pipe2[0]);

        // Вызов дочернего процесса через exec
        execl("./child", "child", file2, NULL);
        perror("Ошибка exec для child2");
        exit(1);
    }

    // Родительский процесс: читает строки из input.txt и передает их через каналы
    close(pipe1[0]);  // Закрываем конец для чтения
    close(pipe2[0]);

    FILE *input_fp = fopen(inputFile, "r");
    if (input_fp == NULL) {
        perror("Ошибка открытия входного файла");
        exit(1);
    }

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), input_fp)) {
        buffer[strcspn(buffer, "\n")] = 0;

        if (rand() % 100 < 80) {
            strncat(buffer, "\n", sizeof(buffer) - strlen(buffer) - 1);

            write(pipe1[1], buffer, strlen(buffer));
        } else {
            strncat(buffer, "\n", sizeof(buffer) - strlen(buffer) - 1);
            write(pipe2[1], buffer, strlen(buffer));
        }
    }

    close(pipe1[1]);  // Закрываем конец для записи
    close(pipe2[1]);

    fclose(input_fp);

    // Ожидание завершения дочерних процессов
    wait(NULL);
    wait(NULL);
}


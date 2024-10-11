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
    FILE *input_fp = fopen(inputFile, "r");
    if (input_fp == NULL) {
        perror("Ошибка открытия входного файла");
        exit(1);
    }

    // Чтение строк из файла
    while (fgets(buffer, sizeof(buffer), input_fp)) {
        // Убираем символ новой строки
        buffer[strcspn(buffer, "\n")] = 0;

        // Генерация случайного числа для фильтрации
        if (rand() % 100 < 80) {
            printf("Sending to pipe1: %s\n", buffer);  // Отладочное сообщение
            write(pipe1[1], buffer, strlen(buffer));
        } else {
            printf("Sending to pipe2: %s\n", buffer);  // Отладочное сообщение
            write(pipe2[1], buffer, strlen(buffer));
        }
    }

    fclose(input_fp);

    // Закрываем концы для записи после отправки данных
    close(pipe1[1]);
    close(pipe2[1]);

    // Ожидание завершения дочерних процессов
    wait(NULL);  // Ждем завершения child1
    wait(NULL);  // Ждем завершения child2
}

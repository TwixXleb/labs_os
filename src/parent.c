#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>

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
        close(pipe1[1]);  // Закрываем конец для записи
        char pipe_in_str[10];
        sprintf(pipe_in_str, "%d", pipe1[0]);
        execl("./child", "child", pipe_in_str, file1, NULL);
        perror("Ошибка выполнения exec для child1");
        exit(1);
    }

    pid_t child2_pid = fork();
    if (child2_pid == -1) {
        perror("Ошибка создания child2");
        exit(1);
    }
    if (child2_pid == 0) {
        close(pipe2[1]);  // Закрываем конец для записи
        char pipe_in_str[10];
        sprintf(pipe_in_str, "%d", pipe2[0]);
        execl("./child", "child", pipe_in_str, file2, NULL);
        perror("Ошибка выполнения exec для child2");
        exit(1);
    }

    // Родительский процесс: читает строки из input.txt
    close(pipe1[0]);  // Закрываем конец для чтения
    close(pipe2[0]);

    srand(time(NULL));  // Инициализация случайных чисел
    char buffer[256];  // Буфер для строки

    FILE *input_fp = fopen(inputFile, "r");
    if (input_fp == NULL) {
        perror("Ошибка открытия входного файла");
        exit(1);
    }

    // Чтение строк из файла
    while (fgets(buffer, sizeof(buffer), input_fp)) {
        // Убираем символ новой строки
        buffer[strcspn(buffer, "\n")] = 0;

        int random_value = rand() % 100;
        if (random_value < 80) {
            // Отправляем в pipe1
            printf("Parent is sending to pipe1: %s\n", buffer);  // Отладочное сообщение
            strcat(buffer, "\n");  // Добавляем новую строку
            if (write(pipe1[1], buffer, strlen(buffer)) == -1) {
                perror("Ошибка записи в pipe1");
            }
        } else {
            // Отправляем в pipe2
            printf("Parent is sending to pipe2: %s\n", buffer);  // Отладочное сообщение
            strcat(buffer, "\n");  // Добавляем новую строку
            if (write(pipe2[1], buffer, strlen(buffer)) == -1) {
                perror("Ошибка записи в pipe2");
            }
        }

        // После записи в канал очищаем буфер
        memset(buffer, 0, sizeof(buffer));
    }

    fclose(input_fp);

    // Закрываем концы для записи после отправки данных
    close(pipe1[1]);
    close(pipe2[1]);

    // Ожидание завершения дочерних процессов
    wait(NULL);  // Ждем завершения child1
    wait(NULL);  // Ждем завершения child2
}

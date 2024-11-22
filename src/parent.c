#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#define INPUT_FILE "input.txt"

int main() {
    int pipe1[2], pipe2[2];
    pid_t pid1, pid2;

    // Создаем два пайпа
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Создаем первый дочерний процесс
    pid1 = fork();
    if (pid1 == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid1 == 0) {
        // Дочерний процесс 1
        close(pipe1[1]); // Закрываем запись в первый пайп
        dup2(pipe1[0], STDIN_FILENO); // Перенаправляем stdin на чтение из первого пайпа
        close(pipe1[0]); // Закрываем лишние дескрипторы
        execl("./child", "child", "output_1.txt", NULL); // Запускаем дочерний процесс
        perror("execl");
        exit(EXIT_FAILURE);
    }

    // Создаем второй дочерний процесс
    pid2 = fork();
    if (pid2 == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid2 == 0) {
        // Дочерний процесс 2
        close(pipe2[1]); // Закрываем запись во второй пайп
        dup2(pipe2[0], STDIN_FILENO); // Перенаправляем stdin на чтение из второго пайпа
        close(pipe2[0]); // Закрываем лишние дескрипторы
        execl("./child", "child", "output_2.txt", NULL); // Запускаем дочерний процесс
        perror("execl");
        exit(EXIT_FAILURE);
    }

    // Родительский процесс
    close(pipe1[0]); // Закрываем чтение из первого пайпа
    close(pipe2[0]); // Закрываем чтение из второго пайпа

    FILE *input_file = fopen(INPUT_FILE, "r");
    if (!input_file) {
        perror("Failed to open input file");
        return 1;
    }

    srand(time(NULL));

    char line[256];
    while (fgets(line, sizeof(line), input_file)) {
        // Убираем символ новой строки, если он есть
        line[strcspn(line, "\n")] = 0;

        // Определяем, в какой дочерний процесс отправить строку
        if (rand() % 10 < 8) {
            // 80% вероятность
            write(pipe1[1], line, strlen(line) + 1);
        } else {
            // 20% вероятность
            write(pipe2[1], line, strlen(line) + 1);
        }
    }

    fclose(input_file);
    close(pipe1[1]); // Закрываем запись в первый пайп
    close(pipe2[1]); // Закрываем запись во второй пайп

    // Ждем завершения дочерних процессов
    wait(NULL);
    wait(NULL);

    return 0;
}
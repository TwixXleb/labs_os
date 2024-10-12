#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

int main() {
    int pipe1[2], pipe2[2];
    char input_file[256] = "input.txt";
    char output_file1[256] = "output1.txt";
    char output_file2[256] = "output2.txt";

    // Инициализация генератора случайных чисел
    srand(time(NULL));

    // Создание pipe1 и pipe2
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("Pipe failed");
        return 1;
    }

    // Создание первого дочернего процесса
    pid_t child1_pid = fork();
    if (child1_pid == 0) {
        // В дочернем процессе child1
        close(pipe1[1]); // Закрываем сторону записи
        dup2(pipe1[0], STDIN_FILENO); // Перенаправляем ввод из pipe1
        execl("./child", "./child", output_file1, NULL);
        perror("exec failed for child1");
        return 1;
    } else if (child1_pid < 0) {
        perror("Fork failed for child1");
        return 1;
    }

    // Создание второго дочернего процесса
    pid_t child2_pid = fork();
    if (child2_pid == 0) {
        // В дочернем процессе child2
        close(pipe2[1]); // Закрываем сторону записи
        dup2(pipe2[0], STDIN_FILENO); // Перенаправляем ввод из pipe2
        execl("./child", "./child", output_file2, NULL);
        perror("exec failed for child2");
        return 1;
    } else if (child2_pid < 0) {
        perror("Fork failed for child2");
        return 1;
    }

    // В родительском процессе
    close(pipe1[0]); // Закрываем сторону чтения pipe1
    close(pipe2[0]); // Закрываем сторону чтения pipe2

    // Открываем входной файл и читаем строки
    FILE *input = fopen(input_file, "r");
    if (!input) {
        perror("Cannot open input file");
        return 1;
    }

    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), input) != NULL) {
        // Определяем, в какой pipe отправить строку (80% вероятность для pipe1)
        if ((rand() % 100) < 80) {
            write(pipe1[1], buffer, strlen(buffer) + 1);
        } else {
            write(pipe2[1], buffer, strlen(buffer) + 1);
        }
    }

    // Закрываем стороны записи pipe'ов
    close(pipe1[1]);
    close(pipe2[1]);

    fclose(input);

    // Ожидание завершения дочерних процессов
    wait(NULL);
    wait(NULL);

    return 0;
}

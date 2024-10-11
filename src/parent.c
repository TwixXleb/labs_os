#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void create_child_processes(int pipe1[2], int pipe2[2], char* file1, char* file2) {
    pid_t child1_pid, child2_pid;

    if ((child1_pid = fork()) == 0) {
        // Дочерний процесс 1
        close(pipe1[1]);  // Закрываем сторону записи
        dup2(pipe1[0], STDIN_FILENO);  // Перенаправляем pipe на стандартный ввод
        execlp("./child", "./child", file1, NULL);  // Запуск дочернего процесса
        perror("Ошибка execlp");
        exit(EXIT_FAILURE);
    }

    if ((child2_pid = fork()) == 0) {
        // Дочерний процесс 2
        close(pipe2[1]);  // Закрываем сторону записи
        dup2(pipe2[0], STDIN_FILENO);  // Перенаправляем pipe на стандартный ввод
        execlp("./child", "./child", file2, NULL);  // Запуск дочернего процесса
        perror("Ошибка execlp");
        exit(EXIT_FAILURE);
    }
}

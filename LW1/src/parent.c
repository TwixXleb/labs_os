#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <parent.h>
#include <utils.h>

void Parent(const char* pathToChild1, const char* pathToChild2, FILE* stream) {
    int pipe1[2], pipe2[2];
    pid_t pid1, pid2;

    char fileName1[MAX_BUFFER];
    char fileName2[MAX_BUFFER];
    char input[MAX_BUFFER];

    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("Pipe failed");
        exit(-1);
    }

    fgets(fileName1, MAX_BUFFER, stream);
    fileName1[strcspn(fileName1, "\n")] = 0;
    
    fgets(fileName2, MAX_BUFFER, stream);
    fileName2[strcspn(fileName2, "\n")] = 0;

    pid1 = fork();

    if (pid1 == -1) {
        perror("fork failed");
        exit(-1);
    }

    if (pid1 == 0) {
        // Закрытие записи в пайп родителя
        close(pipe1[1]);
        dup2(pipe1[0], STDIN_FILENO); // перенаправление стандартного ввода
        close(pipe1[0]);

        execl(pathToChild1, pathToChild1, fileName1, NULL);
        perror("execl failed");
        exit(-1);
    }

    pid2 = fork();

    if (pid2 == -1) {
        perror("fork failed");
        exit(-1);
    }

    if (pid2 == 0) {
        // Закрытие записи в пайп родителя
        close(pipe2[1]);
        dup2(pipe2[0], STDIN_FILENO); // перенаправление стандартного ввода
        close(pipe2[0]);

        execl(pathToChild2, pathToChild2, fileName2, NULL);
        perror("execl failed");
        exit(-1);
    }

    // Закрытие чтения в пайпах родителя
    close(pipe1[0]);
    close(pipe2[0]);

    // Инициализация генератора случайных чисел
    srand(getpid());

    while (strcmp(input, "q") != 0) {
        fgets(input, MAX_BUFFER, stream);
        input[strcspn(input, "\n")] = 0;

        // Генерация случайного числа от 0 до 99
        int randomChoice = rand() % 100;

        if (randomChoice < 80) {
            // 80% шанс отправить в первый пайп
            write(pipe1[1], input, strlen(input) + 1);
        } else {
            // 20% шанс отправить во второй пайп
            write(pipe2[1], input, strlen(input) + 1);
        }
        sleep(1);
    }

    // Закрытие записи в пайпы
    close(pipe1[1]);
    close(pipe2[1]);

    wait(NULL); // Ожидание завершения первого процесса
    wait(NULL); // Ожидание завершения второго процесса
}

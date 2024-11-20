#include "../include/parent.h"
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>
#include <ctime>

Parent::Parent(const std::string& inputFile) : inputFileName(inputFile) {
        // Инициализация генератора случайных чисел
        srand(time(0));
}

void Parent::run() {
    createChildren();
    processStrings();

    // Закрытие каналов после передачи данных
    close(pipe1_fd[1]);
    close(pipe2_fd[1]);

    // Ожидание завершения дочерних процессов
    int status;
    waitpid(child1_pid, &status, 0);
    waitpid(child2_pid, &status, 0);
}

void Parent::createChildren() {

    // Получаем путь к текущему исполняемому файлу
    char exePath[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", exePath, sizeof(exePath) - 1);
    if (len != -1) {
        exePath[len] = '\0';
    } else {
        perror("readlink");
        exit(EXIT_FAILURE);
    }

    // Получаем директорию исполняемого файла
    std::string dirPath = dirname(exePath);

    // Формируем путь к child_exe
    std::string childExePath = dirPath + "/child_exe";

    // Создание канала для первого дочернего процесса
    if (pipe(pipe1_fd) == -1) {
        std::cerr << "Не удалось создать канал для первого дочернего процесса." << std::endl;
        exit(EXIT_FAILURE);
    }

    // Создание первого дочернего процесса
    child1_pid = fork();
    if (child1_pid == -1) {
        std::cerr << "Не удалось создать первый дочерний процесс." << std::endl;
        exit(EXIT_FAILURE);
    } else if (child1_pid == 0) {
        // В дочернем процессе 1
        close(pipe1_fd[1]); // Закрываем запись
        dup2(pipe1_fd[0], STDIN_FILENO); // Перенаправляем stdin
        close(pipe1_fd[0]);

        execl(childExePath.c_str(), "child_exe", "output1.txt", (char*)NULL);
        std::cerr << "Ошибка запуска первого дочернего процесса." << std::endl;
        exit(EXIT_FAILURE);
    } else {
        // В родительском процессе
        close(pipe1_fd[0]); // Закрываем чтение
    }

    // Создание канала для второго дочернего процесса
    if (pipe(pipe2_fd) == -1) {
        std::cerr << "Не удалось создать канал для второго дочернего процесса." << std::endl;
        exit(EXIT_FAILURE);
    }

    // Создание второго дочернего процесса
    child2_pid = fork();
    if (child2_pid == -1) {
        std::cerr << "Не удалось создать второй дочерний процесс." << std::endl;
        exit(EXIT_FAILURE);
    } else if (child2_pid == 0) {
        // В дочернем процессе 2
        close(pipe2_fd[1]); // Закрываем запись
        dup2(pipe2_fd[0], STDIN_FILENO); // Перенаправляем stdin
        close(pipe2_fd[0]);

        execl(childExePath.c_str(), "child_exe", "output2.txt", (char*)NULL);
        std::cerr << "Ошибка запуска второго дочернего процесса." << std::endl;
        exit(EXIT_FAILURE);
    } else {
        // В родительском процессе
        close(pipe2_fd[0]); // Закрываем чтение
    }
}

void Parent::processStrings() {
    std::ifstream infile(inputFileName);
    if (!infile.is_open()) {
        std::cerr << "Не удалось открыть входной файл: " << inputFileName << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string line;
    while (std::getline(infile, line)) {
        int randomNumber = rand() % 100;
        if (randomNumber < 80) {
            // Отправляем в первый дочерний процесс
            sendStringToChild(line, 1);
        } else {
            // Отправляем во второй дочерний процесс
            sendStringToChild(line, 2);
        }
    }
    infile.close();
}

void Parent::sendStringToChild(const std::string& str, int childNum) {
ssize_t bytes_written;
if (childNum == 1) {
bytes_written = write(pipe1_fd[1], str.c_str(), str.length());
write(pipe1_fd[1], "\n", 1); // Добавляем перенос строки
if (bytes_written == -1) {
std::cerr << "Не удалось записать в канал первого дочернего процесса." << std::endl;
}
} else if (childNum == 2) {
bytes_written = write(pipe2_fd[1], str.c_str(), str.length());
write(pipe2_fd[1], "\n", 1); // Добавляем перенос строки
if (bytes_written == -1) {
std::cerr << "Не удалось записать в канал второго дочернего процесса." << std::endl;
}
} else {
std::cerr << "Неверный номер дочернего процесса." << std::endl;
}
}

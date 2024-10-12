#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include "../include/utils.h"

void child_process(int pipe_in, const char* file_name) {
    char buffer[256];
    int n;

    // Open the file for writing
    int file_fd = open(file_name, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (file_fd == -1) {
        perror("Failed to open output file");
        exit(1);
    }

    // Read from pipe and process
    while ((n = read(pipe_in, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[n] = '\0';  // Null terminate the string
        printf("Child received: %s\n", buffer);  // Отладочное сообщение
        remove_vowels(buffer);  // Process: remove vowels
        write(file_fd, buffer, strlen(buffer));  // Write the result to the file with correct length
    }

    close(file_fd);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <output_file>\n", argv[0]);
        return 1;
    }

    child_process(STDIN_FILENO, argv[1]);  // Передаем stdin как pipe_in и argv[1] как имя файла
    return 0;
}


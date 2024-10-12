#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "../include/utils.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <output_file>\n", argv[0]);
        exit(1);
    }

    char buffer[256];
    int n;

    int file_fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (file_fd == -1) {
        perror("Failed to open output file");
        exit(1);
    }

    while ((n = read(STDIN_FILENO, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[n] = '\0';
        remove_vowels(buffer);
        write(file_fd, buffer, strlen(buffer));
    }

    close(file_fd);
    return 0;
}

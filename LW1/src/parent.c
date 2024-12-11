#include "../include/utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>

int main() {
    int pipe1[2];
    int pipe2[2];

    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("Pipe failed");
        exit(1);
    }

    pid_t pid1 = fork();
    if (pid1 == -1) {
        perror("Fork failed");
        exit(1);
    }

    if (pid1 == 0) {
        // Child1 process
        close(pipe1[1]); // Close write end
        close(pipe2[0]); // Close unused pipe2 read end
        close(pipe2[1]); // Close unused pipe2 write end

        // Duplicate pipe1 read end to stdin
        dup2(pipe1[0], STDIN_FILENO);
        close(pipe1[0]);

        // Open output1 for writing
        freopen("output1", "w", stdout);

        // Execute child1
        execl("./child1", "child1", NULL);
        perror("Exec failed");
        exit(1);
    }

    pid_t pid2 = fork();
    if (pid2 == -1) {
        perror("Fork failed");
        exit(1);
    }

    if (pid2 == 0) {
        // Child2 process
        close(pipe2[1]); // Close write end
        close(pipe1[0]); // Close unused pipe1 read end
        close(pipe1[1]); // Close unused pipe1 write end

        // Duplicate pipe2 read end to stdin
        dup2(pipe2[0], STDIN_FILENO);
        close(pipe2[0]);

        // Open output2 for writing
        freopen("output2", "w", stdout);

        // Execute child2
        execl("./child2", "child2", NULL);
        perror("Exec failed");
        exit(1);
    }

    // Parent process
    close(pipe1[0]); // Close read end
    close(pipe2[0]); // Close read end

    srand(time(NULL)); // Seed random number generator

    // Generate and send strings to children
    for (int i = 0; i < 10; i++) {
        char* str = malloc(256);
        snprintf(str, 256, "String %d", i);
        int random = rand() % 100;

        if (random < 80) {
            // Send to child1
            write(pipe1[1], str, strlen(str)+1);
        } else {
            // Send to child2
            write(pipe2[1], str, strlen(str)+1);
        }
        free(str);
    }

    // Close pipe write ends after sending
    close(pipe1[1]);
    close(pipe2[1]);

    // Wait for children to finish
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    return 0;
}
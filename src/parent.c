
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "../include/child.h"

void parent_process(const char* file1, const char* file2) {
    int pipe1[2], pipe2[2];
    
    // Create pipes
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("Failed to create pipes");
        exit(1);
    }

    pid_t child1_pid = fork();
    if (child1_pid == 0) {
        close(pipe1[1]);  // Close unused write end
        child_process(pipe1[0], file1);
        close(pipe1[0]);
        exit(0);
    }

    pid_t child2_pid = fork();
    if (child2_pid == 0) {
        close(pipe2[1]);  // Close unused write end
        child_process(pipe2[0], file2);
        close(pipe2[0]);
        exit(0);
    }

    // Parent process: send data
    close(pipe1[0]);  // Close unused read end
    close(pipe2[0]);  // Close unused read end

    srand(time(NULL));  // Seed for randomness
    char* strings[] = {"First example string", "Another input", "Random text for testing"};
    
    for (int i = 0; i < 3; i++) {
        if (rand() % 100 < 80) {
            write(pipe1[1], strings[i], strlen(strings[i]));
        } else {
            write(pipe2[1], strings[i], strlen(strings[i]));
        }
    }

    close(pipe1[1]);  // Close write ends after sending data
    close(pipe2[1]);

    wait(NULL);  // Wait for child processes to finish
    wait(NULL);
}

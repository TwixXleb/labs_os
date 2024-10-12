#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include "../include/utils.h"
#include "../include/child.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <output_file>\n", argv[0]);
        return 1;
    }

    child_process(STDIN_FILENO, argv[1]);  // Передаем stdin как pipe_in и argv[1] как имя файла
    return 0;
}

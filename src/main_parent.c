#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include "../include/parent.h"

int main(int argc, char* argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <input_file> <output_file1> <output_file2>\n", argv[0]);
        return 1;
    }

    parent_process(argv[1], argv[2], argv[3]);
    return 0;
}

#include <stdio.h>
#include <stdlib.h>

#include "../include/utils.h"

int main() {
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), stdin)) {
        // Remove newline character
        buffer[strcspn(buffer, "\n")] = '\0';
        char* processed = remove_vowels(buffer);
        fprintf(stdout, "%s\n", processed);
        free(processed);
    }
    return 0;
}
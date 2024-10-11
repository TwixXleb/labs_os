#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 256

void remove_vowels(char* str) {
    char* p = str;
    char* q = str;

    while (*p) {
        if (!strchr("AEIOUaeiou", *p)) {
            *q++ = *p;
        }
        p++;
    }
    *q = '\0';
}

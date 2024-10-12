
#include <stdio.h>
#include <string.h>
#include "../include/utils.h"

void remove_vowels(char* str) {
    char* p = str;
    while (*p) {
        if (*p != 'a' && *p != 'e' && *p != 'i' && *p != 'o' && *p != 'u' &&
            *p != 'A' && *p != 'E' && *p != 'I' && *p != 'O' && *p != 'U') {
            *str++ = *p;
        }
        p++;
    }
    *str = '\0';
}

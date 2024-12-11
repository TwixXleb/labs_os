#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* remove_vowels(const char* str) {
    if (str == NULL) {
        return NULL;
    }
    const char vowels[] = "aeiouAEIOU";
    char* result = malloc(strlen(str) + 1);
    char* ptr = result;
    while (*str) {
        if (strchr(vowels, *str) == NULL) {
            *ptr++ = *str;
        }
        str++;
    }
    *ptr = '\0';
    return result;
}
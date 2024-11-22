#include <string.h>

#include <utils.h>

void remove_vowels(char *str) {
    char *src = str, *dst = str;
    while (*src) {
        if (*src != 'a' && *src != 'e' && *src != 'i' && *src != 'o' && *src != 'u' &&
            *src != 'A' && *src != 'E' && *src != 'I' && *src != 'O' && *src != 'U') {
            *dst++ = *src;
        }
        src++;
    }
    *dst = '\0';
}
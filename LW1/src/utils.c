#include <utils.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void remove_vowels(char* str) {
    if (str == NULL) {
        return;
    }
    int i = 0, j = 0;
    while (str[i] != '\0') {
        char lower_char = tolower(str[i]);
        if (lower_char != 'a' && lower_char != 'e' && lower_char != 'i' &&
            lower_char != 'o' && lower_char != 'u' && lower_char != 'y') {
            str[j++] = str[i];
        }
        i++;
    }
    str[j] = '\0';
}
}
#include <stdio.h>
#include "include/process_functions.h"

int main() {
    char testStr[] = "Hello World";
    printf("Пример работы remove_vowels:\n");
    printf("До: %s\n", testStr);
    remove_vowels(testStr);
    printf("После: %s\n", testStr);
    return 0;
}

#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>

typedef float (*EFunc)(int);
typedef int (*PrimeCountFunc)(int, int);

int main() {
    void *libHandle = NULL;
    EFunc E_func = NULL;
    PrimeCountFunc PrimeCount_func = NULL;
    int choice, arg1, arg2;
    int useSecondLib = 0;

    while (1) {
        // Закрываем предыдущую библиотеку, если она была загружена
        if (libHandle) {
            dlclose(libHandle);
        }

        // Загружаем нужную библиотеку
        const char *libPath = useSecondLib ? "./libfirst.so" : "./libsecond.so";  // Путь к библиотеке без префикса "lib"
        printf("Loading library: %s\n", libPath);

        // Загружаем библиотеку
        libHandle = dlopen(libPath, RTLD_LAZY);  // Открываем библиотеку для отложенной загрузки символов
        if (!libHandle) {
            fprintf(stderr, "Unable to load library: %s\n", dlerror());
            return 1;
        }

        // Получаем указатели на функции из библиотеки
        E_func = (EFunc) dlsym(libHandle, useSecondLib ? "E" : "E_2");
        PrimeCount_func = (PrimeCountFunc) dlsym(libHandle, useSecondLib ? "PrimeCount" : "PrimeCount_2");

        if (!E_func || !PrimeCount_func) {  // Проверка на ошибки при получении функций
            fprintf(stderr, "Unable to find functions: %s\n", dlerror());
            dlclose(libHandle);
            return 1;
        }

        // Основной цикл программы
        printf("Enter command (0 - switch lib, 1 - call E, 2 - call PrimeCount, -1 - exit): ");
        scanf("%d", &choice);

        if (choice == -1) break;  // Выход из программы

        switch (choice) {
            case 0:  // Переключение между библиотеками
                useSecondLib = !useSecondLib;
                printf("Switched to %s library\n", useSecondLib ? "first" : "second");
                break;
            case 1:  // Вызов функции E
                printf("Enter x: ");
                scanf("%d", &arg1);
                printf("E = %.5f\n", E_func(arg1));  // Вызов функции E
                break;
            case 2:  // Вызов функции PrimeCount
                printf("Enter A and B: ");
                scanf("%d %d", &arg1, &arg2);
                printf("PrimeCount = %d\n", PrimeCount_func(arg1, arg2));  // Вызов функции PrimeCount
                break;
            default:
                printf("Invalid command\n");
                break;
        }
    }

    if (libHandle) {
        dlclose(libHandle);  // Закрытие библиотеки перед завершением программы
    }

    return 0;
}

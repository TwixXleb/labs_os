#include <stdio.h>
#include <windows.h>
#include <string.h>

typedef int (*PrimeCountFunc)(int, int);
typedef double (*EFunc)(int);

int main() {
    char libChoice[10];
    char command[10];
    char *libPath = NULL;
    HMODULE libHandle = NULL;

    PrimeCountFunc PrimeCount = NULL;
    EFunc E = NULL;

    while (1) {
        printf("\nChoose the library (prime/elib) or type 'exit' to quit: ");
        scanf("%s", libChoice);

        if (strcmp(libChoice, "exit") == 0) {
            break;
        }

        if (strcmp(libChoice, "prime") == 0) {
            libPath = "libprime.dll";
        } else if (strcmp(libChoice, "elib") == 0) {
            libPath = "libe.dll";
        } else {
            printf("Invalid library name. Please choose 'prime' or 'elib'.\n");
            continue;
        }

        if (libHandle) {
            FreeLibrary(libHandle);
            libHandle = NULL;
            PrimeCount = NULL;
            E = NULL;
        }

        libHandle = LoadLibrary(libPath);
        if (!libHandle) {
            DWORD errorCode = GetLastError();
            printf("Error loading the library %s. Error code: %lu\n", libPath, errorCode);
            continue;
        }

        if (strcmp(libChoice, "prime") == 0) {
            PrimeCount = (PrimeCountFunc)GetProcAddress(libHandle, "PrimeCount");
            if (!PrimeCount) {
                printf("Error finding PrimeCount function in library %s\n", libPath);
                FreeLibrary(libHandle);
                libHandle = NULL;
                continue;
            }
        } else if (strcmp(libChoice, "elib") == 0) {
            E = (EFunc)GetProcAddress(libHandle, "E");
            if (!E) {
                printf("Error finding E function in library %s\n", libPath);
                FreeLibrary(libHandle);
                libHandle = NULL;
                continue;
            }
        }

        while (1) {
            printf("\nAvailable commands:\n");
            printf("1 - PrimeCount (only for prime)\n");
            printf("2 - E (only for elib)\n");
            printf("change - to switch library\n");
            printf("exit - to quit program\n");
            printf("Enter the command: ");
            scanf("%s", command);

            if (strcmp(command, "exit") == 0) {
                goto cleanup;
            }

            if (strcmp(command, "change") == 0) {
                break;
            }

            if (strcmp(command, "1") == 0 && PrimeCount) {
                int A, B;
                printf("Enter number values of A and B: ");
                scanf("%d %d", &A, &B);
                printf("Number of prime digits [%d, %d]: %d\n", A, B, PrimeCount(A, B));
            } else if (strcmp(command, "2") == 0 && E) {
                int x;
                printf("Enter x: ");
                scanf("%d", &x);
                printf("Number value of e if x = %d: %lf\n", x, E(x));
            } else {
                printf("Invalid command or function not available.\n");
            }
        }
    }

    cleanup:
    if (libHandle) {
        FreeLibrary(libHandle);
    }
    printf("Program terminated.\n");
    return 0;
}

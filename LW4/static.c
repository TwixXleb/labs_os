#include <stdio.h>
#include <first_headers.h>
#include <second_headers.h>

void callE(int x, int useSecondLib) {
    if (useSecondLib) {
        printf("Using second E implementation: %.5f\n", E_2(x));
    } else {
        printf("Using first E implementation: %.5f\n", E(x));
    }
}

void callPrimeCount(int A, int B, int useSecondLib) {
    if (useSecondLib) {
        printf("Using second PrimeCount implementation: %d\n", PrimeCount_2(A, B));
    } else {
        printf("Using first PrimeCount implementation: %d\n", PrimeCount(A, B));
    }
}

int main() {
    int choice, arg1, arg2;
    int useSecondLib = 0;

    while (1) {
        printf("Enter command (0 - switch lib, 1 - call E, 2 - call PrimeCount, -1 - exit): ");
        scanf("%d", &choice);

        if (choice == -1) break;

        switch (choice) {
            case 0:
                useSecondLib = !useSecondLib;
                printf("Switched to %s library\n", useSecondLib ? "second" : "first");
                break;
            case 1:
                printf("Enter x: ");
                scanf("%d", &arg1);
                callE(arg1, useSecondLib);
                break;
            case 2:
                printf("Enter A and B: ");
                scanf("%d %d", &arg1, &arg2);
                callPrimeCount(arg1, arg2, useSecondLib);
                break;
            default:
                printf("Invalid command\n");
                break;
        }
    }

    return 0;
}

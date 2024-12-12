#include <stdio.h>
#include "../prime_count/prime_count.h"
#include "../e_calc/e_calc.h"
#include <string.h>

int main() {
    char command[10];

    while (1) {
        printf("\nAvailable commands:\n");
        printf("1 - PrimeCount (test prime_count library)\n");
        printf("2 - E (test e_calc library)\n");
        printf("q - Exit\n");
        printf("Enter the command: ");
        scanf("%s", command);

        if (strcmp(command, "q") == 0) {
            break;
        }

        if (strcmp(command, "1") == 0) {
            int A, B;
            printf("Enter number values of A and B: ");
            scanf("%d %d", &A, &B);
            printf("Number of prime digits [%d, %d]: %d\n", A, B, PrimeCount(A, B));
        } else if (strcmp(command, "2") == 0) {
            int x;
            printf("Enter x: ");
            scanf("%d", &x);
            printf("Number value of e if x = %d: %lf\n", x, E(x));
        } else {
            printf("Invalid command.\n");
        }
    }

    printf("Program terminated.\n");
    return 0;
}

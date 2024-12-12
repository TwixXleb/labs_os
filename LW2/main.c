#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <utils.h>

pthread_mutex_t mutex;
int countOfActiveThreads = 0;
int maxCountOfThreads = 0;

int main(int argc, char* argv[]) {
    if (argc == 3) {
        if (pthread_mutex_init(&mutex, NULL) != 0) {
            perror("Could not initialize mutex!\n");
            exit(-1);
        }

        maxCountOfThreads = atoi(argv[1]) - 1;
        int *array;
        int n;

        if (*argv[2] == 'i') {
            printf("Введите количество элементов в массиве: ");
            scanf("%d", &n);

            array = (int*)malloc(n * sizeof(int));
            printf("Введите элементы массива:\n");
            for (int i = 0; i < n; i++) {
                scanf("%d", &array[i]);
            }
        } else {
            srand(time(NULL));
            n = rand() % 10000;

            array = (int *)malloc(n * sizeof(int));
            for (int i = 0; i < n; i++) {
                array[i] = rand() % 10000;
            }
            printf("Исходный массив:\n");
            for (int i = 0; i < n; i++) {
                printf("%d\n", array[i]);
            }
            printf("\n");
        }

        MergeData data = {array, 0, n - 1};
        MergeSort(&data);

        printf("Отсортированный массив:\n");
        for (int i = 0; i < n; i++) {
            printf("%d\n", array[i]);
        }
        printf("\n");

        free(array);
    } else {
        printf("Использование: ./lab2 <максимальное количество потоков> <i/r>\ni - ввести ");
        printf("произвольный массив\nr - сгенерировать случайный массив\n");
    }

    return 0;
}
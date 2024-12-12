#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

#include <utils.h>

void Merge(int *array, const int left, const int mid, const int right) {
    int leftSize = mid - left + 1;
    int rightSize = right - mid;

    int *leftArray = malloc(leftSize * sizeof(int));
    int *rightArray = malloc(rightSize * sizeof(int));

    for (int i = 0; i < leftSize; ++i) {
        leftArray[i] = array[left + i];
    }
    for (int i = 0; i < rightSize; ++i) {
        rightArray[i] = array[mid + 1 + i];
    }

    int i = 0, j = 0, k = left;
    while (i < leftSize && j < rightSize) {
        if (leftArray[i] <= rightArray[j]) {
            array[k++] = leftArray[i++];
        } else {
            array[k++] = rightArray[j++];
        }
    }

    while (i < leftSize) {
        array[k++] = leftArray[i++];
    }
    while (j < rightSize) {
        array[k++] = rightArray[j++];
    }

    free(leftArray);
    free(rightArray);
}

void *MergeSort(void *arg) {
    MergeData *data = arg;

    if (data->left < data->right) {
        int mid = data->left + (data->right - data->left) / 2;

        MergeData leftData = {data->array, data->left, mid};
        MergeData rightData = {data->array, mid + 1, data->right};

        pthread_t leftThread;
        int leftThreadCreated = 0;

        if (countOfActiveThreads < maxCountOfThreads && (mid - data->left) > 1000) {
            pthread_mutex_lock(&mutex);
            ++countOfActiveThreads;
            pthread_mutex_unlock(&mutex);
            if (!pthread_create(&leftThread, NULL, MergeSort, &leftData)) {
                leftThreadCreated = 1;
            }
        }

        if (!leftThreadCreated) {
            MergeSort(&leftData);
        }

        if (leftThreadCreated) {
            pthread_join(leftThread, NULL);
            pthread_mutex_lock(&mutex);
            --countOfActiveThreads;
            pthread_mutex_unlock(&mutex);
        }

        MergeSort(&rightData);

        Merge(data->array, data->left, mid, data->right);
    }
    return NULL;
}
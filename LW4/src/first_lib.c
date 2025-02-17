#include <first_headers.h>
#include <math.h>
#include <stdbool.h>

// Функция для вычисления числа e через (1 + 1/x)^x
float E(int x) {
    return pow((1 + 1.0/x), x);
}

// Функция для проверки простоты числа
bool is_prime(int num) {
    if (num <= 1) return false;
    for (int i = 2; i <= num / 2; i++) {
        if (num % i == 0) return false;
    }
    return true;
}

// Функция для подсчёта простых чисел на отрезке
int PrimeCount(int A, int B) {
    int count = 0;
    for (int i = A; i <= B; i++) {
        if (is_prime(i)) {
            count++;
        }
    }
    return count;
}

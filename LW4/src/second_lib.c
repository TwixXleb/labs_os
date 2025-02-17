#include <second_headers.h>
#include <math.h>
#include <stdbool.h>

// Функция для вычисления числа e с помощью суммы ряда 1/n!
float E_2(int x) {
    float result = 1.0;
    float fact = 1.0;
    for (int n = 1; n <= x; n++) {
        fact *= n;
        result += 1.0 / fact;
    }
    return result;
}

// Функция для подсчёта простых чисел на отрезке с помощью решета Эратосфена
int PrimeCount_2(int A, int B) {
    bool sieve[B + 1];
    for (int i = 0; i <= B; i++) sieve[i] = true;

    sieve[0] = sieve[1] = false;  // 0 и 1 не простые числа
    for (int i = 2; i * i <= B; i++) {
        if (sieve[i]) {
            for (int j = i * i; j <= B; j += i) {
                sieve[j] = false;
            }
        }
    }

    int count = 0;
    for (int i = A; i <= B; i++) {
        if (sieve[i]) count++;
    }
    return count;
}

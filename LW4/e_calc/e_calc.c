#include <math.h>
#include <e_calc.h>

__declspec(dllexport) double E(int x) {
    double e = 0.0;
    double factorial = 1.0;
    for (int n = 0; n <= x; n++) {
        if (n > 0) factorial *= n; // Вычисляем факториал
        e += 1.0 / factorial;
    }
    return e;
}


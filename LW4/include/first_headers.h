#ifndef MAI_OS_FIRST_HEADERS_H
#define MAI_OS_FIRST_HEADERS_H

#ifdef _WIN32
    #define EXPORT __declspec(dllexport)
#else
    #define EXPORT __attribute__((visibility("default")))
#endif

float E(int x);
int PrimeCount(int A, int B);

#endif //MAI_OS_FIRST_HEADERS_H

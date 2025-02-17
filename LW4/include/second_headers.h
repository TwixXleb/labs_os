#ifndef MAI_OS_SECOND_HEADERS_H
#define MAI_OS_SECOND_HEADERS_H

#ifdef _WIN32
    #define EXPORT __declspec(dllexport)
#else
    #define EXPORT __attribute__((visibility("default")))
#endif

float E_2(int x);
int PrimeCount_2(int A, int B);

#endif //MAI_OS_SECOND_HEADERS_H

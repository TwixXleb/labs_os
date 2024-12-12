#ifndef PRIME_COUNT_H
#define PRIME_COUNT_H

#ifdef _WIN32
#define EXPORT __declspec(dllimport)
#else
#define EXPORT
#endif

EXPORT int PrimeCount(int A, int B);

#endif

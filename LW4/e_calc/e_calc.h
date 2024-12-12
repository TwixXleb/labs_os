#ifndef E_CALC_H
#define E_CALC_H

#ifdef _WIN32
#define EXPORT __declspec(dllimport)
#else
#define EXPORT
#endif

EXPORT double E(int x);

#endif

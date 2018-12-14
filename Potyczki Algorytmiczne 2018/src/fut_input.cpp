#include "futbol.h"

#define Example1 1
#define Example2 2

#define MarekSokolowski1 3
#define MarekSokolowski50 4
#define MarekSokolowski6 5

#define VARIANT MarekSokolowski6

#if VARIANT == Example1
    const int n = 4, k = 2, p = 999999937;
#endif

#if VARIANT == Example2
    const int n = 10, k = 7, p = 13;
#endif

#if VARIANT == MarekSokolowski1
    const int n = 440581, k = 133004, p = 144495917;
#endif

#if VARIANT == MarekSokolowski50
    const int n = 997365040, k = 496917823, p = 999287789;
#endif

#if VARIANT == MarekSokolowski6
    const volatile int n = 104356305, k = 37890070, p = 231647681;
#endif

extern int node_id;

int GetN() { return node_id == 0 ? n : 0; }
int GetK() { return k; }
int GetP() { return p; } 
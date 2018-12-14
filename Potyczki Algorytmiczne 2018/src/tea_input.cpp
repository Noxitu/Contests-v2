#include <vector>
#include "teatr.h"

#define Example 1
#define BigResult 2
#define BigInput 3
#define TomaszNowak 4
#define MateuszLewko1 5
#define MateuszLewko2 6


#define VARIANT TomaszNowak

#if VARIANT == Example
const std::vector<int> data = {5, 2, 4, 4, 3};
#endif

#if VARIANT == BigResult
const std::vector<int> data = []()
{
    std::vector<int> ret;
    for (int i = 0; i < 1000000; ++i)
    {
        ret.push_back(1000000-i);
    }
    return ret;
}();
#endif

#if VARIANT == Example || VARIANT == BigResult
int GetN() { return (int) data.size(); }
int GetElement(int i) {
    return data.at(i);
}
#endif

#if VARIANT == BigInput
int GetN() { return 100*1000*1000; }
int GetElement(int i) {
    return i/100;
}
#endif

#if VARIANT == TomaszNowak
int GetN() { return int(1e8); }
int GetElement(int i) { return (i * 1ll * i) % int(1e6) + 1; }
#endif


#if VARIANT == MateuszLewko1
int GetN() { return 100 * 1000 * 1000; }
int GetElement(int i) { 
    return 1 + (((long long)i * 27) + i & (i %113) + ((i % 37) ^ (i % 20)) * 3571) % 1000000;
}
#endif


#if VARIANT == MateuszLewko2
int GetN() { return 100 * 1000 * 1000; }
int GetElement(int i) { 
if (i < 100 * 1000 * 1000 - 3) return 1 + i / 100;
else return 100 * 1000 * 1000 - i;
}
#endif


#if VARIANT == -1

#endif


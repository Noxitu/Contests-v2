#include "message.h"
#include "futbol.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <tuple>
#include <numeric>

const int PARAMETER = 5;


struct Magic 
{ 
   int value;
   unsigned int magic;
   int shift; 
};

template<typename T>
struct imul_result
{
   T high;
   unsigned int low;
};

inline imul_result<unsigned int> imul(unsigned int a, unsigned int b)
{
   //std::cerr << "imul32 " << a << " * " << b << std::endl;
   const unsigned long long result = 1LL * a * b;
   //std::cerr << "imul32 result = " << result << std::endl;

   return { 
      (result >> 32) & 0xffffffff,
      result & 0xffffffff
   };
}

inline imul_result<unsigned long long> imul(unsigned long long a, unsigned int b)
{
   //std::cerr << "imul64 " << a << " * " << b << std::endl;
   const unsigned int a_high = (a >> 32) & 0xffffffff;
   const unsigned int a_low = a & 0xffffffff;
   
   const auto res1 = imul(a_high, b);
   const auto res2 = imul(a_low, b);

   return { 
      (((unsigned long long) res1.high) << 32) + res1.low + res2.high,
      res2.low
   };
}

template<typename T>
inline T magic_calculate(const T lhs, const Magic rhs)
{
    const auto res = imul( lhs, rhs.magic );
    //std::cerr << "imul64 = (" << res.high << " << 32 ) + " << res.low << std::endl;

    T quotient = (res.high >> rhs.shift);// - (res.low >> 31);

    //std::cerr << "quotient = " << (res.high >> rhs.shift) << " - " << (res.low >> 31) << std::endl;
    //std::cerr << "quotient = " << quotient << std::endl;
    //std::cerr << "value = " << rhs.value << std::endl;

    T ret = lhs - quotient * rhs.value;

    if (ret + rhs.value < rhs.value) ret += rhs.value;

    return lhs - quotient * rhs.value;
}

int operator% (const int lhs, const Magic rhs) { return magic_calculate((unsigned int) lhs, rhs); }
long long operator% (const long long lhs, const Magic rhs) { return magic_calculate((unsigned long long) lhs, rhs); }

constexpr Magic magic(const long long ad)
{
    const long long two31 = 0x80000000LL;
    const long long anc = two31 - 1 - two31 % ad;

    for (int p = 32;; ++p)
    {
      const long long pow = (1LL << p);

      const long long q1 = pow / anc;
      const long long r1 = pow % anc;

      const long long q2 = pow / ad;
      const long long r2 = pow % ad;

      const long long delta = ad - r2;

      if (q1 > delta || q1 == delta && r1 != 0)
      {
         return {
            (int) ad, 
            (unsigned int) (q2+1), 
            (int) (p-32)
         };
      }
    }
}

inline std::tuple<int, int, int> egcd(int a, int b)
{
    if (a == 0)
        return std::tuple<int, int, int>{b, 0, 1};

    int g, y, x;
    std::tie(g, y, x) = egcd(b % a, a);

    return std::tuple<int, int, int>{g, x - (b / a) * y, y};
}

inline int modinv(int a, int m)
{
    int g, x, y;
    std::tie(g, x, y) = egcd(a, m);

    return (x+m) % m;
}

const int MAX_N = 1000000000;

void run()
{
    using namespace std;

    const int nodes = NumberOfNodes();
    const int id = MyNodeId();

    vector<int> work_offset(nodes+1, 0);

    for (int i = 0; i < nodes; ++i)
    {
        work_offset[i+1] = work_offset[i] + PARAMETER + i;
    }

    const int work_total = work_offset.back();

    const int input_k = GetK();
    const int MAX_K = std::min(input_k, MAX_N - input_k);
    const int p_value = GetP();
    const auto p = magic(p_value);

    
    const int work_begin = work_offset[id];
    const int work_end = work_offset[id+1];

    //cerr << ":: Work [" << work_begin << ", " << work_end << ") size = " << work_end-work_begin << endl;

    const int begin_k = 1LL * MAX_K * work_begin / work_total + 1;
    const int end_k = 1LL * MAX_K * work_end / work_total + 1;
    const int k_count = end_k - begin_k;

    //cerr << ":: Computing [" << begin_k << ", " << end_k << ")" << endl;

    std::vector<int> computed(k_count);

    for (int k = begin_k; k < end_k; ++k)
    {
        computed[k-begin_k] = modinv(k, p_value);
    }


    int answer = 1;
    int state = 1;

    int n;

    if (id == 0)
    {
        n = GetN();
    }
    else
    {
        Receive(id-1);
        n = GetInt(id-1);
        answer = GetInt(id-1);
        state = GetInt(id-1);
    }

    if (n == -1)
    {
        if (id != nodes-1)
        {
            PutInt(id+1, -1);
            PutInt(id+1, -1);
            PutInt(id+1, -1);
            Send(id+1);
        }
        return;
    }

    int actual_k = std::min(input_k, n - input_k);

    const int end2_k = std::min(end_k, actual_k+1);

    //cerr << ":: Joining [" << begin_k << ", " << end2_k << ")" << endl;

    long long sum = 0;

    const int *inv = &computed[0];
    for (int k = begin_k; k < end2_k; ++k, ++inv)
    {
        state = (int) (1LL * state * (n-k+1) % p * (*inv) % p);
        sum += state;
    }
    answer = (int) ((answer + sum) % p);

    if (end2_k == actual_k+1)
    {
        cout << answer << endl;

        if (id != nodes-1)
        {
            PutInt(id+1, -1);
            PutInt(id+1, -1);
            PutInt(id+1, -1);
            Send(id+1);
        }
    }
    else
    {
        if (id != nodes-1)
        {
            PutInt(id+1, n);
            PutInt(id+1, answer);
            PutInt(id+1, state);
            Send(id+1);
        }
    }
}

int main()
{
    run();
    return 0;
}
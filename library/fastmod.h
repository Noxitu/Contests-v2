
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
#include <gtest/gtest.h>
#include "fastmod.h"
#include <random>

TEST(fastmod, works_on_int)
{
    std::default_random_engine engine(42);
    std::uniform_int_distribution<int> a_dist(1, 1100000000);
    std::uniform_int_distribution<int> mod_dist(1, 1100000000);

    const int a = a_dist(engine);
    const int m = mod_dist(engine);
    const auto M = magic(m);

    for (int i = 0; i < 100000000; ++i)
    {
        ASSERT_EQ(a % m, a % M) << "On #" << (i+1) << " test"
                                << "\n  a = " << a 
                                << "\n  m = " << m 
                                << "\n  magic = " << M.magic 
                                << "\n  shift = " << M.shift;
    }
}

TEST(fastmod, works_on_long_long)
{
    std::default_random_engine engine(42);
    std::uniform_int_distribution<long long> a_dist(1, 1100000000000000000);
    std::uniform_int_distribution<int> mod_dist(1, 1100000000);

    const long long a = a_dist(engine);
    const int m = mod_dist(engine);
    const auto M = magic(m);

    for (int i = 0; i < 100000000; ++i)
    {
        ASSERT_EQ(a % m, a % M) << "On #" << (i+1) << " test"
                                << "\n  a = " << a 
                                << "\n  m = " << m 
                                << "\n  magic = " << M.magic 
                                << "\n  shift = " << M.shift;
    }
}

TEST(fastmod, works_on_long_long_custom)
{
    const long long a = 16368657578756354;
    const int m = 144495917;
    const auto M = magic(m);

    ASSERT_EQ(a % m, a % M) << "real quotient = " << (a/m)
                            << "\n  a = " << a 
                            << "\n  m = " << m 
                            << "\n  magic = " << M.magic 
                            << "\n  shift = " << M.shift;
}

TEST(fastmod, works_on_long_long_custom2)
{
    const long long a = 309045637428952500;
    const int m = 787846415;
    const auto M = magic(m);

    ASSERT_EQ(a % m, a % M) << "real quotient = " << (a/m)
                            << "\n  a = " << a 
                            << "\n  m = " << m 
                            << "\n  magic = " << M.magic 
                            << "\n  shift = " << M.shift;
}
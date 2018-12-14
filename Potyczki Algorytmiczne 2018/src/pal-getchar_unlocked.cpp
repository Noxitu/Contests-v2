#include <cstdio>
#include <cstdlib>
#include <tuple>

template<int base, int modulus>
class Hasher
{
private:
    int forward_state = 0;

    int backward_one = 1;
    int backward_state = 0;

public:
    inline void feed(char c)
    {
        forward_state = (1LL * forward_state * base + c) % modulus;

        backward_state = (1LL * c * backward_one + backward_state) % modulus;
        backward_one = (1LL * backward_one * base) % modulus;
    }

    inline int forward() const { return forward_state; }
    inline int backward() const { return backward_state; }
};

template<typename ...HasherTypes>
class Hashers;

template<typename FirstHasher, typename ...OtherHashers>
class Hashers<FirstHasher, OtherHashers...>
{
private:
    FirstHasher first;
    Hashers<OtherHashers...> second;

public:
    inline void feed(char c)
    {
        first.feed(c);
        second.feed(c);
    }

    inline auto forward() -> decltype(std::tuple_cat(
            std::tuple<int>(first.forward()), 
            second.forward() 
        )) const
    {
        return std::tuple_cat(
            std::tuple<int>(first.forward()), 
            second.forward() 
        );
    }

    inline auto backward() -> decltype(std::tuple_cat(
            std::tuple<int>(first.forward()), 
            second.forward() 
        )) const
    {
        return std::tuple_cat(
            std::tuple<int>(first.backward()), 
            second.backward() 
        );
    }
};

template<>
class Hashers<>
{
public:
    inline void feed(char c) {}
    inline std::tuple<> forward() const { return {}; }
    inline std::tuple<> backward() const { return {}; }
};

void test()
{   
    Hashers<
        Hasher<26, 2038081211>,
        Hasher<26, 2038081219>,
        Hasher<26, 2038081223>,
        // Just for sure:
        Hasher<23, 2038081211>,
        Hasher<29, 2038081211>
        > hasher;

    while (true)
    {
        int c = getchar_unlocked();

        if (c == EOF) break;
        if (c < 'a' || c > 'z') continue;

        hasher.feed(c - 'a');
    }

    bool isProbablyPalindrome = (hasher.forward() == hasher.backward());
    puts(isProbablyPalindrome ? "TAK\n" : "NIE\n");
}

int main()
{
    test();
    return 0;
}

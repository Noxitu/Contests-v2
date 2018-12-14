#include <iostream>
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

    inline auto forward() const
    {
        return std::tuple_cat(
            std::tuple<int>(first.forward()), 
            second.forward() 
        );
    }

    inline auto backward() const
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
    using namespace std;

    int length;
    cin >> length >> ws;

    Hashers<
        Hasher<26, 2038081211>,
        Hasher<26, 2038081219>,
        Hasher<26, 2038081223>,
        /*Hasher<26, 2038081229>,
        Hasher<26, 2038081231>,
        Hasher<26, 2038081289>,
        Hasher<26, 2038082257>,
        Hasher<26, 2038082261>,
        Hasher<26, 2038082269>,
        Hasher<26, 2038082317>,
        Hasher<26, 2038082833>,
        Hasher<26, 2038082867>,
        Hasher<26, 2038082873>,
        Hasher<26, 2038082897>,
        Hasher<26, 2038082947>,
        Hasher<26, 2038082993>,*/
        // Just for sure:
        Hasher<23, 2038081211>,
        Hasher<29, 2038081211>
        > hasher;

    char c;
    while (cin >> c)
    {
        c -= 'a';
        hasher.feed(c);
    }

    bool isProbablyPalindrome = (hasher.forward() == hasher.backward());
    cout << (isProbablyPalindrome ? "TAK\n" : "NIE\n");
}

int main()
{   
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    test();
    return 0;
}

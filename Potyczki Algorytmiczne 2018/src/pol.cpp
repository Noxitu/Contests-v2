#include <iostream>
#include <string>
#include <array>
#include <algorithm>

void test()
{
    using namespace std;

    array<char, 256> types;
    types.fill('1');

    for (unsigned char c : "aeiouy")
        types[c] = '2';

    string word;
    cin >> word;

    for_each(word.begin(), word.end(), [&](char &c) { c = types[c]; });

    auto calculate = [&]() -> long long
    {
        if (word.size() < 3)
            return 0;

        long long answer = 0;

        const auto begin = word.begin();
        const auto end = word.end();

        auto it = begin+2;
        for (; it != end; ++it)
        {
            const bool is_seq = (*it == *prev(it) && *it == *prev(it, 2));

            if (is_seq) break;
        }

        auto last_seq = it;

        for (2; it != end; ++it)
        {
            const bool is_seq = (*it == *prev(it) && *it == *prev(it, 2));

            if (is_seq)
                last_seq = it;

            answer += distance(begin, last_seq)-1;
        }

        return answer;
    };

    cout << calculate() << '\n';
}

int main()
{   
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    test();
    return 0;
}
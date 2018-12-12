#include <iostream>
#include <iterator>
#include <vector>
#include <algorithm>
#include <string>

class Number
{
public:
    std::string value = "0";

public:
    Number(std::string init = "0") : value(init) {}

    void append(std::string sufix)
    {
        value += sufix;
    }

    friend std::ostream& operator<< (std::ostream &out, const Number &number)
    {
        return out << number.value;
    }

    friend std::istream& operator>> (std::istream &in, Number &number)
    {
        return in >> number.value;
    }

    std::pair<size_t, const std::string&> key() const
    {
        return {value.size(), value};
    }

    size_t size() const { return value.size(); }
    void pop() { value.pop_back(); }
    
    friend bool operator< (const Number &lhs, const Number &rhs) { return lhs.key() < rhs.key(); }
    friend bool operator<= (const Number &lhs, const Number &rhs) { return lhs.key() <= rhs.key(); }

    friend bool operator>= (const Number &lhs, const Number &rhs) { return lhs.key() >= rhs.key(); }

    void operator++ ()
    {
        auto it = std::prev(value.end());

        while (*it == '9') {

            *it = '0';

            /*if (it == value.begin())
            {
                value = "1" + value;
                return;
            }*/

            --it;
        }

        *it += 1;
    }
};

void test()
{
    using namespace std;

    int n;
    cin >> n;

    vector<Number> input(n);
    copy_n(istream_iterator<Number>(cin), n, input.begin());

    long long answer = 0;
    long long zeros = 0;
    Number previous = {"0"};

    for (const Number &prefix : input)
    {
        Number low = prefix;
        Number high = prefix;

        while (previous >= high)
        {
            answer += 1;
            low.append("0");
            high.append("9");
        }

        answer += zeros;

        constexpr int MAX_LENGTH = 20;

        if (low.size() > MAX_LENGTH)
        {
            //if (previous.size() != MAX_LENGTH) throw std::logic_error("Incorrect previous size");

            low.pop();
            high.pop();

            zeros += 1;

            previous = {"0"};
        }

        if (low <= previous)
        {
            low = previous;
            ++low;
        }

        previous = low;
    }

    cout << answer << endl;
}

int main()
{   
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    test();
    return 0;
}
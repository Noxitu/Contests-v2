#include <iostream>
#include <iterator>
#include <algorithm>

void test()
{
    using namespace std;

    int n;
    cin >> n;

    int ones = 0;

    for (int i = 0; i < n; ++i)
    {
        int a;
        cin >> a;
        ones += (a == 1 ? 1 : 0);
    };
    
    cout << max(2-ones, 0) << '\n';
    cout << "2\n"
            "1 2\n";
}

int main()
{   
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    test();
    return 0;
}
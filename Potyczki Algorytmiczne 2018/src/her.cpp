#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_set>

void test()
{

    using namespace std;

    int n, m, k;
    cin >> n >> m >> k;

    vector<vector<int>> connections1(n, vector<int>());
    vector<vector<int>> connections2(n, vector<int>());
    
    for (int i = 0; i < m; ++i)
    {
        int a, b;
        cin >> a >> b;

        connections1[a-1].push_back(b-1);
        connections2[b-1].push_back(a-1);
    }

    vector<int> degrees1(n, 1);
    vector<int> degrees2(n, 1);
        
    for (int i = 0; i < n; ++i)
    {
        for (int j : connections1[i])
            degrees1[j] = max(degrees1[j], degrees1[i]+1);
    }

    for (int i = n-1; i >= 0; --i)
    {
        for (int j : connections2[i])
            degrees2[j] = max(degrees2[j], degrees2[i]+1);
    }

    int cutoff = 0;
    vector<int> subset;
    subset.reserve(n);

    while (true)
    {
        subset.clear();
        for (int i = 0; i < n; ++i)
            if (degrees1[i] > cutoff && degrees2[i] > cutoff)
                subset.push_back(i);

        //cerr << "reduced to " << subset.size() << endl;

        if (subset.size() < 60)
            break;

        ++cutoff;
    }

    const int S = (int) subset.size();
    if (S < k) { cout << -1 << endl; return; }

    bool valid = true;
    vector<int> idx(k);
    for (int i = 0; i < k; ++i)
        idx[i] = i;

    auto advance = [&]()
    {
        for (int i = k-1; i >= 0; --i)
        {
            idx[i] += 1;

            for (int j = 1; i+j < k; ++j)
                idx[i+j] = idx[i]+j;

            if (idx[k-1] < S-1)
                return;
        }

        valid = false;
    };

    vector<int> degrees(n);

    int answer = n;

    while(valid)
    {
        fill(degrees.begin(), degrees.end(), 1);

        for (auto i : idx)
            degrees[subset[i]] = 0;

        //cerr << "Testing without: ";
        //for (auto i : idx) cerr << subset[i] << ' '; cerr << endl;

        int highest = 0;
        
        for (int i = 0; i < n; ++i)
        {
            highest = max(highest, degrees[i]);

            for (int j : connections1[i])
            {
                if (degrees[j] == 0) continue;

                degrees[j] = max(degrees[j], degrees[i]+1);
            }
        }

        //cerr << "score = " << highest << endl;

        answer = min(answer, highest);

        advance();
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
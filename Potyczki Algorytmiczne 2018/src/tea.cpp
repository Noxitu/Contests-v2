#include "message.h"
#include "teatr.h"
#include <iostream>
#include <vector>

class MyTree
{
private:
    std::vector<std::vector<int>> data;
    
public:
    MyTree(int size)
    {
        while (size > 1)
        {
            data.push_back(std::vector<int>(size, 0));
            
            size = size/2 + (size%2 == 0 ? 0 : 1);
        }
    }
    
    void add(int value)
    {
        for (int i = 0; i < data.size(); ++i)
        {
            data[i][value] += 1;
            
            value /= 2;
        }
    }
    
    int query(const int value)
    {
        int ret = data[0][value];
        int x = value;
        int mask = 1;
        
        for (int i = 0; i < data.size(); ++i)
        {
            if (mask&value)
                ret += data[i][x-1];
            
            mask *= 2;
            x /= 2;
        }
        
        return ret;
    }
};


void run()
{
    using namespace std;

    const int nodes = NumberOfNodes();
    const int id = MyNodeId();

    const int n = GetN();

    std::vector<std::pair<int, int>> easy_tasks;

    for (int i = 0; i < nodes; ++i)
        for (int j = 0; j < nodes; ++j)
        {
            if (i < j)
                easy_tasks.push_back({i, j});
        }

    {
        long long answer = 0;

        vector<int> counts(1000002, 0);

        int counted_i = -1;

        auto solve_easy_task = [&](int i1, int i2)
        {
            if (i1 != counted_i)
            {
                counted_i = i1;
                fill(counts.begin(), counts.end(), 0);

                const int begin1 = 1LL * (i1) * n / nodes;
                const int end1 = 1LL * (i1+1) * n / nodes;

                for (int pos = begin1; pos != end1; ++pos)
                    counts[GetElement(pos)] += 1;

                for (int size = 1000000; size >= 0; --size)
                    counts[size] += counts[size+1];
            }

            const int begin2 = 1LL * (i2) * n / nodes;
            const int end2 = 1LL * (i2+1) * n / nodes;

            for (int pos = begin2; pos != end2; ++pos)
                answer += counts[GetElement(pos)+1];
        };

        auto solve_hard_task = [&](int i)
        {
            const int begin = 1LL * (i) * n / nodes;
            const int end = 1LL * (i+1) * n / nodes;

            MyTree tree(1000002);

            int count = 0;

            for (int pos = begin; pos != end; ++pos)
            {
                const int e = GetElement(pos);
                tree.add(e);
                count += 1;

                answer += (count - tree.query(e));
            }
        };

        const int begin = 1LL * (id) * easy_tasks.size() / nodes;
        const int end = 1LL * (id+1) * easy_tasks.size() / nodes;

        for (int i = begin; i < end; ++i)
            solve_easy_task(easy_tasks[i].first, easy_tasks[i].second);

        solve_hard_task(id);

        PutLL(0, answer);
        Send(0);
    }

    if (id != 0)
        return;

    long long total = 0;

    for (int i = 0; i < nodes; ++i)
    {
        const int source = Receive(-1);
        total += GetLL(source);
    }

    cout << total << endl;
}

int main()
{
    run();
    return 0;
}
#include <algorithm>
#include <iostream>
#include <vector>

class DeadlockDetector
{
    int N;                             // number of processes
    std::vector<std::vector<int>> WFG; // Wait-For Graph: WFG[i] lists processes that i waits on
    std::vector<int> publicLabel;      // u[i]
    std::vector<int> privateLabel;     // v[i]
    int nextLabel = 1;

  public:
    // Constructor: initialize N, WFG and labels to zero
    DeadlockDetector(int n) : N(n), WFG(n), publicLabel(n, 0), privateLabel(n, 0) {};

    // Block rule: process i blocks on process j
    void block(int i, int j)
    {
        // add edge i->j
        WFG[i].push_back(j);
        // update labels: new label = max(u[i], u[j]) + 1
        int k = std::max({publicLabel[i], publicLabel[j], nextLabel}) + 1;
        publicLabel[i] = privateLabel[i] = k;
        nextLabel = k;
    }

    // Transmit rule: propagate higher labels backward along edges
    void transmit()
    {
        bool changed;
        // iterate until no label changes occur
        do
        {
            changed = false;
            for (int x = 0; x < N; ++x)
            {
                for (int y : WFG[x])
                {
                    if (publicLabel[y] > publicLabel[x])
                    {
                        publicLabel[x] = publicLabel[y];
                        changed = true;
                    }
                }
            }
        } while (changed);
    }

    bool dfsCycle(int u, std::vector<std::vector<int>> &WFG, std::vector<bool> &visited, std::vector<bool> &inStack,
                  int &foundAt)
    {
        visited[u] = inStack[u] = true;
        for (int v : WFG[u])
        {
            if (!visited[v])
            {
                if (dfsCycle(v, WFG, visited, inStack, foundAt))
                    return true;
            }
            else if (inStack[v])
            {
                foundAt = v;
                return true;
            }
        }
        inStack[u] = false;
        return false;
    }

    bool detectCycle(int &cycleStart)
    {
        std::vector<bool> visited(N, false), inStack(N, false);
        for (int i = 0; i < N; ++i)
        {
            if (!visited[i])
            {
                if (dfsCycle(i, WFG, visited, inStack, cycleStart))
                    return true;
            }
        }
        return false;
    }
};

int main()
{
    int N;
    std::cout << "Enter number of processes: ";
    std::cin >> N;
    if (N <= 0)
    {
        std::cerr << "Invalid number of processes.\n";
        return 1;
    }

    DeadlockDetector detector(N);
    int M;
    std::cout << "Enter number of block operations: ";
    std::cin >> M;

    std::cout << "Enter " << M << " pairs 'i j' (process i blocks on process j):\n";
    for (int t = 0; t < M; ++t)
    {
        int i, j;
        std::cin >> i >> j;
        if (i < 0 || i >= N || j < 0 || j >= N)
        {
            std::cerr << "Invalid process ID: " << i << " or " << j << "\n";
            return 1;
        }
        detector.block(i, j); // apply Block rule
        detector.transmit();  // propagate labels after each block
    }

    int cycleNode;
    if (detector.detectCycle(cycleNode))
    {
        std::cout << "Deadlock detected involving process " << cycleNode << ".\n";
    }
    else
    {
        std::cout << "No deadlock detected.\n";
    }

    return 0;
}

#include <iostream>
#include <vector>

// WFG
void buildGraph(const std::vector<std::vector<int>> &procWait, const std::vector<int> &resOwner,
                std::vector<std::vector<int>> &graph)
{
    std::size_t nProcs = procWait.size();
    std::size_t nRess = resOwner.size();
    // Zeroing matrix
    for (std::size_t i = 0; i < nProcs; ++i)
        for (std::size_t j = 0; j < nProcs; ++j)
            graph[i][j] = 0;

    for (std::size_t i = 0; i < nProcs; ++i)
    {
        for (std::size_t j = 0; j < nRess; ++j)
        {
            if (procWait[i][j] && resOwner[j] != -1)
            {
                int owner = resOwner[j];
                graph[i][owner] = 1;
            }
        }
    }
}

// Recursive DFS for detecting cycle in graph
bool dfsCycle(int u, const std::vector<std::vector<int>> &graph, std::vector<bool> &visited,
              std::vector<bool> &recStack)
{
    visited[u] = true;
    recStack[u] = true;

    for (size_t v = 0; v < graph[u].size(); ++v)
    {
        if (graph[u][v])
        {
            if (!visited[v] && dfsCycle(v, graph, visited, recStack))
            {
                return true;
            }
            if (recStack[v])
            {
                return true;
            }
        }
    }

    recStack[u] = false;
    return false;
}

// Check if cycle exists in graph
bool hasDeadlock(const std::vector<std::vector<int>> &graph)
{
    std::size_t nProcs = graph.size();
    std::vector<bool> visited(nProcs, false);
    std::vector<bool> recStack(nProcs, false);

    for (int i = 0; i < nProcs; ++i)
    {
        if (!visited[i])
        {
            if (dfsCycle(i, graph, visited, recStack))
                return true;
        }
    }
    return false;
}

int main()
{
    int nProcs, nRess;
    std::cout << "Enter the number of processes: ";
    std::cin >> nProcs;
    std::cout << "Enter the number of resources: ";
    std::cin >> nRess;

    std::vector<std::vector<int>> proc_wait(nProcs, std::vector<int>(nRess));
    std::cout << "Enter expectation matrix proc_wait (0/1):\n";
    for (int i = 0; i < nProcs; ++i)
    {
        for (int j = 0; j < nRess; ++j)
        {
            std::cin >> proc_wait[i][j];
        }
    }

    // Load resource owner table
    std::vector<int> res_owner(nRess);
    std::cout << "Enter table res_owner (process ID or -1):\n";
    for (int j = 0; j < nRess; ++j)
    {
        std::cin >> res_owner[j];
    }

    // Expectations graph
    std::vector<std::vector<int>> graph(nProcs, std::vector<int>(nProcs));
    buildGraph(proc_wait, res_owner, graph);

    // Deadlock detection
    bool deadlock = hasDeadlock(graph);
    std::cout << (deadlock ? "Deadlock detected!" : "No deadlock.") << std::endl;
}

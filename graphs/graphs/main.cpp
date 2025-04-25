#include <fstream>
#include <iostream>
#include <vector>

std::vector<std::vector<int>> readAdjacencyMatrix(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cout << "Cannot open file.\n";
        exit(0);
    }
    int N;
    file >> N;
    std::vector<std::vector<int>> matrix(N, std::vector<int>(N));

    for (std::size_t i = 0; i < N; ++i)
    {
        for (std::size_t j = 0; j < N; ++j)
        {
            file >> matrix[i][j];
        }
    }

    return matrix;
}

bool goodCandidate(std::size_t startNode, const std::vector<std::vector<int>> &adjacencyMatrix)
{
    const std::size_t N = adjacencyMatrix.size();

    if (startNode >= N)
    {
        std::cerr << "Cannot start at node out of range.\n";
        return false;
    }

    std::vector<bool> visited(N, false);
    visited[startNode] = true;

    // Direct connections
    for (std::size_t i = 0; i < N; ++i)
    {
        if (adjacencyMatrix[startNode][i] == 1)
        {
            visited[i] = true;
        }
    }

    // Go through all nodes until they are available
    bool ok = true;
    while (ok)
    {
        ok = false;
        for (std::size_t i = 0; i < N; ++i)
        {
            if (visited[i] == true)
            {
                for (std::size_t j = 0; j < N; ++j)
                {
                    if ((adjacencyMatrix[i][j] == 1) && (visited[j] == false))
                    {
                        visited[j] = true; // Possible connection
                        ok = true;         // Size changed -> keep going
                    }
                }
            }
        }
    }

    for (bool b : visited)
    {
        if (b == false) // Could not access
        {
            return false;
        }
    }

    return true;
}

void checkAllNodes(const std::vector<std::vector<int>> adjacencyMatrix)
{
    for (std::size_t startNode = 0; startNode < adjacencyMatrix.size(); ++startNode)
    {
        std::cout << "Node: " << startNode << " result: ";
        std::cout << (goodCandidate(startNode, adjacencyMatrix) ? "Good\n" : "Bad\n");
    }
}

int main()
{
    std::vector<std::vector<int>> M1 = readAdjacencyMatrix("graph.txt");
    std::cout << "graph.txt\n";
    checkAllNodes(M1);

    std::vector<std::vector<int>> M2 = readAdjacencyMatrix("graph2.txt");
    std::cout << "\ngraph2.txt\n";
    checkAllNodes(M2);
    return 0;
}

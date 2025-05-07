#include <iostream>
#include <map>
#include <queue>
#include <string>
#include <vector>

struct Node
{
    int id;            // node ID
    int parent;        // parent ID (0 = none)
    bool hasToken;     // whether it currently holds the token
    std::queue<int> q; // IDs of requesting nodes

    Node(int ID = 0) : id(ID), parent(0), hasToken(false) {};
};

std::vector<Node> nodes; // global list

// Helper function to print the entire system state
void printState()
{
    std::cout << "-------------------------------\n";
    std::cout << "System state:\n";
    for (auto &n : nodes)
    {
        std::cout << "P" << n.id << " | parent = " << (n.parent ? "P" + std::to_string(n.parent) : "none")
                  << " | hasToken = " << (n.hasToken ? "T" : " ") << " | queue = [";
        std::queue<int> tmp = n.q;
        while (!tmp.empty())
        {
            std::cout << "P" << tmp.front();
            tmp.pop();
            if (!tmp.empty())
                std::cout << ", ";
        }
        std::cout << "]\n";
    }
    std::cout << "-------------------------------\n\n";
}

// Climb up the tree from 'requester' until the token holder is reached
// For each hop, forward the request and reverse the edge to shorten future paths
// Returns the ID of the node that holds the token
int climbToTokenHolder(int requester)
{
    int curr = requester;
    while (!nodes[curr - 1].hasToken)
    {
        int p = nodes[curr - 1].parent;
        if (p == 0)
        {
            // Reached the root
            break;
        }
        std::cout << "P" << curr << " -> REQUEST -> P" << p << "\n";
        nodes[p - 1].q.push(requester);
        // Reverse the edge so that curr now points directly to p
        nodes[curr - 1].parent = p;
        curr = p;
    }
    return curr;
}

// Deliver the token from 'holder' down to 'target'
// Each hop reverses the edge and prints the handover
void receiveToken(int holder, int target)
{
    std::cout << "<<< P" << holder << " releases TOKEN\n";
    nodes[holder - 1].hasToken = false;

    int curr = holder;
    // Continue handing off until we reach the target
    while (!nodes[curr - 1].q.empty())
    {
        int next = nodes[curr - 1].q.front();
        nodes[curr - 1].q.pop();

        if (next == curr)
        {
            // // Self-request; ignore and break
            break;
        }

        std::cout << "P" << curr << " -> TOKEN -> P" << next << "\n";
        // Reverse the edge: curr now points to next
        nodes[curr - 1].parent = next;
        nodes[next - 1].hasToken = true;
        curr = next;
        if (curr == target)
            break;
    }

    // Remove any leftover self-request in the target's queue
    if (!nodes[target - 1].q.empty() && nodes[target - 1].q.front() == target)
    {
        nodes[target - 1].q.pop();
    }

    // target becomes the new root (token holder)
    nodes[target - 1].parent = 0;
    nodes[target - 1].hasToken = true;

    std::cout << "P" << target << " enters CS\n";
}

// Initiates a CS request from node u.
// If u already holds the token, it enters CS immediately.
// Otherwise, it enqueues its request and triggers climb/receive logic
void requestCS(int u)
{
    std::cout << ">>> P" << u << " requests CS\n";

    // if node already has the token, enter CS immediately
    if (nodes[u - 1].hasToken)
    {
        std::cout << "P" << u << " already has token -> entering CS\n";
        // Clear any previous requests and simulate self handoff
        while (!nodes[u - 1].q.empty())
        {
            nodes[u - 1].q.pop();
        }
        nodes[u - 1].q.push(u);

        receiveToken(u, u);
        printState();
        return;
    }

    // Enqueue this node's request
    nodes[u - 1].q.push(u);

    // If this is the only pending request at u, climb to token holder and hand off
    if (nodes[u - 1].q.size() == 1)
    {
        int holder = climbToTokenHolder(u);
        receiveToken(holder, u);
    }

    printState();
}

int main()
{
    int N;
    std::cout << "Enter number of nodes N: ";
    std::cin >> N;
    nodes.resize(N);
    for (int i = 1; i <= N; i++)
    {
        nodes[i - 1] = Node(i);
    }

    std::cout << "Enter ID of the node that initially has the token: ";
    int root;
    std::cin >> root;
    nodes[root - 1].hasToken = true;

    std::cout << "Enter parent for each node (0 if none):\n";
    for (int i = 1; i <= N; i++)
    {
        std::cout << " Parent of P" << i << ": ";
        std::cin >> nodes[i - 1].parent;
    }

    printState();

    int M;
    std::cout << "How many CS requests to simulate? ";
    std::cin >> M;
    while (M--)
    {
        int u;
        std::cout << "\nRequesting node ID: P";
        std::cin >> u;
        requestCS(u);
    }

    std::cout << "Simulation complete.\n";
    return 0;
}

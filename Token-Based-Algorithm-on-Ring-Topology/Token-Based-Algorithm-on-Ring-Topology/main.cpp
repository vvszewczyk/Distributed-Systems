#include <iostream>
#include <queue>
#include <stdexcept>

class TokenRing
{
    struct Node
    {
        int ID;     // node ID
        Node *next; // pointer to the next node in the ring

        Node(int id, Node *n) : ID(id), next(n) {};
    };

    struct Token
    {
        int currentOwner = -1;        // ID of node holding the token
        std::queue<int> requestQueue; // queue of pending token requests
    };

    Node *head = new Node(0, nullptr); // start of the ring
    Token coin;                        // token held in the ring

  public:
    // Build a ring of N nodes and set initial token owner
    void createStructure(int N, int tokenOwner)
    {
        if (N <= 0 || tokenOwner >= N)
        {
            throw std::invalid_argument("Wrong N or token owner ID");
        }

        Node *curr = head;

        // set token owner if it's the head node
        if (curr->ID == tokenOwner)
        {
            coin.currentOwner = curr->ID;
        }

        // create remaining nodes and assign tokenOwner when found
        for (int id = 1; id < N; ++id)
        {
            curr->next = new Node(id, nullptr);
            if (id == tokenOwner)
            {
                coin.currentOwner = id;
            }
            curr = curr->next;
        }

        curr->next = head; // close the ring
    }

    // Print ring structure and token state
    void printTokenRing() const
    {
        Node *curr = head;
        do
        {
            std::cout << "ID: " << curr->ID << " next->ID:" << curr->next->ID << "\n";
            curr = curr->next;
        } while (curr != head);

        std::cout << "ID of Node which has token: " << coin.currentOwner << "\n";

        if (!coin.requestQueue.empty())
        {
            std::queue<int> toPrint = coin.requestQueue;
            std::cout << "Remaining queue: [";
            while (!toPrint.empty())
            {
                std::cout << toPrint.front() << " ";
                toPrint.pop();
            }
            std::cout << "]\n";
        }
    }

    // Simulate sending a request from a node to the token holder
    void sendRequest(int requesterID)
    {
        Node *curr = head;
        do
        {
            if (curr->ID == coin.currentOwner)
            {
                coin.requestQueue.push(requesterID);
                std::cout << "Node " << requesterID << " has been added to queue.\n";
                return;
            }
            curr = curr->next;
        } while (curr != head);

        throw std::runtime_error("Token owner not found.\n");
    }

    // Process one token pass: dequeue request, circulate token, enter CS
    void processToken()
    {
        if (coin.requestQueue.empty())
        {
            std::cout << "No pending requests. Token stays with Node " << coin.currentOwner << ".\n";
            return;
        }

        int nextTokenOwner = coin.requestQueue.front();
        coin.requestQueue.pop();

        // find current token holder in the ring
        Node *curr = head;
        while (curr->ID != coin.currentOwner)
        {
            curr = curr->next;
        }

        // pass token along until it reaches the next owner
        while (curr->ID != nextTokenOwner)
        {
            std::cout << "Passing token from " << curr->ID << " -> " << curr->next->ID << "\n";
            curr = curr->next;
        }

        coin.currentOwner = nextTokenOwner; // update token ownership
        std::cout << "Token received by Node " << nextTokenOwner << " (new Phold)\n";
        std::cout << "Entering CS\n";

        // display remaining requests
        std::cout << "Remaining queue: [";
        std::queue<int> temp = coin.requestQueue;
        while (!temp.empty())
        {
            std::cout << temp.front() << " ";
            temp.pop();
        }
        std::cout << "]\n";
    }
};

int main()
{
    int N;
    std::cout << "Enter size of ring: ";
    std::cin >> N;
    TokenRing TR;

    std::cout << "\nEnter ID of token owner: ";
    int tokenOwner;
    std::cin >> tokenOwner;

    try
    {
        TR.createStructure(N, tokenOwner); // build ring and assign initial holder
    }
    catch (const std::exception &e)
    {
        std::cout << e.what();
        return 1;
    }

    int M;
    std::cout << "Enter number of token requests: ";
    std::cin >> M;

    std::cout << "Enter " << M << " node IDs that request the token:\n";
    for (int i = 0; i < M; ++i)
    {
        int requesterID;
        std::cin >> requesterID;
        TR.sendRequest(requesterID); // collect all requests
    }

    TR.printTokenRing();

    std::cout << "\n--- Processing token requests ---\n";
    for (int i = 0; i < M; ++i)
    {
        TR.processToken(); // serve each request in FIFO order
    }

    std::cout << "\nFinal state:\n";
    TR.printTokenRing();

    std::cout << "Simulation complete.\n";
    return 0;
}

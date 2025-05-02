#include <algorithm>
#include <ctime>
#include <iostream>
#include <queue>
#include <random>
#include <tuple>
#include <utility>
#include <vector>

/* ----------------------  protocol definitions  ---------------------- */

enum class State
{
    Released,
    Wanted,
    Held
};

enum class MessageType
{
    Request,
    Reply
};

struct Message
{
    int from; // sender ID
    int to;   // receiver ID
    MessageType type;
    int timestamp; // Lamport time of *sender*

    Message(int f, int t, MessageType ty, int ts) : from(f), to(t), type(ty), timestamp(ts) {};
};

/* -------------------------  node behaviour  ------------------------- */

struct Node
{
    int id;        // Unique ID (tie breaker)
    int clock = 0; // Lamport logical clock
    State state = State::Released;
    int requestTs = -1;         // Frozen timestamp of *my* request
    std::vector<bool> gotReply; // REPLY bitmap
    std::vector<int> deferred;  // Queued requesters

    Node(int id, int N) : id(id), gotReply(N, false) {};

    /* --- broadcasting REQUEST to every other node --- */
    void broadcastRequest(std::queue<Message> &bus, int N)
    {
        state = State::Wanted;
        clock++;           // Local event
        requestTs = clock; // Freeze timestamp
        std::fill(gotReply.begin(), gotReply.end(), false);
        gotReply[id] = true;

        for (std::size_t i = 0; i < N; ++i)
        {
            if (i != id)
            {
                clock++; // Tick for each send
                bus.emplace(id, i, MessageType::Request, requestTs);
            }
        }
    }

    /* --- handle an incoming message --- */
    void recieveRequest(const Message &m, std::queue<Message> &bus, int N)
    {
        clock = std::max(clock, m.timestamp) + 1; // Lamport's rule

        if (m.type == MessageType::Reply) /* ------ REPLY ------ */
        {
            gotReply[m.from] = true;

            // Have I collected REPLYs from every other node?
            bool ok = true;
            for (std::size_t i = 0; i < N; ++i)
            {
                if (i == id)
                {
                    continue;
                }
                if (gotReply[i] == false)
                {
                    ok = false;
                    break;
                }
            }

            if (state == State::Wanted && ok)
            {
                state = State::Held;
                std::cout << "Node " << id << " ENTERS CS (timestamp=" << clock << ")\n";

                // CRITICAL SECTION

                state = State::Released;
                std::cout << "Node " << id << " LEAVES  CS  (timestamp=" << clock << ")\n";

                // Serve every deferred requester
                for (int dst : deferred)
                {
                    clock++;
                    bus.emplace(id, dst, MessageType::Reply, clock);
                }
                deferred.clear();
            }
        }
        else /* ---- REQUEST ---- */
        {
            auto his = std::tie(m.timestamp, m.from);
            auto mine = std::tie(requestTs, id);

            bool deferIt = (state == State::Held) || (state == State::Wanted && his > mine); // already inside CS

            if (deferIt == true)
            {
                deferred.push_back(m.from); // Answer later
            }
            else
            {
                clock++; // Tick for send
                bus.emplace(id, m.from, MessageType::Reply, clock);
            }
        }
    }
};

int main()
{
    std::size_t N, M;
    std::cout << "Number of nodes (N): ";
    std::cin >> N;
    std::cout << "Number of CS requests (M): ";
    std::cin >> M;

    std::vector<Node> nodes;
    nodes.reserve(N);
    for (std::size_t i = 0; i < N; ++i)
    {
        nodes.emplace_back(i, N);
    }

    std::queue<Message> bus; // global *network* queue

    for (std::size_t k = 0; k < M; ++k)
    {
        nodes[k % N].broadcastRequest(bus, N);
    }

    while (bus.empty() == false)
    {
        Message m = bus.front();
        bus.pop();
        nodes[m.to].recieveRequest(m, bus, N);
    }
}

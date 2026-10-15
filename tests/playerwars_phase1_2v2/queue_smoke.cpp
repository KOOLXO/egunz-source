// Compile from repository root:
//   c++ -std=c++17 -Itests/playerwars_phase1_2v2/stubs \
//     tests/playerwars_phase1_2v2/queue_smoke.cpp -o /tmp/pw_queue_smoke
//   /tmp/pw_queue_smoke
#include <array>
#include <cassert>
#include <iostream>
#include "../../src/MatchServer/MPlayerWarsQueue.h"

int main()
{
    using Result = MPlayerWarsQueue::JoinResult;
    MPlayerWarsQueue queue;
    std::array<MPlayerWarsQueue::Ticket, 4> match{};

    assert(!queue.Next2v2(match));
    assert(queue.Join(90, 10, MUID(0, 1), 2, 100) == Result::Joined);
    assert(queue.Join(90, 10, MUID(0, 1), 2, 101) == Result::AlreadyQueued);
    assert(queue.Join(91, 10, MUID(0, 2), 2, 101) == Result::AccountAlreadyQueued);
    assert(queue.Join(40, 11, MUID(0, 2), 2, 102) == Result::Joined);
    assert(queue.Join(70, 12, MUID(0, 3), 2, 103) == Result::Joined);
    assert(!queue.Next2v2(match));
    assert(queue.Join(20, 13, MUID(0, 4), 2, 104) == Result::Joined);
    assert(queue.Next2v2(match));
    // Tickets are selected by queue order, not sorted by character id.
    assert(match[0].character_id == 90);
    assert(match[1].character_id == 40);
    assert(match[2].character_id == 70);
    assert(match[3].character_id == 20);
    assert(queue.Count() == 4); // Selecting must not consume any ticket.
    assert(!queue.Cancel(90, MUID(0, 99))); // Only owner can cancel.
    assert(queue.Cancel(90, MUID(0, 1)));
    assert(!queue.Next2v2(match));
    assert(queue.Join(50, 14, MUID(0, 5), 2, 105) == Result::Joined);
    assert(queue.Next2v2(match));
    assert(match[0].character_id == 40);
    assert(match[3].character_id == 50);

    // A player going offline invalidates a ticket before matching.
    queue.Prune([](const MPlayerWarsQueue::Ticket& ticket) {
        return ticket.character_id != 70;
    });
    assert(queue.Count() == 3);
    assert(!queue.Next2v2(match));
    assert(queue.Join(30, 15, MUID(0, 6), 2, 106) == Result::Joined);
    assert(queue.Next2v2(match));

    for (const auto& ticket : match)
        assert(queue.Cancel(ticket.character_id, ticket.connection_uid));
    assert(queue.Count() == 0);
    assert(!queue.Next2v2(match));
    std::cout << "Player Wars 2v2 queue smoke tests passed\n";
}

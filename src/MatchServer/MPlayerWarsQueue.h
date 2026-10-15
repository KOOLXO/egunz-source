#pragma once
// EGunZ Player Wars / experimental branch only.
//
// Phase 1 is a server-owned WAITING queue, NOT competitive matchmaking.
// No client MMR, result, or identity is accepted. Match creation and rating
// updates are intentionally out of scope for this first network handshake.

#include <algorithm>
#include <array>
#include <cstdint>
#include <map>
#include <vector>
#include "MUID.h"

class MPlayerWarsQueue
{
public:
    struct Ticket
    {
        std::uint64_t id;
        int character_id;
        int account_id;
        MUID connection_uid;
        int playlist; // 2 = 2v2 for phase 1.
        std::uint64_t joined_at;
    };

    enum class JoinResult
    {
        Joined,
        AlreadyQueued,
        AccountAlreadyQueued
    };

    JoinResult Join(int character_id, int account_id, const MUID& connection_uid,
                    int playlist, std::uint64_t now)
    {
        const auto it = tickets_.find(character_id);
        if (it != tickets_.end())
            return it->second.connection_uid == connection_uid
                ? JoinResult::AlreadyQueued : JoinResult::AccountAlreadyQueued;

        for (const auto& entry : tickets_)
            if (entry.second.account_id == account_id)
                return JoinResult::AccountAlreadyQueued;

        tickets_.emplace(character_id, Ticket{
            next_ticket_id_++, character_id, account_id, connection_uid, playlist, now
        });
        return JoinResult::Joined;
    }

    bool Cancel(int character_id, const MUID& connection_uid)
    {
        const auto it = tickets_.find(character_id);
        if (it == tickets_.end() || it->second.connection_uid != connection_uid)
            return false;
        tickets_.erase(it);
        return true;
    }

    template <typename IsStillEligible>
    void Prune(IsStillEligible&& is_still_eligible)
    {
        for (auto it = tickets_.begin(); it != tickets_.end();)
        {
            if (!is_still_eligible(it->second))
                it = tickets_.erase(it);
            else
                ++it;
        }
    }

    // Phase-1 solo queue: select the four oldest 2v2 tickets. Do not consume
    // them here: the server removes tickets only after a successful launch.
    bool Next2v2(std::array<Ticket, 4>& out) const
    {
        if (tickets_.size() < out.size())
            return false;

        std::vector<Ticket> waiting;
        waiting.reserve(tickets_.size());
        for (const auto& entry : tickets_)
            if (entry.second.playlist == 2)
                waiting.push_back(entry.second);
        if (waiting.size() < out.size())
            return false;

        std::sort(waiting.begin(), waiting.end(),
            [](const Ticket& a, const Ticket& b) { return a.id < b.id; });
        std::copy_n(waiting.begin(), out.size(), out.begin());
        return true;
    }

    std::size_t Count() const { return tickets_.size(); }
    const Ticket* Find(int character_id) const
    {
        auto it = tickets_.find(character_id);
        return it == tickets_.end() ? nullptr : &it->second;
    }

private:
    std::map<int, Ticket> tickets_;
    std::uint64_t next_ticket_id_ = 1;
};

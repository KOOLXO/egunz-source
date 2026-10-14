#pragma once
// EGunZ Player Wars / experimental branch only.
//
// Phase 1 is a server-owned WAITING queue, NOT competitive matchmaking.
// No client MMR, result, or identity is accepted. Match creation and rating
// updates are intentionally out of scope for this first network handshake.

#include <cstdint>
#include <map>
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

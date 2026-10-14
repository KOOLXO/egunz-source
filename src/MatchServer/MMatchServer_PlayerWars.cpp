// EGunZ Player Wars: experimental JOIN/CANCEL handshake only.
// Matchmaking, stage creation, MMR and persistence are not enabled here.
#include "stdafx.h"
#include "MMatchServer.h"
#include "MSharedCommandTable.h"
#include "MMatchObject.h"
#include "MMatchGlobal.h"

namespace {
void ReplyPlayerWarsQueue(MMatchServer* server, MMatchObject* player,
                          MPlayerWarsQueueResult result, int queue_count)
{
    MCommand* command = server->CreateCommand(
        MC_MATCH_PLAYERWARS_RESPONSE_QUEUE, player->GetUID());
    command->AddParameter(new MCmdParamInt(static_cast<int>(result)));
    command->AddParameter(new MCmdParamInt(queue_count));
    server->RouteToListener(player, command);
}
}

void MMatchServer::OnPlayerWarsFindMatch(const MUID& connection_uid, int playlist)
{
    // Sender identity comes from the authenticated network connection.
    MMatchObject* player = GetPlayerByCommUID(connection_uid);
    if (!IsEnabledObject(player) || !player->GetCharInfo() ||
        !player->GetAccountInfo())
        return;

    auto respond = [this, player](MPlayerWarsQueueResult status) {
        ReplyPlayerWarsQueue(this, player, status, static_cast<int>(m_PlayerWarsQueue.Count()));
    };

    if (playlist != 2)
    {
        respond(MPlayerWarsQueueResult::UnsupportedPlaylist);
        return;
    }

    if (player->GetPlace() != MMP_LOBBY ||
        player->GetStageUID() != MUID(0, 0) ||
        player->IsLaunchedGame())
    {
        respond(MPlayerWarsQueueResult::NotInLobby);
        return;
    }

    if (player->IsLadderChallenging() || player->GetLadderGroupID() != 0)
    {
        respond(MPlayerWarsQueueResult::OtherQueue);
        return;
    }

    const int cid = player->GetCharInfo()->m_nCID;
    const int aid = player->GetAccountInfo()->m_nAID;
    if (cid <= 0 || aid <= 0)
    {
        respond(MPlayerWarsQueueResult::InvalidIdentity);
        return;
    }

    switch (m_PlayerWarsQueue.Join(cid, aid, player->GetUID(),
                                    playlist, GetTickTime()))
    {
    case MPlayerWarsQueue::JoinResult::Joined:
        respond(MPlayerWarsQueueResult::Joined);
        break;
    case MPlayerWarsQueue::JoinResult::AlreadyQueued:
        respond(MPlayerWarsQueueResult::AlreadyQueued);
        break;
    case MPlayerWarsQueue::JoinResult::AccountAlreadyQueued:
        respond(MPlayerWarsQueueResult::AccountAlreadyQueued);
        break;
    }
}

void MMatchServer::OnPlayerWarsCancelMatch(const MUID& connection_uid)
{
    MMatchObject* player = GetPlayerByCommUID(connection_uid);
    if (!IsEnabledObject(player) || !player->GetCharInfo())
        return;

    const bool cancelled = m_PlayerWarsQueue.Cancel(
        player->GetCharInfo()->m_nCID, player->GetUID());
    ReplyPlayerWarsQueue(this, player,
        cancelled ? MPlayerWarsQueueResult::Cancelled
                  : MPlayerWarsQueueResult::NotQueued,
        static_cast<int>(m_PlayerWarsQueue.Count()));
}

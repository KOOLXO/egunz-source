// EGunZ Player Wars: experimental JOIN/CANCEL handshake only.
// Matchmaking, stage creation, MMR and persistence are not enabled here.
#include "stdafx.h"
#include "MMatchServer.h"
#include "MSharedCommandTable.h"
#include "MMatchObject.h"
#include "MMatchGlobal.h"
#include "MMatchConfig.h"
#include "MMatchStage.h"
#include "MMatchGameType.h"
#include "MMatchMap.h"
#include <array>

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

    // This experimental path must never touch the Clan War matchmaker or
    // its database/scoring rules. Dedicated ranked server-mode integration
    // will be reviewed after the first local 2v2 smoke test.
    if (MGetServerConfig()->GetServerMode() != MSM_NORMAL_)
    {
        respond(MPlayerWarsQueueResult::Unavailable);
        return;
    }

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

 
// One 2v2 solo-queue launch per server tick. The existing LadderJoin and
// MC_MATCH_LADDER_LAUNCH commands are reused for network/stage compatibility;
// the Clan War manager, clan points and ladder-team DB are NOT invoked.
void MMatchServer::ProcessPlayerWarsMatches()
{
    if (MGetServerConfig()->GetServerMode() != MSM_NORMAL_)
        return;

    const u64 now = GetTickTime();
    if (now - m_LastPlayerWarsMatchTick < 1000)
        return;
    m_LastPlayerWarsMatchTick = now;

    std::array<MPlayerWarsQueue::Ticket, 4> tickets;
    if (!m_PlayerWarsQueue.Next2v2(tickets))
        return;

    std::array<MMatchObject*, 4> players{};
    for (size_t i = 0; i < tickets.size(); ++i)
    {
        const auto& ticket = tickets[i];
        MMatchObject* player = GetObject(ticket.connection_uid);
        if (!IsEnabledObject(player) || !player->GetCharInfo() ||
            !player->GetAccountInfo() ||
            player->GetCharInfo()->m_nCID != ticket.character_id ||
            player->GetAccountInfo()->m_nAID != ticket.account_id ||
            player->GetPlace() != MMP_LOBBY ||
            player->GetStageUID() != MUID(0, 0) ||
            player->IsLaunchedGame() ||
            player->IsLadderChallenging() || player->GetLadderGroupID() != 0)
        {
            m_PlayerWarsQueue.Cancel(ticket.character_id, ticket.connection_uid);
            return;
        }
        players[i] = player;
    }

    const MMATCH_GAMETYPE gameType = MMATCH_GAMETYPE_DEATHMATCH_TEAM;
    if (!MGetGameTypeMgr()->IsCorrectGameType(gameType))
        return;

    // Mansion is our Phase-1 deterministic test map; fall back to the first
    // valid, enabled map if the server has disabled Mansion.
    int mapIndex = -1;
    if (MIsCorrectMap(MMATCH_MAP_MANSION) &&
        MGetServerConfig()->IsEnableMap(MMATCH_MAP_MANSION))
        mapIndex = MMATCH_MAP_MANSION;
    else
    {
        for (int map = 0; map < MMATCH_MAP_MAX; ++map)
            if (MIsCorrectMap(map) &&
                MGetServerConfig()->IsEnableMap(static_cast<MMATCH_MAP>(map)))
            {
                mapIndex = map;
                break;
            }
    }
    if (mapIndex < 0)
        return;

    MUID stageUID(0, 0);
    if (!StageAdd(nullptr, "PLAYER_WARS_2V2", true, "", &stageUID))
        return;
    MMatchStage* stage = FindStage(stageUID);
    if (!stage)
        return;

    stage->SetStageType(MST_LADDER);
    stage->ChangeRule(gameType);
    MMatchStageSetting* setting = stage->GetStageSetting();
    setting->SetMasterUID(MUID(0, 0));
    setting->SetMapIndex(mapIndex);
    setting->SetGameType(gameType);
    setting->SetLimitTime(3);
    setting->SetRoundMax(5); // Short real match for the first four-client test.
    setting->SetAutoTeamBalancing(false);
    setting->GetStageSetting()->nMaxPlayers = 4;
    setting->GetStageSetting()->bForcedEntryEnabled = false;

    bool joined = true;
    for (size_t i = 0; i < players.size(); ++i)
        if (!LadderJoin(players[i]->GetUID(), stageUID, i < 2 ? MMT_RED : MMT_BLUE))
        {
            joined = false;
            break;
        }

    if (joined)
    {
        RouteToStage(stageUID, CreateCmdResponseStageSetting(stageUID));
        joined = stage->StartGame();
    }

    if (!joined)
    {
        for (auto* player : players)
            if (IsEnabledObject(player) && player->GetStageUID() == stageUID)
                StageLeave(player->GetUID(), stageUID);
        StageRemove(stageUID, nullptr);

        // Avoid retry loops when a stage cannot launch.
        for (size_t i = 0; i < tickets.size(); ++i)
        {
            m_PlayerWarsQueue.Cancel(tickets[i].character_id, tickets[i].connection_uid);
            if (IsEnabledObject(players[i]))
                ReplyPlayerWarsQueue(this, players[i],
                    MPlayerWarsQueueResult::LaunchFailed,
                    static_cast<int>(m_PlayerWarsQueue.Count()));
        }
        return;
    }

    // The launch command enters the original GunZ game flow. Do not record
    // ranked results or award ELO here: those require Phase-2 authoritative
    // result handling and SQLite persistence.
    ReserveAgent(stage);
    MCommand* launch = CreateCommand(MC_MATCH_LADDER_LAUNCH, MUID(0, 0));
    launch->AddParameter(new MCmdParamUID(stageUID));
    launch->AddParameter(new MCmdParamStr(const_cast<char*>(stage->GetMapName())));
    RouteToStage(stageUID, launch);

    for (const auto& ticket : tickets)
        m_PlayerWarsQueue.Cancel(ticket.character_id, ticket.connection_uid);
}

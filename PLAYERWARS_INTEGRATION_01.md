# EGunZ Player Wars — experimental network handshake

Branch: `feature/playerwars-queue-v1`, branched from `egunz` (October 14, 2026).

## Scope
First integration milestone only: authenticated client request -> MatchServer validation -> in-memory 2v2 wait ticket -> server response -> cancellation -> pruning disconnected/ineligible tickets.

No new ranked lobby button, no ratings/MMR, no automated matchmaking, no ready check, no matchmaking tick, no stage launch, and no wins/losses. The standalone queue prototype previously tested is **not yet connected to MatchServer**; this small server bridge is the phase-1 waiting-ticket handshake. It will be replaced/extended as the queue engine is integrated.

## Security
Do not trust client-supplied character ID, account ID, MMR, or match result. The only field accepted from the client is playlist (currently 2=2v2). The server resolves identity from the authenticated connection and verifies lobby state and other-ladder status. One ticket per account; duplicate requests are idempotent. Stale tickets are pruned during MatchServer::OnRun.

## Testing
1. Do **not** build with `EGUNZ_SERVER_DIR` pointed to the working server. This project has a CMake post-build deployment command that copies MatchServer.exe.
2. Make a clean local checkout of this experimental branch, configure a separate build directory, and set `EGUNZ_SERVER_DIR=` empty.
3. Confirm both GunZ client and MatchServer compile; packet tables must match.
4. Experimental client chat commands are provided: `/pwfind` (JOIN 2v2) and `/pwcancel` (CANCEL). These are test triggers, not the final Player Wars lobby UI. Verify JOIN, duplicate JOIN, CANCEL, disconnect pruning, attempts outside lobby, and separate accounts.
5. Do not use on a public game server. No matchmaking/ratings are implemented.

## Review before next iteration
- Attach authoritative player ratings and the previously tested queue simulator to the MatchServer bridge.
- Decide what queue-eligibility restrictions and ranked playlists apply.
- Wire a dedicated Player Wars lobby button/status UI.
- Test the full flow with multiple isolated local clients.

No changes should be merged into `egunz` without separate build and runtime tests.

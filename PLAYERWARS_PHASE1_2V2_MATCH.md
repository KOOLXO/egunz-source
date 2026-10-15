# EGunZ Player Wars (Ladder / Ranked) - Phase 1 2v2 test

Branch: `feature/playerwars-phase1-2v2-match`, based on `feature/playerwars-queue-v1`.

## Current implementation
- Four **solo** players use `/pwfind` to enter an authenticated 2v2 waiting queue (one ticket per account).
- A MatchServer tick prunes ineligible/disconnected tickets, selects the first four by queue order, and assigns the first two to RED and next two to BLUE.
- MatchServer creates a private `PLAYER_WARS_2V2` stage, sets Team Deathmatch / Mansion where enabled (otherwise first allowed valid map), 4 max players, 5 rounds, 3-minute limit.
- Reuses the original OpenGunZ `LadderJoin`, `MC_MATCH_LADDER_PREPARE`, `MC_MATCH_LADDER_LAUNCH` and client game transition. **This does not use the original clan team strategy or Clan War queue.**
- Leaves the original EGunZ `egunz` branch and working-server binaries unchanged.
- **Phase-1 test is restricted to `[SERVER] MODE=normal`.** Do not enable it on `clan`, `ladder`, or `test` servers until mode integration and result handling are reviewed.
- No ELO, rank, party matching, ready check, match result persistence, XML Player Wars UI, or competitive rewards. Normal GunZ Team Deathmatch rules govern the test match.

## Unit tests (queue only)
Run from repo root on a Linux/macOS machine with C++17:
```bash
c++ -std=c++17 -Itests/playerwars_phase1_2v2/stubs \
  tests/playerwars_phase1_2v2/queue_smoke.cpp -o /tmp/pw_queue_smoke
/tmp/pw_queue_smoke
```
The test-only stub MUID does not replace the real in-game MUID. Tests validate queue ordering, duplicate-account protection, cancellation authorization, pruning, and four-ticket selection. They **do not** compile the Windows MatchServer or prove network play.

## Isolated Windows build - prevent auto-deploy!
The existing CMake preset normally copies built Gunz.exe and MatchServer.exe into the working server/client directories. **Disable both copy destinations**, not just MatchServer. Do not configure or build the branch with either destination pointing to a live directory.

Example PowerShell (adjust existing repo location as required):
```powershell
git -C C:\egunz-dev\ogz fetch origin
git -C C:\egunz-dev\ogz worktree add C:\egunz-dev\ogz-playerwars-phase1 origin/feature/playerwars-phase1-2v2-match
cd C:\egunz-dev\ogz-playerwars-phase1\src
cmake --preset x86-release -DEGUNZ_SERVER_DIR= -DEGUNZ_CLIENT_DIR=
cmake --build --preset x86-release --target MatchServer Gunz
```
Use separate copied test client/server game-data directories for runtime tests. Do not overwrite `ogz-client` / `ogz-server`, and never merge this branch without both a build and runtime test.

## Full Phase-1 runtime acceptance checklist (not yet verified)
1. Both MatchServer.exe and Gunz.exe build from the same branch, with the protocol commands matching.
2. Isolated `normal` MatchServer starts and four different test accounts connect.
3. From lobby, first three clients type `/pwfind` and stay queued. A duplicate join is acknowledged but does not add another ticket.
4. Fourth account enters `/pwfind`; exactly two RED and two BLUE players receive ladder prepare/launch commands and enter a 2v2 Team Deathmatch on a valid map.
5. Finish the five-round match; all four clients reach the normal GunZ result flow and can return to the lobby.
6. Test `/pwcancel`, disconnect-before-match, and a second match without leftover queue tickets.
7. Confirm Clan War matchmaking, clan points and saved clan match results are unmodified by the Player Wars test.
8. Record client/MatchServer logs if stage creation, launch, match completion, or lobby return fails.

**Current evidence:** the isolated queue C++ smoke test passed using a test-only MUID stub. A Windows game/server build and real four-client test have not been run from this environment.

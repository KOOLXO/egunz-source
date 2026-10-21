# EGunZ Classic UI scaling + General Lobby (Steps 2 and 3)

This is an **isolated candidate** branch. Keep the production client intact until a Windows build and in-game tests pass.

## Changes made

- Initialize Mint with the actual configured screen dimensions **before** loading XML widgets.
- Fit classic 800x600 XML bounds into the selected resolution with one uniform scale factor (minimum of the width and height ratios), centered horizontally on widescreen. Widget hitboxes and sprites scale with those same bounds.
- Create XML and Mint default fonts at correspondingly scaled pixel heights.
- Keep the custom player-list sizing ratio consistent with that scale; compress its channel columns when a narrower panel requires it.
- Post an asynchronous UI reload after `/resolution <width> <height>` to avoid freeing a chat widget inside its own command callback. Avoid reloading resources during active gameplay.

## General Lobby XML

The **companion client asset** is not part of the source repository; it is distributed as `Client/Interface/default/Lobby.xml` in the separate EGunZ Classic Steps 2+3 candidate ZIP. Its source is the client's Dropbox `/GunZ Clients/ogz-client/Interface/default/Lobby.xml`.

The lobby patch centers channel chat at x=400 of the 800-wide design canvas, moves the player list right without overlapping chat, and adds static gold section headers. Existing widget identifiers (including `ChannelChattingInput`, `LobbyChannelPlayerList`, `LobbyShopCaller`, `LobbyEquipmentCaller`, `ChannelListFrameCaller`, room pagination and logout) are preserved. The original Lobby.xml and an XML diff are included in the ZIP.

The client-side asset must be copied into the **test build's** `Interface/default/Lobby.xml` after creating a backup, not straight into all tester laptops.

## Offline checks performed

The candidate XML parses; original widget-ID multiset is preserved (plus two noninteractive headings). Channel chat output/input align and do not overlap the player list. Layout rectangle projections fit at 800x600, 1024x768, 1280x720, 1920x1080, 2560x1440, 3440x1440, and 3840x2160. Existing menu/room navigation coordinates are unchanged.

The source branch was checked statically for existing General Lobby listener bindings. **No Windows compilation or actual rendering/input test has been performed.**

## Required Windows acceptance tests before merge

1. Compile the Windows client from this branch; verify no compiler/linker or font-resource errors.
2. Compare 1080p, 1440p and 4K screenshots: texts legible, bitmaps aligned, no cropped panels.
3. Click every General Lobby action: channel select, room join/create/quick join, pagination, player list tabs, shop, inventory, option menu, logout.
4. Send and receive channel chat, whisper, paste text, check focus and cursor and unexpected keyboard shortcuts.
5. Enter/leave Stage and Clan War lobbies; verify legacy nested layouts, scrollbars, item slots, tooltips, and focus don't regress.
6. Exercise `/resolution` on lobby and restart after changing from active combat; validate button hitboxes and live reflow.
7. Confirm no rendering or input regression at ultrawide aspect ratios. The current design intentionally retains a centered 4:3 UI rather than stretching it to fill the widescreen.

Rollback: switch to `egunz` and restore the original `Lobby.xml` from backup.

**Known limitation:** UI reload is intentionally suppressed while `GUNZ_GAME` is active. A later safe exit/reload path and full resolution-menu coverage need runtime validation. This work is not production-complete until all acceptance checks pass.

# Lobby, modal and HUD scaling

Based on `feature/crosshair-settings` at `84b7e93`. This retains the camera
slider click handling, crosshair pixel sizing and color picker.

The lobby previously resized its 800x600 widget rectangles without resizing its
fonts. `ZMsgBox` used a fixed 400-pixel width, and the Channel List was not among
the main resize roots. Health meshes, filled gauges and numbers used the center
4:3 viewport, which inset them on a widescreen display.

## Changes

- Lobby and stage text use 16 pixels at 1080p, 21 at 1440p and 32 at 4K.
  Text-area line heights and list rows follow the same viewport scale. Windows
  GDI DPI is compensated when rasterizing the new fonts.
- Channel List gets a centered 640x480 logical layout with readable tabs,
  full-width list rows, private-channel input, and 140x40 action buttons.
  Positions and hit targets are assigned together on resize and before opening.
- Confirmation dialogs use the menu's dark panel, cyan accent and shared font.
  Text wrapping determines their height. Dialog buttons use 140x40 logical
  bounds; Enter/Escape, custom listeners and confirmation behavior are retained.
- Escape menu uses the same scale without the previous 1.5x cap at 4K.
- Legacy XML children are restored after their parent's anchor pass, preventing
  double scaling. Generated scrollbar controls keep their owner's layout.
- Health emblem/plate, animated gauge fills and numbers share a left-edge
  offset. A 16-pixel margin at 1080p becomes 32 at 4K. Only this HUD group is
  translated; centered announcements and other screen effects retain their anchors.
  Stretch mode has no additional offset. HUD labels use the readable font.

## Automated verification

Run from the repository root:

```sh
g++ -std=c++14 -Wall -Wextra -pedantic tests/UiLayoutTests.cpp -o /tmp/ui-tests
/tmp/ui-tests
g++ -std=c++14 tests/CameraSettingsTests.cpp -o /tmp/camera-tests
/tmp/camera-tests
g++ -std=c++14 tests/CrossHairSettingsTests.cpp -o /tmp/crosshair-tests
/tmp/crosshair-tests
git diff --check
```

The layout checks cover 720p, 1080p, 1440p, 4K, 1024x768 and ultrawide;
repeated resolution changes; 96/120/144/192 DPI; popup fit; final menu button fit;
and matching health-plate/gauge translations at different projection depths.
They do not render the Windows game client.

## Required Windows verification

Build with the existing VS2022 client build procedure, then test at 1920x1080,
2560x1440 and 3840x2160 (32bpp). A Windows/Direct3D client build and visual
acceptance test have not been performed in the Linux authoring environment.

1. Enter the lobby. Check chat, input caret, player stats, player names, level,
   clan columns and channel breadcrumb. Open the stage and check its text too.
2. Open Channels. Check General/Private/Clan tabs, long channel names, scrolling,
   private-channel input, join, cancel, keyboard shortcuts and actual click bounds.
3. In a match, open Return to lobby. Check the styled title/message, wrapping,
   Enter/Escape and each button's full clickable area. Cancel must stay in match;
   confirming must return to lobby.
4. Check the Z plate, HP/AP/EXP fills and numbers move together. Take damage,
   respawn and observe the plate animation. Check 4:3 and stretch mode as well.
5. Switch 1080p -> 4K -> 1440p -> 1080p, including canceling a resolution change.
   Open each popup again. Nothing should grow or shift after repeated changes.
6. Recheck camera minus/plus, crosshair preview/gameplay size at 0.30 and 1.00,
   and the color picker.

The legacy lobby graphic skin is retained. Assets are loaded from the installed
client and are not present in this source repository; final visual fit against
that skin must be checked on the user's Windows client.

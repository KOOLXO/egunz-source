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

Build with the existing Visual Studio `x86-release` CMake preset, then test at 1920x1080,
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


## Remaining-screen follow-up

- Game Creation uses a centered 640x300 layout. The room name and password
  have separate 40-pixel-high inputs; OK and Cancel use 140x44 targets.
- Replays uses a centered 900x620 layout with a full-width, 36-pixel-row file
  list, scaled scrollbar arrows/thumb, separate warning lines and View/Close
  actions. The filename column no longer retains a legacy half-width tab stop.
- Game Options uses a centered 1000x600 layout: dropdown labels/values occupy
  the left column, full-row clickable checkboxes occupy the right, and HP/AP
  fields have separate labels. Dropdown height is capped to available space
  inside the frame. Existing values, restrictions, listeners, shortcuts and
  disabled/hidden states remain controlled by the game.
- These three panels share the dark/cyan menu styling and DPI-neutral fonts.
  Layout is reapplied after the legacy resize pass and before showing a panel.
  Duplicate XML IDs such as ParentClose/Label are resolved within their frame.
- The classic scoreboard mesh, names, icons, highlights and quest HP/AP bars
  move together to center the frame horizontally and vertically. Text scales
  with the viewport; names/clans stay within their columns, and spectators
  have a separate footer below all 16 player rows. Clan emblems use consistent
  pixel/relative units and the caller's draw origin, clip and font are restored.
- Character Selection uses 64-pixel-tall rows with 12-pixel gaps, 20-pixel name
  text, and separate level/clan areas. The full row remains clickable. Occupied
  slots pack together even when account slot numbers have gaps; account-data
  updates reapply the layout instead of restoring the old row positions.
  Selection keeps its existing label colors and adds a brighter selected bar.
  Create/Delete/Select/Logout retain the skin artwork with scaled hit bounds.
- List row-height changes now recalculate the visible row count and scroll
  range immediately, including when the outer bounds have not changed.

All pixel sizes above are at 1920x1080; they scale by 4/3 at 1440p and 2 at 4K.
Automated geometry checks also cover 800x600, 1280x1024 and 5120x1440.
They validate fit, spacing, centering and matching mesh/text translation; they
cannot validate rendered Windows fonts or the installed skin.

### Build and run on the confirmed Windows checkout

1. Close Gunz. In the existing PowerShell session (where Git is on PATH):

   ```powershell
   Set-Location C:\egunz-dev\ogz\ogz-source
   git switch fix/ui-scaling-4k
   git pull --ff-only origin fix/ui-scaling-4k
   ```

2. In Visual Studio, open `C:\egunz-dev\ogz\ogz-source\src`, select
   `x86-release`, and build the client with the existing CMake setup.
3. Copy the **newly built** executable into the complete client folder. Building
   alone did not refresh that folder in the previous Windows check. Back up its
   current executable before replacing it:

   ```powershell
   $built = 'C:\egunz-dev\ogz\ogz-source\build\x86-release\bin\Gunz.exe'
   $client = 'C:\egunz-dev\ogz\ogz-client'
   Get-Item $built | Select-Object FullName, LastWriteTime, Length
   Copy-Item "$client\Gunz.exe" "$client\Gunz.exe.backup-$(Get-Date -Format yyyyMMdd-HHmmss)"
   Copy-Item $built "$client\Gunz.exe" -Force
   Get-FileHash $built, "$client\Gunz.exe" | Select-Object Path, Hash
   Start-Process "$client\Gunz.exe" -WorkingDirectory $client
   ```

   Run from the complete client folder, which contains `fmod.dll` and game
   assets. Launching the executable directly from the build bin lacks them.

4. Test Game Creation name/password typing and OK/Cancel; Replays selection,
   scrolling, warnings, View/Close; Game Options dropdowns, checkbox label
   clicks, HP/AP and Apply/Cancel. Disabled settings must remain disabled.
5. Check the scoreboard with long names/clans, 16 rows, spectators, teams and
   quest mode. Check Character Selection with multiple characters and vacant
   slot numbers; select each row and check the four bottom actions. Do not
   confirm character deletion unless intending to delete a character.
6. Compare 1080p, 1440p and 4K, reopen each panel after switching, then return
   to the original resolution and compare against a fresh launch.

Shop/inventory redesign and the separately reported intermittent glyph/UI
corruption after changing resolution remain deferred. This follow-up does not
claim to resolve that renderer/resource-reset issue.

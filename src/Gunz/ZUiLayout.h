#pragma once
#include <algorithm>

// Pixel sizes are authored at 1080p. Always derive from the current viewport,
// never multiply a previously scaled rectangle (resolution rollback uses this too).
namespace ZUiLayout
{
inline float Scale(int width, int height)
{
    return (std::max)(.1f, (std::min)(width / 1920.f, height / 1080.f));
}
inline int Pixels(float logical, float scale) { return int(logical * scale + .5f); }
struct PixelFont
{
    int points, nativeHeight;
    float scale;
    PixelFont(int pixels, int dpi)
    {
        dpi = (std::max)(1, dpi);
        points = (std::max)(1, (pixels * 72 + dpi / 2) / dpi);
        nativeHeight = (std::max)(1, points * dpi / 72);
        scale = float(pixels) / nativeHeight;
    }
};
inline float ClipShift(float pixels, int width, float projectionW)
{
    return 2.f * pixels / width * projectionW;
}
struct Dialog
{
    int x, y, width, height;
    Dialog(int w, int h, int lines)
    {
        const float scale = Scale(w, h);
        width = Pixels(560, scale);
        height = (std::min)(h - Pixels(32, scale),
            Pixels(152, scale) + (std::max)(2, lines) * Pixels(24, scale));
        x = (w - width) / 2; y = (h - height) / 2;
    }
};
inline int HudLeft(int width, int height, int correctedWidth)
{
    // A 4:3 display already has the original small margin built into the mesh.
    return (std::min)((width - correctedWidth) / 2,
        Pixels(16, Scale(width, height)));
}
inline int HudShift(int width, int height, int correctedWidth)
{
    return HudLeft(width, height, correctedWidth) - (width - correctedWidth) / 2;
}
// These rectangles are shared by runtime layout and the geometry checks.
struct Rect { int x, y, w, h; };
struct Screen
{
    float scale;
    int x, y, width, height;
    Screen(int viewportWidth, int viewportHeight, int logicalWidth, int logicalHeight)
        : scale(Scale(viewportWidth, viewportHeight)), x(0), y(0),
          width(Pixels(logicalWidth, scale)), height(Pixels(logicalHeight, scale))
    {
        x = (viewportWidth - width) / 2; y = (viewportHeight - height) / 2;
    }
    Rect Child(int left, int top, int w, int h) const
    { return {Pixels(left, scale), Pixels(top, scale), Pixels(w, scale), Pixels(h, scale)}; }
};
struct CharacterCreation : Screen
{
    CharacterCreation(int w, int h) : Screen(w, h, 560, 680)
    {
        // The scene renders the character toward the left. Keep the complete
        // form beside it and derive every rectangle from the current viewport.
        x = w - width - Pixels(40, scale);
    }
    int FieldY(int row) const { return row == 0 ? 80 : 140 + (row - 1) * 52; }
    Rect Caption(int row) const { return Child(24, FieldY(row), 136, row == 0 ? 44 : 40); }
    Rect Field(int row) const { return Child(176, FieldY(row), 360, row == 0 ? 44 : 40); }
    Rect EquipmentCaption(int row) const { return Child(24, 400 + row * 28, 136, 26); }
    Rect Equipment(int row) const { return Child(176, 400 + row * 28, 360, 26); }
    Rect Action(int index) const { return Child(236 + index * 156, 612, 144, 44); }
    Rect EquipmentHeading() const { return Child(24, 364, 512, 28); }
};
inline int ScoreboardShift(int correctedWidth)
{
    // The classic mesh spans approximately .245 .. .985 of the 4:3 canvas.
    return -Pixels(.115f * correctedWidth, 1.f);
}
inline int ScoreboardShiftY(int height) { return -Pixels(.085f * height, 1.f); }
inline Rect CharacterRow(int width, int height, int row)
{
    const float scale = Scale(width, height);
    return {width - Pixels(560, scale), height - Pixels(480 - row * 76, scale),
        Pixels(520, scale), Pixels(64, scale)};
}
struct Channel
{
    float scale;
    int x, y, width, height;
    Channel(int w, int h) : scale(Scale(w, h)), width(Pixels(640, scale)),
        height(Pixels(480, scale))
    {
        x = (w - width) / 2; y = (h - height) / 2;
    }
};
}

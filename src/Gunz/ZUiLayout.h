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

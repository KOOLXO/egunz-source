#include "../src/Gunz/ZUiLayout.h"
#include "../src/Gunz/ZEscapeMenuLayout.h"
#include <cassert>
#include <cmath>
#include <iostream>
#include <utility>

int main()
{
    // Include repeated changes and rollback: no layout may depend on its history.
    const int resolutions[][2] = {{1920,1080}, {3840,2160}, {2560,1440},
        {1920,1080}, {1280,720}, {800,600}, {1024,768}, {1280,1024},
        {3440,1440}, {5120,1440}, {3840,2160}};
    for (const auto& resolution : resolutions)
    {
        const int w = resolution[0], h = resolution[1];
        const float scale = ZUiLayout::Scale(w,h);
        const ZUiLayout::Channel channel(w,h);
        const ZUiLayout::Dialog dialog(w,h,2), longDialog(w,h,100);
        const ZEscapeMenuLayout menu(w,h);
        for (const auto& bounds : {channel, ZUiLayout::Channel(w,h)})
        {
            assert(bounds.x >= 0 && bounds.y >= 0);
            assert(bounds.x + bounds.width <= w && bounds.y + bounds.height <= h);
            assert(std::abs(2*bounds.x + bounds.width - w) <= 1);
        }
        // The remaining popups must fit and stay centered on narrow, wide and
        // repeated viewport changes. Bounds are recomputed from logical units.
        for (const auto& dimensions : {std::pair<int,int>{640,300}, {900,620}, {1000,600}})
        {
            const ZUiLayout::Screen screen(w, h, dimensions.first, dimensions.second);
            assert(screen.x >= 0 && screen.y >= 0);
            assert(screen.x + screen.width <= w && screen.y + screen.height <= h);
            assert(std::abs(2*screen.x + screen.width - w) <= 1);
            assert(std::abs(2*screen.y + screen.height - h) <= 1);
            const auto content = screen.Child(24, 64, dimensions.first - 48, dimensions.second - 88);
            assert(content.x >= 0 && content.y >= 0);
            assert(content.x + content.w <= screen.width);
            assert(content.y + content.h <= screen.height);
        }
        // Four full-height character targets must stay separated and above the
        // bottom actions even at 800x600 and when revisiting a prior resolution.
        int previousBottom = 0;
        for (int i = 0; i < 4; ++i)
        {
            const auto row = ZUiLayout::CharacterRow(w, h, i);
            assert(row.x >= 0 && row.x + row.w <= w);
            assert(row.y >= previousBottom);
            assert(row.h >= ZUiLayout::Pixels(20,scale) + ZUiLayout::Pixels(24,scale));
            assert(row.y + row.h < h - ZUiLayout::Pixels(76,scale));
            previousBottom = row.y + row.h;
        }
        assert(dialog.x >= 0 && dialog.y >= 0);
        assert(longDialog.y >= 0 && longDialog.y + longDialog.height <= h);
        assert(menu.Width <= w && menu.Height <= h);
        // The last action's clickable rectangle must remain inside the frame.
        assert(menu.RowY(5) + menu.Row + menu.Padding == menu.Height);
        assert(ZUiLayout::Pixels(460,scale) < channel.height);
        // Physical font height remains the same at different Windows display DPIs.
        for (int dpi : {96,120,144,192})
        {
            int pixels = ZUiLayout::Pixels(16,scale);
            ZUiLayout::PixelFont font(pixels,dpi);
            assert(std::abs(font.nativeHeight * font.scale - pixels) < .001f);
        }
        int corrected = int(h * (4.f/3.f));
        const int boardX = ZUiLayout::ScoreboardShift(corrected);
        const int boardY = ZUiLayout::ScoreboardShiftY(h);
        // Both mesh endpoints become equidistant from the viewport center.
        // Check translation at two depths; the frame must not be rescaled.
        const float boardLeft = (w-corrected)/2 + .245f*corrected + boardX;
        const float boardRight = (w-corrected)/2 + .985f*corrected + boardX;
        assert(std::abs((boardLeft + boardRight)/2 - w/2.f) <= 1.f);
        assert(std::abs((.185f*h + .985f*h)/2 + boardY - h/2.f) <= 1.f);
        for (float projectionW : {1.f,-1.f})
            for (float depth : {10.f,1000.f})
            {
                const float dx = depth * ZUiLayout::ClipShift(float(boardX),w,projectionW) / (depth*projectionW);
                const float dy = -depth * ZUiLayout::ClipShift(float(boardY),h,projectionW) / (depth*projectionW);
                assert(std::abs(dx*w/2 - boardX) < .001f);
                assert(std::abs(-dy*h/2 - boardY) < .001f);
            }
        int shift = ZUiLayout::HudShift(w,h,corrected);
        int left = ZUiLayout::HudLeft(w,h,corrected);
        // Plate, filled gauges and text share the same translation.
        assert((w-corrected)/2 + shift == left);
        assert(shift <= 0);
        for (float projectionW : {1.f, -1.f})
        {
            // Project a point at two depths, for left- and right-handed matrices.
            for (float z : {10.f,1000.f})
            {
                float ndcShift = z * ZUiLayout::ClipShift(float(shift),w,projectionW) / (z*projectionW);
                assert(std::abs(ndcShift*w/2 - shift) < .001f);
            }
        }
        // Stretch mode has no 4:3 inset, so no additional shift is applied.
        assert(ZUiLayout::HudShift(w,h,w) == 0);
    }
    assert(ZUiLayout::Pixels(16,ZUiLayout::Scale(1920,1080)) == 16);
    assert(ZUiLayout::Pixels(16,ZUiLayout::Scale(2560,1440)) == 21);
    assert(ZUiLayout::Pixels(16,ZUiLayout::Scale(3840,2160)) == 32);
    assert(ZUiLayout::HudLeft(3840,2160,2880) == 32);
    std::cout << "Popup, character row, scoreboard, DPI and HUD layout checks passed\n";
}

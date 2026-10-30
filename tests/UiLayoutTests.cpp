#include "../src/Gunz/ZUiLayout.h"
#include "../src/Gunz/ZEscapeMenuLayout.h"
#include <cassert>
#include <cmath>
#include <iostream>

int main()
{
    // Include repeated changes and rollback: no layout may depend on its history.
    const int resolutions[][2] = {{1920,1080}, {3840,2160}, {2560,1440},
        {1920,1080}, {1280,720}, {1024,768}, {3440,1440}, {3840,2160}};
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
    std::cout << "UI layout, DPI and HUD projection checks passed\n";
}

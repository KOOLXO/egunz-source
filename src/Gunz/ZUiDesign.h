#pragma once
#include "ZUiLayout.h"
#include "Mint4R2.h"
#include "MFrame.h"
#include "MButton.h"
#include "Mint.h"

// Shared palette and pixel-sized typography for lobby and modal UI.
namespace ZUiDesign
{
inline float Scale() { return ZUiLayout::Scale(MGetWorkspaceWidth(), MGetWorkspaceHeight()); }
inline int Pixels(float value) { return ZUiLayout::Pixels(value, Scale()); }
inline int Dpi()
{
    HDC dc = GetDC(g_hWnd);
    const int dpi = dc ? (std::max)(1, GetDeviceCaps(dc, LOGPIXELSY)) : 96;
    if (dc) ReleaseDC(g_hWnd, dc);
    return dpi;
}
inline bool CreateFont(MFontR2& font, const char* alias, int pixels, int dpi, bool bold)
{
    const ZUiLayout::PixelFont size(pixels, dpi);
    return font.Create(alias, "Arial", size.points, size.scale, bold);
}
inline MFontR2* Font(bool title = false)
{
    static MFontR2 text, heading;
    static int oldPixels = 0, oldDpi = 0;
    const int pixels = (std::max)(8, Pixels(16)), dpi = Dpi();
    if (pixels != oldPixels || dpi != oldDpi)
    {
        text.Destroy(); heading.Destroy();
        if (CreateFont(text, "EGunZUiText", pixels, dpi, false) &&
            CreateFont(heading, "EGunZUiTitle", (std::max)(10, Pixels(20)), dpi, true))
        {
            oldPixels = pixels; oldDpi = dpi;
        }
    }
    return title ? &heading : &text;
}
class PanelLook : public MFrameLook
{
public:
    void OnDraw(MFrame* frame, MDrawContext* dc) override
    {
        auto r = frame->GetInitialClientRect();
        dc->SetColor(MCOLOR(0xFF1A2029)); dc->FillRectangle(r);
        dc->SetColor(MCOLOR(0xFF3B4655)); dc->Rectangle(r);
        dc->SetColor(MCOLOR(0xFF45C2D1)); dc->FillRectangle(0, 0, r.w, Pixels(3));
        dc->SetFont(Font(true)); dc->SetColor(MCOLOR(0xFFF2F5F8));
        dc->Text(MRECT(Pixels(24), Pixels(12), r.w - Pixels(48), Pixels(36)),
            frame->MWidget::GetText(), MAM_LEFT | MAM_VCENTER);
    }
    MRECT GetClientRect(MFrame*, const MRECT& r) override { return r; }
};
class ButtonLook : public MButtonLook
{
public:
    void OnDraw(MButton* button, MDrawContext* dc) override
    {
        const auto r = button->GetInitialClientRect();
        const bool over = button->IsEnable() && button->IsMouseOver();
        const bool active = over || button->GetCheck();
        dc->SetColor(MCOLOR(button->IsButtonDown() ? 0xFF173D47 : active ? 0xFF24505C : 0xFF262F3C));
        dc->FillRectangle(r);
        dc->SetColor(MCOLOR(active ? 0xFF45C2D1 : 0xFF3B4655)); dc->Rectangle(r);
        dc->SetFont(Font());
        dc->SetColor(MCOLOR(button->IsEnable() ? 0xFFF2F5F8 : 0xFF78818D));
        dc->Text(r, button->GetText(), MAM_HCENTER | MAM_VCENTER);
    }
    MRECT GetClientRect(MButton*, const MRECT& r) override { return r; }
};
inline PanelLook* Panel() { static PanelLook look; return &look; }
inline ButtonLook* Button() { static ButtonLook look; return &look; }
}

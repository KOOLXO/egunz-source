#pragma once
#include "ZUiLayout.h"
#include "Mint4R2.h"
#include "MFrame.h"
#include "MButton.h"
#include "Mint.h"
#include "MEdit.h"
#include "MComboBox.h"

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
// Keep Mint's editing, selection and scrolling behavior; replace only drawing.
class EditLook : public MEditLook
{
public:
    void OnFrameDraw(MEdit* edit, MDrawContext* dc) override
    {
        const auto r = edit->GetInitialClientRect();
        dc->SetColor(MCOLOR(0xFF111820)); dc->FillRectangle(r);
        dc->SetColor(MCOLOR(edit->IsFocus() ? 0xFF45C2D1 : 0xFF78818D)); dc->Rectangle(r);
        dc->SetFont(Font());
    }
    MRECT GetClientRect(MEdit*, const MRECT& r) override
    { return MRECT(r.x + Pixels(10), r.y + Pixels(6), r.w - Pixels(20), r.h - Pixels(12)); }
};
class ComboLook : public MButtonLook
{
public:
    void OnDraw(MButton* button, MDrawContext* dc) override
    {
        const auto r = button->GetInitialClientRect();
        dc->SetColor(MCOLOR(0xFF111820)); dc->FillRectangle(r);
        dc->SetColor(MCOLOR(button->IsEnable() ? 0xFF78818D : 0xFF3B4655)); dc->Rectangle(r);
        dc->SetFont(Font());
        dc->SetColor(MCOLOR(button->IsEnable() ? 0xFFF2F5F8 : 0xFF78818D));
        dc->Text(MRECT(Pixels(10), 0, r.w - Pixels(46), r.h), button->GetText(), MAM_LEFT | MAM_VCENTER);
        dc->Text(MRECT(r.w - Pixels(32), 0, Pixels(24), r.h), "v", MAM_HCENTER | MAM_VCENTER);
    }
    MRECT GetClientRect(MButton*, const MRECT& r) override { return r; }
};
class CheckLook : public MButtonLook
{
public:
    void OnDraw(MButton* button, MDrawContext* dc) override
    {
        const auto r = button->GetInitialClientRect();
        const int side = Pixels(22), top = (r.h - side) / 2;
        dc->SetColor(MCOLOR(0xFF111820)); dc->FillRectangle(0, top, side, side);
        dc->SetColor(MCOLOR(button->IsEnable() ? 0xFF78818D : 0xFF3B4655)); dc->Rectangle(0, top, side, side);
        if (button->GetCheck())
        {
            dc->SetColor(MCOLOR(button->IsEnable() ? 0xFF45C2D1 : 0xFF78818D));
            dc->FillRectangle(Pixels(5), top + Pixels(5), side - Pixels(10), side - Pixels(10));
        }
        dc->SetFont(Font());
        dc->SetColor(MCOLOR(button->IsEnable() ? 0xFFF2F5F8 : 0xFF78818D));
        dc->Text(MRECT(Pixels(34), 0, r.w - Pixels(34), r.h), button->GetText(), MAM_LEFT | MAM_VCENTER);
    }
    MRECT GetClientRect(MButton*, const MRECT& r) override { return r; }
};
class ListLook : public MListBoxLook
{
protected:
    void OnFrameDraw(MListBox* list, MDrawContext* dc) override
    {
        const auto r = list->GetInitialClientRect();
        dc->SetColor(MCOLOR(0xFF111820)); dc->FillRectangle(r);
        dc->SetColor(MCOLOR(0xFF3B4655)); dc->Rectangle(r);
    }
public:
    ListLook()
    {
        m_SelectedPlaneColor = MCOLOR(0xFF24505C);
        m_UnfocusedSelectedPlaneColor = MCOLOR(0xFF24505C);
        m_SelectedTextColor = MCOLOR(0xFFF2F5F8);
    }
    MRECT GetClientRect(MListBox*, const MRECT& r) override
    { return MRECT(r.x + Pixels(8), r.y + Pixels(4), r.w - Pixels(16), r.h - Pixels(8)); }
};
class ScrollArrowLook : public MArrowLook
{
public:
    void OnDraw(MArrow* arrow, MDrawContext* dc) override
    {
        const auto r = arrow->GetInitialClientRect();
        dc->SetColor(MCOLOR(0xFF262F3C)); dc->FillRectangle(r);
        dc->SetFont(Font()); dc->SetColor(MCOLOR(0xFFF2F5F8));
        dc->Text(r, arrow->m_nDirection == 0 ? "^" : "v", MAM_HCENTER | MAM_VCENTER);
    }
    MSIZE GetDefaultSize(MArrow*) override { return MSIZE(Pixels(28), Pixels(28)); }
    MRECT GetClientRect(MArrow*, MRECT& r) override { return r; }
};
class ScrollThumbLook : public MThumbLook
{
public:
    void OnDraw(MThumb* thumb, MDrawContext* dc) override
    {
        dc->SetColor(MCOLOR(0xFF45C2D1)); dc->FillRectangle(thumb->GetInitialClientRect());
    }
    MRECT GetClientRect(MThumb*, const MRECT& r) override { return r; }
};
inline EditLook* Edit() { static EditLook look; return &look; }
inline ComboLook* Combo() { static ComboLook look; return &look; }
inline CheckLook* Check() { static CheckLook look; return &look; }
inline ListLook* List() { static ListLook look; return &look; }
inline void StyleList(MListBox* list)
{
    static ScrollArrowLook arrow;
    static ScrollThumbLook thumb;
    // ComboListBox owns a separate non-virtual look slot from MListBox.
    if (!strcmp(list->GetClassName(), MINT_COMBOLISTBOX))
        static_cast<MComboListBox*>(list)->ChangeCustomLook(List());
    else list->ChangeCustomLook(List());
    list->SetFont(Font());
    list->m_FontAlign = MAM_LEFT | MAM_VCENTER;
    list->GetScrollBar()->ChangeCustomArrowLook(&arrow);
    list->GetScrollBar()->ChangeCustomThumbLook(&thumb);
    list->SetItemHeight(Pixels(36));
    // Refresh scrollbar bounds after installing its scaled arrow size.
    const auto r = list->GetInitialClientRect();
    const int breadth = list->GetScrollBar()->GetDefaultBreadth();
    list->GetScrollBar()->SetBounds(r.x + r.w - breadth, r.y + 1, breadth, r.h - 1);
}
inline PanelLook* Panel() { static PanelLook look; return &look; }
inline ButtonLook* Button() { static ButtonLook look; return &look; }
}

#pragma once
#include "Mint4R2.h"
#include "MFrame.h"
#include "MButton.h"
#include "MLabel.h"
#include "MComboBox.h"
#include "MSlider.h"
#include "MTabCtrl.h"
#include "Mint.h"
#include "MEdit.h"
#include "CrossHairSettings.h"

// Option.xml is a 960 x 680 logical layout. Restore it on each resize;
// never derive a new layout from already scaled widget coordinates.
namespace OptionsReadability
{
static float Scale = 1.f;
static MFontR2 TextFont, TitleFont;
static int FontSize = 0;
static int FontDPI = 0;
static int Pixels(float value) { return int(value * Scale + .5f); }
static bool CreatePixelFont(MFontR2& font, const char* alias, int pixels, int dpi, bool bold)
{
    // RFont interprets height as points using the game window's GDI DPI.
    // Compensate locally so Windows display scaling cannot enlarge text past its row.
    const int points = max(1, (pixels * 72 + dpi / 2) / dpi);
    const int nativeHeight = max(1, points * dpi / 72);
    return font.Create(alias, "Arial", points, float(pixels) / nativeHeight, bold);
}

class ContentLook : public MFrameLook
{
public:
    void OnDraw(MFrame*, MDrawContext*) override {}
    MRECT GetClientRect(MFrame*, const MRECT& r) override { return r; }
};
class PanelLook : public MFrameLook
{
public:
    void OnDraw(MFrame* frame, MDrawContext* dc) override
    {
        const auto r = frame->GetInitialClientRect();
        dc->SetColor(MCOLOR(0xFF1A2029)); dc->FillRectangle(r);
        dc->SetColor(MCOLOR(0xFF45C2D1)); dc->FillRectangle(0, 0, r.w, Pixels(3));
        dc->SetColor(MCOLOR(0xFF3B4655)); dc->Rectangle(r);
        dc->HLine(0, Pixels(60), r.w);
        const bool main = !strcmp(frame->m_szIDLName, "OptionGroup");
        dc->HLine(0, r.h - Pixels(main ? 76 : 72), r.w);
        dc->SetFont(&TitleFont); dc->SetColor(MCOLOR(0xFFF2F5F8));
        auto title = MRECT(Pixels(24), Pixels(12), r.w - Pixels(48), Pixels(36));
        dc->Text(title, frame->GetText(), MAM_LEFT | MAM_VCENTER);
    }
    // The XML positions include the header/footer. Do not introduce another inset.
    MRECT GetClientRect(MFrame*, const MRECT& r) override { return r; }
};
class ButtonLook : public MButtonLook
{
public:
    void OnDraw(MButton* button, MDrawContext* dc) override
    {
        auto r = button->GetInitialClientRect();
        const bool selected = button->GetCheck() || button->IsMouseOver();
        dc->SetColor(MCOLOR(button->IsButtonDown() ? 0xFF173D47 : selected ? 0xFF24505C : 0xFF262F3C));
        dc->FillRectangle(r);
        dc->SetColor(MCOLOR(selected ? 0xFF45C2D1 : 0xFF3B4655)); dc->Rectangle(r);
        dc->SetFont(&TextFont); dc->SetColor(MCOLOR(0xFFF2F5F8));
        r.x += Pixels(8); r.w -= Pixels(16);
        dc->Text(r, button->GetText(), MAM_HCENTER | MAM_VCENTER);
    }
    MRECT GetClientRect(MButton*, const MRECT& r) override { return r; }
};
class ColorButtonLook : public MButtonLook
{
public:
    void OnDraw(MButton* button, MDrawContext* dc) override
    {
        const char* ids[] = {"CrossHairSwatchWhite", "CrossHairSwatchGreen", "CrossHairSwatchCyan",
            "CrossHairSwatchYellow", "CrossHairSwatchRed", "CrossHairSwatchPink"};
        u32 color = 0xFFFFFF;
        if (!strcmp(button->m_szIDLName, "CrossHairColorButton"))
        {
            auto* resource = ZGetGameInterface()->GetIDLResource();
            if (auto* hex = static_cast<MEdit*>(resource->FindWidget("CrossHairColorOption")))
                color = CrossHairSettings::ParseColor(hex->GetText(), ZGetConfiguration()->GetCrossHairColor());
        }
        else
            for (int i = 0; i < 6; ++i)
                if (!strcmp(button->m_szIDLName, ids[i])) color = CrossHairSettings::PresetColor(i);
        auto r = button->GetInitialClientRect();
        dc->SetColor(MCOLOR(0xFF000000 | color)); dc->FillRectangle(r);
        dc->SetColor(MCOLOR(button->IsMouseOver() ? 0xFF45C2D1 : 0xFF8C9BAA)); dc->Rectangle(r);
        const int brightness = int((color >> 16) & 255) * 299 +
            int((color >> 8) & 255) * 587 + int(color & 255) * 114;
        dc->SetFont(&TextFont);
        dc->SetColor(MCOLOR(brightness >= 128000 ? 0xFF111820 : 0xFFF2F5F8));
        dc->Text(r, button->GetText(), MAM_HCENTER | MAM_VCENTER);
    }
    MRECT GetClientRect(MButton*, const MRECT& r) override { return r; }
};
class CheckLook : public MButtonLook
{
public:
    void OnDraw(MButton* button, MDrawContext* dc) override
    {
        auto r = button->GetInitialClientRect();
        const int side = Pixels(18), y = (r.h - side) / 2;
        dc->SetColor(MCOLOR(0xFF111820)); dc->FillRectangle(0, y, side, side);
        dc->SetColor(MCOLOR(button->IsMouseOver() ? 0xFF45C2D1 : 0xFF8C9BAA)); dc->Rectangle(0, y, side, side);
        if (button->GetCheck())
        {
            dc->SetColor(MCOLOR(0xFF45C2D1));
            dc->FillRectangle(Pixels(4), y + Pixels(4), side - Pixels(8), side - Pixels(8));
        }
        dc->SetFont(&TextFont); dc->SetColor(MCOLOR(0xFFF2F5F8));
        r.x += Pixels(32); r.w -= Pixels(32);
        dc->Text(r, button->GetText(), MAM_LEFT | MAM_VCENTER);
    }
    MRECT GetClientRect(MButton*, const MRECT& r) override { return r; }
};
class ComboLook : public MButtonLook
{
public:
    void OnDraw(MButton* button, MDrawContext* dc) override
    {
        auto r = button->GetInitialClientRect();
        dc->SetColor(MCOLOR(0xFF111820)); dc->FillRectangle(r);
        dc->SetColor(MCOLOR(0xFF8C9BAA)); dc->Rectangle(r);
        dc->SetFont(&TextFont); dc->SetColor(MCOLOR(0xFFF2F5F8));
        auto text = r; text.x += Pixels(8); text.w -= Pixels(40);
        dc->Text(text, button->GetText(), MAM_LEFT | MAM_VCENTER);
        auto arrow = r; arrow.x = r.w - Pixels(28); arrow.w = Pixels(24);
        dc->Text(arrow, "v", MAM_HCENTER | MAM_VCENTER);
    }
    MRECT GetClientRect(MButton*, const MRECT& r) override { return r; }
};
class SliderLook : public MScrollBarLook
{
public:
    void OnDraw(MScrollBar* slider, MDrawContext* dc) override
    {
        const auto r = slider->GetInitialClientRect();
        dc->SetColor(MCOLOR(0xFF52616D));
        dc->FillRectangle(0, r.h / 2 - Pixels(2), r.w, Pixels(4));
    }
    MRECT GetClientRect(MScrollBar*, const MRECT& r) override { return r; }
};
class SliderArrowLook : public MArrowLook
{
public:
    void OnDraw(MArrow* arrow, MDrawContext* dc) override
    {
        auto r = arrow->GetInitialClientRect();
        dc->SetColor(MCOLOR(0xFF262F3C)); dc->FillRectangle(r);
        dc->SetFont(&TextFont); dc->SetColor(MCOLOR(0xFFF2F5F8));
        dc->Text(r, (arrow->m_nDirection == 0 || arrow->m_nDirection == 3) ? "-" : "+", MAM_HCENTER | MAM_VCENTER);
    }
    MRECT GetClientRect(MArrow*, MRECT& r) override { return r; }
    MSIZE GetDefaultSize(MArrow*) override { return MSIZE(Pixels(28), Pixels(32)); }
};
class SliderThumbLook : public MThumbLook
{
public:
    void OnDraw(MThumb* thumb, MDrawContext* dc) override
    {
        auto r = thumb->GetInitialClientRect();
        dc->SetColor(MCOLOR(0xFF45C2D1)); dc->FillRectangle(r);
    }
    MRECT GetClientRect(MThumb*, const MRECT& r) override { return r; }
};
class SliderThumbSize : public MSliderThumbLook
{
public:
    MSIZE GetDefaultSize(MSliderThumb*) override { return MSIZE(Pixels(18), Pixels(32)); }
};
static ContentLook Content;
static PanelLook Panel;
static ButtonLook Button;
static ColorButtonLook ColorButton;
static CheckLook Check;
static ComboLook Combo;
static SliderLook Slider;
static SliderArrowLook SliderArrow;
static SliderThumbLook SliderThumb;
static SliderThumbSize ThumbSize;

static void LayoutChildren(MWidget* parent)
{
    for (int i = 0; i < parent->GetChildCount(); ++i)
    {
        auto* child = parent->GetChild(i);
        const char* type = child->GetClassName();
        child->SetFont(&TextFont);
        if (!strcmp(type, "Frame"))
            static_cast<MFrame*>(child)->ChangeCustomLook(&Content);
        else if (!strcmp(type, "Button"))
        {
            auto* button = static_cast<MButton*>(child);
            const char* id = child->m_szIDLName;
            const bool tab = !strncmp(id, "Show", 4) || !strncmp(id, "ControlTabBtn", 13) || !strncmp(id, "OptionsAdvancedTab", 18);
            if (!strncmp(id, "CrossHairSwatch", 15) || !strcmp(id, "CrossHairColorButton"))
                button->ChangeCustomLook(&ColorButton);
            else
                button->ChangeCustomLook(button->GetType() == MBT_NORMAL || tab ? static_cast<MButtonLook*>(&Button) : &Check);
        }
        else if (!strcmp(type, "ComboBox"))
        {
            auto* combo = static_cast<MComboBox*>(child);
            combo->ChangeCustomLook(&Combo);
            combo->SetDropSize(Pixels(180));
        }
        else if (!strcmp(type, "ComboListBox"))
            static_cast<MListBox*>(child)->SetItemHeight(Pixels(28));
        else if (!strcmp(type, "Slider"))
        {
            auto* slider = static_cast<MSlider*>(child);
            slider->ChangeCustomLook(&Slider);
            slider->ChangeCustomArrowLook(&SliderArrow);
            slider->ChangeCustomThumbLook(&SliderThumb);
            static_cast<MSliderThumb*>(slider->m_pThumb)->ChangeCustomLook(&ThumbSize);
        }
        else if (!strcmp(type, "Label"))
        {
            auto* label = static_cast<MLabel*>(child);
            label->SetTextColor(MCOLOR(0xFFF2F5F8));
            label->SetAlignment(MAM_LEFT | MAM_VCENTER);
        }
        const auto r = child->GetIDLRect();
        if (r.w > 0 && r.h > 0)
            child->SetBounds(Pixels(r.x), Pixels(r.y), Pixels(r.w), Pixels(r.h));
        // Scrollbar internals and dropdown lists own their positions and listeners.
        if (!strcmp(type, "Frame")) LayoutChildren(child);
    }
}

static bool ControlsSelected(MIDLResource* resource)
{
    auto* button = resource->FindWidget("ShowKeyboardOptionGroup");
    return button && static_cast<MButton*>(button)->GetCheck();
}

static void Apply(bool opening = false)
{
    auto* resource = ZGetGameInterface()->GetIDLResource();
    auto* overlay = resource->FindWidget("Option");
    auto* group = resource->FindWidget("OptionGroup");
    if (!overlay || !group) return;
    const int screenW = RGetScreenWidth(), screenH = RGetScreenHeight();
    const float desired = max(1.f, min(2.f, screenH / 1080.f));
    Scale = max(.1f, min(desired, min((screenW - 32) / 960.f, (screenH - 32) / 680.f)));
    const int size = max(8, Pixels(16));
    HDC fontDC = GetDC(g_hWnd);
    const int dpi = fontDC ? max(1, GetDeviceCaps(fontDC, LOGPIXELSY)) : 96;
    if (fontDC) ReleaseDC(g_hWnd, fontDC);
    if (size != FontSize || dpi != FontDPI)
    {
        if (FontSize) { TextFont.Destroy(); TitleFont.Destroy(); }
        if (!CreatePixelFont(TextFont, "EGunZOptionsText", size, dpi, false) ||
            !CreatePixelFont(TitleFont, "EGunZOptionsTitle", max(8, Pixels(20)), dpi, true)) return;
        FontSize = size;
        FontDPI = dpi;
    }
    overlay->SetBounds(0, 0, screenW, screenH);
    static_cast<MFrame*>(group)->ChangeCustomLook(&Panel);
    group->SetBounds((screenW - Pixels(960)) / 2, (screenH - Pixels(680)) / 2, Pixels(960), Pixels(680));
    LayoutChildren(group);
    if (auto* invert = resource->FindWidget("InvertJoystick")) invert->Enable(false);
    if (opening)
    {
        for (const char* name : {"OptionTabControl", "ControlTabControl", "OptionsAdvancedTabControl"})
            if (auto* tab = resource->FindWidget(name)) static_cast<MTabCtrl*>(tab)->SetSelIndex(0);
    }
    for (const char* name : {"ResizeConfirm", "ViewConfirm", "NetworkPortConfirm", "CrossHairColorPicker"})
    {
        auto* popup = resource->FindWidget(name);
        if (!popup) continue;
        static_cast<MFrame*>(popup)->ChangeCustomLook(&Panel);
        const bool colorPicker = !strcmp(name, "CrossHairColorPicker");
        const int width = colorPicker ? 640 : 560, height = colorPicker ? 480 : 260;
        popup->SetBounds((screenW - Pixels(width)) / 2, (screenH - Pixels(height)) / 2, Pixels(width), Pixels(height));
        if (opening && colorPicker) popup->Show(false);
        LayoutChildren(popup);
    }
}
}

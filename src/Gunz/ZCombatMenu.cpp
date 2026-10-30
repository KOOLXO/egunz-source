#include "stdafx.h"
#include "ZCombatMenu.h"
#include "MWidget.h"
#include "MFrame.h"
#include "MButton.h"
#include "MDrawContext.h"
#include "Mint.h"
#include "Mint4R2.h"
#include "ZGameInterface.h"
#include "ZInterfaceListener.h"
#include "ZEscapeMenuLayout.h"

namespace EscapeMenuDesign
{
static MWidget* FindChild(MWidget* parent, const char* id)
{
    if (!parent) return nullptr;
    for (int i = 0; i < parent->GetChildCount(); ++i)
    {
        auto* child = parent->GetChild(i);
        if (!strcmp(child->m_szIDLName, id)) return child;
    }
    return nullptr;
}

static ZEscapeMenuLayout Layout(1920, 1080);
static MFontR2 Font;
static int FontPoints = 0;

class FrameLook : public MFrameLook
{
public:
    void OnDraw(MFrame* frame, MDrawContext* dc) override
    {
        const auto r = frame->GetInitialClientRect();
        dc->SetColor(MCOLOR(0xF51A2029)); dc->FillRectangle(r);
        dc->SetColor(MCOLOR(0xFF3B4655)); dc->Rectangle(r);
        dc->SetColor(MCOLOR(0xFF45C2D1)); dc->FillRectangle(r.x, r.y, r.w, 3);
        dc->SetFont(&Font);
        dc->SetColor(MCOLOR(0xFF45C2D1));
        dc->Text(r.x + Layout.Padding, r.y + Layout.Padding, "EGUNZ CLASSIC");
        dc->SetColor(MCOLOR(0xFFF2F5F8));
        dc->Text(r.x + Layout.Padding, r.y + Layout.Header / 2, "GAME MENU");
    }
    MRECT GetClientRect(MFrame*, const MRECT& r) override
    {
        return MRECT(r.x + Layout.Padding, r.y + Layout.Header,
            r.w - 2 * Layout.Padding, r.h - Layout.Header - Layout.Padding);
    }
};

class ButtonLook : public MButtonLook
{
public:
    void OnDraw(MButton* button, MDrawContext* dc) override
    {
        const auto r = button->GetInitialClientRect();
        const bool resume = !strcmp(button->m_szIDLName, "CombatMenuClose");
        const bool quit = !strcmp(button->m_szIDLName, "Exit");
        const bool enabled = button->IsEnable();
        const bool over = enabled && button->IsMouseOver();
        const bool down = enabled && button->IsButtonDown();
        dc->SetColor(MCOLOR(down ? 0xFF173D47 : over ? 0xFF354B5C :
            resume ? 0xFF24505C : 0xFF262F3C));
        dc->FillRectangle(r);
        dc->SetColor(MCOLOR(over || resume ? 0xFF45C2D1 : 0xFF3B4655));
        dc->Rectangle(r);
        dc->SetFont(&Font);
        dc->SetColor(MCOLOR(!enabled ? 0xFF78818D : quit ? 0xFFEFA5A5 : 0xFFF2F5F8));
        auto text = r; text.x += Layout.Padding / 2; text.w -= Layout.Padding;
        dc->Text(text, button->GetText(), MAM_LEFT | MAM_VCENTER);
    }
};
static FrameLook Frame;
static ButtonLook Button;
}

ZCombatMenu::ZCombatMenu()
{
    m_FrameStr = "CombatMenuFrame";
    m_ItemStr[ZCMI_OPTION] = "OptionFrame";
    m_ItemStr[ZCMI_CLOSE] = "CombatMenuClose";
    m_ItemStr[ZCMI_BATTLE_EXIT] = "BattleExit";
    m_ItemStr[ZCMI_STAGE_EXIT] = "StageExit";
    m_ItemStr[ZCMI_PROG_EXIT] = "Exit";
}
ZCombatMenu::~ZCombatMenu() {}

bool ZCombatMenu::IsEnableItem(ZCOMBAT_MENU_ITEM item)
{
    auto* menu = ZGetGameInterface()->GetIDLResource()->FindWidget(m_FrameStr);
    auto* widget = EscapeMenuDesign::FindChild(menu, m_ItemStr[item].c_str());
    return widget && widget->IsEnable();
}
void ZCombatMenu::EnableItem(ZCOMBAT_MENU_ITEM item, bool enabled)
{
    auto* menu = ZGetGameInterface()->GetIDLResource()->FindWidget(m_FrameStr);
    auto* widget = EscapeMenuDesign::FindChild(menu, m_ItemStr[item].c_str());
    if (widget) widget->Enable(enabled);
}

void ZCombatMenu::ShowModal(bool show)
{
    auto* resource = ZGetGameInterface()->GetIDLResource();
    auto* widget = resource->FindWidget(m_FrameStr);
    if (!widget) return;
    if (show)
    {
        using namespace EscapeMenuDesign;
        Layout = ZEscapeMenuLayout(MGetWorkspaceWidth(), MGetWorkspaceHeight());
        if (FontPoints != Layout.FontPoints)
        {
            if (FontPoints) Font.Destroy();
            if (Font.Create("EGunZEscapeMenuFont", "Arial", Layout.FontPoints, 1.f, true))
                FontPoints = Layout.FontPoints;
        }
        auto* frame = static_cast<MFrame*>(widget);
        frame->ChangeCustomLook(&Frame);
        frame->SetBounds((MGetWorkspaceWidth() - Layout.Width) / 2,
            (MGetWorkspaceHeight() - Layout.Height) / 2, Layout.Width, Layout.Height);
        const char* ids[] = {"CombatMenuClose", "OptionFrame", "TrainingFrame", "BattleExit", "StageExit", "Exit"};
        const char* labels[] = {"Resume match", "Settings", "Training", "Leave match", "Return to lobby", "Quit game"};
        const char keys[] = {'X', 'O', 'T', 'R', 'L', 'Q'};
        for (int i = 0; i < 6; ++i)
        {
            auto* entry = FindChild(frame, ids[i]);
            if (!entry || entry->GetParent() != frame) continue;
            auto* button = static_cast<MButton*>(entry);
            button->ChangeCustomLook(&Button);
            button->SetBounds(Layout.Padding, Layout.RowY(i),
                Layout.Width - 2 * Layout.Padding, Layout.Row);
            button->SetText(labels[i]);
            button->SetAccelerator(keys[i]);
            button->Show(true);
            if (i == 1)
            {
                button->Enable(true);
                button->SetListener(ZGetOptionFrameButtonListener());
            }
        }
    }
    widget->Show(show, true);
}

bool ZCombatMenu::IsVisible()
{
    auto* widget = ZGetGameInterface()->GetIDLResource()->FindWidget(m_FrameStr);
    return widget && widget->IsVisible();
}

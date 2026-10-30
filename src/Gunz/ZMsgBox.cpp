#include "stdafx.h"

#include "ZGameClient.h"
#include "ZApplication.h"
#include "ZMsgBox.h"
#include "Mint.h"
#include "ZUiDesign.h"


// ZMsgBox Listener
class ZMsgBoxListener : public MListener{
public:
	virtual bool OnCommand(MWidget* pWidget, const char* szMessage){
		ZMsgBox* pMsgBox = (ZMsgBox*)pWidget;
		if (pMsgBox->GetCustomListener()) {
			bool bResult = pMsgBox->GetCustomListener()->OnCommand(pWidget, szMessage);
			pMsgBox->SetCustomListener(NULL);
			return bResult;
		}

		if(MWidget::IsMsg(szMessage, MMSGBOX_OK)==true){
			const char* pText = pWidget->GetText();

			// 인원이 꽉차 종료
//			if(!_stricmp(pText, MGetErrorString(MERR_CLIENT_FULL_PLAYERS)))
			if(!_stricmp(pText, ZErrStr(MERR_CLIENT_FULL_PLAYERS)))
			{
				if (ZApplication::GetInstance()->GetLaunchMode() == ZApplication::ZLAUNCH_MODE_NETMARBLE)
				{

					ZApplication::Exit();
					return true;
				}
			}
		}

		pWidget->Show(false);

		return false;
	}
};
ZMsgBoxListener	g_MsgBoxListener;

MListener* ZGetMsgBoxListener(void)
{
	return &g_MsgBoxListener;
}

// ZConfirmMsgBox Listener : Default Listener 일뿐. 물어볼때마다 CustomListener 지정해서 쓸것
class ZConfirmMsgBoxListener : public MListener{
public:
	virtual bool OnCommand(MWidget* pWidget, const char* szMessage){
		ZMsgBox* pMsgBox = (ZMsgBox*)pWidget;
		if (pMsgBox->GetCustomListener()) {
			bool bResult = pMsgBox->GetCustomListener()->OnCommand(pWidget, szMessage);
			pMsgBox->SetCustomListener(NULL);
			return bResult;
		}

		pWidget->Show(false);
		return false;
	}
};
ZConfirmMsgBoxListener	g_CofirmMsgBoxListener;

MListener* ZGetConfirmMsgBoxListener(void)
{
	return &g_CofirmMsgBoxListener;
}

constexpr auto ZMSGBOX_TRANSIENT_TIME = 200;

void ZMsgBox::Show(bool bVisible, bool bModal)
{
	auto elapsed = (GetGlobalTimeMS() - m_nShowTime);

	if (m_bNextVisible == m_bVisible && m_bVisible == bVisible && elapsed > ZMSGBOX_TRANSIENT_TIME)
		return;

	if(m_bNextVisible!=bVisible){
		if (elapsed < ZMSGBOX_TRANSIENT_TIME)
			m_nShowTime = GetGlobalTimeMS() - (ZMSGBOX_TRANSIENT_TIME - elapsed);
		else
			m_nShowTime=GetGlobalTimeMS();
	}
	m_bNextVisible = bVisible;

	MMsgBox::Show(bVisible,bModal);
	m_bVisible = true;

	Enable(bVisible);
}

void ZMsgBox::LayoutDialog()
{
    m_LayoutWidth = MGetWorkspaceWidth();
    m_LayoutHeight = MGetWorkspaceHeight();
    m_LayoutDpi = ZUiDesign::Dpi();
    ChangeCustomLook(ZUiDesign::Panel());
    SetFont(ZUiDesign::Font());
    m_pMessage->SetFont(ZUiDesign::Font());
    m_pMessage->SetLineHeight(ZUiDesign::Pixels(24));
    m_pMessage->SetTextColor(MCOLOR(0xFFF2F5F8));
    m_pMessage->SetScrollBarEnable(false);
    for (auto* button : {m_pOK, m_pCancel})
        if (button) { button->SetFont(ZUiDesign::Font()); button->ChangeCustomLook(ZUiDesign::Button()); }
    const int width = ZUiDesign::Pixels(560);
    const int textWidth = width - ZUiDesign::Pixels(48);
    const int lines = MMGetLineCount(m_pMessage->GetFont(), GetText(), textWidth);
    const ZUiLayout::Dialog layout(MGetWorkspaceWidth(), MGetWorkspaceHeight(), lines);
    SetBounds(layout.x, layout.y, layout.width, layout.height);
    OnSize(layout.width, layout.height); // Also reflow when only the DPI/font changed.
}

void ZMsgBox::OnSize(int w, int h)
{
    const int padding = ZUiDesign::Pixels(24), gap = ZUiDesign::Pixels(12);
    const int buttonW = ZUiDesign::Pixels(140), buttonH = ZUiDesign::Pixels(40);
    const int buttonY = h - padding - buttonH;
    m_pMessage->SetBounds(padding, ZUiDesign::Pixels(64), w - 2 * padding,
        buttonY - ZUiDesign::Pixels(80));
    if (m_pCancel) m_pCancel->SetBounds(w - padding - buttonW, buttonY, buttonW, buttonH);
    if (m_pOK) m_pOK->SetBounds(w - padding - buttonW - (m_pCancel ? buttonW + gap : 0),
        buttonY, buttonW, buttonH);
}

bool ZMsgBox::OnShow()
{
    LayoutDialog();
    if (m_pOK && m_pOK->IsVisible()) m_pOK->SetFocus();
    else if (m_pCancel) m_pCancel->SetFocus();
    return true;
}

void ZMsgBox::OnDraw(MDrawContext* pDC)
{
	if (m_bNextVisible && (m_LayoutWidth != MGetWorkspaceWidth() ||
		m_LayoutHeight != MGetWorkspaceHeight() || m_LayoutDpi != ZUiDesign::Dpi())) LayoutDialog();
	float fOpacity = 0;
	if(m_bNextVisible==false){	// Hide
		fOpacity = 1.0f - min(float(GetGlobalTimeMS() - m_nShowTime) / (float)ZMSGBOX_TRANSIENT_TIME, 1.0f);
		if(fOpacity==0.0f) {
			m_bVisible = false;
			m_bExclusive = false;
		}
		SetOpacity(u8(fOpacity*0xFF));
	}
	else{	// Show
		fOpacity = min(float(GetGlobalTimeMS() - m_nShowTime) / (float)ZMSGBOX_TRANSIENT_TIME, 1.0f);
		SetOpacity(u8(fOpacity*0xFF));
	}

	MMsgBox::OnDraw(pDC);
}

ZMsgBox::ZMsgBox(const char* szMessage, MWidget* pParent, MListener* pListener, MMsgBoxType nType)
: MMsgBox(szMessage,pParent,pListener,nType)
{
	m_bCanShade = false;
	m_bNextVisible = false;
	m_nShowTime = GetGlobalTimeMS() - ZMSGBOX_TRANSIENT_TIME * 2;
	SetOpacity(0);
	m_pCustomListener = NULL;
}

ZMsgBox::~ZMsgBox(void)
{
}

void ZMsgBox::SetText(const char* szText)
{
    MMsgBox::SetText(szText);
    LayoutDialog();
}

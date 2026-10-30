#include "stdafx.h"
#include "ZIDLResource.h"
#include "ZGame.h"
#include "Mint4R2.h"
#include "RealSpace2.h"
#include "Mint.h"
#include "MBScrollBarLook.h"
#include "ZFrame.h"
#include "ZMapListBox.h"
#include "ZScoreListBox.h"
#include "ZMeshView.h"
#include "ZMeshViewList.h"
#include "ZCharacterView.h"
#include "ZCharacterViewList.h"
#include "ZEquipmentListBox.h"
#include "ZStageInfoBox.h"
#include "ZItemSlotView.h"
#include "ZRoomListBox.h"
#include "ZPlayerListBox.h"
#include "ZCanvas.h"
#include "ZPlayerSelectListBox.h"
#include "ZBmNumLabel.h"
#include "ZClanListBox.h"
#include "ZServerView.h"
#include "ZStringResManager.h"
#include "ZActionKey.h"
#include "RGMain.h"
#include "ZUiDesign.h"
#include "MEdit.h"
#include "MLabel.h"
#include "MButton.h"
#include "MSlider.h"
#include "ZConfiguration.h"
#include "CrossHairSettings.h"
#include "ZSettingsSlider.h"
#include "MFrame.h"
#include "ZCrossHair.h"

ZIDLResource::ZIDLResource() = default;
ZIDLResource::~ZIDLResource() = default;

ZCanvas* ZIDLResource::GetCanvas(MXmlElement& element)
{
	MXmlElement childElement;
	char szBuf[4096];
	MWidget* pParentWidget;	MListener* pListener;

	pListener = pParentWidget = GetParentWidget(element);
	ZCanvas* pCanvas = new ZCanvas("", pParentWidget, pListener);
	InsertWidget(element, pCanvas);

	int iCount = element.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		memset(szBuf, 0, sizeof(szBuf));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szBuf);

		GetCommonWidgetProperty(pCanvas, childElement, szBuf);
	}

	return pCanvas;
}

ZMapListBox* ZIDLResource::GetMapListBox(MXmlElement& element)
{
	MXmlElement childElement;
	char szBuf[4096];
	MWidget* pParentWidget;	MListener* pListener;

	pListener = pParentWidget = GetParentWidget(element);
	ZMapListBox* pListBox = new ZMapListBox("", pParentWidget, pListener);
	InsertWidget(element, pListBox);

	int iCount = element.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		memset(szBuf, 0, sizeof(szBuf));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szBuf);

		GetCommonWidgetProperty(pListBox, childElement, szBuf);
	}

	return pListBox;
}

ZScoreBoardFrame* ZIDLResource::GetScoreBoardFrame(MXmlElement& element)
{
	MXmlElement childElement;
	char szBuf[4096];
	MWidget* pParentWidget;	MListener* pListener;
	MBFrameLook* pFrameLook = NULL;

	pListener = pParentWidget = GetParentWidget(element);
	ZScoreBoardFrame* pFrame = new ZScoreBoardFrame("", pParentWidget, pListener);
	InsertWidget(element, pFrame);

	int iCount = element.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		memset(szBuf, 0, sizeof(szBuf));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szBuf);

		if (GetCommonWidgetProperty(pFrame, childElement, szBuf)) continue;

		if (!strcmp(szBuf, "FRAMELOOK"))
		{
			char szItem[256];
			memset(szItem, 0, sizeof(szItem));
			childElement.GetContents(szItem);

			map<string, MBFrameLook*>::iterator itor = m_FrameLookMap.find(szItem);
			if (itor != m_FrameLookMap.end())
			{
				pFrameLook = (MBFrameLook*)(*itor).second;
				pFrame->ChangeCustomLook((MFrameLook*)pFrameLook);
			}
		}
	}


	return pFrame;
}


ZScoreListBox* ZIDLResource::GetScoreListBox(MXmlElement& element)
{
	MXmlElement childElement;
	char szBuf[4096];
	MWidget* pParentWidget;	MListener* pListener;

	pListener = pParentWidget = GetParentWidget(element);
	ZScoreListBox* pListBox = new ZScoreListBox("", pParentWidget, pListener);
	InsertWidget(element, pListBox);

	int iCount = element.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		memset(szBuf, 0, sizeof(szBuf));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szBuf);

		GetCommonWidgetProperty(pListBox, childElement, szBuf);
	}

	return pListBox;
}

ZItemSlotView* ZIDLResource::GetItemSlot(MXmlElement& element)
{
	MXmlElement childElement;
	char szBuf[4096];

	MWidget* pParentWidget = GetParentWidget(element);
	ZItemSlotView* pWidget = new ZItemSlotView("", pParentWidget, pParentWidget);
	InsertWidget(element, pWidget);

	pWidget->SetParts(MMCIP_END);
	char szParts[64];
	element.GetAttribute(szParts, "parts", "");
	if (!_stricmp(szParts, "head")) pWidget->SetParts(MMCIP_HEAD);
	else if (!_stricmp(szParts, "chest")) pWidget->SetParts(MMCIP_CHEST);
	else if (!_stricmp(szParts, "hands")) pWidget->SetParts(MMCIP_HANDS);
	else if (!_stricmp(szParts, "legs")) pWidget->SetParts(MMCIP_LEGS);
	else if (!_stricmp(szParts, "feet")) pWidget->SetParts(MMCIP_FEET);
	else if (!_stricmp(szParts, "fingerl")) pWidget->SetParts(MMCIP_FINGERL);
	else if (!_stricmp(szParts, "fingerr")) pWidget->SetParts(MMCIP_FINGERR);
	else if (!_stricmp(szParts, "melee")) pWidget->SetParts(MMCIP_MELEE);
	else if (!_stricmp(szParts, "primary")) pWidget->SetParts(MMCIP_PRIMARY);
	else if (!_stricmp(szParts, "secondary")) pWidget->SetParts(MMCIP_SECONDARY);
	else if (!_stricmp(szParts, "custom1")) pWidget->SetParts(MMCIP_CUSTOM1);
	else if (!_stricmp(szParts, "custom2")) pWidget->SetParts(MMCIP_CUSTOM2);

	int iCount = element.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		memset(szBuf, 0, sizeof(szBuf));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szBuf);

		GetCommonWidgetProperty(pWidget, childElement, szBuf);

		if (!strcmp(szBuf, "BITMAP"))
		{
			MBitmap* pBitmap = GetBitmap(childElement);
			if (pBitmap != NULL)
			{
				pWidget->SetBackBitmap(pBitmap);
			}
		}
		if( !strcmp(szBuf, "STRETCH") )
		{
			pWidget->SetStretch( true );
		}
	}

	return pWidget;

}
ZMeshView* ZIDLResource::GetMeshView(MXmlElement& element)
{
	MXmlElement childElement;
	char szBuf[4096];

	MWidget* pParentWidget = GetParentWidget(element);
	ZMeshView* pWidget = new ZMeshView("", pParentWidget, pParentWidget);
	InsertWidget(element, pWidget);

	int iCount = element.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		memset(szBuf, 0, sizeof(szBuf));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szBuf);

		GetCommonWidgetProperty(pWidget, childElement, szBuf);

		if (!strcmp(szBuf, "BUTTONLOOK"))
		{
			char szItem[256];
			memset(szItem, 0, sizeof(szItem));
			childElement.GetContents(szItem);

			map<string, MBButtonLook*>::iterator itor = m_ButtonLookMap.find(szItem);
			if (itor != m_ButtonLookMap.end())
			{
				pWidget->ChangeCustomLook((MButtonLook*)(*itor).second);
			}
			pWidget->SetLook(true);
		}
	}

	return pWidget;
}

ZMeshViewList* ZIDLResource::GetMeshViewList(MXmlElement& element)
{
	MXmlElement childElement;
	char szBuf[4096];

	MWidget* pParentWidget = GetParentWidget(element);
	ZMeshViewList* pWidget = new ZMeshViewList("", pParentWidget, pParentWidget);
	InsertWidget(element, pWidget);

	int iCount = element.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		memset(szBuf, 0, sizeof(szBuf));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szBuf);

		if(GetCommonWidgetProperty(pWidget, childElement, szBuf)) continue;

		if(!strcmp(szBuf, "ITEMWIDTH")){
			int nWidth = 80;
			childElement.GetContents(&nWidth);
			pWidget->SetItemWidth(nWidth);
		}

	}

	return pWidget;
}

ZCharacterView* ZIDLResource::GetCharacterView(MXmlElement& element)
{
	MXmlElement childElement;
	char szBuf[4096];

	MWidget* pParentWidget = GetParentWidget(element);
	ZCharacterView* pWidget = new ZCharacterView("", pParentWidget, pParentWidget);
	InsertWidget(element, pWidget);

	int iCount = element.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		memset(szBuf, 0, sizeof(szBuf));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szBuf);

		GetCommonWidgetProperty(pWidget, childElement, szBuf);

		if(!strcmp(szBuf, "VISIBLEEQUIPMENT")){
			bool bVisibleEquipment = false;
			childElement.GetContents(&bVisibleEquipment);
			pWidget->SetVisibleEquipment(bVisibleEquipment);
		}
	}

	return pWidget;
}

ZCharacterViewList* ZIDLResource::GetCharacterViewList(MXmlElement& element)
{
	MXmlElement childElement;
	char szBuf[4096];

	MWidget* pParentWidget = GetParentWidget(element);
	ZCharacterViewList* pWidget = new ZCharacterViewList("", pParentWidget, pParentWidget);
	InsertWidget(element, pWidget);

	int iCount = element.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		memset(szBuf, 0, sizeof(szBuf));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szBuf);

		if(GetCommonWidgetProperty(pWidget, childElement, szBuf)) continue;

		if(!strcmp(szBuf, "ITEMWIDTH")){
			int nWidth = 80;
			childElement.GetContents(&nWidth);
			pWidget->SetItemWidth(nWidth);
		}
	}

	return pWidget;
}

ZEquipmentListBox* ZIDLResource::GetEquipmentListBox(MXmlElement& element)
{
	MXmlElement childElement;
	char szBuf[4096];

	MWidget* pParentWidget = GetParentWidget(element);
	ZEquipmentListBox* pWidget = new ZEquipmentListBox("", pParentWidget, pParentWidget);
	pWidget->m_FontAlign = MAM_VCENTER;

	InsertWidget(element, pWidget);

	int iCount = element.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		memset(szBuf, 0, sizeof(szBuf));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szBuf);

		if (GetCommonWidgetProperty(pWidget, childElement, szBuf)) continue;
		else if (!strcmp(szBuf, "LISTBOXLOOK"))
		{
			char szItem[256];
			memset(szItem, 0, sizeof(szItem));
			childElement.GetContents(szItem);

			map<string, MBListBoxLook*>::iterator itor = m_ListBoxLookMap.find(szItem);
			if (itor != m_ListBoxLookMap.end())
			{
				MBListBoxLook* pListBoxLook = NULL;
				pListBoxLook = (*itor).second;
				pWidget->ChangeCustomLook(pListBoxLook);
			}
		}else if (!strcmp(szBuf, "SCROLLBARLOOK"))
		{
			char szItem[256];
			memset(szItem, 0, sizeof(szItem));
			childElement.GetContents(szItem);

			map<string, MBScrollBarLook*>::iterator itor = m_ScrollBarLookMap.find(szItem);
			if (itor != m_ScrollBarLookMap.end())
				pWidget->GetScrollBar()->ChangeCustomLook((*itor).second);
		}else if (!strcmp(szBuf, "ARROWLOOK"))
		{
			char szItem[256];
			memset(szItem, 0, sizeof(szItem));
			childElement.GetContents(szItem);

			map<string, MBArrowLook*>::iterator itor = m_ArrowLookMap.find(szItem);
			if (itor != m_ArrowLookMap.end())
				pWidget->GetScrollBar()->ChangeCustomArrowLook((*itor).second);
		}else if (!strcmp(szBuf, "THUMBLOOK"))
		{
			char szItem[256];
			memset(szItem, 0, sizeof(szItem));
			childElement.GetContents(szItem);

			map<string, MBThumbLook*>::iterator itor = m_ThumbLookMap.find(szItem);
			if (itor != m_ThumbLookMap.end())
				pWidget->GetScrollBar()->ChangeCustomThumbLook((*itor).second);
		}
	}

	return pWidget;
}

ZStageInfoBox* ZIDLResource::GetStageInfoBox(MXmlElement& element)
{
	MXmlElement childElement;
	char szBuf[4096];

	MWidget* pParentWidget = GetParentWidget(element);
	ZStageInfoBox* pWidget = new ZStageInfoBox("", pParentWidget, pParentWidget);
	InsertWidget(element, pWidget);

	int iCount = element.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		memset(szBuf, 0, sizeof(szBuf));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szBuf);

		if (GetCommonWidgetProperty(pWidget, childElement, szBuf)) continue;
		else if (!strcmp(szBuf, "STAGEINFOITEMLOOK"))
		{
			char szItem[256];
			memset(szItem, 0, sizeof(szItem));
			childElement.GetContents(szItem);

			map<string, MBListBoxLook*>::iterator itor = m_ListBoxLookMap.find(szItem);
			if (itor != m_ListBoxLookMap.end())
			{
				MBListBoxLook* pListBoxLook = NULL;
				pListBoxLook = (*itor).second;
				pWidget->SetLook(pListBoxLook);
			}
		}
	}

	return pWidget;
}

ZRoomListBox* ZIDLResource::GetRoomListBox( MXmlElement& element )
{
	MXmlElement childElement;
	char szBuf[4096];
	char szAttr[4096];
	
	MWidget* pParentWidget = GetParentWidget(element);
	ZRoomListBox* pWidget = new ZRoomListBox("", pParentWidget, pParentWidget);
	InsertWidget(element, pWidget);

	int iCount = element.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		memset(szBuf, 0, sizeof(szBuf));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szBuf);

		if(GetCommonWidgetProperty(pWidget, childElement, szBuf)) continue;
		else	if( strcmp(szBuf, "BITMAP") == 0)
		{
			childElement.GetAttribute(szAttr, "type" );
			if( strcmp(szAttr,"frame") == 0)
			{
				pWidget->SetFrameImage( GetBitmap(childElement));				
			}
			else if( strcmp(szAttr, "back") == 0 )
			{
				childElement.GetContents(szAttr);

				MBitmap* pBitmap =GetBitmap(childElement);
				if (pBitmap)
				{
					pWidget->SetBannerImage(szAttr, pBitmap);
					GetRGMain().AddMapBanner(szAttr, pBitmap);
				}
			}
 			else if( _stricmp( szAttr,"icon" )==0 )
			{
				int mode;
				childElement.GetAttribute(&mode, "mode");
				childElement.GetContents(szAttr);
				MBitmap* pBitmap = GetBitmap(childElement);
				if(pBitmap)
					pWidget->SetIconImage( (MMATCH_GAMETYPE)mode, pBitmap );
			}
		}
		else if ( strcmp(szBuf, "SIZE") == 0 )
		{
			childElement.GetAttribute(szAttr, "type" );
			if( strcmp(szAttr,"width") == 0)
			{
				float w;
				childElement.GetContents( &w );
				pWidget->SetWidth( w );
			}
			if( strcmp(szAttr,"height") == 0)
			{
				float h;
				childElement.GetContents( &h );
				pWidget->SetHeight( h );
			}
		}
	}
	return pWidget;
}

ZClanListBox* ZIDLResource::GetClanListBox( MXmlElement& element )
{
	MXmlElement childElement;
	char szBuf[4096];

	MWidget* pParentWidget = GetParentWidget(element);
	ZClanListBox* pWidget = new ZClanListBox("", pParentWidget, pParentWidget);
	InsertWidget(element, pWidget);

	int iCount = element.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		memset(szBuf, 0, sizeof(szBuf));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szBuf);

		if(GetCommonWidgetProperty(pWidget, childElement, szBuf)) continue;
	}
	return pWidget;
}

ZServerView* ZIDLResource::GetServerView(MXmlElement& element)
{
	MXmlElement childElement;
	char szBuf[ 4096];

	MWidget* pParentWidget = GetParentWidget( element);
	ZServerView* pWidget = new ZServerView( "", pParentWidget, pParentWidget);
	InsertWidget( element, pWidget);

	int iCount = element.GetChildNodeCount();
	for (int i = 0;  i < iCount;  i++)
	{
		memset(szBuf, 0, sizeof( szBuf));
		childElement = element.GetChildNode( i);
		childElement.GetTagName( szBuf);

		GetCommonWidgetProperty( pWidget, childElement, szBuf);
	}

	return pWidget;
}

ZPlayerListBox* ZIDLResource::GetPlayerListBox( MXmlElement& element )
{
	MXmlElement childElement;
	char szBuf[4096];
	char szAttr[4096];

	MWidget* pParentWidget = GetParentWidget(element);
	ZPlayerListBox* pWidget = new ZPlayerListBox("", pParentWidget, pParentWidget);
	pWidget->SetListener(pWidget);
	InsertWidget(element, pWidget);

	int iCount = element.GetChildNodeCount();

	bool bMode1 = false;

	for (int i = 0; i < iCount; i++)
	{
		memset(szBuf, 0, sizeof(szBuf));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szBuf);

		if (GetCommonWidgetProperty(pWidget, childElement, szBuf)) {
			continue;
		}
		else if( strcmp(szBuf, "SIZE" ) == 0 )
		{
			childElement.GetAttribute(szAttr, "type");
			if( strcmp(szAttr,"width") == 0 )
			{
				float temp;
				childElement.GetContents(&temp);
				pWidget->SetWidth( temp );
			}
			else if( strcmp(szAttr,"height") == 0 )
			{
				float temp;
				childElement.GetContents(&temp);
				pWidget->SetHeight( temp );
			}
		}
		else if(strcmp(szBuf, "MODE1" ) == 0 ) {
			bMode1 = true;
		}
	}

	if(bMode1==false)
		pWidget->InitUI(ZPlayerListBox::PlayerListMode::Channel);
	else
		pWidget->InitUI(ZPlayerListBox::PlayerListMode::Stage);

	return pWidget;
}

ZPlayerSelectListBox* ZIDLResource::GetPlayerSelectListBox(MXmlElement& element)
{
	MXmlElement childElement;
	char szBuf[4096];

	MWidget* pParentWidget = GetParentWidget(element);
	ZPlayerSelectListBox* pWidget = new ZPlayerSelectListBox("SelectPlayer", pParentWidget, pParentWidget);
	pWidget->SetListener(pWidget);
	InsertWidget(element, pWidget);

	int iCount = element.GetChildNodeCount();

	bool bMode1 = false;

	for (int i = 0; i < iCount; i++)
	{
		memset(szBuf, 0, sizeof(szBuf));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szBuf);

		if(GetCommonWidgetProperty(pWidget, childElement, szBuf)) continue;
		else if(!strcmp(szBuf, "MULTISELECT"))
		{
			pWidget->m_bMultiSelect = true;
		}
	}
	return pWidget;
}

ZBmNumLabel* ZIDLResource::GetBmNumLabel(MXmlElement& element)
{
	MXmlElement childElement;
	char szBuf[1024];
	MWidget* pParentWidget;	MListener* pListener;

	pListener = pParentWidget = GetParentWidget(element);
	ZBmNumLabel* pBmLabel = new ZBmNumLabel(MINT_ZBMNUMLABEL, pParentWidget, pListener);
	InsertWidget(element, pBmLabel);

	int iCount = element.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		memset(szBuf, 0, sizeof(szBuf));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szBuf);

		if (GetCommonWidgetProperty(pBmLabel, childElement, szBuf)) continue;
		else if (!strcmp(szBuf, "FONTSIZE"))
		{
			pBmLabel->SetCharSize(GetSize(childElement));
		}
		else if (!strcmp(szBuf, "BITMAP"))
		{
			MBitmap* pBitmap = GetBitmap(childElement);

			if (pBitmap != NULL)
			{
				pBmLabel->SetLabelBitmap(pBitmap);
			}
		}
	}
	return pBmLabel;
}

ZActionKey* ZIDLResource::GetActionKey(MXmlElement& element)
{
	MXmlElement childElement;
	char szBuf[4096];
	MWidget* pParentWidget;	MListener* pListener;

	pListener = pParentWidget = GetParentWidget(element);
	ZActionKey* pActionKey = (ZActionKey*)Mint::GetInstance()->NewWidget(MINT_ACTIONKEY, "", pParentWidget, pListener);
	InsertWidget(element, pActionKey);

	int iCount = element.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		memset(szBuf, 0, sizeof(szBuf));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szBuf);

		if (GetCommonWidgetProperty(pActionKey, childElement, szBuf)) continue;	}

	return pActionKey;
}

void ZIDLResource::Parse(MXmlElement& element)
{
	MIDLResource::Parse(element);

	char szTagName[256];
	element.GetTagName(szTagName);

	// Gunz Customized Widgets...
	if (!strcmp(szTagName, "MAPLISTBOX"))
	{
		GetMapListBox(element);
	}
	else if (!strcmp(szTagName, "CANVAS"))
	{
		GetCanvas(element);
	}
	else if (!strcmp(szTagName, "SCORELISTBOX"))
	{
		GetScoreListBox(element);
	}
	else if (!strcmp(szTagName, "SCOREBOARD"))
	{
		GetScoreBoardFrame(element);
	}
	else if (!strcmp(szTagName, "MESHVIEW"))
	{
		GetMeshView(element);
	}
	else if (!strcmp(szTagName, "MESHVIEWLIST"))
	{
		GetMeshViewList(element);
	}
	else if (!strcmp(szTagName, "CHARACTERVIEW"))
	{
		GetCharacterView(element);
	}
	else if (!strcmp(szTagName, "CHARACTERVIEWLIST"))
	{
		GetCharacterViewList(element);
	}
	else if (!strcmp(szTagName, "EQUIPMENTLISTBOX"))
	{
		GetEquipmentListBox(element);
	}
	else if(!strcmp(szTagName, "STAGEINFOBOX"))
	{
		GetStageInfoBox(element);
	}
	else if (!strcmp(szTagName, "ITEMSLOT"))
	{
		GetItemSlot(element);
	}
	else if (!strcmp(szTagName, "ROOMLIST"))
	{
		GetRoomListBox(element);
	}
	else if(!strcmp(szTagName,"PLAYERLISTBOX"))
	{
		GetPlayerListBox(element);
	}
	else if(!strcmp(szTagName,"PLAYERSELECTLISTBOX"))
	{
		GetPlayerSelectListBox(element);
	}
	else if(!strcmp(szTagName,"ZBMNUMLABEL"))
	{
		GetBmNumLabel(element);
	}
	else if (!strcmp(szTagName, "CLANLIST"))
	{
		GetClanListBox(element);
	}
	else if (!strcmp(szTagName, "SERVERVIEW"))
	{
		GetServerView(element);
	}
	else if (!strcmp(szTagName,"ACTIONKEY"))
	{
		GetActionKey(element);
	}
}

MFrame*	ZIDLResource::CreateFrame(const char* szName, MWidget* pParent, MListener* pListener)
{
	MFrame* pFrame = new ZFrame(szName, pParent, pListener);
	return pFrame;
}

MFont* ZIDLResource::CreateFont(char* szAliasName, char* szFontName, int nHeight, 
								bool bBold, bool bItalic, int nOutlineStyle, bool bAntialiasing, DWORD nColorArg1, DWORD nColorArg2)
{
	MFontR2* pNew = new MFontR2;
	pNew->Create(szAliasName, szFontName, nHeight, 1.0f, bBold, bItalic, nOutlineStyle, -1, bAntialiasing, nColorArg1, nColorArg2);
	return pNew;
}

void ZIDLResource::TransText(const char* szSrc, char* szOut, int maxlen)
{
	strcpy_safe(szOut, maxlen, ZGetStringResManager()->GetStringFromXml(szSrc));

	if (!_strnicmp(szOut, "STR:", 4))
	{
		mlog("%s , %s\n", szSrc, szOut);
	}
}

void ZGetInterfaceSkinPath(char* pOutPath, int maxlen, const char* szSkinName)
{
	sprintf_safe(pOutPath, maxlen, "%s%s/", PATH_INTERFACE, szSkinName);
}


// Add controls to existing skins without requiring a separate client XML update.
void ZIDLResource::EnsureCrossHairOptions()
{
	auto* combo = FindWidget("CrossHairComboBox");
	if (!combo || FindWidget("CrossHairSizeOption")) return;
	auto* parent = combo->GetParent();
	if (!parent) return;
	// Layout uses the XML's unscaled logical coordinates, as OptionsReadability does.
	const auto logical = combo->GetIDLRect();
	auto r = logical.w > 0 && logical.h > 0 ? logical : combo->GetRect();
	const auto parentLogical = parent->GetIDLRect();
	const int parentWidth = parentLogical.w > 0 ? parentLogical.w : parent->GetClientRect().w;
	// Reserve a larger actual-size preview without pushing fields into the footer.
	if (parentWidth >= 600 && r.w > 160)
	{
		r.w -= 64;
		combo->m_IDLRect = r;
		combo->SetBounds(r);
	}
	auto registerControl = [&](const char* id, MWidget* widget) {
		strcpy_safe(widget->m_szIDLName, id);
		widget->m_IDLRect = widget->GetRect();
		InsertWidget(id, widget);
	};
	const int gap = std::max(2, r.h / 4);
	// Compact rows leave room for all controls above the existing Save/Cancel buttons.
	const int row = r.h;
	const int labelX = std::max(2, r.h / 12);
	auto* editLook = static_cast<MEdit*>(FindWidget("NetworkPort1"));

	auto addLabel = [&](const char* id, const char* text, int y) {
		auto* label = new MLabel(text, parent);
		label->SetFont(combo->GetFont());
		label->SetBounds(labelX, y, r.x - labelX - gap, r.h);
		registerControl(id, label);
	};
	auto addEdit = [&](const char* id, int maxLength, int x, int y, int width, MListener* listener = nullptr) {
		auto* edit = new MEdit(maxLength, "", parent, listener);
		edit->SetFont(combo->GetFont());
		if (editLook) edit->ChangeCustomLook(editLook->GetLook());
		edit->SetBounds(x, y, width, r.h);
		registerControl(id, edit);
	};

	struct CrossHairListener : MListener
	{
		u32 previousColor = 0xFFFFFF;
		bool syncingColor = false;
		void SyncColor(ZIDLResource* resource, u32 color, bool writeHex = true)
		{
			syncingColor = true;
			char text[16];
			if (writeHex)
			{
				sprintf_safe(text, "%06X", color & 0xFFFFFF);
				static_cast<MEdit*>(resource->FindWidget("CrossHairColorOption"))->SetText(text);
			}
			const char* sliders[] = {"CrossHairRedSlider", "CrossHairGreenSlider", "CrossHairBlueSlider"};
			const char* values[] = {"CrossHairRedValue", "CrossHairGreenValue", "CrossHairBlueValue"};
			for (int i = 0; i < 3; ++i)
			{
				const int channel = (color >> (16 - 8 * i)) & 255;
				static_cast<MSlider*>(resource->FindWidget(sliders[i]))->SetValue(channel);
				sprintf_safe(text, "%d", channel);
				static_cast<MEdit*>(resource->FindWidget(values[i]))->SetText(text);
			}
			syncingColor = false;
		}
		bool OnCommand(MWidget* sender, const char* message) override
		{
			auto* resource = ZGetGameInterface()->GetIDLResource();
			auto* hex = static_cast<MEdit*>(resource->FindWidget("CrossHairColorOption"));
			auto* picker = resource->FindWidget("CrossHairColorPicker");
			if (hex && picker && !syncingColor)
			{
				const bool click = MWidget::IsMsg(message, MBTN_CLK_MSG);
				const u32 current = CrossHairSettings::ParseColor(hex->GetText(),
					ZGetConfiguration()->GetCrossHairColor());
				if (click && sender == resource->FindWidget("CrossHairColorButton"))
				{
					previousColor = current;
					SyncColor(resource, current);
					picker->Show(true, true);
					return true;
				}
				if (click && (sender == resource->FindWidget("CrossHairColorAccept") ||
					sender == resource->FindWidget("CrossHairColorCancel")))
				{
					SyncColor(resource, sender == resource->FindWidget("CrossHairColorCancel") ?
						previousColor : current);
					picker->Show(false);
					return true;
				}
				const char* swatches[] = {"CrossHairSwatchWhite", "CrossHairSwatchGreen", "CrossHairSwatchCyan",
					"CrossHairSwatchYellow", "CrossHairSwatchRed", "CrossHairSwatchPink"};
				for (int i = 0; i < 6; ++i)
					if (click && sender == resource->FindWidget(swatches[i]))
					{
						SyncColor(resource, CrossHairSettings::PresetColor(i));
						return true;
					}
				if (sender == hex && MWidget::IsMsg(message, MEDIT_CHAR_MSG))
				{
					SyncColor(resource, current, false);
					return true;
				}
				const char* sliders[] = {"CrossHairRedSlider", "CrossHairGreenSlider", "CrossHairBlueSlider"};
				const char* values[] = {"CrossHairRedValue", "CrossHairGreenValue", "CrossHairBlueValue"};
				for (int i = 0; i < 3; ++i)
				{
					auto* channel = static_cast<MSlider*>(resource->FindWidget(sliders[i]));
					auto* value = static_cast<MEdit*>(resource->FindWidget(values[i]));
					const bool fromSlider = sender == channel && MWidget::IsMsg(message, MLIST_VALUE_CHANGED);
					const bool fromEdit = sender == value && MWidget::IsMsg(message, MEDIT_CHAR_MSG);
					if (fromSlider || fromEdit)
					{
						const int amount = fromSlider ? channel->GetValue() :
							CrossHairSettings::ParseChannel(value->GetText(), channel->GetValue());
						const int shift = 16 - 8 * i;
						SyncColor(resource, (current & ~(255u << shift)) | (u32(amount) << shift));
						return true;
					}
				}
			}

			auto* distance = static_cast<MEdit*>(resource->FindWidget("CameraDistanceOption"));
			auto* slider = static_cast<MSlider*>(resource->FindWidget("CameraDistanceSlider"));
			if (distance && slider)
			{
				if (sender == slider && MWidget::IsMsg(message, MLIST_VALUE_CHANGED))
				{
					char text[16];
					sprintf_safe(text, "%d", slider->GetValue());
					distance->SetText(text);
					return true;
				}
				if (sender == distance && MWidget::IsMsg(message, MEDIT_CHAR_MSG))
				{
					slider->SetValue(CameraSettings::ParseDistance(distance->GetText(),
						ZGetConfiguration()->GetCameraDistance()));
					return true;
				}
				if (sender == resource->FindWidget("CameraDistanceResetOption") &&
					MWidget::IsMsg(message, MBTN_CLK_MSG))
				{
					char text[16];
					sprintf_safe(text, "%d", CameraSettings::DefaultDistance);
					distance->SetText(text);
					slider->SetValue(CameraSettings::DefaultDistance);
					return true;
				}
			}
			if (!MWidget::IsMsg(message, MBTN_CLK_MSG)) return false;
			auto* size = static_cast<MEdit*>(resource->FindWidget("CrossHairSizeOption"));
			auto* color = static_cast<MEdit*>(resource->FindWidget("CrossHairColorOption"));
			if (!size || !color) return false;
			if (sender == resource->FindWidget("CrossHairResetOption"))
			{
				size->SetText("1.00");
				color->SetText("FFFFFF");
				return true;
			}
			const bool decrease = sender == resource->FindWidget("CrossHairSmallerOption");
			if (!decrease && sender != resource->FindWidget("CrossHairLargerOption")) return false;
			const float value = CrossHairSettings::ParseSize(size->GetText(),
				ZGetConfiguration()->GetCrossHairSize());
			char text[32];
			sprintf_safe(text, "%.2f", CrossHairSettings::ClampSize(value + (decrease ? -0.05f : 0.05f)));
			size->SetText(text);
			return true;
		}
	};
	static CrossHairListener listener;
	auto addButton = [&](const char* id, const char* text, int x, int y, int width) {
		auto* button = new MButton(text, parent, &listener);
		button->SetFont(combo->GetFont());
		button->SetBounds(x, y, width, r.h);
		registerControl(id, button);
	};

	addLabel("CrossHairSizeLabel", "Crosshair size", r.y + row);
	addLabel("CrossHairColorLabel", "Crosshair color", r.y + 2 * row);
	addButton("CrossHairSmallerOption", "-", r.x, r.y + row, r.h);
	addEdit("CrossHairSizeOption", 16, r.x + r.h + gap, r.y + row, r.w - 2 * (r.h + gap));
	addButton("CrossHairLargerOption", "+", r.x + r.w - r.h, r.y + row, r.h);
	addButton("CrossHairColorButton", "Choose...", r.x, r.y + 2 * row, r.w - 3 * r.h - gap);
	// Reserve a separate preview column; Reset shares the color row.
	const auto previewBounds = FindWidget("CrossHairPreviewCanvas") ?
		FindWidget("CrossHairPreviewCanvas")->GetIDLRect() : MRECT(0, 0, 48, 48);
	const int sideWidth = std::min(std::max(previewBounds.w, 120), parentWidth - r.x - r.w - 2 * gap);
	const int sideX = parentWidth - sideWidth - gap;
	addButton("CrossHairResetOption", "Reset", r.x + r.w - 3 * r.h, r.y + 2 * row, 3 * r.h);
	// An in-game modal keeps custom color selection inside the client.
	if (auto* overlay = FindWidget("Option"))
	{
		auto* picker = new MFrame("Crosshair color", overlay, &listener);
		picker->SetMovable(false);
		picker->SetResizable(false);
		picker->SetBounds(0, 0, 640, 480);
		registerControl("CrossHairColorPicker", picker);
		auto label = [&](const char* id, const char* text, int x, int y, int w) {
			auto* widget = new MLabel(text, picker);
			widget->SetBounds(x, y, w, 32);
			registerControl(id, widget);
		};
		auto button = [&](const char* id, const char* text, int x, int y, int w, int h = 32) {
			auto* widget = new MButton(text, picker, &listener);
			widget->SetBounds(x, y, w, h);
			registerControl(id, widget);
		};
		label("CrossHairColorHint", "Choose a swatch or adjust a custom color", 24, 72, 592);
		const char* names[] = {"White", "Green", "Cyan", "Yellow", "Red", "Pink"};
		const char* ids[] = {"CrossHairSwatchWhite", "CrossHairSwatchGreen", "CrossHairSwatchCyan",
			"CrossHairSwatchYellow", "CrossHairSwatchRed", "CrossHairSwatchPink"};
		for (int i = 0; i < 6; ++i) button(ids[i], names[i], 24 + i * 100, 116, 88, 40);
		const char* labels[] = {"Red", "Green", "Blue"};
		const char* labelIDs[] = {"CrossHairRedLabel", "CrossHairGreenLabel", "CrossHairBlueLabel"};
		const char* sliders[] = {"CrossHairRedSlider", "CrossHairGreenSlider", "CrossHairBlueSlider"};
		const char* values[] = {"CrossHairRedValue", "CrossHairGreenValue", "CrossHairBlueValue"};
		for (int i = 0; i < 3; ++i)
		{
			const int y = 184 + i * 52;
			label(labelIDs[i], labels[i], 24, y, 72);
			auto* slider = new ZSettingsSlider("", picker, &listener);
			slider->SetMinMax(0, 255);
			slider->SetBounds(104, y, 224, 32);
			registerControl(sliders[i], slider);
			auto* edit = new MEdit(3, "", picker, &listener);
			if (editLook) edit->ChangeCustomLook(editLook->GetLook());
			edit->SetBounds(340, y, 60, 32);
			registerControl(values[i], edit);
		}
		label("CrossHairHexLabel", "Hex", 24, 356, 72);
		auto* hex = new MEdit(7, "FFFFFF", picker, &listener);
		if (editLook) hex->ChangeCustomLook(editLook->GetLook());
		hex->SetBounds(104, 356, 168, 32);
		registerControl("CrossHairColorOption", hex);
		label("CrossHairColorPreviewLabel", "Actual-size preview", 424, 328, 192);
		auto* preview = new ZCanvas("", picker);
		preview->SetBounds(424, 176, 192, 144);
		preview->SetOnDrawCallback(ZCrossHair::OnDrawOptionCrossHairPreview);
		registerControl("CrossHairColorPreview", preview);
		button("CrossHairColorCancel", "Cancel", 368, 432, 112);
		button("CrossHairColorAccept", "Use color", 496, 432, 120);
		picker->Show(false);
	}


	// Put distance alongside the existing field-of-view control, in its own tab.
	if (auto* fov = FindWidget("FOVOption"))
	{
		auto* cameraParent = fov->GetParent();
		if (!cameraParent) return;
		const auto anchor = fov->GetIDLRect();
		const auto area = cameraParent->GetIDLRect();
		const int height = std::max(24, anchor.h);
		const int spacing = std::max(6, height / 4);
		const int width = area.w;
		// Find a full free row below FOV rather than overwrite another skin control.
		int y = anchor.y + anchor.h + spacing;
		for (bool collision = true; collision;)
		{
			collision = false;
			for (int i = 0; i < cameraParent->GetChildCount(); ++i)
			{
				const auto occupied = cameraParent->GetChild(i)->GetIDLRect();
				if (occupied.w > 0 && occupied.h > 0 && occupied.y < y + height &&
					occupied.y + occupied.h > y)
				{
					y = occupied.y + occupied.h + spacing;
					collision = true;
				}
			}
		}
		if (width >= 400 && y + height <= area.h)
		{
			const int labelWidth = width / 3;
			const int valueWidth = 3 * height;
			const int resetWidth = 3 * height;
			const int sliderX = labelWidth + spacing;
			const int resetX = width - resetWidth - spacing;
			const int valueX = resetX - spacing - valueWidth;
			auto* label = new MLabel("Camera distance", cameraParent);
			label->SetBounds(spacing, y, labelWidth - spacing, height);
			registerControl("CameraDistanceLabel", label);
			auto* slider = new ZSettingsSlider("", cameraParent, &listener);
			slider->SetMinMax(CameraSettings::MinDistance, CameraSettings::MaxDistance);
			slider->SetBounds(sliderX, y, valueX - spacing - sliderX, height);
			registerControl("CameraDistanceSlider", slider);
			auto* value = new MEdit(8, "", cameraParent, &listener);
			if (editLook) value->ChangeCustomLook(editLook->GetLook());
			value->SetBounds(valueX, y, valueWidth, height);
			registerControl("CameraDistanceOption", value);
			auto* reset = new MButton("Reset", cameraParent, &listener);
			reset->SetBounds(resetX, y, resetWidth, height);
			registerControl("CameraDistanceResetOption", reset);
		}
	}

	// Show the preview at actual size across the height of the crosshair section.
	if (auto* preview = FindWidget("CrossHairPreviewCanvas"))
	{
		auto bounds = previewBounds;
		bounds.w = sideWidth;
		bounds.h = 3 * row - gap;
		// Reset shares the color row, leaving the preview its full height.
		bounds.x = sideX;
		bounds.y = r.y;
		preview->m_IDLRect = bounds;
		preview->SetBounds(bounds);
	}
}


namespace
{
void ReadableChildren(MWidget* widget)
{
    if (!widget) return;
    widget->SetFont(ZUiDesign::Font());
    const char* type = widget->GetClassName();
    if (!strcmp(type, "TextArea"))
        static_cast<MTextArea*>(widget)->SetLineHeight(ZUiDesign::Pixels(22));
    else if (!strcmp(type, "ListBox") || !strcmp(type, "ComboListBox"))
        static_cast<MListBox*>(widget)->SetItemHeight(ZUiDesign::Pixels(28));
    for (int i = 0; i < widget->GetChildCount(); ++i)
        ReadableChildren(widget->GetChild(i));
}
}

void ZIDLResource::ApplyInterfaceReadability()
{
    // These roots use the legacy 800x600 rectangle resize pass. Give their
    // text an independent, DPI-neutral pixel size derived from the viewport.
    // Options and the escape menu own their fonts and are not traversed here.
    for (const char* name : {"Lobby", "Stage"})
        ReadableChildren(FindWidget(name));
    LayoutChannelList();
}

void ZIDLResource::LayoutChannelList()
{
    auto* frame = static_cast<MFrame*>(FindWidget("ChannelListFrame"));
    if (!frame) return;
    const ZUiLayout::Channel layout(MGetWorkspaceWidth(), MGetWorkspaceHeight());
    frame->ChangeCustomLook(ZUiDesign::Panel());
    frame->SetBounds(layout.x, layout.y, layout.width, layout.height);
    ReadableChildren(frame);
    auto place = [this](const char* id, int x, int y, int w, int h, bool button = false)
    {
        auto* child = FindWidget(id);
        if (!child) return;
        child->SetBounds(ZUiDesign::Pixels(x), ZUiDesign::Pixels(y),
            ZUiDesign::Pixels(w), ZUiDesign::Pixels(h));
        if (button) static_cast<MButton*>(child)->ChangeCustomLook(ZUiDesign::Button());
    };
    place("ChannelList_Normal", 24, 72, 140, 40, true);
    place("ChannelList_Private", 24, 124, 140, 40, true);
    place("ChannelList_Clan", 24, 176, 140, 40, true);
    place("MyClanChannel", 24, 240, 140, 40, true);
    place("ChannelList", 184, 72, 432, 280);
    place("PrivateChannelInput", 184, 368, 280, 36);
    place("PrivateChannelEnter", 476, 368, 140, 36, true);
    place("ChannelJoin", 324, 420, 140, 40, true);
    place("ChannelListClose", 476, 420, 140, 40, true);
    if (auto* list = static_cast<MListBox*>(FindWidget("ChannelList")))
    {
        list->SetItemHeight(ZUiDesign::Pixels(32));
        // Channel names occupy the full content width, without a legacy header.
        list->SetVisibleHeader(false);
    }
}

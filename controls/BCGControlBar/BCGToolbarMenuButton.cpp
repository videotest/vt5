//*******************************************************************************
// COPYRIGHT NOTES
// ---------------
// This source code is a part of BCGControlBar library.
// You may use, compile or redistribute it as part of your application 
// for free. You cannot redistribute it as a part of a software development 
// library without the agreement of the author. If the sources are 
// distributed along with the application, you should leave the original 
// copyright notes in the source code without any changes.
// This code can be used WITHOUT ANY WARRANTIES on your own risk.
// 
// For the latest updates to this library, check my site:
// http://welcome.to/bcgsoft
// 
// Stas Levin <bcgsoft@yahoo.com>
//*******************************************************************************

// BCGToolbarMenuButton.cpp: implementation of the CBCGToolbarMenuButton class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "bcgbarres.h"
#include "BCGToolbarMenuButton.h"
#include "BCGMenuBar.h"
#include "BCGPopupMenuBar.h"
#include "BCGCommandManager.h"
#include "globals.h"
#include "BCGKeyboardManager.h"

#include "BCGFrameWnd.h"
#include "BCGMDIFrameWnd.h"
#include "BCGOleIPFrameWnd.h"

//#include "MenuImages.h"
#include "BCGUserToolsManager.h"
#include "BCGUserTool.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static const int SEPARATOR_SIZE = 2;
static const int IMAGE_MARGIN = 2;

IMPLEMENT_SERIAL(CBCGToolbarMenuButton, CBCGToolbarButton, VERSIONABLE_SCHEMA | 1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBCGToolbarMenuButton::CBCGToolbarMenuButton()
{
	Initialize ();
}
//*****************************************************************************************
CBCGToolbarMenuButton::CBCGToolbarMenuButton (UINT uiID, HMENU hMenu, 
								int iImage, LPCTSTR lpszText, BOOL bUserButton)
{
	Initialize ();

	m_nID = uiID;
	m_bUserButton = bUserButton;

	SetImage (iImage);
	m_strText = (lpszText == NULL) ? _T("") : lpszText;

	CreateFromMenu (hMenu);
}
//*****************************************************************************************
void CBCGToolbarMenuButton::Initialize ()
{
//	m_bDrawAccel = true;
	m_bDrawDownArrow = FALSE;
	m_bMenuMode = FALSE;
	m_pPopupMenu = NULL;
	m_pWndParent = NULL;
	m_bDefault = FALSE;
	m_bClickedOnMenu = FALSE;
	m_bHorz = TRUE;
}
//*****************************************************************************************
CBCGToolbarMenuButton::CBCGToolbarMenuButton (const CBCGToolbarMenuButton& src)
{
	m_nID = src.m_nID;
	m_nStyle = src.m_nStyle;
	m_bUserButton = src.m_bUserButton;

	SetImage (src.GetImage ());
	m_strText = src.m_strText;
	m_bDragFromCollection = FALSE;
	m_bText = src.m_bText;
	m_bImage = src.m_bImage;
	m_bDrawDownArrow = src.m_bDrawDownArrow;
	m_bMenuMode = src.m_bMenuMode;
	m_bDefault = src.m_bDefault;

	HMENU hmenu = src.CreateMenu ();
	ASSERT (hmenu != NULL);

	CreateFromMenu (hmenu);
	::DestroyMenu (hmenu);

	m_rect.SetRectEmpty ();

	m_pPopupMenu = NULL;
	m_pWndParent = NULL;

	m_bClickedOnMenu = FALSE;
	m_bHorz = TRUE;
}
//*****************************************************************************************
CBCGToolbarMenuButton::~CBCGToolbarMenuButton()
{
	if (m_pPopupMenu != NULL)
	{
		m_pPopupMenu->m_pParentBtn = NULL;
	}

	while (!m_listCommands.IsEmpty ())
	{
		delete m_listCommands.RemoveHead ();
	}
}

//////////////////////////////////////////////////////////////////////
// Overrides:

extern CRuntimeClass *g_pclassToolbarMenuButtonRuntime;
void CBCGToolbarMenuButton::CopyFrom (const CBCGToolbarButton& s)
{
	CBCGToolbarButton::CopyFrom (s);

	const CBCGToolbarMenuButton& src = (const CBCGToolbarMenuButton&) s;

	m_bDefault = src.m_bDefault;

	while (!m_listCommands.IsEmpty ())
	{
		delete m_listCommands.RemoveHead ();
	}

	for (POSITION pos = src.m_listCommands.GetHeadPosition (); pos != NULL;)
	{
		CBCGToolbarMenuButton* pItem = (CBCGToolbarMenuButton*) src.m_listCommands.GetNext (pos);
		ASSERT (pItem != NULL);
		ASSERT_KINDOF (CBCGToolbarMenuButton, pItem);

		CRuntimeClass* pSrcClass = pItem->GetRuntimeClass ();
		ASSERT (pSrcClass != NULL);

		// [vanek] : создаем кнопку в соответствии с установленным классом g_pclassToolbarMenuButtonRuntime - 10.11.2004
		if( pSrcClass == RUNTIME_CLASS( CBCGToolbarMenuButton ) )
			pSrcClass = g_pclassToolbarMenuButtonRuntime;

		CBCGToolbarMenuButton* pNewItem = (CBCGToolbarMenuButton*) pSrcClass->CreateObject ();
		ASSERT (pNewItem != NULL);
		ASSERT_KINDOF (CBCGToolbarMenuButton, pNewItem);

		pNewItem->CopyFrom (*pItem);
		m_listCommands.AddTail (pNewItem);
	}

//	m_nID = 0;	?????
}
//*****************************************************************************************
void CBCGToolbarMenuButton::Serialize (CArchive& ar)
{
	OnCancelMode ();
	CBCGToolbarButton::Serialize (ar);

	if (ar.IsLoading ())
	{
		while (!m_listCommands.IsEmpty ())
		{
			delete m_listCommands.RemoveHead ();
		}
	}

	m_listCommands.Serialize (ar);
}
//*****************************************************************************************
void CBCGToolbarMenuButton::OnDraw (CDC* pDC, const CRect& rect, CBCGToolBarImages* pImages,
			BOOL bHorz, BOOL bCustomizeMode, BOOL bHighlight,
			BOOL bDrawBorder, BOOL bGrayDisabledButtons)
{
	if (m_bMenuMode)
	{
		DrawMenuItem (pDC, rect, pImages, bCustomizeMode, bHighlight, bGrayDisabledButtons);
		return;
	}

	CSize sizeImage = CMenuImages::Size ();

	if (!bCustomizeMode && !bHighlight &&
		(m_nStyle & (TBBS_CHECKED | TBBS_INDETERMINATE)))
	{
		CRect rectDither = rect;
		rectDither.InflateRect (-afxData.cxBorder2, -afxData.cyBorder2);

		CBCGToolBarImages::FillDitheredRect (pDC, rectDither);
	}

	CRect rectParent = rect;
	CRect rectArrow = rect;

	if (m_bDrawDownArrow)
	{
		if (bHorz)
		{
			rectParent.right -= sizeImage.cx + SEPARATOR_SIZE;
			rectArrow.left = rectParent.right - 1;	// By Sven Ritter
		}
		else
		{
			rectParent.bottom -= sizeImage.cy + SEPARATOR_SIZE;
			rectArrow.top = rectParent.bottom - 1;	// By Sven Ritter
		}
	}

	UINT uiStyle = m_nStyle;

	if (m_bClickedOnMenu && m_nID != 0 && m_nID != (UINT) -1)
	{
		m_nStyle &= ~TBBS_PRESSED;
	}
	else if (m_pPopupMenu != NULL)
	{
		m_nStyle |= TBBS_PRESSED;
	}

	CBCGToolbarButton::OnDraw (pDC, rectParent, pImages, bHorz, 
			bCustomizeMode, bHighlight, bDrawBorder, bGrayDisabledButtons);

	if (m_bDrawDownArrow)
	{
		if (m_nStyle & (TBBS_PRESSED | TBBS_CHECKED))
		{
			rectArrow.OffsetRect (1, 1);
		}

		if ((bHighlight || (m_nStyle & TBBS_PRESSED) ||
			m_pPopupMenu != NULL) &&
			m_nID != 0 && m_nID != (UINT) -1)
		{
			//----------------
			// Draw separator:
			//----------------
			CRect rectSeparator = rectArrow;

			if (bHorz)
			{
				rectSeparator.right = rectSeparator.left + SEPARATOR_SIZE;
			}
			else
			{
				rectSeparator.bottom = rectSeparator.top + SEPARATOR_SIZE;
			}

			pDC->Draw3dRect (rectSeparator, ::GetSysColor (COLOR_3DSHADOW),
											::GetSysColor (COLOR_3DHILIGHT));
		}

		BOOL bDisabled = (bCustomizeMode && !IsEditable ()) ||
			(!bCustomizeMode && (m_nStyle & TBBS_DISABLED));

		int iImage;
		if (bHorz)
		{
			iImage = (bDisabled) ? CMenuImages::IdArowDownDsbl : CMenuImages::IdArowDown;
		}
		else
		{
			iImage = (bDisabled) ? CMenuImages::IdArowLeftDsbl : CMenuImages::IdArowLeft;
		}

		if (m_pPopupMenu != NULL &&
			(m_nStyle & (TBBS_PRESSED | TBBS_CHECKED)) == 0)
		{
			rectArrow.OffsetRect (1, 1);
		}

		CPoint pointImage (
			rectArrow.left + (rectArrow.Width () - sizeImage.cx) / 2,
			rectArrow.top + (rectArrow.Height () - sizeImage.cy) / 2);

		CMenuImages::Draw (pDC, (CMenuImages::IMAGES_IDS) iImage, pointImage);
	}

	if (!bCustomizeMode)
	{
		if ((m_nStyle & (TBBS_PRESSED | TBBS_CHECKED)) ||
			m_pPopupMenu != NULL)
		{
			//-----------------------
			// Pressed in or checked:
			//-----------------------
			if (m_bClickedOnMenu && m_nID != 0 && m_nID != (UINT) -1)
			{
				pDC->Draw3dRect (&rectParent,
					globalData.clrBtnHilite,
					globalData.clrBtnShadow);

				rectArrow.right --;
				rectArrow.bottom --;

				pDC->Draw3dRect (&rectArrow,
					globalData.clrBtnShadow,
					globalData.clrBtnHilite);
			}
			else
			{
				pDC->Draw3dRect (&rect,
					globalData.clrBtnShadow,
					globalData.clrBtnHilite);
			}
		}
		else if (bHighlight && !(m_nStyle & TBBS_DISABLED) &&
			!(m_nStyle & (TBBS_CHECKED | TBBS_INDETERMINATE)))
		{
			pDC->Draw3dRect (&rect, globalData.clrBtnHilite,
									globalData.clrBtnShadow);
		}
	}

	m_nStyle = uiStyle;
}
//*****************************************************************************************
SIZE CBCGToolbarMenuButton::OnCalculateSize (CDC* pDC, const CSize& sizeDefault, BOOL bHorz)
{
	m_bHorz = bHorz;

	int iArrowSize = 0;

	if (m_bDrawDownArrow || m_bMenuMode)
	{
		iArrowSize = (bHorz) ? 
			CMenuImages::Size ().cx + SEPARATOR_SIZE - TEXT_MARGIN : 
			CMenuImages::Size ().cy + SEPARATOR_SIZE - TEXT_MARGIN;
	}

	//--------------------
	// Change accelerator:
	//--------------------
	if (g_pKeyboardManager != NULL &&
		m_bMenuMode &&
		(m_nID < 0xF000 || m_nID >= 0xF1F0))	// Not system.
	{
		//-----------------------------------
		// Remove standard aceleration label:
		//-----------------------------------
		int iTabOffset = m_strText.Find (_T('\t'));
		if (iTabOffset >= 0)
		{
			m_strText = m_strText.Left (iTabOffset);
		}

		//---------------------------------
		// Add an actual accelartion label:
		//---------------------------------
		CString strAccel;
		CFrameWnd* pParent = m_pWndParent == NULL ?
			DYNAMIC_DOWNCAST (CFrameWnd, AfxGetMainWnd ()) :
			m_pWndParent->GetTopLevelFrame ();

		if (pParent != NULL &&
			(CBCGKeyboardManager::FindDefaultAccelerator (
				m_nID, strAccel, pParent, TRUE) ||
			CBCGKeyboardManager::FindDefaultAccelerator (
				m_nID, strAccel, pParent->GetActiveFrame (), FALSE)))
		{
			m_strText += _T('\t');
			m_strText += strAccel;
		}
	}

	CSize size = CBCGToolbarButton::OnCalculateSize (pDC, sizeDefault, bHorz);
	if (bHorz)
	{	
		size.cx += iArrowSize;
	}
	else
	{
		size.cy += iArrowSize;
	}

	if (m_bMenuMode)
	{
		size.cx += sizeDefault.cx + 2 * TEXT_MARGIN;
	}

	CBCGPopupMenuBar* pParentMenu =
		DYNAMIC_DOWNCAST (CBCGPopupMenuBar, m_pWndParent);
	if (pParentMenu != NULL)
	{
		size.cy = pParentMenu->GetRowHeight ();
	}

	return size;
}
//*****************************************************************************************
BOOL CBCGToolbarMenuButton::OnClick (CWnd* pWnd, BOOL bDelay)
{
	ASSERT_VALID (pWnd);
	
	m_bClickedOnMenu = FALSE;

	if (m_bDrawDownArrow && !bDelay && !m_bMenuMode)
	{
		if (m_nID == 0 || m_nID == (UINT) -1)
		{
			m_bClickedOnMenu = TRUE;
		}
		else
		{
			CPoint ptMouse;
			::GetCursorPos (&ptMouse);
			pWnd->ScreenToClient (&ptMouse);

			CRect rectArrow = m_rect;
			if (m_bHorz)
			{
				if( !IsPopupButton() )
				{
					rectArrow.left = rectArrow.right - 
						CMenuImages::Size ().cx - SEPARATOR_SIZE;
				}
			}
			else
			{
				if( !IsPopupButton() )
				{
					rectArrow.top = rectArrow.bottom - 
						CMenuImages::Size ().cy - SEPARATOR_SIZE;
				}
			}

			m_bClickedOnMenu = rectArrow.PtInRect (ptMouse);
			if (!m_bClickedOnMenu)
			{
				return FALSE;
			}
		}
	}

	if (!m_bClickedOnMenu && m_nID > 0 && m_nID != (UINT) -1 && !m_bDrawDownArrow)
	{
		return FALSE;
	}

	CBCGMenuBar* pMenuBar = DYNAMIC_DOWNCAST (CBCGMenuBar, m_pWndParent);

	if (m_pPopupMenu != NULL)
	{
		//-----------------------------------------------------
		// Second click to the popup menu item closes the menu:
		//-----------------------------------------------------		
		ASSERT_VALID(m_pPopupMenu);

		m_pPopupMenu->m_bAutoDestroyParent = FALSE;
		m_pPopupMenu->DestroyWindow ();
		m_pPopupMenu = NULL;

		if (pMenuBar != NULL)
		{
			pMenuBar->SetHot (NULL);
		}
	}
	else
	{
		CBCGPopupMenuBar* pParentMenu =
			DYNAMIC_DOWNCAST (CBCGPopupMenuBar, m_pWndParent);

		if (bDelay && pParentMenu != NULL && !CBCGToolBar::IsCustomizeMode ())
		{
			pParentMenu->StartPopupMenuTimer (this);
		}
		else
		{
			//------------------------------------
			// Fixed by Sven Ritter (SpeedProject)
			//------------------------------------
			if (pMenuBar != NULL)
			{
				CBCGToolbarMenuButton* pCurrPopupMenuButton = 
					pMenuBar->GetDroppedDownMenu();
				if (pCurrPopupMenuButton != NULL)
				{
					pCurrPopupMenuButton->OnCancelMode ();
				}
			}
			// ***  End *************************
			
			if (!OpenPopupMenu (pWnd))
			{
				return FALSE;
			}
		}

		if (pMenuBar != NULL)
		{
			pMenuBar->SetHot (this);
		}
	}

	if (m_pWndParent != NULL)
	{
		m_pWndParent->InvalidateRect (m_rect);
	}

	return TRUE;
}
//****************************************************************************************
void CBCGToolbarMenuButton::OnChangeParentWnd (CWnd* pWndParent)
{
	if (pWndParent != NULL)
	{
		if (pWndParent->IsKindOf (RUNTIME_CLASS (CBCGMenuBar)))
		{
			m_bDrawDownArrow = FALSE;
			m_bText = TRUE;
			m_bImage = FALSE;
		}
		else
		{
			m_bDrawDownArrow = (m_nID == 0 || !m_listCommands.IsEmpty ());
		}

		if (pWndParent->IsKindOf (RUNTIME_CLASS (CBCGPopupMenuBar)))
		{
			m_bMenuMode = TRUE;
			m_bText = TRUE;
			m_bImage = FALSE;
			m_bDrawDownArrow = (m_nID == 0 || !m_listCommands.IsEmpty ());
		}
		else
		{
			m_bMenuMode = FALSE;
		}
	}

	m_pWndParent = pWndParent;
}
//****************************************************************************************
void CBCGToolbarMenuButton::CreateFromMenu (HMENU hMenu)
{
	while (!m_listCommands.IsEmpty ())
	{
		delete m_listCommands.RemoveHead ();
	}

	CMenu* pMenu = CMenu::FromHandle (hMenu);
	if (pMenu == NULL)
	{
		return;
	}

	UINT uiDefaultCmd = ::GetMenuDefaultItem (hMenu, FALSE, GMDI_USEDISABLED);

	int iCount = (int) pMenu->GetMenuItemCount ();
	for (int i = 0; i < iCount; i ++)
	{
		// [vanek] : создаем кнопку в соответствии с установленным классом g_pclassToolbarMenuButtonRuntime - 10.11.2004
		CBCGToolbarMenuButton* pItem = 0;
		CRuntimeClass *pclass = g_pclassToolbarMenuButtonRuntime;		
		if( pclass == RUNTIME_CLASS(CBCGToolbarMenuButton) )
			pItem = new CBCGToolbarMenuButton;
		else
            pItem = (CBCGToolbarMenuButton*) pclass->CreateObject ();

		ASSERT_VALID (pItem);

		pItem->m_nID = pMenu->GetMenuItemID (i);
		pMenu->GetMenuString (i, pItem->m_strText, MF_BYPOSITION);

		if (pItem->m_nID == -1)	// Sub-menu...
		{
			CMenu* pSubMenu = pMenu->GetSubMenu (i);
			pItem->CreateFromMenu (pSubMenu->GetSafeHmenu ());
		}
		else if (pItem->m_nID == uiDefaultCmd)
		{
			pItem->m_bDefault = TRUE;
		}

		/// By Guy Hachlili - support for the menu with breaks:
		if (pMenu->GetMenuState (i, MF_BYPOSITION) & MF_MENUBREAK)
		{
			pItem->m_nStyle |= TBBS_BREAK;
		}
		/////////////////////////////

		m_listCommands.AddTail (pItem);
	}
}
//****************************************************************************************
HMENU CBCGToolbarMenuButton::CreateMenu () const
{
	if (m_listCommands.IsEmpty () && m_nID != (UINT) -1 && m_nID != 0)
	{
		return NULL;
	}

	CMenu menu;
	if (!menu.CreatePopupMenu ())
	{
		TRACE(_T("CBCGToolbarMenuButton::CreateMenu (): Can't create popup menu!\n"));
		return NULL;
	}

	BOOL bRes = TRUE;
	DWORD dwLastError = 0;

	UINT uiDefaultCmd = (UINT) -1;

	int i = 0;
	for (POSITION pos = m_listCommands.GetHeadPosition (); pos != NULL; i ++)
	{
		CBCGToolbarMenuButton* pItem = (CBCGToolbarMenuButton*) m_listCommands.GetNext (pos);
		ASSERT (pItem != NULL);
		ASSERT_KINDOF (CBCGToolbarButton, pItem);

		UINT uiStyle = MF_STRING;
		/// By Guy Hachlili - support for the menu with breaks:
		if (pItem->m_nStyle & TBBS_BREAK)
		{
			uiStyle |= MF_MENUBREAK;
		}
		///////////////////////////////////////////////////////

		switch (pItem->m_nID)
		{
		case 0:	// Separator
			bRes = menu.AppendMenu (MF_SEPARATOR);
			if (!bRes)
			{
				dwLastError = GetLastError ();
			}
			break;

		case -1:			// Sub-menu
			{
				HMENU hSubMenu = pItem->CreateMenu ();
				ASSERT (hSubMenu != NULL);

				bRes = menu.AppendMenu (uiStyle | MF_POPUP, 
										(UINT) hSubMenu, pItem->m_strText);
				if (!bRes)
				{
					dwLastError = GetLastError ();
				}

				//--------------------------------------------------------
				// This is incompatibility between Windows 95 and 
				// NT API! (IMHO). CMenu::AppendMenu with MF_POPUP flag 
				// COPIES sub-menu resource under the Win NT and 
				// MOVES sub-menu under Win 95/98 and 2000!
				//--------------------------------------------------------
				if (globalData.bIsWindowsNT4)
				{
					::DestroyMenu (hSubMenu);
				}
			}
			break;

		default:
			if (pItem->m_bDefault)
			{
				uiDefaultCmd = pItem->m_nID;
			}

			bRes = menu.AppendMenu (uiStyle, pItem->m_nID, pItem->m_strText);
			if (!bRes)
			{
				dwLastError = GetLastError ();
			}
		}

		if (!bRes)
		{
			TRACE(_T("CBCGToolbarMenuButton::CreateMenu (): Can't add menu item: %d\n Last error = %x\n"), pItem->m_nID, dwLastError);
			return NULL;
		}
	}

	HMENU hMenu = menu.Detach ();
	if (uiDefaultCmd != (UINT)-1)
	{
		::SetMenuDefaultItem (hMenu, uiDefaultCmd, FALSE);
	}

	return hMenu;
}
//*****************************************************************************************
void CBCGToolbarMenuButton::DrawMenuItem (CDC* pDC, const CRect& rect, CBCGToolBarImages* pImages, 
					BOOL bCustomizeMode, BOOL bHighlight, BOOL bGrayDisabledButtons)
{
	ASSERT_VALID (pDC);
	ASSERT_VALID (this);

	CBCGUserTool* pUserTool = NULL;
	if (g_pUserToolsManager != NULL && !m_bUserButton)
	{
		pUserTool = g_pUserToolsManager->FindTool (m_nID);
	}

	CSize sizeImage = CMenuImages::Size ();

	BOOL bDisabled = (bCustomizeMode && !IsEditable ()) ||
		(!bCustomizeMode && (m_nStyle & TBBS_DISABLED));

	if (m_pPopupMenu != NULL)
	{
		bHighlight = TRUE;
	}

	CFont* pOldFont = NULL;

	CBCGPopupMenuBar* pParentMenu =
		DYNAMIC_DOWNCAST (CBCGPopupMenuBar, m_pWndParent);
	if (m_nID != 0 && m_nID != (UINT) -1 &&
		pParentMenu != NULL && pParentMenu->GetDefaultMenuId () == m_nID)
	{
		pOldFont = (CFont*) pDC->SelectObject (&globalData.fontBold);
	}

	CRect rectImage;
	rectImage = rect;
	rectImage.left += IMAGE_MARGIN;
	rectImage.right = rectImage.left + CBCGToolBar::GetMenuImageSize ().cx + IMAGE_MARGIN;

	CRect rectFrameBtn = rectImage;
	rectFrameBtn.InflateRect (1, -1);

	BOOL bIsRarelyUsed = (CBCGMenuBar::IsRecentlyUsedMenus () && 
		CBCGToolBar::IsCommandRarelyUsed (m_nID));
	
	if (bIsRarelyUsed)
	{
		bIsRarelyUsed = FALSE;

		CBCGPopupMenuBar* pParentMenuBar =
			DYNAMIC_DOWNCAST (CBCGPopupMenuBar, m_pWndParent);

		if (pParentMenuBar != NULL)
		{
			CBCGPopupMenu* pParentMenu = DYNAMIC_DOWNCAST (CBCGPopupMenu, 
				pParentMenuBar->GetParent ());
			if (pParentMenu != NULL && pParentMenu->HideRarelyUsedCommands ())
			{
				bIsRarelyUsed = TRUE;
			}
		}
	}

	BOOL bLightImage = FALSE;

	if (bIsRarelyUsed)
	{
		bLightImage = TRUE;
		if (bHighlight && (m_nStyle & (TBBS_CHECKED | TBBS_INDETERMINATE)))
		{
			bLightImage = FALSE;
		}

		if (GetImage () < 0 && !(m_nStyle & (TBBS_CHECKED | TBBS_INDETERMINATE)))
		{
			bLightImage = FALSE;
		}
	}
	else if (m_nStyle & (TBBS_CHECKED | TBBS_INDETERMINATE))
	{
		bLightImage = !bHighlight;
	}

	//---------------------
	// Fill "checked" area:
	//---------------------
	if (bLightImage)
	{
		CBCGToolBarImages::FillDitheredRect (pDC, rectFrameBtn);
	}

	//----------------
	// Draw the image:
	//----------------
	if (!IsDrawImage ())	// Try to find a matched image
	{
		BOOL bImageSave = m_bImage;
		BOOL bUserButton = m_bUserButton;
		BOOL bSuccess = TRUE;

		m_bImage = TRUE;	// Always try to draw image!
		m_bUserButton = TRUE;

		if (GetImage () < 0)
		{
			m_bUserButton = FALSE;

			if (GetImage () < 0)
			{
				bSuccess = FALSE;
			}
		}

		if (!bSuccess)
		{
			m_bImage = bImageSave;
			m_bUserButton = bUserButton;
		}
	}

	BOOL bImageIsReady = FALSE;

	CRgn rgnClip;
	rgnClip.CreateRectRgnIndirect (&rectImage);

	if (IsDrawImage () && pImages != NULL)
	{
		pDC->SelectObject (&rgnClip);

		CPoint ptImageOffset (
			(rectImage.Width () - CBCGToolBar::GetMenuImageSize ().cx) / 2, 
			(rectImage.Height () - CBCGToolBar::GetMenuImageSize ().cy) / 2);

		BOOL bPressed = FALSE;
		if ((m_nStyle & TBBS_CHECKED) && !bCustomizeMode)
		{
			ptImageOffset.x ++;
			ptImageOffset.y ++;
			bPressed = TRUE;
		}

		if ((m_nStyle & TBBS_PRESSED) || !(m_nStyle & TBBS_DISABLED) ||
						bCustomizeMode)
		{
			if (pUserTool != NULL)
			{
				pUserTool->DrawToolIcon (pDC, 
					CRect (CPoint (rectImage.left + ptImageOffset.x, 
							rectImage.top + ptImageOffset.y),
					CBCGToolBar::GetMenuImageSize ()));
			}
			else
			{
				pImages->Draw (pDC, 
					rectImage.left + ptImageOffset.x, rectImage.top + ptImageOffset.y, 
					GetImage ());
			}

			if (bCustomizeMode || m_nStyle & TBBS_PRESSED)
			{
				bImageIsReady = TRUE;
			}
		}

		if (!bImageIsReady)
		{
			if (pUserTool != NULL)
			{
				pUserTool->DrawToolIcon (pDC, 
					CRect (CPoint (rectImage.left + ptImageOffset.x, 
							rectImage.top + ptImageOffset.y),
					CBCGToolBar::GetMenuImageSize ()));
			}
			else
			{
				pImages->Draw (pDC, 
					rectImage.left + ptImageOffset.x, rectImage.top + ptImageOffset.y, 
					GetImage (), bLightImage, bDisabled && bGrayDisabledButtons);
			}

			bImageIsReady = TRUE;
		}
	}
	else
	{
		CFrameWnd* pParentFrame = m_pWndParent == NULL ?
			DYNAMIC_DOWNCAST (CFrameWnd, AfxGetMainWnd ()) :
			m_pWndParent->GetTopLevelFrame ();

		//------------------------------------
		// Get chance to user draw menu image:
		//------------------------------------
		CBCGMDIFrameWnd* pMainFrame = DYNAMIC_DOWNCAST (CBCGMDIFrameWnd, pParentFrame);
		if (pMainFrame != NULL)
		{
			bImageIsReady = pMainFrame->OnDrawMenuImage (pDC, this, rectImage);
		}
		else	// Maybe, SDI frame...
		{
			CBCGFrameWnd* pFrame = DYNAMIC_DOWNCAST (CBCGFrameWnd, pParentFrame);
			if (pFrame != NULL)
			{
				bImageIsReady = pFrame->OnDrawMenuImage (pDC, this, rectImage);
			}
			else	// Maybe, OLE frame...
			{
				CBCGOleIPFrameWnd* pOleFrame = 
					DYNAMIC_DOWNCAST (CBCGOleIPFrameWnd, pParentFrame);
				if (pOleFrame != NULL)
				{
					bImageIsReady = pOleFrame->OnDrawMenuImage (pDC, this, rectImage);
				}
			}
		}
	}

	pDC->SelectClipRgn (NULL);

	rectImage.right ++;

	if (m_nStyle & TBBS_CHECKED && !bImageIsReady)
	{
		if (!bLightImage && bIsRarelyUsed)
		{
			pDC->FillRect (rectImage, &globalData.brBtnFace);
		}

		CRect rectCheck = rectImage;
		rectCheck.DeflateRect (0, 1);

		CPoint pointCheck (
			rectCheck.left + (rectCheck.Width () - sizeImage.cx) / 2,
			rectCheck.top + (rectCheck.Height () - sizeImage.cy) / 2);

		int iImage = (m_nStyle & TBBS_DISABLED) ? CMenuImages::IdCheckDsbl : CMenuImages::IdCheck;
		CMenuImages::Draw (pDC, (CMenuImages::IMAGES_IDS) iImage, pointCheck);

		pDC->Draw3dRect (rectFrameBtn, globalData.clrBtnShadow, globalData.clrBtnHilite);
	}
	else if (bImageIsReady && bHighlight && !(m_nStyle & TBBS_CHECKED))
	{
		pDC->Draw3dRect (rectFrameBtn, globalData.clrBtnHilite, globalData.clrBtnShadow);
	}
	else if (bImageIsReady && (m_nStyle & TBBS_CHECKED))
	{
		pDC->Draw3dRect (rectFrameBtn, globalData.clrBtnShadow, globalData.clrBtnHilite);
	}

	int iSystemImageId = -1;

	//-------------------------------
	// Try to draw system menu icons:
	//-------------------------------
	if (!bImageIsReady)
	{
		switch (m_nID)
		{
		case SC_MINIMIZE:
			iSystemImageId = bDisabled ? CMenuImages::IdMinimizeDsbl : CMenuImages::IdMinimize;
			break;

		case SC_RESTORE:
			iSystemImageId = bDisabled ? CMenuImages::IdRestoreDsbl : CMenuImages::IdRestore;
			break;

		case SC_CLOSE:
			iSystemImageId = bDisabled ? CMenuImages::IdCloseDsbl : CMenuImages::IdClose;
			break;

		case SC_MAXIMIZE:
			iSystemImageId = bDisabled ? CMenuImages::IdMaximizeDsbl : CMenuImages::IdMaximize;
			break;
		}

		if (iSystemImageId != -1)
		{
			CRect rectSysImage = rectImage;
			rectSysImage.DeflateRect (IMAGE_MARGIN, IMAGE_MARGIN);

			CPoint pointSysImage (
				rectSysImage.left + (rectSysImage.Width () - sizeImage.cx) / 2,
				rectSysImage.top + (rectSysImage.Height () - sizeImage.cy) / 2);

			CMenuImages::Draw (pDC, (CMenuImages::IMAGES_IDS) iSystemImageId, pointSysImage);

			if (bHighlight)
			{
				pDC->Draw3dRect (&rectSysImage, globalData.clrBtnHilite, globalData.clrBtnShadow);
			}
		}
	}

	//-------------------------------
	// Fill text area if highlighted:
	//-------------------------------
	CRect rectText = rect;
	rectText.left = rectFrameBtn.right + IMAGE_MARGIN;

	COLORREF clrText;

	if (bHighlight && !bCustomizeMode)
	{
		CRect rectFill = rectFrameBtn;

		if ((m_nStyle & (TBBS_CHECKED) || bImageIsReady) ||
			iSystemImageId != -1)
		{
			rectFill.left = rectText.left - 1;
		}

		rectFill.right = rect.right - 1;

		pDC->FillRect (&rectFill, &globalData.brHilite);
		clrText =  bDisabled ?
						globalData.clrBtnFace : 
						globalData.clrTextHilite;
	}
	else
	{
		clrText	= bDisabled ?
						globalData.clrGrayedText : 
						globalData.clrBtnText;
	}

	//-------------------------
	// Find acceleration label:
	//-------------------------
	CString strText = m_strText;
	CString strAccel;

	int iTabOffset = m_strText.Find (_T('\t'));
	if (iTabOffset >= 0)
	{
		strText = strText.Left (iTabOffset);
		strAccel = m_strText.Mid (iTabOffset + 1);
	}

	//-----------
	// Draw text:
	//-----------
	rectText.left += TEXT_MARGIN;

	if (!m_bWholeText)
	{
		CString strEllipses (_T("..."));
		while (strText.GetLength () > 0 &&
			pDC->GetTextExtent (strText + strEllipses).cx > rectText.Width ())
		{
			strText = strText.Left (strText.GetLength () - 1);
		}

		strText += strEllipses;
	}

	if (bDisabled && !bHighlight)
	{
		pDC->SetTextColor (globalData.clrBtnHilite);

		CRect rectShft = rectText;
		rectShft.OffsetRect (1, 1);	//andy
		pDC->DrawText (strText, &rectShft, DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
	}

	pDC->SetTextColor (clrText);	//andy
	pDC->DrawText (strText, &rectText, DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);

	//------------------------
	// Draw accelerator label:
	//------------------------
	
	if (!strAccel.IsEmpty ()/* && m_bDrawAccel */)//andy
	{
		CRect rectAccel = rectText;
		rectAccel.right -= TEXT_MARGIN + sizeImage.cx;

		if (bDisabled && !bHighlight)
		{
			pDC->SetTextColor (globalData.clrBtnHilite);

			CRect rectAccelShft = rectAccel;
			rectAccelShft.OffsetRect (1, 1);
			pDC->DrawText (strAccel, &rectAccelShft, DT_SINGLELINE | DT_RIGHT | DT_VCENTER );
		}

		pDC->SetTextColor (clrText);
		pDC->DrawText (strAccel, &rectAccel, DT_SINGLELINE | DT_RIGHT | DT_VCENTER );
	}

	//--------------------------------------------
	// Draw triangle image for the cascade menues:
	//--------------------------------------------
	if (m_nID == (UINT) -1 || m_bDrawDownArrow)
	{
		CRect rectTriangle = rect;
		rectTriangle.left = rectTriangle.right - sizeImage.cx;

		CPoint pointTriangle (
			rectTriangle.left + (rectTriangle.Width () - sizeImage.cx) / 2,
			rectTriangle.top + (rectTriangle.Height () - sizeImage.cy) / 2);

		int iImage = (bHighlight && !bCustomizeMode) ?
						(bDisabled ? CMenuImages::IdArowLeftWhiteDsbl : CMenuImages::IdArowLeftWhite) :
						(bDisabled ? CMenuImages::IdArowLeftDsbl : CMenuImages::IdArowLeft);
		CMenuImages::Draw (pDC, (CMenuImages::IMAGES_IDS) iImage, pointTriangle);
	}

	if (pOldFont != NULL)
	{
		pDC->SelectObject (pOldFont);
	}
}
//****************************************************************************************
void CBCGToolbarMenuButton::OnCancelMode ()
{
	if (m_pPopupMenu != NULL && ::IsWindow (m_pPopupMenu->m_hWnd))
	{
		m_pPopupMenu->SaveState ();
		m_pPopupMenu->m_bAutoDestroyParent = FALSE;
		m_pPopupMenu->CloseMenu ();
	}

	m_pPopupMenu = NULL;

	if (m_pWndParent != NULL && ::IsWindow (m_pWndParent->m_hWnd))
	{
		m_pWndParent->InvalidateRect (m_rect);
		m_pWndParent->UpdateWindow ();
	}
}
//****************************************************************************************
BOOL CBCGToolbarMenuButton::OpenPopupMenu (CWnd* pWnd)
{
	if (m_pPopupMenu != NULL)
	{
		return FALSE;
	}

	if (pWnd == NULL)
	{
		pWnd = m_pWndParent;
	}

	ASSERT (pWnd != NULL);

	//---------------------------------------------------------------
	// Define a new menu position. Place the menu in the right side
	// of the current menu in the poup menu case or under the current 
	// item by default:
	//---------------------------------------------------------------
	CPoint point;

	CBCGPopupMenuBar* pParentMenu =
		DYNAMIC_DOWNCAST (CBCGPopupMenuBar, m_pWndParent);

	CBCGMenuBar* pParentMenuBar =
		DYNAMIC_DOWNCAST (CBCGMenuBar, m_pWndParent);

	if (pParentMenu != NULL)
	{
		point = CPoint (0, m_rect.top - 2);
		pWnd->ClientToScreen (&point);

		CRect rectParent;
		pParentMenu->GetWindowRect (rectParent);

		point.x = rectParent.right + 1;
	}
	else if (pParentMenuBar != NULL && 
		(pParentMenuBar->m_dwStyle & CBRS_ORIENT_HORZ) == 0)
	{
		//------------------------------------------------
		// Parent menu bar is docked vertical, place menu 
		// in the left or right side of the parent frame:
		//------------------------------------------------
		point = CPoint (m_rect.right + 1, m_rect.top);
		pWnd->ClientToScreen (&point);
	}
	else
	{
		point = CPoint (m_rect.left - 1, m_rect.bottom);
		pWnd->ClientToScreen (&point);
	}

	HMENU hMenu = CreateMenu ();
	if (hMenu == NULL && !IsEmptyMenuAllowed ())
	{
		return FALSE;
	}

	m_pPopupMenu = CreatePopupMenu ();
	if (m_pPopupMenu == NULL)
	{
		return FALSE;
	}

	if (m_pPopupMenu->GetMenuItemCount () > 0 && hMenu != NULL)
	{
		::DestroyMenu (hMenu);
		hMenu = NULL;
	}

	m_pPopupMenu->m_pParentBtn = this;
	if (!m_pPopupMenu->Create (pWnd, point.x, point.y, hMenu))
	{
		m_pPopupMenu = NULL;
		return FALSE;
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CBCGToolbarMenuButton diagnostics

#ifdef _DEBUG
void CBCGToolbarMenuButton::AssertValid() const
{
	CObject::AssertValid();
}
//******************************************************************************************
void CBCGToolbarMenuButton::Dump(CDumpContext& dc) const
{
	CObject::Dump (dc);

	CString strId;
	strId.Format (_T("%x"), m_nID);

	dc << "[" << m_strText << " >>>>> ]";
	dc.SetDepth (dc.GetDepth () + 1);

	dc << "{\n";
	for (POSITION pos = m_listCommands.GetHeadPosition (); pos != NULL;)
	{
		CBCGToolbarButton* pButton = (CBCGToolbarButton*) m_listCommands.GetNext (pos);
		ASSERT_VALID (pButton);

		pButton->Dump (dc);
		dc << "\n";
	}

	dc << "}\n";
	dc.SetDepth (dc.GetDepth () - 1);
	dc << "\n";
}

#endif

//******************************************************************************************
int CBCGToolbarMenuButton::OnDrawOnCustomizeList (
	CDC* pDC, const CRect& rect, BOOL bSelected)
{
	CBCGToolbarButton::OnDrawOnCustomizeList (pDC, rect, bSelected);

	if (m_nID == 0 || !m_listCommands.IsEmpty ())	// Popup menu
	{
		CRect rectTriangle = rect;
		rectTriangle.left = rectTriangle.right - CMenuImages::Size ().cx;

		CPoint pointTriangle (
			rectTriangle.left + (rectTriangle.Width () - CMenuImages::Size ().cx) / 2,
			rectTriangle.top + (rectTriangle.Height () - CMenuImages::Size ().cy) / 2);

		int iImage = (bSelected) ? CMenuImages::IdArowLeftWhite : CMenuImages::IdArowLeft;
		CMenuImages::Draw (pDC, (CMenuImages::IMAGES_IDS) iImage, pointTriangle);

		CRect rectLine = rect;
		rectLine.right = rectTriangle.left - 1;
		rectLine.left = rectLine.right - 2;
		rectLine.DeflateRect (0, 2);

		pDC->Draw3dRect (&rectLine, globalData.clrBtnShadow, globalData.clrBtnHilite);
	}

	return rect.Width ();
}
//*******************************************************************************************
BOOL CBCGToolbarMenuButton::OnBeforeDrag () const
{
	if (m_pPopupMenu != NULL)	// Is dropped down
	{
		m_pPopupMenu->CollapseSubmenus ();
	}

	return CBCGToolbarButton::OnBeforeDrag ();
}
//*******************************************************************************************
void CBCGToolbarMenuButton::GetTextHorzOffsets (int& xOffsetLeft, int& xOffsetRight)
{
	xOffsetLeft = CBCGToolBar::GetMenuImageSize ().cx / 2 + TEXT_MARGIN;
	xOffsetRight = CMenuImages::Size ().cx;
}
//*******************************************************************************************
void CBCGToolbarMenuButton::SaveBarState ()
{
	if (m_pWndParent == NULL)
	{
		return;
	}

	CBCGPopupMenu* pParentMenu =
		DYNAMIC_DOWNCAST (CBCGPopupMenu, m_pWndParent->GetParent ());
	if (pParentMenu == NULL)
	{
		return;
	}

	ASSERT_VALID (pParentMenu);

	CBCGPopupMenu* pTopLevelMenu = pParentMenu;
	while ((pParentMenu = DYNAMIC_DOWNCAST (CBCGPopupMenu, pParentMenu->GetParent ()))
		!= NULL)
	{
		pTopLevelMenu = pParentMenu;
	}

	ASSERT_VALID (pTopLevelMenu);
	pTopLevelMenu->SaveState ();
}
//*************************************************************************************************
void CBCGToolbarMenuButton::GetImageRect (CRect& rectImage)
{
	ASSERT_VALID (this);

	rectImage = m_rect;
	rectImage.left += IMAGE_MARGIN;
	rectImage.right = rectImage.left + 
					CBCGToolBar::GetMenuImageSize ().cx + IMAGE_MARGIN;
}

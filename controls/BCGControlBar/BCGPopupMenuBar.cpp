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

// BCGPopupMenuBar.cpp : implementation file
//

#include "stdafx.h"
#include <afxpriv.h>

#pragma warning (disable : 4201)
	#include "mmsystem.h"
#pragma warning (default : 4201)

#include "BCGPopupMenuBar.h"
#include "BCGToolbarButton.h"
#include "BCGToolbarMenuButton.h"
#include "BCGPopupMenu.h"
#include "BCGCommandManager.h"
#include "globals.h"
#include "BCGToolbarMenuButton.h"
#include "bcgbarres.h"
#include "bcglocalres.h"
#include "BCGMenuBar.h"
#include "BCGToolbarComboBoxButton.h"
#include "BCGUserToolsManager.h"
#include "BCGregistry.h"
#include "bcgsound.h"

#include "BCGMDIFrameWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const int iVertMargin = 1;
static const int iHorzMargin = 1;
static const int iSeparatorHeight = 8;
static const int iMinTabSpace = 10;
static const int iEmptyMenuWidth = 50;
static const int iEmptyMenuHeight = 20;

static const int uiPopupTimerEvent = 1;

UINT CBCGPopupMenuBar::m_uiPopupTimerDelay = (UINT) -1;

/////////////////////////////////////////////////////////////////////////////
// CBCGPopupMenuBar

IMPLEMENT_SERIAL(CBCGPopupMenuBar, CBCGToolBar, 1)

CBCGPopupMenuBar::CBCGPopupMenuBar() :
	m_uiDefaultMenuCmdId (0),
	m_pDelayedPopupMenuButton (NULL),
	m_bFirstClick (TRUE),
	m_iOffset (0),
	m_xSeparatorOffsetLeft (0),
	m_xSeparatorOffsetRight (0),
	m_iMaxWidth (-1),
	m_bAreAllCommandsShown (TRUE)
{
	m_bMenuMode = TRUE;
}

CBCGPopupMenuBar::~CBCGPopupMenuBar()
{
}


BEGIN_MESSAGE_MAP(CBCGPopupMenuBar, CBCGToolBar)
	//{{AFX_MSG_MAP(CBCGPopupMenuBar)
	ON_WM_NCPAINT()
	ON_WM_NCCALCSIZE()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_CREATE()
	ON_MESSAGE(WM_IDLEUPDATECMDUI, OnIdleUpdateCmdUI)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBCGPopupMenuBar message handlers

//modified by andy
__declspec (dllexport)
DWORD g_nLastCommandID = (DWORD)-1;
__declspec( dllexport )
bool g_bWaitingForCommandFlag = 0;
//end


BOOL CBCGPopupMenuBar::OnSendCommand (const CBCGToolbarButton* pButton)
{
	ASSERT_VALID (pButton);

	if ((pButton->m_nStyle & TBBS_DISABLED) != 0 ||
		pButton->m_nID < 0 || pButton->m_nID == (UINT)-1)
	{
		return FALSE;
	}

	CBCGToolbarMenuButton* pMenuButton = 
		DYNAMIC_DOWNCAST (CBCGToolbarMenuButton, pButton);
	if (pMenuButton != NULL && (pMenuButton->m_pPopupMenu != NULL ||
		pMenuButton->IsPopupButton() /*SBT1728,comment 1*/))
	{
		return FALSE;
	}
	InvokeMenuCommand (pButton->m_nID);

	return TRUE;
}
//**************************************************************************************
void CBCGPopupMenuBar::InvokeMenuCommand (UINT uiCmdId)
{
	ASSERT (uiCmdId != (UINT) -1);

	GetOwner()->SendMessage(WM_SETMESSAGESTRING, AFX_IDS_IDLEMESSAGE);

	//--------------------
	// Deactivate menubar:
	//--------------------
	CBCGPopupMenu* pParentMenu = DYNAMIC_DOWNCAST (CBCGPopupMenu, GetParent ());
	if (pParentMenu != NULL)
	{
		CBCGToolBar* pToolBar = NULL;
		for (CBCGPopupMenu* pMenu = pParentMenu; pMenu != NULL;
			pMenu = pMenu->GetParentPopupMenu ())
		{
			CBCGToolbarMenuButton* pParentButton = pMenu->GetParentButton ();
			if (pParentButton == NULL)
			{
				break;
			}

			//andy
			pParentButton->OnClosePopupMenu( uiCmdId );
			//
		
			pToolBar = 
				DYNAMIC_DOWNCAST (CBCGToolBar, pParentButton->GetParentWnd ());
		}

		if (pToolBar != NULL)
		{
			pToolBar->Deactivate ();
		}
	}

	CFrameWnd* pParentFrame = GetParentFrame ();
	ASSERT_VALID (pParentFrame);


	HWND hwnd_owner = 0;
	{
		if( GetOwner() )
			hwnd_owner = GetOwner()->GetSafeHwnd();
	}

	HWND hwnd_parent = 0;
	if( pParentFrame )
		hwnd_parent = pParentFrame->GetSafeHwnd();

	if (uiCmdId != 0)
	{
		CBCGToolBar::m_bPressed = m_bInCommand = TRUE;

		BCGPlaySystemSound (BCGSOUND_MENU_COMMAND);

		//----------------------------------
		// Send command to the parent frame:
		//----------------------------------

		

		AddCommandUsage (uiCmdId);

//modified by andy

		bool bSendToParent = true;

		if( g_bWaitingForCommandFlag )
		{
			g_nLastCommandID = uiCmdId;
			bSendToParent = false;
		}
		
		if( pParentMenu /*&& pParentMenu->CanProcessGlobalCmd()*/ )
		{
			//if( bSendToParent) //AAM
			if( bSendToParent || pParentMenu->CanProcessGlobalCmd() )
			{

				if (!pParentMenu->PostCommand (uiCmdId) && // Alex Corazzin (2)
					(g_pUserToolsManager == NULL ||
					!g_pUserToolsManager->InvokeTool (uiCmdId)))
				{
					if( GetOwner() && ::IsWindow( hwnd_owner ) && ::IsWindow( GetOwner()->m_hWnd ) )
						GetOwner()->PostMessage (WM_COMMAND, uiCmdId);
					else
					{
						ASSERT( false );
					}
				}
			}
		}
//end
	}

	CBCGToolBar::m_bPressed = m_bInCommand = FALSE;

	if( ::IsWindow( hwnd_parent ) )
		pParentFrame->DestroyWindow ();
	else
		ASSERT( false );
}
//***************************************************************
void CBCGPopupMenuBar::AdjustLocations ()
{
	if (GetSafeHwnd () == NULL ||
		!::IsWindow (m_hWnd))
	{
		return;
	}

	ASSERT_VALID(this);

	if (m_xSeparatorOffsetLeft == 0)
	{
		//-----------------------------------------------------------
		// To enable MS Office 2000 look, we'll draw the separators
		// bellow the menu text only (in the previous versions
		// separator has been drawn on the whole menu row). Ask
		// menu button about text area offsets:
		//-----------------------------------------------------------
		CBCGToolbarMenuButton::GetTextHorzOffsets (
			m_xSeparatorOffsetLeft,
			m_xSeparatorOffsetRight);
	}

	CRect rectClient;	// Client area rectangle
	GetClientRect (&rectClient);

	CClientDC dc (this);
	CFont* pOldFont = (CFont*) dc.SelectObject (&globalData.fontRegular);
	ASSERT (pOldFont != NULL);

	int y = rectClient.top + iVertMargin - m_iOffset * GetRowHeight ();

	/// By Guy Hachlili - support for the menu with breaks:
	int origy = y;
	int x = rectClient.left;
	int right = (m_arColumns.GetSize() == 0 ||
		CBCGToolBar::IsCustomizeMode ()) ?	
			rectClient.Width() :
			m_arColumns [0];
	int nColumn = 0;
	/////////

	CSize sizeMenuButton = GetMenuImageSize ();
	sizeMenuButton += CSize (2 * iHorzMargin, 2 * iVertMargin);

	sizeMenuButton.cy = max (sizeMenuButton.cy, 
							globalData.GetTextHeight ());

	for (POSITION pos = m_Buttons.GetHeadPosition (); pos != NULL;)
	{
		CBCGToolbarButton* pButton = (CBCGToolbarButton*) m_Buttons.GetNext (pos);
		ASSERT (pButton != NULL);

		/// By Guy Hachlili - support for the menu with breaks:
		if ((pButton->m_nStyle & TBBS_BREAK) && (y != origy) &&
			!CBCGToolBar::IsCustomizeMode ())
		{
			y = origy;
			nColumn ++;
			x = right + iHorzMargin;
			right = m_arColumns [nColumn];
		}
		////////////////////
		
		CRect rectButton;
		rectButton.top = y;

		if (pButton->m_nStyle & TBBS_SEPARATOR)
		{
			rectButton.left = x + m_xSeparatorOffsetLeft;
			rectButton.right = right + rectClient.left - m_xSeparatorOffsetRight;
			rectButton.bottom = rectButton.top + iSeparatorHeight;
		}
		else
		{
			CSize sizeButton = pButton->OnCalculateSize (&dc, 
									sizeMenuButton, TRUE);

			rectButton.left = x;
			rectButton.right = right + rectClient.left;
			rectButton.bottom = rectButton.top + sizeButton.cy;
		}

		pButton->SetRect (rectButton);
		y += rectButton.Height ();
	}

	dc.SelectObject (pOldFont);

	//--------------------------------------------------
	// Something may changed, rebuild acceleration keys:
	//--------------------------------------------------
	RebuildAccelerationKeys ();
}
//***************************************************************************************
void CBCGPopupMenuBar::DrawSeparator (CDC* pDC, const CRect& rect, BOOL /*bHorz*/)
{
	CRect rectSeparator = rect;

	rectSeparator.top += rectSeparator.Height () / 2;
	rectSeparator.bottom = rectSeparator.top + 2;

	pDC->Draw3dRect (rectSeparator, globalData.clrBtnShadow,
									globalData.clrBtnHilite);
}
//***************************************************************************************
CSize CBCGPopupMenuBar::CalcSize ()
{
	CSize size (0, 0);

	CClientDC dc (this);
	CFont* pOldFont = (CFont*) dc.SelectObject (&globalData.fontRegular);
	ASSERT (pOldFont != NULL);

	if (m_Buttons.IsEmpty ())
	{
		size = CSize (iEmptyMenuWidth, iEmptyMenuHeight);
	}
	else
	{
		//////// By Guy Hachlili - support for the menu with breaks:
		CSize column (0, 0);
		m_arColumns.RemoveAll ();
		//////////////////////////

		CSize sizeMenuButton = GetMenuImageSize ();
		sizeMenuButton += CSize (2 * iHorzMargin, 2 * iVertMargin);

		sizeMenuButton.cy = max (sizeMenuButton.cy, 
								globalData.GetTextHeight ());

		for (POSITION pos = m_Buttons.GetHeadPosition (); pos != NULL;)
		{
			CBCGToolbarButton* pButton = (CBCGToolbarButton*) m_Buttons.GetNext (pos);
			ASSERT (pButton != NULL);

			CSize sizeButton = pButton->OnCalculateSize (&dc, 
				sizeMenuButton, TRUE);

			//////// By Guy Hachlili - support for the menu with breaks:
			if ((pButton->m_nStyle & TBBS_BREAK) &&
				!CBCGToolBar::IsCustomizeMode ())
			{
				if ((column.cx != 0) && (column.cy != 0))
				{
					size.cy = max (column.cy, size.cy);
					size.cx += column.cx + iHorzMargin;
					m_arColumns.Add (size.cx);
				}
				column.cx = column.cy = 0;
			}
			///////////////////////////////

			int iHeight = sizeButton.cy;

			if (pButton->m_nStyle & TBBS_SEPARATOR)
			{
				iHeight = iSeparatorHeight;
			}
			else
			{
				if (pButton->IsDrawText () &&
					pButton->m_strText.Find (_T('\t')) > 0)
				{
					sizeButton.cx += iMinTabSpace;
				}

				pButton->m_bWholeText = 
					(m_iMaxWidth <= 0 || 
					sizeButton.cx <= m_iMaxWidth - 2 * iHorzMargin);

				column.cx = max (sizeButton.cx, column.cx);
			}

			column.cy += iHeight;
		}

		size.cy = max (column.cy, size.cy);
		size.cx += column.cx;
	}

	size.cy += 2 * iVertMargin;
	size.cx += 2 * iHorzMargin;

	if (m_iMaxWidth > 0 && size.cx > m_iMaxWidth)
	{
		size.cx = m_iMaxWidth;
	}

	m_arColumns.Add (size.cx);

	dc.SelectObject (pOldFont);
	return size;
}
//***************************************************************************************
void CBCGPopupMenuBar::OnNcPaint() 
{
	//--------------------------------------
	// Disable gripper and borders painting!
	//--------------------------------------
}
//***************************************************************************************
void CBCGPopupMenuBar::OnNcCalcSize(BOOL /*bCalcValidRects*/, NCCALCSIZE_PARAMS FAR* /*lpncsp*/) 
{
	//-----------------------------------------------
	// Don't leave space for the gripper and borders!
	//-----------------------------------------------
}
//****************************************************************************************
void CBCGPopupMenuBar::DrawDragMarker (CDC* pDC)
{
	CPen* pOldPen = (CPen*) pDC->SelectObject (&m_penDrag);

	for (int i = 0; i < 2; i ++)
	{
		pDC->MoveTo (m_rectDrag.left, m_rectDrag.top + m_rectDrag.Height () / 2 + i - 1);
		pDC->LineTo (m_rectDrag.right, m_rectDrag.top + m_rectDrag.Height () / 2 + i - 1);

		pDC->MoveTo (m_rectDrag.left + i, m_rectDrag.top + i);
		pDC->LineTo (m_rectDrag.left + i, m_rectDrag.bottom - i);

		pDC->MoveTo (m_rectDrag.right - i - 1, m_rectDrag.top + i);
		pDC->LineTo (m_rectDrag.right - i - 1, m_rectDrag.bottom - i);
	}

	pDC->SelectObject (pOldPen);
}
//********************************************************************************
int CBCGPopupMenuBar::FindDropIndex (const CPoint p, CRect& rectDrag) const
{
	const int iCursorSize = 6;

	GetClientRect (&rectDrag);

	if (m_Buttons.IsEmpty ())
	{
		rectDrag.bottom = rectDrag.top + iCursorSize;
		return 0;
	}

	CPoint point = p;
	if (point.y < 0)
	{
		point.y = 0;
	}

	int iDragButton = -1;
	int iIndex = 0;
	for (POSITION pos = m_Buttons.GetHeadPosition (); pos != NULL; iIndex ++)
	{
		CBCGToolbarButton* pButton = (CBCGToolbarButton*) m_Buttons.GetNext (pos);
		ASSERT (pButton != NULL);

		CRect rect = pButton->Rect ();
		if (point.y < rect.top)
		{
			iDragButton = iIndex;
			rectDrag.top = rect.top;
			break;
		}
		else if (point.y <= rect.bottom)
		{
			rectDrag = rect;
			if (point.y - rect.top > rect.bottom - point.y)
			{
				iDragButton = iIndex + 1;
				rectDrag.top = rectDrag.bottom;
			}
			else
			{
				iDragButton = iIndex;
				rectDrag.top = rect.top;
			}
			break;
		}
	}

	if (iDragButton == -1)
	{
		rectDrag.top = rectDrag.bottom - iCursorSize;
		iDragButton = iIndex;
	}

	rectDrag.bottom = rectDrag.top + iCursorSize;
	rectDrag.OffsetRect (0, -iCursorSize / 2);

	return iDragButton;
}
//***************************************************************************************
CBCGToolbarButton* CBCGPopupMenuBar::CreateDroppedButton (COleDataObject* pDataObject)
{
	CBCGToolbarButton* pButton = CBCGToolbarButton::CreateFromOleData (pDataObject);
	ASSERT (pButton != NULL);

	CBCGToolbarMenuButton* pMenuButton = 
		DYNAMIC_DOWNCAST (CBCGToolbarMenuButton, pButton);

	if (pMenuButton == NULL)
	{
		pMenuButton = new CBCGToolbarMenuButton (
			pButton->m_nID, NULL, 
				pButton->IsLocked () ? -1 : pButton->GetImage (), 
				pButton->m_strText,
			pButton->m_bUserButton);
		ASSERT (pMenuButton != NULL);

		pMenuButton->m_bText = TRUE;
		pMenuButton->m_bImage = TRUE;

		BOOL bRes = pButton->ExportToMenuButton (*pMenuButton);
		delete pButton;
		
		if (!bRes || pMenuButton->m_strText.IsEmpty ())
		{
			delete pMenuButton;
			return NULL;
		}
	}

	return pMenuButton;
}
//****************************************************************************************
BOOL CBCGPopupMenuBar::ImportFromMenu (HMENU hMenu, BOOL bShowAllCommands)
{
	RemoveAllButtons ();
	m_bAreAllCommandsShown = TRUE;
	m_HiddenItemsAccel.RemoveAll ();

	if (hMenu == NULL)
	{
		return FALSE;
	}

	CMenu* pMenu = CMenu::FromHandle (hMenu);
	if (pMenu == NULL)
	{
		return FALSE;
	}

	//***** MSI START *****
	// By Mike Harvey (MSI)
    CFrameWnd* pMainWindow = DYNAMIC_DOWNCAST (CFrameWnd, AfxGetMainWnd ());
    if (pMainWindow != NULL)
	{
		WPARAM theMenu = WPARAM(hMenu);
		LPARAM theItem = MAKELPARAM(m_iOffset, 0);
		pMainWindow->SendMessage(WM_INITMENUPOPUP, theMenu, theItem);
	}
	//***** MSI END *****

	int iCount = (int) pMenu->GetMenuItemCount ();
	BOOL bPrevWasSeparator = FALSE;
	BOOL bFirstItem = TRUE;

	for (int i = 0; i < iCount; i ++)
	{
		UINT uiCmd = pMenu->GetMenuItemID (i);
		UINT uiState = pMenu->GetMenuState (i, MF_BYPOSITION);

		HMENU hSubMenu = NULL;

		CString strText;
		pMenu->GetMenuString (i, strText, MF_BYPOSITION);

		switch (uiCmd)
		{
		case 0:
			if (!bPrevWasSeparator && !bFirstItem && i != iCount - 1)
			{
				InsertSeparator ();
				bFirstItem = FALSE;
				bPrevWasSeparator = TRUE;
			}
			break;

		case -1:
			hSubMenu = pMenu->GetSubMenu (i)->GetSafeHmenu ();
			ASSERT (hSubMenu != NULL);

		default:
			if (bShowAllCommands ||
				CBCGMenuBar::IsShowAllCommands () ||
				!CBCGToolBar::IsCommandRarelyUsed (uiCmd))
			{
				CBCGToolbarMenuButton item (uiCmd, hSubMenu,
											-1, strText);
				item.m_bText = TRUE;
				item.m_bImage = FALSE;
//!!!andy
				item.m_iImage = CMD_MGR.GetCmdImage(uiCmd, FALSE);
				item.m_iUserImage = CMD_MGR.GetCmdImage(uiCmd, TRUE);

				if( item.m_iUserImage != -1)item.m_bUserButton = TRUE;
				//if( item.m_iUserImage == -1)item.m_bUserButton = TRUE;
				

				int iIndex = InsertButton (item);
				if (iIndex >= 0)
				{
					CBCGToolbarButton* pButton = GetButton (iIndex);
					ASSERT (pButton != NULL);

					pButton->m_bImage = (pButton->GetImage () >= 0);

					if (g_pUserToolsManager == NULL ||
						!g_pUserToolsManager->IsUserToolCmd (uiCmd))
					{
						if ((uiState & MF_DISABLED) || (uiState & MF_GRAYED))
						{
							pButton->m_nStyle |= TBBS_DISABLED;
						}
					}

					if (uiState & MF_CHECKED)
					{
						pButton->m_nStyle |= TBBS_CHECKED;
					}

					/// By Guy Hachlili - support for the menu with breaks:
					if (uiState & MF_MENUBREAK)
					{
						pButton->m_nStyle |= TBBS_BREAK;
					}
					///////////////
				}

				bPrevWasSeparator = FALSE;
				bFirstItem = FALSE;
			}
			else if (CBCGToolBar::IsCommandRarelyUsed (uiCmd) &&
				CBCGToolBar::IsCommandPermitted (uiCmd))
			{
				m_bAreAllCommandsShown = FALSE;
				
				int iAmpOffset = strText.Find (_T('&'));
				if (iAmpOffset >= 0 && iAmpOffset < strText.GetLength () - 1)
				{
					UINT uHotKey = (UINT) toupper (strText.GetAt (iAmpOffset + 1));
					m_HiddenItemsAccel.SetAt (uHotKey, uiCmd);
				}
			}
		}
	}

	m_uiDefaultMenuCmdId = ::GetMenuDefaultItem (hMenu, FALSE, GMDI_USEDISABLED);
	return TRUE;
}
//****************************************************************************************
HMENU CBCGPopupMenuBar::ExportToMenu () const
{
	CMenu menu;
	menu.CreatePopupMenu ();

	for (POSITION pos = m_Buttons.GetHeadPosition (); pos != NULL;)
	{
		CBCGToolbarButton* pButton = (CBCGToolbarButton*) m_Buttons.GetNext (pos);
		ASSERT (pButton != NULL);

		if (pButton->m_nStyle & TBBS_SEPARATOR)
		{
			menu.AppendMenu (MF_SEPARATOR);
			continue;
		}

		if (!pButton->IsKindOf (RUNTIME_CLASS (CBCGToolbarMenuButton)))
		{
			continue;
		}
		//andy - store image and user image
		CMD_MGR.SetCmdImage(pButton->m_nID, -1, false );
		CMD_MGR.SetCmdImage(pButton->m_nID, -1, true );
		if(  pButton->GetImage() != -1 )
			CMD_MGR.SetCmdImage(pButton->m_nID, pButton->GetImage(), pButton->m_bUserButton );
		


		CBCGToolbarMenuButton* pMenuButton = (CBCGToolbarMenuButton*) pButton;

		HMENU hPopupMenu = pMenuButton->CreateMenu ();
		if (hPopupMenu != NULL)
		{
			UINT uiStyle = (MF_STRING | MF_POPUP);
			
			/// By Guy Hachlili - support for the menu with breaks:
			if (pButton->m_nStyle & TBBS_BREAK)
			{
				uiStyle |= MF_MENUBREAK;
			}
			//////////////////////

			menu.AppendMenu (uiStyle, (UINT) hPopupMenu, pMenuButton->m_strText);

			//--------------------------------------------------------
			// This is incompatibility between Windows 95 and 
			// NT API! (IMHO). CMenu::AppendMenu with MF_POPUP flag 
			// COPIES sub-menu resource under the Win NT and 
			// MOVES sub-menu under Win 95/98 and 2000!
			//--------------------------------------------------------
			if (globalData.bIsWindowsNT4)
			{
				::DestroyMenu (hPopupMenu);
			}
		}
		else
		{
			menu.AppendMenu (MF_STRING, pMenuButton->m_nID, pMenuButton->m_strText);
		}
	}

	HMENU hMenu = menu.Detach ();

	::SetMenuDefaultItem (hMenu, m_uiDefaultMenuCmdId, FALSE);
	return hMenu;
}
//***************************************************************************************
void CBCGPopupMenuBar::OnChangeHot (int iHot)
{
	if (iHot == -1 && !CBCGToolBar::IsCustomizeMode ())
	{
		return;
	}

	CBCGToolbarMenuButton* pCurrPopupMenu = NULL;

	for (POSITION pos = m_Buttons.GetHeadPosition (); pos != NULL;)
	{
		CBCGToolbarButton* pButton = (CBCGToolbarButton*) m_Buttons.GetNext (pos);
		ASSERT_VALID (pButton);

		CBCGToolbarMenuButton* pMenuButton =
			DYNAMIC_DOWNCAST (CBCGToolbarMenuButton, pButton);

		if (pMenuButton != NULL && pMenuButton->IsDroppedDown ())
		{
			pCurrPopupMenu = pMenuButton;
			break;
		}
	}

	CBCGToolbarMenuButton* pMenuButton = NULL;
	if (iHot >= 0)
	{
		CBCGToolbarButton* pButton = GetButton (iHot);
		if (pButton == NULL)
		{
			ASSERT (FALSE);
		}
		else
		{
			ASSERT_VALID (pButton);
			pMenuButton = DYNAMIC_DOWNCAST (CBCGToolbarMenuButton, pButton);
		}
	}

	if (pMenuButton != pCurrPopupMenu)
	{
		if (pCurrPopupMenu != NULL)
		{
			pCurrPopupMenu->OnCancelMode ();

			CBCGPopupMenu* pParentMenu = DYNAMIC_DOWNCAST (CBCGPopupMenu, GetParent ());
			if (pParentMenu != NULL)
			{
				CBCGPopupMenu::ActivatePopupMenu (GetParentFrame (), pParentMenu);
			}
		}

		if (pMenuButton != NULL && 
			(pMenuButton->m_nID == (UINT) -1 || pMenuButton->m_bDrawDownArrow))
		{
			pMenuButton->OnClick (this);
		}
	}
}
//****************************************************************************************
void CBCGPopupMenuBar::OnDestroy() 
{
	KillTimer (uiPopupTimerEvent);
	m_pDelayedPopupMenuButton = NULL;

	for (POSITION pos = m_Buttons.GetHeadPosition (); pos != NULL;)
	{
		CBCGToolbarButton* pButton = (CBCGToolbarButton*) m_Buttons.GetNext (pos);
		ASSERT_VALID (pButton);

		CBCGToolbarMenuButton* pMenuButton =
			DYNAMIC_DOWNCAST (CBCGToolbarMenuButton, pButton);

		if (pMenuButton != NULL && pMenuButton->IsDroppedDown ())
		{
			CBCGPopupMenu* pMenu = pMenuButton->m_pPopupMenu;
			if (pMenu != NULL && ::IsWindow (pMenu->m_hWnd))
			{
				pMenu->SaveState ();
				pMenu->PostMessage (WM_CLOSE);
			}
		}
	}

	CBCGToolBar::OnDestroy();
}
//****************************************************************************************
BOOL CBCGPopupMenuBar::OnKey(UINT nChar)
{
	BOOL bProcessed = FALSE;

	POSITION posSel = 
		(m_iHighlighted < 0) ? NULL : m_Buttons.FindIndex (m_iHighlighted);
	CBCGToolbarButton* pOldSelButton = 
		(posSel == NULL) ? NULL : (CBCGToolbarButton*) m_Buttons.GetAt (posSel);
	CBCGToolbarButton* pNewSelButton = pOldSelButton;
	int iNewHighlight = m_iHighlighted;

	switch (nChar)
	{
	case VK_RETURN:
		{
			bProcessed = TRUE;

			// Try to cascase a popup menu and, if failed 
			CBCGToolbarMenuButton* pMenuButton = DYNAMIC_DOWNCAST (CBCGToolbarMenuButton,
							pOldSelButton);
			if (pMenuButton != NULL &&
				!pMenuButton->OpenPopupMenu ())
			{
				GetOwner()->SendMessage(WM_SETMESSAGESTRING, AFX_IDS_IDLEMESSAGE);
				OnSendCommand (pMenuButton);
			}
		}
		break;

	case VK_HOME:
		posSel = NULL;
		// Like "Before first"...

	case VK_DOWN:
		//-----------------------------
		// Find next "selecteble" item:
		//-----------------------------
		{
			bProcessed = TRUE;
			if (m_Buttons.IsEmpty ())
			{
				break;
			}

			POSITION pos = posSel;
			if (pos != NULL)
			{
				m_Buttons.GetNext (pos);
			}

			if (pos == NULL)
			{
				pos = m_Buttons.GetHeadPosition ();
				iNewHighlight = 0;
			}
			else
			{
				iNewHighlight ++;
			}

			POSITION posFound = NULL;
			while (pos != posSel)
			{
				posFound = pos;

				CBCGToolbarButton* pButton = (CBCGToolbarButton*) m_Buttons.GetNext (pos);
				ASSERT_VALID (pButton);

				if ((pButton->m_nStyle & TBBS_SEPARATOR) == 0)
				{
					break;
				}

				iNewHighlight ++;
				if (pos == NULL)
				{
					pos = m_Buttons.GetHeadPosition ();
					iNewHighlight = 0;
				}
			}

			if (posFound != NULL)
			{
				pNewSelButton = (CBCGToolbarButton*) m_Buttons.GetAt (posFound);
			}
		}
		break;

	case VK_END:
		posSel = NULL;
		// Like "After last"....

	case VK_UP:
		//---------------------------------
		// Find previous "selecteble" item:
		//---------------------------------
		{
			bProcessed = TRUE;
			if (m_Buttons.IsEmpty ())
			{
				break;
			}

			POSITION pos = posSel;
			if (pos != NULL)
			{
				m_Buttons.GetPrev (pos);
			}
			if (pos == NULL)
			{
				pos = m_Buttons.GetTailPosition ();
				iNewHighlight = m_Buttons.GetCount () - 1;
			}
			else
			{
				iNewHighlight --;
			}

			POSITION posFound = NULL;
			while (pos != posSel)
			{
				posFound = pos;

				CBCGToolbarButton* pButton = (CBCGToolbarButton*) m_Buttons.GetPrev (pos);
				ASSERT_VALID (pButton);

				if ((pButton->m_nStyle & TBBS_SEPARATOR) == 0)
				{
					break;
				}

				iNewHighlight --;
				if (pos == NULL)
				{
					pos = m_Buttons.GetTailPosition ();
					iNewHighlight = m_Buttons.GetCount () - 1;
				}
			}

			if (posFound != NULL)
			{
				pNewSelButton = (CBCGToolbarButton*) m_Buttons.GetAt (posFound);
			}
		}
		break;

	default:
		// Process acceleration key:
		if (!IsCustomizeMode ())
		{
			UINT nUpperChar = toupper (nChar);

			CBCGToolbarButton* pButton;
			if (m_AcellKeys.Lookup (nUpperChar, pButton))
			{
				ASSERT_VALID (pButton);

				pNewSelButton = pButton;

				//-------------------
				// Find button index:
				//-------------------
				int iIndex = 0;
				for (POSITION pos = m_Buttons.GetHeadPosition ();
					pos != NULL; iIndex ++)
				{
					CBCGToolbarButton* pListButton = (CBCGToolbarButton*) m_Buttons.GetNext (pos);
					ASSERT (pListButton != NULL);

					if (pListButton == pButton)
					{
						iNewHighlight = iIndex;
						break;
					}
				}
				
				CBCGToolbarMenuButton* pMenuButton =
					DYNAMIC_DOWNCAST (CBCGToolbarMenuButton, pButton);

				if (pMenuButton != NULL)
				{
					if (pMenuButton->OpenPopupMenu ())
					{
						if (pMenuButton->m_pPopupMenu != NULL)
						{
							//--------------------------
							// Select a first menu item:
							//--------------------------
							pMenuButton->m_pPopupMenu->SendMessage (WM_KEYDOWN, VK_HOME);
						}
					}
					else
					{
						// ------ By John Young:-------------------
						// If the newly selected item is not highlighted,
						// then make the menu go away.

						if ((pButton->m_nStyle & TBBS_DISABLED) != 0)
						{
							InvokeMenuCommand (0);
							return TRUE;
						}
						//-----------------------------------------

						bProcessed = OnSendCommand (pMenuButton);
						if (bProcessed)
						{
							return TRUE;
						}
					}
				}
			}
			else if (CBCGMenuBar::m_bRecentlyUsedMenus &&
				!m_bAreAllCommandsShown)
			{
				///---------------------------------------------------
				// Maybe, this accelerator is belong to "hidden' item?
				//----------------------------------------------------
				UINT uiCmd = 0;
				if (m_HiddenItemsAccel.Lookup (nUpperChar, uiCmd))
				{
					InvokeMenuCommand (uiCmd);
					return TRUE;
				}
			}
		}
	}

	if (pNewSelButton != pOldSelButton)
	{
		ASSERT_VALID (pNewSelButton);
		ASSERT (iNewHighlight >= 0 && iNewHighlight < m_Buttons.GetCount ());
		ASSERT (GetButton (iNewHighlight) == pNewSelButton);

		//------------------------------------
		// Fixed by Sven Ritter (SpeedProject)
		//------------------------------------
		if (IsCustomizeMode ())
		{
			m_iSelected = iNewHighlight;
		}
		// --- End ---

		m_iHighlighted = iNewHighlight;

		if (pOldSelButton != NULL)
		{
			InvalidateRect (pOldSelButton->Rect ());
		}

		InvalidateRect (pNewSelButton->Rect ());
		UpdateWindow ();

		if (pNewSelButton->m_nID != (UINT) -1)
		{
			ShowCommandMessageString (pNewSelButton->m_nID);
		}
	}

	return bProcessed;
}
//**************************************************************************************
void CBCGPopupMenuBar::OnTimer(UINT_PTR nIDEvent) 
{
	if (nIDEvent == uiPopupTimerEvent)
	{
		KillTimer (uiPopupTimerEvent);

		CBCGToolbarMenuButton* pDelayedPopupMenuButton = m_pDelayedPopupMenuButton;
		m_pDelayedPopupMenuButton = NULL;

		if (pDelayedPopupMenuButton != NULL &&
			m_iHighlighted >= 0 &&
			m_iHighlighted < m_Buttons.GetCount () &&
			GetButton (m_iHighlighted) == pDelayedPopupMenuButton)
		{
			ASSERT_VALID (pDelayedPopupMenuButton);
			pDelayedPopupMenuButton->OpenPopupMenu (this);
		}
	}
}
//**************************************************************************************
void CBCGPopupMenuBar::StartPopupMenuTimer (CBCGToolbarMenuButton* pMenuButton)
{
	if (m_pDelayedPopupMenuButton != NULL)
	{
		KillTimer (uiPopupTimerEvent);
	}

	if ((m_pDelayedPopupMenuButton = pMenuButton) != NULL)
	{
		SetTimer (uiPopupTimerEvent, m_uiPopupTimerDelay, NULL);
	}
}
//**********************************************************************************
void CBCGPopupMenuBar::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_bFirstClick = FALSE;

	CRect rectClient;
	GetClientRect (&rectClient);

	if (!IsCustomizeMode () && 
		!rectClient.PtInRect (point))
	{
		CBCGToolBar* pDestBar = FindDestBar (point);
		if (pDestBar != NULL)
		{
			CPoint ptDest = point;
			MapWindowPoints (pDestBar, &ptDest, 1);

			pDestBar->SendMessage (	WM_LBUTTONDOWN, 
									nFlags, 
									MAKELPARAM (ptDest.x, ptDest.y));
		}
	}

	CBCGToolBar::OnLButtonDown(nFlags, point);
}
//**********************************************************************************
void CBCGPopupMenuBar::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CRect rectClient;
	GetClientRect (&rectClient);

	if (!m_bFirstClick &&
		!IsCustomizeMode () && 
		!rectClient.PtInRect (point))
	{
		CBCGToolBar* pDestBar = FindDestBar (point);
		if (pDestBar != NULL)
		{
			MapWindowPoints (pDestBar, &point, 1);
			pDestBar->SendMessage (	WM_LBUTTONUP, 
									nFlags, 
									MAKELPARAM (point.x, point.y));
		}

		CFrameWnd* pParentFrame = GetParentFrame ();
		ASSERT_VALID (pParentFrame);

		pParentFrame->DestroyWindow ();
		return;
	}

	if (!IsCustomizeMode () && m_iHighlighted >= 0)
	{
		m_iButtonCapture = m_iHighlighted;
	}

	m_bFirstClick = FALSE;
	CBCGToolBar::OnLButtonUp (nFlags, point);
}
//**********************************************************************************
BOOL CBCGPopupMenuBar::OnSetDefaultButtonText (CBCGToolbarButton* pButton)
{
	ASSERT_VALID (pButton);

	CBCGPopupMenu* pParentMenu = DYNAMIC_DOWNCAST (CBCGPopupMenu, GetParent ());
	if (pParentMenu != NULL)
	{
		CBCGToolBar* pToolBar = pParentMenu->GetParentToolBar ();
		if (pToolBar != NULL && pToolBar->OnSetDefaultButtonText (pButton))
		{
			return TRUE;
		}
	}

	return CBCGToolBar::OnSetDefaultButtonText (pButton);
}
//****************************************************************************************
BOOL CBCGPopupMenuBar::EnableContextMenuItems (CBCGToolbarButton* pButton, CMenu* pPopup)
{
	if (!CBCGToolBar::IsCustomizeMode ())
	{
		// Disable context menu
		return FALSE;
	}

	ASSERT_VALID (pButton);
	ASSERT_VALID (pPopup);

	pPopup->EnableMenuItem (ID_BCGBARRES_TOOLBAR_IMAGE, MF_GRAYED | MF_BYCOMMAND);
	pPopup->EnableMenuItem (ID_BCGBARRES_TOOLBAR_TEXT, MF_GRAYED | MF_BYCOMMAND);
	pPopup->EnableMenuItem (ID_BCGBARRES_TOOLBAR_IMAGE_AND_TEXT, MF_GRAYED | MF_BYCOMMAND);

	pButton->m_bText = TRUE;
	CBCGToolBar::EnableContextMenuItems (pButton, pPopup);

	return TRUE;
}
//****************************************************************************************
void CBCGPopupMenuBar::OnMouseMove(UINT nFlags, CPoint point) 
{
	CRect rectClient;
	GetClientRect (&rectClient);

	if (IsCustomizeMode () ||
		rectClient.PtInRect (point))
	{
		CBCGToolBar::OnMouseMove(nFlags, point);
	}
	else
	{
		CBCGToolBar* pDestBar = FindDestBar (point);
		if (pDestBar != NULL)
		{
			MapWindowPoints (pDestBar, &point, 1);
			pDestBar->SendMessage (	WM_MOUSEMOVE, 
									nFlags, 
									MAKELPARAM (point.x, point.y));
		}
	}
}
//***************************************************************************************
CBCGToolBar* CBCGPopupMenuBar::FindDestBar (CPoint point)
{
	ScreenToClient (&point);

	CRect rectClient;

	CBCGPopupMenu* pPopupMenu = DYNAMIC_DOWNCAST (CBCGPopupMenu, GetParent ());
	ASSERT_VALID (pPopupMenu);

	CBCGPopupMenu* pLastPopupMenu = pPopupMenu;

	//-------------------------------
	// Go up trougth all popup menus:
	//-------------------------------
	while ((pPopupMenu = pPopupMenu->GetParentPopupMenu ()) != NULL)
	{
		CBCGPopupMenuBar* pPopupMenuBar = pPopupMenu->GetMenuBar ();
		ASSERT_VALID (pPopupMenuBar);

		pPopupMenuBar->GetClientRect (&rectClient);
		pPopupMenuBar->MapWindowPoints (this, &rectClient);

		if (rectClient.PtInRect (point))
		{
			return pPopupMenuBar;
		}

		pLastPopupMenu = pPopupMenu;
	}

	ASSERT_VALID (pLastPopupMenu);

	//--------------------
	// Try parent toolbar:
	//--------------------
	CBCGToolBar* pToolBar = pLastPopupMenu->GetParentToolBar ();
	if (pToolBar != NULL)
	{
		pToolBar->GetClientRect (&rectClient);
		pToolBar->MapWindowPoints (this, &rectClient);

		if (rectClient.PtInRect (point))
		{
			return pToolBar;
		}
	}

	return NULL;
}

bool	CBCGPopupMenuBar::CanDragDropButtons()
{
	CBCGMDIFrameWnd *pMainFrame = DYNAMIC_DOWNCAST(CBCGMDIFrameWnd, GetOwner());
	CBCGPopupMenu* pParentMenu = DYNAMIC_DOWNCAST (CBCGPopupMenu, GetParent ());
	if (pMainFrame != NULL && pParentMenu != NULL)
	{
		CString sName = pParentMenu->GetTitle();
		if (!sName.IsEmpty() && !pMainFrame->GetEnableChangeMenu(sName))
			return false;
	}
	return __super::CanDragDropButtons();
}


//*********************************************************************************************
DROPEFFECT CBCGPopupMenuBar::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	//A.M. modification (18.04.2006, BT5157). Some menues mustn't be changed by user.
	if (!CanDragDropButtons())
		return DROPEFFECT_NONE;
	/*CBCGMDIFrameWnd *pMainFrame = DYNAMIC_DOWNCAST(CBCGMDIFrameWnd, GetOwner());
	CBCGPopupMenu* pParentMenu = DYNAMIC_DOWNCAST (CBCGPopupMenu, GetParent ());
	if (pMainFrame != NULL && pParentMenu != NULL)
	{
		CString sName = pParentMenu->GetTitle();
		if (!sName.IsEmpty() && !pMainFrame->GetEnableChangeMenu(sName))
			return DROPEFFECT_NONE;
	}*/
	//end A.M. modification

	//-----------------------------------------------
	// Disable MOVING menu item into one of submenus!
	//-----------------------------------------------
	if ((dwKeyState & MK_CONTROL) == 0)
	{
		CBCGPopupMenu* pParentMenu = DYNAMIC_DOWNCAST (CBCGPopupMenu, GetParent ());
		if (pParentMenu != NULL)
		{
			CBCGToolBar* pParentBar = pParentMenu->GetParentToolBar ();
			CBCGToolbarMenuButton* pParentButton = pParentMenu->GetParentButton ();

			if (pParentBar != NULL && pParentButton != NULL &&
				pParentBar->IsDragButton (pParentButton))
			{
				return DROPEFFECT_NONE;
			}
		}
	}

	return CBCGToolBar::OnDragOver(pDataObject, dwKeyState, point);
}
//*****************************************************************************************
void CBCGPopupMenuBar::OnFillBackground (CDC* pDC)
{
	ASSERT_VALID (pDC);

	if (CBCGToolBar::IsCustomizeMode () ||
		!CBCGMenuBar::m_bRecentlyUsedMenus)
	{
		return;
	}

	//--------------------------------------------------------------
	// Only menubar first-level menus may hide rarely used commands:
	//--------------------------------------------------------------
	CBCGPopupMenu* pParentMenu = DYNAMIC_DOWNCAST (CBCGPopupMenu, GetParent ());
	if (pParentMenu == NULL || !pParentMenu->HideRarelyUsedCommands ())
	{
		return;
	}

	BOOL bFirstRarelyUsedButton = TRUE;
	CRect rectRarelyUsed;

	for (POSITION pos = m_Buttons.GetHeadPosition (); pos != NULL;)
	{
		CBCGToolbarButton* pButton = (CBCGToolbarButton*) m_Buttons.GetNext (pos);
		ASSERT (pButton != NULL);

		if (pButton->m_nStyle & TBBS_SEPARATOR)
		{
			if (pos != NULL &&
				CBCGToolBar::IsCommandRarelyUsed (
					((CBCGToolbarButton*) m_Buttons.GetAt (pos))->m_nID))
			{
				continue;
			}
		}

		BOOL bDraw = FALSE;

		if (CBCGToolBar::IsCommandRarelyUsed (pButton->m_nID))
		{
			if (bFirstRarelyUsedButton)
			{
				bFirstRarelyUsedButton = FALSE;
				rectRarelyUsed = pButton->Rect ();
			}

			if (pos == NULL)	// Last button
			{
				rectRarelyUsed.bottom = pButton->Rect ().bottom;
				bDraw = TRUE;
			}
		}
		else
		{
			if (!bFirstRarelyUsedButton)
			{
				rectRarelyUsed.bottom = pButton->Rect ().top;
				bDraw = TRUE;
			}

			bFirstRarelyUsedButton = TRUE;
		}

		if (bDraw)
		{
			CBCGToolBarImages::FillDitheredRect (pDC, rectRarelyUsed);
			pDC->Draw3dRect (rectRarelyUsed, globalData.clrBtnShadow,
							globalData.clrBtnHilite);
		}
	}
}
//*************************************************************************************
INT_PTR CBCGPopupMenuBar::OnToolHitTest(CPoint point, TOOLINFO* pTI) const
{
	ASSERT_VALID(this);

	int nHit = ((CBCGPopupMenuBar*)this)->HitTest(point);
	if (nHit != -1)
	{
		CBCGToolbarButton* pButton = 
			DYNAMIC_DOWNCAST (CBCGToolbarButton, GetButton (nHit));

		if (pButton != NULL)
		{
			if (pTI != NULL)
			{
				pTI->uId = pButton->m_nID;
				pTI->hwnd = GetSafeHwnd ();
				pTI->rect = pButton->Rect ();
			}

			if (!pButton->OnToolHitTest (this, pTI))
			{
				nHit = pButton->m_nID;
			}
		}
	}

	return nHit;
}
//**********************************************************************************
int CBCGPopupMenuBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CBCGToolBar::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (m_uiPopupTimerDelay == (UINT) -1)	// Not defined yet
	{
		m_uiPopupTimerDelay = 500;
		CBCGRegistry reg (FALSE, TRUE);

		if (reg.Open (_T("Control Panel\\Desktop")))
		{
			CString strVal;

			// By David Wang
			if (reg.Read (_T("MenuShowDelay"), strVal))
			{
				m_uiPopupTimerDelay = (UINT) _ttol (strVal);

				//------------------------
				// Just limit it to 5 sec:
				//------------------------
				m_uiPopupTimerDelay = min (5000, m_uiPopupTimerDelay);
			}
		}
	}
	
	return 0;
}
//*****************************************************************
void CBCGPopupMenuBar::SetButtonStyle(int nIndex, UINT nStyle)
{
	CBCGToolbarButton* pButton = GetButton (nIndex);
	if (pButton == NULL)
	{
		ASSERT (FALSE);
		return;
	}

	UINT nOldStyle = pButton->m_nStyle;
	if (nOldStyle != nStyle)
	{
		// update the style and invalidate
		pButton->m_nStyle = nStyle;

		// invalidate the button only if both styles not "pressed"
		if (!(nOldStyle & nStyle & TBBS_PRESSED))
		{
			CBCGToolbarMenuButton* pMenuButton =
				DYNAMIC_DOWNCAST (CBCGToolbarMenuButton, GetButton (nIndex));

			BOOL bWasChecked = nOldStyle & TBBS_CHECKED;
			BOOL bChecked = nStyle & TBBS_CHECKED;

			// If checked style was changed. redraw check box (or image) area only:
			if (pMenuButton != NULL && bWasChecked != bChecked)
			{
				CRect rectImage;
				pMenuButton->GetImageRect (rectImage);
				rectImage.InflateRect( 1, 1 );

				InvalidateRect (rectImage);
				UpdateWindow ();
			}
			else if ((nOldStyle ^ nStyle) != TBSTATE_PRESSED)
			{
				InvalidateButton(nIndex);
			}
		}
	}
}
// By Alex Corazzin (2)
// ---------------------------------------------------------------
LRESULT CBCGPopupMenuBar::OnIdleUpdateCmdUI(WPARAM wParam, LPARAM)
{
	// handle delay hide/show
	BOOL bVis = GetStyle() & WS_VISIBLE;
	UINT swpFlags = 0;
	if ((m_nStateFlags & delayHide) && bVis)
	{
		swpFlags = SWP_HIDEWINDOW;
	}
	else if ((m_nStateFlags & delayShow) && !bVis)
	{
		swpFlags = SWP_SHOWWINDOW;
	}

	m_nStateFlags &= ~(delayShow|delayHide);
	if (swpFlags != 0)
	{
		SetWindowPos (NULL, 0, 0, 0, 0, swpFlags|
			SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE);
	}
	
	// the style must be visible and if it is docked
	// the dockbar style must also be visible
	if ((GetStyle() & WS_VISIBLE) &&
		(m_pDockBar == NULL || (m_pDockBar->GetStyle() & WS_VISIBLE)))
	{
		CFrameWnd* pTarget = GetCommandTarget ();
		if (pTarget == NULL || !pTarget->IsFrameWnd())
		{
			pTarget = GetParentFrame();
		}

		if (pTarget != NULL)
		{
			OnUpdateCmdUI (pTarget, (BOOL)wParam);
		}
	}

	return 0L;
}
// Fine aggiunta
// ---------------------------------------------------------------

CFrameWnd* CBCGPopupMenuBar::GetCommandTarget () const
{
	CBCGPopupMenu* pParentMenu = DYNAMIC_DOWNCAST (CBCGPopupMenu, GetParent ());
	if (pParentMenu != NULL && pParentMenu->GetMessageWnd () != NULL)
	{
         return pParentMenu;
	}

	return CBCGToolBar::GetCommandTarget ();
}

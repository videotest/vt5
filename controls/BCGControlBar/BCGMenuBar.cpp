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

// BCGMenuBar.cpp : implementation file
//

#include "stdafx.h"

#include "globals.h"
#include "BCGMenuBar.h"
#include "BCGToolbarButton.h"
#include "BCGToolbarMenuButton.h"
#include "BCGToolbarSystemMenuButton.h"
#include "BCGToolbarMenuButtonsButton.h"
#include "BCGPopupMenu.h"
#include "MenuHash.h"
#include "bcgbarres.h"
#include "bcglocalres.h"
#include "BCGMDIFrameWnd.h"
#include "BCGFrameWnd.h"
#include "BCGOleIPFrameWnd.h"
#include "BCGMultiDocTemplate.h"
#include "BCGDockBar.h"
#include "RegPath.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CBCGMenuBar, CBCGToolBar, VERSIONABLE_SCHEMA | 1)

HHOOK CBCGMenuBar::m_hookMouseDrag = NULL;
CBCGMenuBar* CBCGMenuBar::m_pDraggedMenuBar = NULL;
BOOL CBCGMenuBar::m_bShowAllCommands = FALSE;
BOOL CBCGMenuBar::m_bRecentlyUsedMenus = TRUE;
BOOL CBCGMenuBar::m_bShowAllMenusDelay = TRUE;
BOOL CBCGMenuBar::m_bMenuShadows = TRUE;
BOOL CBCGMenuBar::m_bHighlightDisabledItems = FALSE;

static const UINT uiShowAllItemsTimerId = 1;
static const int iShowAllItemsTimerFreq = 5000;	// 5 sec

static const CString strMenuProfile = _T("BCGMenuBar");
extern CMenuHash g_menuHash;

/////////////////////////////////////////////////////////////////////////////
// CBCGMenuBar

CBCGMenuBar::CBCGMenuBar()
{
	m_bMaximizeMode = FALSE;
	m_hMenu = NULL;
	m_hDefaultMenu = NULL;
	m_hSysMenu = NULL;
	m_hSysIcon = NULL;
	m_uiDefMenuResId = 0;
	m_nSystemButtonsNum = 0;
	m_nSystemButtonsNumSaved = 0;
	m_bHaveButtons = FALSE;
	m_szSystemButton = CSize (0, 0);
	m_bAutoDocMenus = TRUE;
}

CBCGMenuBar::~CBCGMenuBar()
{
	SetDragMode (FALSE);
	::DestroyMenu (m_hMenu);
}


BEGIN_MESSAGE_MAP(CBCGMenuBar, CBCGToolBar)
	//{{AFX_MSG_MAP(CBCGMenuBar)
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_SETFOCUS()
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_TIMER()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_SETTINGCHANGE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CBCGMenuBar message handlers

void CBCGMenuBar::CreateFromMenu (HMENU hMenu, BOOL bDefaultMenu, BOOL bForceUpdate)
{
	if (GetFocus () == this)
	{
		GetParentFrame()->SetFocus ();
	}

	if (m_hMenu != hMenu || IsCustomizeMode () || bForceUpdate)
	{
		g_menuHash.SaveMenuBar (m_hMenu, this);

		BOOL bMaximizeMode = m_bMaximizeMode;
		m_bMaximizeMode = FALSE;

		m_hMenu = hMenu;
		if (bDefaultMenu)
		{
			m_hDefaultMenu = hMenu;
		}

		if (!g_menuHash.LoadMenuBar (hMenu, this))
		{
			CMenu* pMenu = CMenu::FromHandle (hMenu);
			if (pMenu == NULL)
			{
				return;
			}

			RemoveAllButtons ();

			int iCount = (int) pMenu->GetMenuItemCount ();
			for (int i = 0; i < iCount; i ++)
			{
				UINT uiID = pMenu->GetMenuItemID (i);
				
				CString strText;
				pMenu->GetMenuString (i, strText, MF_BYPOSITION);
				
				switch (uiID)
				{
				case -1:	// Pop-up menu
					{
						CMenu* pPopupMenu = pMenu->GetSubMenu (i);
						ASSERT (pPopupMenu != NULL);

						CBCGToolbarMenuButton button (
							0, pPopupMenu->GetSafeHmenu (), -1, strText);
						button.m_bText = TRUE;
						button.m_bImage = FALSE;

						InsertButton (button);
					}
					break;

				case 0:		// Separator
					InsertSeparator ();
					break;

				default:	// Regular command
					{
						CBCGToolbarButton button (uiID, -1, strText);
						button.m_bText = TRUE;
						button.m_bImage = FALSE;

						InsertButton (button);
					}
					break;
				}
			}
		}

		if (bMaximizeMode)
		{
			CMDIFrameWnd* pParentFrame = DYNAMIC_DOWNCAST (CMDIFrameWnd, m_pParentWnd);
			if (pParentFrame != NULL)
			{
				SetMaximizeMode (TRUE, pParentFrame->MDIGetActive ());
			}
		}

		if (GetSafeHwnd () != NULL)
		{
			AdjustLayout ();
		}

		RebuildAccelerationKeys ();
	}
}
//***************************************************************************************
CSize CBCGMenuBar::CalcDynamicLayout (int nLength, DWORD dwMode)
{
	return CalcLayout(dwMode, nLength);
}
//**********************************************************************************
CSize CBCGMenuBar::CalcLayout(DWORD dwMode, int nLength)
{
	OnChangeHot (-1);

	//------------------------------------------
	// Is menu bar have the buttons with images?
	//------------------------------------------
	m_bHaveButtons = FALSE;

	for (POSITION pos = m_Buttons.GetHeadPosition (); pos != NULL;)
	{
		CBCGToolbarButton* pButton = (CBCGToolbarButton*) m_Buttons.GetNext (pos);
		ASSERT_VALID (pButton);

		if (!pButton->IsKindOf (RUNTIME_CLASS (CBCGToolbarMenuButtonsButton)) &&
			!pButton->IsKindOf (RUNTIME_CLASS (CBCGToolbarSystemMenuButton)) &&
			pButton->m_bImage && pButton->IsDrawImage ())
		{
			m_bHaveButtons = TRUE;
			break;
		}
	}

	if (m_pDockContext != NULL &&
		m_pDockContext->m_bDragging)
	{
		SetDragMode (TRUE);
	}

	if ((dwMode == (LM_HORZ | LM_MRUWIDTH)) ||
		(m_pDockContext != NULL && !m_pDockContext->m_bDragging &&
		IsFloating ()))
	{
		return CBCGToolBar::CalcLayout (dwMode, nLength);
	}

	BOOL bVert = (dwMode & LM_VERTDOCK) || ((dwMode & LM_HORZ) == 0);

	CRect rect; rect.SetRectEmpty ();
	
	if (m_pDockBar != NULL)
	{
		rect = ((CBCGDockBar*) m_pDockBar)->m_rectLayout;
	}

	if (rect.IsRectEmpty ())
	{
		CWnd* pFrame = GetOwner ();
		pFrame->GetClientRect(&rect);
	}

	CSize size;

	CRect rectClient;
	rectClient.SetRectEmpty();
	CalcInsideRect (rectClient, (dwMode & LM_HORZ));

	if (!bVert)
	{
		WrapToolBar (rect.Width ());

		//-------------------------------------
		// Calculate size again after wrapping:
		//-------------------------------------
		size = CalcSize (FALSE);
		size.cx = rect.Width () - rectClient.Width() / 2;
		size.cy -= rectClient.Height();
	}
	else
	{
		//-------------------------------------
		// Calculate size again after wrapping:
		//-------------------------------------
		size = CalcSize (TRUE);

		size.cy = rect.Height () - rectClient.Height() / 2;
		size.cx -= rectClient.Width();
	}

	//--------------------------------------------------
	// Something may changed, rebuild acceleration keys:
	//--------------------------------------------------
	RebuildAccelerationKeys ();
	
	return size;
}
//***************************************************************************************
void CBCGMenuBar::SetMaximizeMode (BOOL bMax, CWnd* pWnd)
{
	if (m_bMaximizeMode == bMax)
	{
		return;
	}

	if (bMax)
	{
		if (pWnd == NULL)
		{
			bMax = FALSE;
		}
		else
		{
            // get the sys menu.
			m_hSysMenu = pWnd->GetSystemMenu (FALSE)->GetSafeHmenu ();

            // If we have a system menu, then add a system menu button.
            if (m_hSysMenu != NULL)
            {
				m_hSysIcon = pWnd->GetIcon (FALSE);
				if (m_hSysIcon == NULL)
				{
					m_hSysIcon = (HICON) GetClassLongPtr (*pWnd, GCLP_HICONSM);
				}

                InsertButton (CBCGToolbarSystemMenuButton (m_hSysMenu, m_hSysIcon), 0);
            }

            LONG style = ::GetWindowLong(*pWnd, GWL_STYLE);

            // Assume no buttons.
            m_nSystemButtonsNum = 0;

            // Add a minimize box if required.
            if (style & WS_MINIMIZEBOX)
            {
			    InsertButton (CBCGToolbarMenuButtonsButton (SC_MINIMIZE));
                m_nSystemButtonsNum++;
            }

            // Add a restore box if required.
            if (style & WS_MAXIMIZEBOX)
            {
    			InsertButton (CBCGToolbarMenuButtonsButton (SC_RESTORE));
                m_nSystemButtonsNum++;
            }

            // Add a close box if required.
            if (m_hSysMenu != NULL)
            {
				//--------------------------------------------------------------
				// Jan Vasina: check if the maximized window has its system menu 
				// with the close button enabled:
				//--------------------------------------------------------------
				CBCGToolbarMenuButtonsButton closeButton (SC_CLOSE);

				MENUITEMINFO menuInfo;
				ZeroMemory(&menuInfo,sizeof(MENUITEMINFO));
				menuInfo.cbSize = sizeof(MENUITEMINFO);
				menuInfo.fMask = MIIM_STATE;

				::GetMenuItemInfo(m_hSysMenu, SC_CLOSE, FALSE, &menuInfo);

				if ((menuInfo.fState & MFS_GRAYED) || 
					(menuInfo.fState & MFS_DISABLED))
				{
					closeButton.m_nStyle |= TBBS_DISABLED;
				}

				InsertButton (closeButton);
				m_nSystemButtonsNum++;
            }
		}
	}
	else
	{
		m_nSystemButtonsNumSaved = m_nSystemButtonsNum;	// By Jon Wang

        // Remove first button if a system menu was added.
        if (m_hSysMenu != NULL)
        {
		    RemoveButton (0);
        }

		for (int i = 0; i < m_nSystemButtonsNum; i ++)
		{
			CBCGToolbarMenuButtonsButton* pButton = 
				(CBCGToolbarMenuButtonsButton*) m_Buttons.RemoveTail ();
			ASSERT_KINDOF (CBCGToolbarMenuButtonsButton, pButton);
			delete pButton;
		}
        // Now we habe no system buttons on the menu.
        m_nSystemButtonsNum = 0;
	}

	m_bMaximizeMode = bMax;
	AdjustLayout ();
}
//***************************************************************************************
void CBCGMenuBar::RestoreMaximizeMode ()
{
	if (m_bMaximizeMode)
	{
		return;
	}

	CBCGToolbarSystemMenuButton button (m_hSysMenu, m_hSysIcon);
	InsertButton (button, 0);
	
	InsertButton (CBCGToolbarMenuButtonsButton (SC_MINIMIZE));
	InsertButton (CBCGToolbarMenuButtonsButton (SC_RESTORE));
	InsertButton (CBCGToolbarMenuButtonsButton (SC_CLOSE));

	m_bMaximizeMode = TRUE;
	m_nSystemButtonsNum = m_nSystemButtonsNumSaved;

	GetParentFrame ()->RecalcLayout ();

	Invalidate ();
	UpdateWindow ();
}
//***************************************************************************************
void CBCGMenuBar::AdjustLocations ()
{
	CBCGToolBar::AdjustLocations ();

	if (!m_bMaximizeMode)
	{
		return;
	}

	CRect rectClient;
	GetClientRect (&rectClient);
	
	BOOL bHorz = m_dwStyle & CBRS_ORIENT_HORZ ? TRUE : FALSE;

	int iButtonWidth = m_szSystemButton.cx;
	int iButtonHeight = m_szSystemButton.cy;

	POSITION pos = m_Buttons.GetTailPosition ();
	CRect rect = rectClient;
	
	rectClient.SetRectEmpty ();
	CalcInsideRect (rectClient, bHorz);

	if (!bHorz)
	{
		rect.bottom += rectClient.Height ();
	}

	rect.left = rect.right - iButtonWidth;
	rect.top = rect.bottom - iButtonHeight;

	for (int i = 0; i < m_nSystemButtonsNum; i ++)
	{
		ASSERT (pos != NULL);

		CBCGToolbarMenuButtonsButton* pButton = 
			(CBCGToolbarMenuButtonsButton*) m_Buttons.GetPrev (pos);
		ASSERT_KINDOF (CBCGToolbarMenuButtonsButton, pButton);

		pButton->SetRect (rect);

		if (bHorz)
		{
			rect.OffsetRect (-iButtonWidth - 1, 0);
		}
		else
		{
			rect.OffsetRect (0, -iButtonHeight - 1);
		}
	}
}
//***************************************************************************************
BOOL CBCGMenuBar::OnSendCommand (const CBCGToolbarButton* pButton)
{
	CBCGToolbarMenuButtonsButton* pSysButton =
		DYNAMIC_DOWNCAST (CBCGToolbarMenuButtonsButton, pButton);
	if (pSysButton == NULL)
	{
		return FALSE;
	}

	if (pSysButton->m_uiSystemCommand != SC_CLOSE &&
		pSysButton->m_uiSystemCommand != SC_MINIMIZE &&
		pSysButton->m_uiSystemCommand != SC_RESTORE)
	{
		ASSERT (FALSE);
		return TRUE;
	}

	CMDIFrameWnd* pParentFrame = 
		DYNAMIC_DOWNCAST (CMDIFrameWnd, m_pParentWnd);

	if (pParentFrame == NULL)
	{
		MessageBeep ((UINT) -1);
		return TRUE;
	}
		
	CMDIChildWnd* pChild = pParentFrame->MDIGetActive ();
	ASSERT_VALID (pChild);

	pChild->SendMessage (WM_SYSCOMMAND, pSysButton->m_uiSystemCommand);
	return TRUE;
}
//*************************************************************************************
INT_PTR CBCGMenuBar::OnToolHitTest(CPoint point, TOOLINFO* pTI) const
{
	ASSERT_VALID(this);

	int nHit = ((CBCGMenuBar*)this)->HitTest(point);
	if (nHit != -1)
	{
		CBCGToolbarButton* pButton = GetButton (nHit);
		if (pButton == NULL ||
			pButton->IsKindOf (RUNTIME_CLASS (CBCGToolbarMenuButton)))
		{
			//-----------------------------------	
			// Don't show tooltips on menu items!
			//-----------------------------------
			return -1;
		}
	}

	return CBCGToolBar::OnToolHitTest (point, pTI);
}
//*************************************************************************************
int CBCGMenuBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CBCGToolBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	//------------------------------------
	// Attach menubar to the parent frame:
	//------------------------------------

	//----------------------
	// First, try MDI frame:
	//----------------------
	CBCGMDIFrameWnd* pWndParentMDIFrame = 
		DYNAMIC_DOWNCAST (CBCGMDIFrameWnd, m_pParentWnd);
	if (pWndParentMDIFrame != NULL)
	{
		pWndParentMDIFrame->m_Impl.SetMenuBar (this);
	}
	else
	{
		CBCGFrameWnd* pWndParentFrame = 
			DYNAMIC_DOWNCAST (CBCGFrameWnd, m_pParentWnd);
		if (pWndParentFrame != NULL)
		{
			pWndParentFrame->m_Impl.SetMenuBar (this);
		}
		else
		{
			CBCGOleIPFrameWnd* pOleFrame = 
				DYNAMIC_DOWNCAST (CBCGOleIPFrameWnd, GetParentFrame ());
			if (pOleFrame != NULL)
			{
				pOleFrame->m_Impl.SetMenuBar (this);
			}
		}
	}

	//----------------------------
	// Set default menu bar title:
	//----------------------------
	CBCGLocalResource locaRes;

	CString strTitle;
	strTitle.LoadString (IDS_BCGBARRES_MENU_BAR_TITLE);
		
	SetWindowText (strTitle);

	//-------------------------------------------------------------
	// Force the menu bar to be hiden whren the in-place editing is
	// is activated (server application shows its own menu):
	//-------------------------------------------------------------
	SetBarStyleEx (GetBarStyle() | CBRS_HIDE_INPLACE);

	//------------------------------
	// Calculate system button size:
	//------------------------------
	CalcSysButtonSize ();
	return 0;
}
//*************************************************************************************
BOOL CBCGMenuBar::LoadState (LPCTSTR lpszProfileName, int nIndex, UINT /*uiID*/)
{
	ASSERT (m_hDefaultMenu != NULL);

	CString strProfileName = ::BCGGetRegPath (strMenuProfile, lpszProfileName);

	//------------------------------------------------------------
	// Save current maximize mode (system buttons are not saved!):
	//------------------------------------------------------------
	BOOL bMaximizeMode = m_bMaximizeMode;
	SetMaximizeMode (FALSE);

	CDocManager* pDocManager = AfxGetApp ()->m_pDocManager;
	if (m_bAutoDocMenus && pDocManager != NULL)
	{
		//---------------------------------------
		// Walk all templates in the application:
		//---------------------------------------
		for (POSITION pos = pDocManager->GetFirstDocTemplatePosition (); pos != NULL;)
		{
			CBCGMultiDocTemplate* pTemplate = 
				(CBCGMultiDocTemplate*) pDocManager->GetNextDocTemplate (pos);
			ASSERT_VALID (pTemplate);
			ASSERT_KINDOF (CDocTemplate, pTemplate);

			//-----------------------------------------------------
			// We are interessing CMultiDocTemplate objects with
			// the sahred menu only....
			//-----------------------------------------------------
			if (!pTemplate->IsKindOf (RUNTIME_CLASS (CMultiDocTemplate)) ||
				pTemplate->m_hMenuShared == NULL)
			{
				continue;
			}

			UINT uiMenuResId = pTemplate->GetResId ();
			ASSERT (uiMenuResId != 0);

			//---------------------------------------------------------------
			// Load menubar from registry and associate it with the
			// template shared menu:
			//---------------------------------------------------------------
			if (CBCGToolBar::LoadState (strProfileName, nIndex, uiMenuResId))
			{
				g_menuHash.SaveMenuBar (pTemplate->m_hMenuShared, this);
			}
		}
	}

	//----------------------
	// Load defualt menubar:
	//----------------------
	if (CBCGToolBar::LoadState (strProfileName, nIndex, 0))
	{
		g_menuHash.SaveMenuBar (m_hDefaultMenu, this);
	}

	//----------------------
	// Restore current menu:
	//----------------------
	if (bMaximizeMode)
	{
		RestoreMaximizeMode ();
	}

	if (m_hMenu != NULL && g_menuHash.LoadMenuBar (m_hMenu, this))
	{
		GetParentFrame ()->RecalcLayout ();
		Invalidate ();
		UpdateWindow ();
	}

	RebuildAccelerationKeys ();
	return TRUE;
}
//*************************************************************************************
BOOL CBCGMenuBar::SaveState (LPCTSTR lpszProfileName, int nIndex, UINT /*uiID*/)
{
	ASSERT (m_hDefaultMenu != NULL);

	CString strProfileName = ::BCGGetRegPath (strMenuProfile, lpszProfileName);

	g_menuHash.SaveMenuBar (m_hMenu, this);

	//------------------------------------------------------------
	// Save current maximize mode (system buttons are not saved!):
	//------------------------------------------------------------
	BOOL bMaximizeMode = m_bMaximizeMode;
	SetMaximizeMode (FALSE);

	CDocManager* pDocManager = AfxGetApp ()->m_pDocManager;
	if (m_bAutoDocMenus && pDocManager != NULL)
	{
		//---------------------------------------
		// Walk all templates in the application:
		//---------------------------------------
		for (POSITION pos = pDocManager->GetFirstDocTemplatePosition (); pos != NULL;)
		{
			CBCGMultiDocTemplate* pTemplate = 
				(CBCGMultiDocTemplate*) pDocManager->GetNextDocTemplate (pos);
			ASSERT_VALID (pTemplate);
			ASSERT_KINDOF (CDocTemplate, pTemplate);

			//-----------------------------------------------------
			// We are interessing CMultiDocTemplate objects with
			// the sahred menu only....
			//-----------------------------------------------------
			if (!pTemplate->IsKindOf (RUNTIME_CLASS (CMultiDocTemplate)) ||
				pTemplate->m_hMenuShared == NULL)
			{
				continue;
			}

			UINT uiMenuResId = pTemplate->GetResId ();
			ASSERT (uiMenuResId != 0);

			//----------------------------------------------------------
			// Load menubar associated with the template shared menu and
			// save it in the registry:
			//----------------------------------------------------------
			if (g_menuHash.LoadMenuBar (pTemplate->m_hMenuShared, this))
			{
				CBCGToolBar::SaveState (strProfileName, nIndex, uiMenuResId);
			}
		}
	}

	//-------------------
	// Save default menu:
	//-------------------
	if (g_menuHash.LoadMenuBar (m_hDefaultMenu, this))
	{
		CBCGToolBar::SaveState (strProfileName, nIndex, 0);
	}

	//----------------------
	// Restore current menu:
	//----------------------
	if (m_hMenu != NULL && g_menuHash.LoadMenuBar (m_hMenu, this))
	{
		GetParentFrame ()->RecalcLayout ();
		Invalidate ();
		UpdateWindow ();
	}

	if (bMaximizeMode)
	{
		RestoreMaximizeMode ();
	}

	return TRUE;
}
//*****************************************************************************************
void CBCGMenuBar::OnLButtonDown(UINT nFlags, CPoint point) 
{
	int iHit = HitTest (point);

	//--------------------------------------------------------
	// Disable control bar dragging if any menues are dropped!
	//--------------------------------------------------------
	if (iHit < 0 &&	// Click outside of buttons
		GetDroppedDownMenu () != NULL)
	{
//		Do nothing
	}
	else
	{	
		CBCGToolBar::OnLButtonDown(nFlags, point);
	}
}
//****************************************************************************************
BOOL CBCGMenuBar::RestoreOriginalstate ()
{
	ASSERT (m_hMenu != NULL);
	HMENU hMenuCurr = m_hMenu;
	g_menuHash.SaveMenuBar (m_hMenu, this);

	BOOL bCurrMenuIsRestored = FALSE;

	//------------------------------------------------------------
	// Save current maximize mode (system buttons are not saved!):
	//------------------------------------------------------------
	BOOL bMaximizeMode = m_bMaximizeMode;
	SetMaximizeMode (FALSE);

	CDocManager* pDocManager = AfxGetApp ()->m_pDocManager;
	if (pDocManager != NULL)
	{
		//---------------------------------------
		// Walk all templates in the application:
		//---------------------------------------
		for (POSITION pos = pDocManager->GetFirstDocTemplatePosition (); pos != NULL;)
		{
			CBCGMultiDocTemplate* pTemplate = 
				(CBCGMultiDocTemplate*) pDocManager->GetNextDocTemplate (pos);
			ASSERT_VALID (pTemplate);
			ASSERT_KINDOF (CDocTemplate, pTemplate);

			//-----------------------------------------------------
			// We are interessing CMultiDocTemplate objects with
			// the shared menu only....
			//-----------------------------------------------------
			if (!pTemplate->IsKindOf (RUNTIME_CLASS (CMultiDocTemplate)) ||
				pTemplate->m_hMenuShared == NULL)
			{
				continue;
			}

			UINT uiMenuResId = pTemplate->GetResId ();
			ASSERT (uiMenuResId != 0);

			//-------------------------------------
			// Restore original menu from resource:
			//-------------------------------------
			HINSTANCE hInst = AfxFindResourceHandle (
				MAKEINTRESOURCE (uiMenuResId), RT_MENU);

			BOOL bCurr = (hMenuCurr == pTemplate->m_hMenuShared);
			pTemplate->m_hMenuShared = ::LoadMenu (hInst, MAKEINTRESOURCE (uiMenuResId));

			CreateFromMenu (pTemplate->m_hMenuShared, FALSE);
			g_menuHash.SaveMenuBar (pTemplate->m_hMenuShared, this);

			if (bCurr)
			{
				hMenuCurr = pTemplate->m_hMenuShared;
				bCurrMenuIsRestored = TRUE;
			}
		}
	}

	//----------------------
	// Load defualt menubar:
	//----------------------
	if (m_uiDefMenuResId != 0)
	{
		HINSTANCE hInst = AfxFindResourceHandle (
			MAKEINTRESOURCE (m_uiDefMenuResId), RT_MENU);

		m_hDefaultMenu = ::LoadMenu (hInst, MAKEINTRESOURCE (m_uiDefMenuResId));

		OnDefaultMenuLoaded(m_hDefaultMenu);

		CreateFromMenu (m_hDefaultMenu, TRUE);
		g_menuHash.SaveMenuBar (m_hDefaultMenu, this);

		if (!bCurrMenuIsRestored)
		{
			hMenuCurr = m_hDefaultMenu;
		}
	}

	//----------------------
	// Restore current menu:
	//----------------------
	if (g_menuHash.LoadMenuBar (hMenuCurr, this))
	{
		m_hMenu = hMenuCurr;

		if (!bMaximizeMode)
		{
			GetParentFrame ()->RecalcLayout ();
			Invalidate ();
			UpdateWindow ();
		}
	}

	if (bMaximizeMode)
	{
		RestoreMaximizeMode ();
	}

	return TRUE;
}
//********************************************************************************************
void CBCGMenuBar::SetDefaultMenuResId (UINT uiResId)
{
	m_uiDefMenuResId = uiResId;
}
//******************************************************************
BOOL CBCGMenuBar::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN)
	{
		//-----------------------------------------------------------
		// Fisrt, try to move keyboard control to the drop-down menu:
		//-----------------------------------------------------------
		CBCGToolbarMenuButton* pMenuButon = GetDroppedDownMenu ();
		if (pMenuButon != NULL)
		{
			return CControlBar::PreTranslateMessage(pMsg);
		}

		int iTotalItems = GetCount ();
		if (m_bMaximizeMode)
		{
			iTotalItems -= m_nSystemButtonsNum;
		}

		if (m_iHighlighted >= 0 && m_iHighlighted < iTotalItems)
		{
			int iButton = m_iHighlighted;

			switch (pMsg->wParam)
			{
			case VK_ESCAPE:
				{
					Deactivate ();
					RestoreFocus ();
					m_bShowAllCommands = FALSE;
				}
				break;

			case VK_RIGHT:
				if (++ m_iHighlighted >= iTotalItems)
				{
					m_iHighlighted = 0;
				}

				InvalidateButton (iButton);
				InvalidateButton (m_iHighlighted);
				UpdateWindow ();
				break;

			case VK_LEFT:
				if (-- m_iHighlighted < 0)
				{
					m_iHighlighted = iTotalItems - 1;
				}

				InvalidateButton (iButton);
				InvalidateButton (m_iHighlighted);
				UpdateWindow ();
				break;

			case VK_DOWN:
				DropDownMenu (GetButton (m_iHighlighted));
				return TRUE;

			case VK_RETURN:
				if (!DropDownMenu (GetButton (m_iHighlighted)))
				{
					ProcessCommand (GetButton (m_iHighlighted));
				}
				return TRUE;

			default:
				if (TranslateChar ((int) pMsg->wParam))
				{
					return TRUE;
				}
			}
		}
	}

	return CBCGToolBar::PreTranslateMessage(pMsg);
}
//**************************************************************************************
void CBCGMenuBar::OnSetFocus(CWnd* pOldWnd) 
{
	CBCGToolBar::OnSetFocus(pOldWnd);

	if (GetDroppedDownMenu () == NULL)
	{
		GetOwner()->SendMessage (WM_SETMESSAGESTRING, AFX_IDS_IDLEMESSAGE);

		int iFirstItem = (m_bMaximizeMode) ? 1 : 0;
		if (m_iHighlighted < 0 && iFirstItem < GetCount ())
		{
			m_iHighlighted = iFirstItem;
			InvalidateButton (iFirstItem);
		}
	}
}
//**************************************************************************************
BOOL CBCGMenuBar::Create(CWnd* pParentWnd,
			DWORD dwStyle,
			UINT nID)
{
	m_pParentWnd = pParentWnd;
	return CBCGToolBar::Create (pParentWnd, dwStyle, nID);
}
//***************************************************************************************
BOOL CBCGMenuBar::CreateEx(CWnd* pParentWnd, DWORD dwCtrlStyle,
		DWORD dwStyle,
		CRect rcBorders,
		UINT nID)
{
	m_pParentWnd = pParentWnd;
	return CBCGToolBar::CreateEx (pParentWnd, dwCtrlStyle, dwStyle, rcBorders, nID);
}
//***************************************************************************************
LRESULT CALLBACK CBCGMenuBar::BCGMenuBarMouseProc (int nCode, WPARAM wParam, LPARAM lParam)
{
	MOUSEHOOKSTRUCT* lpMS = (MOUSEHOOKSTRUCT*) lParam;
	ASSERT (lpMS != NULL);

	if (wParam == WM_LBUTTONUP || wParam == WM_NCLBUTTONUP)
	{
		if (m_pDraggedMenuBar != NULL &&
			m_pDraggedMenuBar->m_pDockContext != NULL &&
			m_pDraggedMenuBar->m_pDockContext->m_bDragging)
		{
			//------------------------------------------------------------------------
			// End of dragging. Don't allow the menubar to be docked on the right side
			// of any control bars!
			//------------------------------------------------------------------------
			m_pDraggedMenuBar->m_pDockContext->m_rectDragVert.top = 0;
			m_pDraggedMenuBar->m_pDockContext->m_rectDragHorz.left = 0;

			m_pDraggedMenuBar->SetDragMode (FALSE);
		}
	}

	return CallNextHookEx (m_hookMouseDrag, nCode, wParam, lParam);
}
//************************************************************************************************
void CBCGMenuBar::SetDragMode (BOOL bOn)
{
	if (bOn)
	{
		if (m_hookMouseDrag == NULL)	// Not installed yet, set it now!
		{
			m_hookMouseDrag = ::SetWindowsHookEx (WH_MOUSE, BCGMenuBarMouseProc, 
				0, GetCurrentThreadId ());
			if (m_hookMouseDrag == NULL)
			{
				TRACE (_T("CBCGMenuBar: Can't set mouse hook!\n"));
				return;
			}
		}

		m_pDraggedMenuBar = this;
	}
	else
	{
		 if (m_hookMouseDrag != NULL)
		 {
			::UnhookWindowsHookEx (m_hookMouseDrag);
			m_hookMouseDrag = NULL;
		}

		m_pDraggedMenuBar = NULL;
	}
}
//************************************************************************************************
void CBCGMenuBar::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos)
{
	CBCGToolBar::OnWindowPosChanged(lpwndpos);
	SetDragMode (FALSE);
}
//*****************************************************************************************
CSize CBCGMenuBar::CalcFixedLayout(BOOL bStretch, BOOL bHorz)
{
	if (m_Buttons.IsEmpty ())
	{
		return GetButtonSize ();
	}

	DWORD dwMode = bStretch ? LM_STRETCH : 0;
	dwMode |= bHorz ? LM_HORZ : 0;

	return CalcLayout (dwMode);
}
//*****************************************************************************************
BOOL CBCGMenuBar::OnSetDefaultButtonText (CBCGToolbarButton* pButton)
{
	ASSERT_VALID (pButton);

	CString strText;
	if (FindMenuItemText (m_hMenu, pButton->m_nID, strText))
	{
		pButton->m_strText = strText;
		return TRUE;
	}

	return CBCGToolBar::OnSetDefaultButtonText (pButton);
}
//****************************************************************************************
BOOL CBCGMenuBar::FindMenuItemText (HMENU hMenu, const UINT nItemID, CString& strOutText)
{
	if (hMenu == NULL || nItemID == 0 || nItemID == (UINT) -1)
	{
		return FALSE;
	}

	CMenu* pMenu = CMenu::FromHandle (hMenu);
	if (pMenu == NULL)
	{
		return FALSE;
	}

	int iCount = (int) pMenu->GetMenuItemCount ();
	for (int i = 0; i < iCount; i ++)
	{
		UINT uiID = pMenu->GetMenuItemID (i);
		if (uiID == nItemID)	// Found!
		{
			pMenu->GetMenuString (i, strOutText, MF_BYPOSITION);
			return TRUE;
		}
		else if (uiID == -1)	// Pop-up menu
		{
			CMenu* pPopupMenu = pMenu->GetSubMenu (i);
			ASSERT (pPopupMenu != NULL);

			if (FindMenuItemText (pPopupMenu->GetSafeHmenu (), nItemID, strOutText))
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}
//******************************************************************************************
int CBCGMenuBar::FindDropIndex (const CPoint point, CRect& rectDrag) const
{
	int iIndex = CBCGToolBar::FindDropIndex (point, rectDrag);
	if (m_bMaximizeMode && iIndex >= 0)
	{
		//--------------------------------------
		// Maybe drag left from the system icon?
		//--------------------------------------
		if (iIndex == 0)
		{
			return -1;
		}

		//-----------------------------------------
		// Maybe drag right of the system buttons?
		//-----------------------------------------
		if (iIndex > GetCount () - m_nSystemButtonsNum)
		{
			iIndex = GetCount () - m_nSystemButtonsNum;

			if (m_nSystemButtonsNum > 0)
			{
				//----------------------------------------------------------
				// Put drag rectangle right of the last "non-system" button:
				//----------------------------------------------------------

				CBCGToolbarButton* pLastButton = GetButton (iIndex - 1);
				ASSERT_VALID (pLastButton);

				CRect rectBtn = pLastButton->Rect ();
				CPoint ptDrag (rectBtn.right, rectBtn.top + rectBtn.Height () / 2);

				VERIFY (CBCGToolBar::FindDropIndex (ptDrag, rectDrag) == iIndex);
			}
		}
	}

	return iIndex;
}
//****************************************************************************************
void CBCGMenuBar::OnChangeHot (int iHot)
{
	CBCGToolBar::OnChangeHot (iHot);

	KillTimer (uiShowAllItemsTimerId);

	if (GetDroppedDownMenu () == NULL)
	{
		m_bShowAllCommands = FALSE;
	}
	else
	{
		SetTimer (uiShowAllItemsTimerId, iShowAllItemsTimerFreq, NULL);
	}
}
//****************************************************************************************
void CBCGMenuBar::SetShowAllCommands (BOOL bShowAllCommands)
{
	m_bShowAllCommands = bShowAllCommands;
}
//***************************************************************************************
CBCGToolbarButton* CBCGMenuBar::GetMenuItem (int iItem) const
{
	if (m_bMaximizeMode)
	{
		iItem --;	// Ignore system-menu button
	}

	return GetButton (iItem);
}
//***************************************************************************************
CBCGToolbarSystemMenuButton* CBCGMenuBar::GetSystemMenu () const
{
	if (!m_bMaximizeMode)
	{
		return NULL;
	}

	ASSERT (!m_Buttons.IsEmpty ());
	return DYNAMIC_DOWNCAST (CBCGToolbarSystemMenuButton, m_Buttons.GetHead ());
}
//***************************************************************************************
CBCGToolbarMenuButtonsButton* CBCGMenuBar::GetSystemButton (UINT uiBtn, BOOL bByCommand) const
{
	if (!m_bMaximizeMode)
	{
		return NULL;
	}

	if (bByCommand)
	{
		for (POSITION pos = m_Buttons.GetTailPosition (); pos != NULL;)
		{
			CBCGToolbarMenuButtonsButton* pButton = 
				DYNAMIC_DOWNCAST (CBCGToolbarMenuButtonsButton,
									m_Buttons.GetPrev (pos));

			if (pButton == NULL)
			{
				break;
			}

			if (pButton->m_nID == uiBtn)
			{
				return pButton;
			}
		}

		return NULL;
	}
	// else - by index:
	if ((int) uiBtn < 0 || (int) uiBtn >= m_nSystemButtonsNum)
	{
		ASSERT (FALSE);
		return NULL;
	}

	int iIndex = m_Buttons.GetCount () - m_nSystemButtonsNum + uiBtn;

	CBCGToolbarMenuButtonsButton* pButton = 
		DYNAMIC_DOWNCAST (CBCGToolbarMenuButtonsButton, GetButton (iIndex));
	ASSERT_VALID (pButton);

	return pButton;
}
//************************************************************************************
BOOL CBCGMenuBar::SetMenuFont (LPLOGFONT lpLogFont, BOOL bHorz)
{
	if (!globalData.SetMenuFont (lpLogFont, bHorz))
	{
		return FALSE;
	}
	
	//-------------------------------------------
	// Recalculate all toolbars and menus layout:
	//-------------------------------------------
	extern CObList gAllToolbars;

	for (POSITION pos = gAllToolbars.GetHeadPosition (); pos != NULL;)
	{
		CBCGToolBar* pToolBar = (CBCGToolBar*) gAllToolbars.GetNext (pos);
		ASSERT (pToolBar != NULL);

		if (CWnd::FromHandlePermanent (pToolBar->m_hWnd) != NULL)
		{
			ASSERT_VALID(pToolBar);
			pToolBar->AdjustLayout ();
		}
	}

	return TRUE;
}
//************************************************************************************
inline const CFont& CBCGMenuBar::GetMenuFont (BOOL bHorz)
{
	return bHorz ? globalData.fontRegular : globalData.fontVert;
}
//************************************************************************************
void CBCGMenuBar::OnTimer(UINT_PTR nIDEvent) 
{
	if (nIDEvent == uiShowAllItemsTimerId)
	{
		CPoint ptCursor;

		::GetCursorPos (&ptCursor);
		ScreenToClient (&ptCursor);

		//--------------------------------------------------------------
		// Check that the popup-menu is still exist and mouse cursor is
		// within the menu button:
		//--------------------------------------------------------------
		CBCGToolbarMenuButton* pMenuButon = GetDroppedDownMenu ();
		if (pMenuButon != NULL && pMenuButon->m_pPopupMenu != NULL &&
			pMenuButon->m_rect.PtInRect (ptCursor) &&
			!pMenuButon->m_pPopupMenu->AreAllCommandsShown ())
		{
			pMenuButon->m_pPopupMenu->ShowAllCommands ();
		}

		KillTimer (uiShowAllItemsTimerId);
	}
	
	CBCGToolBar::OnTimer(nIDEvent);
}
//*******************************************************************************************
void CBCGMenuBar::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CBCGToolBar::OnLButtonDblClk(nFlags, point);

	if (IsShowAllCommands () || IsCustomizeMode ())
	{
		return;
	}

	int iButton = HitTest(point);
	if (iButton < 0)
	{
		return;
	}

	CBCGToolbarMenuButton* pMenuButton = DYNAMIC_DOWNCAST (CBCGToolbarMenuButton, GetButton(iButton));
	if (pMenuButton == NULL)
	{
		return;
	}

	//////////////////////////////////////////////
	// Add by Yuhu.Wang on 99/11/04
	// Special deal to system menu button 
	//--------------------------------------------
	if(pMenuButton->IsKindOf (RUNTIME_CLASS (CBCGToolbarSystemMenuButton)))
	{
		return;
	}
	//--------------------------------------------
	//////////////////////////////////////////////

	m_bShowAllCommands = TRUE;
	pMenuButton->OnCancelMode ();

	if (pMenuButton->OnClick (this, FALSE))
	{
		OnChangeHot (iButton);

		InvalidateButton (iButton);
		UpdateWindow(); // immediate feedback
	}
}
//***********************************************************************************
void CBCGMenuBar::CalcSysButtonSize ()
{
	CWindowDC dc (NULL);

	CDC dcMem;
	dcMem.CreateCompatibleDC (NULL);	// Assume display!

	int iButtonWidth = ::GetSystemMetrics (SM_CXSIZE);
	int iButtonHeight = ::GetSystemMetrics (SM_CYSIZE);

	CBitmap bmpMem;
	bmpMem.CreateCompatibleBitmap (&dc, iButtonWidth, iButtonHeight);

	CBitmap* pBmpOriginal = dcMem.SelectObject (&bmpMem);

	CRect rectBtn (0, 0, iButtonWidth, iButtonHeight);
	dcMem.DrawFrameControl (rectBtn, DFC_CAPTION, DFCS_ADJUSTRECT);

	m_szSystemButton = rectBtn.Size ();
	dcMem.SelectObject (pBmpOriginal);
}
//*********************************************************************************
void CBCGMenuBar::OnSettingChange(UINT uFlags, LPCTSTR lpszSection) 
{
	CBCGToolBar::OnSettingChange(uFlags, lpszSection);
	
	CalcSysButtonSize ();
	Invalidate ();
	UpdateWindow ();
}

//****************************************************************************************
//andy
BOOL CBCGMenuBar::EnableContextMenuItems (CBCGToolbarButton* pButton, CMenu* pPopup)
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

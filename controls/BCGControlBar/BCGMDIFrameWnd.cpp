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

// BCGMDIFrameWnd.cpp : implementation file
//

#include "stdafx.h"
#include "BCGMDIFrameWnd.h"
#include "BCGToolbar.h"
#include "BCGMenuBar.h"
#include "BCGPopupMenu.h"
#include "BCGToolbarMenuButton.h"
#include "BCGSizingControlBar.h"
#include "bcglocalres.h"
#include "bcgbarres.h"
#include "BCGDockContext.h"
#include "BCGWindowsManagerDlg.h"
#include "BCGUserToolsManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBCGMDIFrameWnd

IMPLEMENT_DYNCREATE(CBCGMDIFrameWnd, CMDIFrameWnd)

#pragma warning (disable : 4355)

CBCGMDIFrameWnd::CBCGMDIFrameWnd() :
	m_Impl (this),
	m_hmenuWindow (NULL),
	m_bContextHelp (FALSE),
	m_bDoSubclass (TRUE),
	m_uiWindowsDlgMenuId (0),
	m_bShowWindowsDlgAlways (FALSE)
{
}

#pragma warning (default : 4355)

CBCGMDIFrameWnd::~CBCGMDIFrameWnd()
{
}


BEGIN_MESSAGE_MAP(CBCGMDIFrameWnd, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CBCGMDIFrameWnd)
	ON_WM_MENUCHAR()
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_ACTIVATE()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(BCGM_CREATETOOLBAR, OnToolbarCreateNew)
	ON_REGISTERED_MESSAGE(BCGM_DELETETOOLBAR, OnToolbarDelete)
	ON_COMMAND( ID_CONTEXT_HELP, OnContextHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBCGMDIFrameWnd message handlers

BOOL CBCGMDIFrameWnd::OnSetMenu (HMENU hmenu)
{
	if (m_Impl.m_pMenuBar != NULL)
	{
		m_Impl.m_pMenuBar->CreateFromMenu (hmenu == NULL ? m_Impl.m_hDefaultMenu : hmenu);
		return TRUE;
	}

	return FALSE;
}
//*******************************************************************************************
BOOL CBCGMDIFrameWnd::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	if (!CMDIFrameWnd::OnCreateClient(lpcs, pContext))
	{
		return FALSE;
	}

	if (m_bDoSubclass)
	{
		m_wndClientArea.SubclassWindow (m_hWndMDIClient);
	}

	return TRUE;
}
//*******************************************************************************************
LRESULT CBCGMDIFrameWnd::OnMenuChar(UINT nChar, UINT nFlags, CMenu* pMenu) 
{
	if (m_Impl.OnMenuChar (nChar))
	{
		return MAKELPARAM (MNC_EXECUTE, -1);
	}
		
	return CMDIFrameWnd::OnMenuChar(nChar, nFlags, pMenu);
}
//*******************************************************************************************
void CBCGMDIFrameWnd::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos) 
{
	CMDIFrameWnd::OnWindowPosChanged(lpwndpos);
	
	if (m_Impl.m_pMenuBar != NULL)
	{
		BOOL bMaximized;
		CMDIChildWnd* pChild = MDIGetActive (&bMaximized);

		if (pChild == NULL || !bMaximized)
		{
			m_Impl.m_pMenuBar->SetMaximizeMode (FALSE);
		}
		else
		{
			m_Impl.m_pMenuBar->SetMaximizeMode (TRUE, pChild);
		}
	}
}
//*******************************************************************************************
BOOL CBCGMDIFrameWnd::PreTranslateMessage(MSG* pMsg) 
{
	switch (pMsg->message)
	{
	case WM_SYSKEYDOWN:
	case WM_CONTEXTMENU:
		if (m_Impl.m_pActivePopupMenu != NULL &&
			::IsWindow (m_Impl.m_pActivePopupMenu->m_hWnd) &&
			pMsg->wParam == VK_MENU)
		{
			//andy
			if( !m_Impl.m_pActivePopupMenu->GetMenuBar()->IsApprenceDislpayed() )
				m_Impl.m_pActivePopupMenu->SendMessage (WM_CLOSE);
			return TRUE;
		}
		else if (m_Impl.ProcessKeyboard ((int) pMsg->wParam))
		{
			return TRUE;
		}
		break;

	case WM_SYSKEYUP:
//		if (m_Impl.m_pMenuBar != NULL)
		// Bt Sven Ritter
		if (m_Impl.m_pMenuBar != NULL && (VK_MENU == pMsg->wParam))
		{
			if (m_Impl.m_pMenuBar == GetFocus ())
			{
				SetFocus ();
			}
			else
			{
				m_Impl.m_pMenuBar->SetFocus ();
			}
			return TRUE;
		}

		if (m_Impl.m_pActivePopupMenu != NULL &&
			::IsWindow (m_Impl.m_pActivePopupMenu->m_hWnd))
		{
			return TRUE;	// To prevent system menu opening
		}
		break;

	case WM_KEYDOWN:
		//-----------------------------------------
		// Pass keyboard action to the active menu:
		//-----------------------------------------
		if (m_Impl.ProcessKeyboard ((int) pMsg->wParam))
		{
			return TRUE;
		}
		break;

	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
		{
			CPoint pt (LOWORD(pMsg->lParam), HIWORD(pMsg->lParam));
			CWnd* pWnd = CWnd::FromHandle(pMsg->hwnd);

			if (pWnd != NULL)
			{
				pWnd->ClientToScreen (&pt);
			}

			if (m_Impl.ProcessMouseClick (pMsg->message, pt, pMsg->hwnd))
			{
				return TRUE;
			}

			if (pMsg->message == WM_RBUTTONUP &&
				!CBCGToolBar::IsCustomizeMode ())
			{
				//---------------------------------------
				// Activate the control bar context menu:
				//---------------------------------------
				CDockBar* pBar = DYNAMIC_DOWNCAST(CDockBar, pWnd);
				if (pBar != NULL)
				{
					CPoint pt;

					pt.x = LOWORD(pMsg->lParam);
					pt.y = HIWORD(pMsg->lParam);
					pBar->ClientToScreen(&pt);

					SendMessage (BCGM_TOOLBARMENU,
								(WPARAM) GetSafeHwnd (),
								MAKELPARAM (pt.x, pt.y));

					return true; //AAM - bug fixed (иначе последний обработанный мессаг из popup menu дальше по цепочке шел)
				}
			}
		}
		break;

	case WM_NCLBUTTONDOWN:
	case WM_NCLBUTTONUP:
	case WM_NCRBUTTONDOWN:
	case WM_NCRBUTTONUP:
	case WM_NCMBUTTONDOWN:
	case WM_NCMBUTTONUP:
		if (m_Impl.ProcessMouseClick (pMsg->message,
			CPoint (LOWORD(pMsg->lParam), HIWORD(pMsg->lParam)),
			pMsg->hwnd))
		{
			return TRUE;
		}
		break;

	case WM_MOUSEMOVE:
		{
			CPoint pt (LOWORD(pMsg->lParam), HIWORD(pMsg->lParam));
			CWnd* pWnd = CWnd::FromHandle(pMsg->hwnd);
			if (pWnd != NULL)
			{
				pWnd->ClientToScreen (&pt);
			}

			if (m_Impl.ProcessMouseMove (pt))
			{
				return TRUE;
			}
		}
	}

	return CMDIFrameWnd::PreTranslateMessage(pMsg);
}
//*******************************************************************************************
BOOL CBCGMDIFrameWnd::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if (HIWORD (wParam) == 1)
	{
		UINT uiCmd = LOWORD (wParam);

		CBCGToolBar::AddCommandUsage (uiCmd);

		//---------------------------
		// Simmulate ESC keystroke...
		//---------------------------
		if (m_Impl.ProcessKeyboard (VK_ESCAPE))
		{
			return TRUE;
		}

		if (g_pUserToolsManager != NULL &&
			g_pUserToolsManager->InvokeTool (uiCmd))
		{
			return TRUE;
		}
	}

	if (!CBCGToolBar::IsCustomizeMode ())
	{
		return CMDIFrameWnd::OnCommand(wParam, lParam);
	}

	return FALSE;
}
//*******************************************************************************************
HMENU CBCGMDIFrameWnd::GetWindowMenuPopup (HMENU hMenuBar)
{
	m_hmenuWindow = CMDIFrameWnd::GetWindowMenuPopup (hMenuBar);
	return m_hmenuWindow;
}
//********************************************************************************************
BOOL CBCGMDIFrameWnd::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext) 
{
	m_Impl.m_nIDDefaultResource = nIDResource;
	
	if (!CMDIFrameWnd::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
	{
		return FALSE;
	}

	m_Impl.OnLoadFrame ();
	return TRUE;
}
//*******************************************************************************************
void CBCGMDIFrameWnd::OnClose() 
{
	m_Impl.OnCloseFrame();	
	CMDIFrameWnd::OnClose();
}
//*******************************************************************************************
BOOL CBCGMDIFrameWnd::PreCreateWindow(CREATESTRUCT& cs) 
{
	m_Impl.RestorePosition(cs);	
	return CMDIFrameWnd::PreCreateWindow(cs);
}
//*******************************************************************************************
BOOL CBCGMDIFrameWnd::ShowPopupMenu (CBCGPopupMenu* pMenuPopup)
{
	if (!m_Impl.OnShowPopupMenu (pMenuPopup, this))
	{
		return FALSE;
	}

	if (!CBCGToolBar::IsCustomizeMode () && m_hmenuWindow != NULL &&
		pMenuPopup != NULL && pMenuPopup->GetMenu () != NULL)
	{
		//-----------------------------------------------------------
		// Check the popup menu for the "Windows..." menu maching...:
		//-----------------------------------------------------------
		HMENU hMenuPop = pMenuPopup->GetMenuBCG();
		BOOL bIsWindowMenu = FALSE;

		int iItemMax = ::GetMenuItemCount (hMenuPop);
		for (int iItemPop = 0; !bIsWindowMenu && iItemPop < iItemMax; iItemPop ++)
		{
			UINT nID = ::GetMenuItemID( hMenuPop, iItemPop);
			bIsWindowMenu =  (nID >= AFX_IDM_WINDOW_FIRST && nID <= AFX_IDM_WINDOW_LAST);
		}

		if (bIsWindowMenu)
		{
			CMenu* pMenu = CMenu::FromHandle (m_hmenuWindow);
			if (pMenu != NULL)
			{
				int iCount = (int) pMenu->GetMenuItemCount ();
				BOOL bIsFirstWindowItem = TRUE;
				BOOL bIsStandradWindowsDlg = FALSE;

				for (int i = 0; i < iCount; i ++)
				{
					UINT uiCmd = pMenu->GetMenuItemID (i);
					if (uiCmd < AFX_IDM_FIRST_MDICHILD || uiCmd == (UINT) -1)
					{
						continue;
					}

					if (m_uiWindowsDlgMenuId != 0 &&
						uiCmd == AFX_IDM_FIRST_MDICHILD + 9)
					{
						// Don't add standrd "Windows..." command
						bIsStandradWindowsDlg = TRUE;
						continue;
					}

					if (bIsFirstWindowItem)
					{
						pMenuPopup->InsertSeparator ();
						bIsFirstWindowItem = FALSE;
					}

					CString strText;
					pMenu->GetMenuString (i, strText, MF_BYPOSITION);

					CBCGToolbarMenuButton button (uiCmd, NULL /* No submenus - assume */, 
													-1, strText);

					UINT uiState = pMenu->GetMenuState (i, MF_BYPOSITION);
					if (uiState & MF_CHECKED)
					{
						button.m_nStyle |= TBBS_CHECKED;
					}

					pMenuPopup->InsertItem (button);
				}

				if (m_uiWindowsDlgMenuId != 0 &&
					(bIsStandradWindowsDlg || m_bShowWindowsDlgAlways))
				{
					if (!CBCGToolBar::GetBasicCommands ().IsEmpty ())
					{
						CBCGToolBar::AddBasicCommand (m_uiWindowsDlgMenuId);
					}

					//-----------------------------
					// Add our "Windows..." dialog:
					//-----------------------------
					pMenuPopup->InsertItem (
						CBCGToolbarMenuButton (m_uiWindowsDlgMenuId,
								NULL, -1, m_strWindowsDlgMenuText));
				}
			}
		}
	}

	if (pMenuPopup != NULL && pMenuPopup->m_bShown)
	{
		return TRUE;
	}

	return OnShowPopupMenu (pMenuPopup);
}
//**********************************************************************************
void CBCGMDIFrameWnd::OnClosePopupMenu (CBCGPopupMenu* pMenuPopup)
{
	if (m_Impl.m_pActivePopupMenu == pMenuPopup)
	{
		m_Impl.m_pActivePopupMenu = NULL;
	}
}
//*******************************************************************************************
LRESULT CBCGMDIFrameWnd::OnToolbarCreateNew(WPARAM,LPARAM lp)
{
	ASSERT (lp != NULL);
	CBCGToolBar *ptoolbar = (CBCGToolBar *)m_Impl.CreateNewToolBar ((LPCTSTR) lp);
	if (!IsVerticalDockEnabled())
		ptoolbar->EnableDocking(CBRS_ALIGN_TOP|CBRS_ALIGN_BOTTOM);
	return (LRESULT) ptoolbar;
}
//***************************************************************************************
LRESULT CBCGMDIFrameWnd::OnToolbarDelete(WPARAM,LPARAM lp)
{
	CBCGToolBar* pToolbar = (CBCGToolBar*) lp;
	ASSERT_VALID (pToolbar);

	return (LRESULT) m_Impl.DeleteToolBar (pToolbar);
}
//***************************************************************************************
void CBCGMDIFrameWnd::WinHelp(DWORD dwData, UINT nCmd) 
{
	if (dwData > 0 || !m_bContextHelp)
	{
		CMDIFrameWnd::WinHelp(dwData, nCmd);
	}
	else
	{
		OnContextHelp ();
	}
}
//***************************************************************************************
void CBCGMDIFrameWnd::OnContextHelp ()
{
	m_bContextHelp = TRUE;

	if (!m_bHelpMode && CanEnterHelpMode())
	{
		CBCGToolBar::SetHelpMode ();
	}

	CMDIFrameWnd::OnContextHelp ();

	if (!m_bHelpMode)
	{
		CBCGToolBar::SetHelpMode (FALSE);
	}

	m_bContextHelp = FALSE;
}
//****************************************************************************************
void CBCGMDIFrameWnd::EnableDocking (DWORD dwDockStyle)
{
	m_Impl.FrameEnableDocking (this, dwDockStyle);
	m_pFloatingFrameClass = RUNTIME_CLASS(CBCGMiniDockFrameWnd);
}
//***************************************************************************************
void CBCGMDIFrameWnd::DockControlBarLeftOf (CControlBar* pBar, CControlBar* pLeftOf)
{
	m_Impl.DockControlBarLeftOf (pBar, pLeftOf);
}
//***************************************************************************************
void CBCGMDIFrameWnd::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	CMDIFrameWnd::OnActivate(nState, pWndOther, bMinimized);
	
	if (nState == WA_CLICKACTIVE)
	{
		UpdateWindow ();
	}
}
//***************************************************************************************
void CBCGMDIFrameWnd::EnableWindowsDialog (UINT uiMenuId, 
										   LPCTSTR lpszMenuText,
										   BOOL bShowAllways)
{
	ASSERT (lpszMenuText != NULL);
	ASSERT (uiMenuId != 0);

	m_uiWindowsDlgMenuId = uiMenuId;
	m_strWindowsDlgMenuText = lpszMenuText;
	m_bShowWindowsDlgAlways = bShowAllways;
}
//****************************************************************************
void CBCGMDIFrameWnd::ShowWindowsDialog ()
{
	CBCGLocalResource locaRes;

	CBCGWindowsManagerDlg dlg (this);
	dlg.DoModal ();
}

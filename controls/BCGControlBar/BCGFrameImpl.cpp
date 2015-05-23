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

// BCGFrameImpl.cpp: implementation of the CBCGFrameImpl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "BCGFrameImpl.h"
#include "BCGToolBar.h"
#include "BCGDialogBar.h"
#include "BCGSizingControlBar.h"
#include "BCGMenuBar.h"
#include "bcglocalres.h"
#include "bcgbarres.h"
#include "BCGPopupMenu.h"
#include "BCGToolbarMenuButton.h"
#include "BCGWorkspace.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CObList	gAllToolbars;
extern CBCGWorkspace* g_pWorkspace;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//andy
__declspec( dllexport )
CRuntimeClass *g_pclassToolbarRuntime = RUNTIME_CLASS( CBCGToolBar );
__declspec( dllexport )
CRuntimeClass *g_pclassToolbarMenuButtonRuntime = RUNTIME_CLASS( CBCGToolbarMenuButton );
__declspec( dllexport )
CRuntimeClass *g_pclassDockBarRuntime = RUNTIME_CLASS( CBCGToolDockBar );


CBCGFrameImpl::CBCGFrameImpl(CFrameWnd* pFrame) :
	m_pFrame (pFrame),
	m_pActivePopupMenu (NULL),
	m_uiUserToolbarFirst ((UINT)-1),
	m_uiUserToolbarLast ((UINT)-1),
	m_pMenuBar (NULL),
	m_hDefaultMenu (NULL),
	m_nIDDefaultResource (0)
{
	ASSERT_VALID (m_pFrame);
}
//**************************************************************************************
CBCGFrameImpl::~CBCGFrameImpl()
{
	//-----------------------------
	// Clear user-defined toolbars:
	//-----------------------------
	while (!m_listUserDefinedToolbars.IsEmpty ())
	{
		delete m_listUserDefinedToolbars.RemoveHead ();
	}
}
//**************************************************************************************
void CBCGFrameImpl::OnCloseFrame()
{
	//----------------------------------------------------------------------
	// Automatically load state and frame position if CBCGWorkspace is used:
	//----------------------------------------------------------------------
	if (g_pWorkspace != NULL)
	{
		g_pWorkspace->OnClosingMainFrame (this);

		//---------------------------
		// Store the Windowplacement:
		//---------------------------
		if (::IsWindow (m_pFrame->GetSafeHwnd ()))
		{
			WINDOWPLACEMENT wp;
			wp.length = sizeof (WINDOWPLACEMENT);

			if (m_pFrame->GetWindowPlacement (&wp))
			{
				//---------------------------
				// Make sure we don't pop up 
				// minimized the next time
				//---------------------------
				if (wp.showCmd != SW_SHOWMAXIMIZED)
				{
					wp.showCmd = SW_SHOWNORMAL;
				}

				g_pWorkspace->StoreWindowPlacement (
					wp.rcNormalPosition, wp.flags, wp.showCmd);
			}
		}

	}
}
//**************************************************************************************
void CBCGFrameImpl::RestorePosition(CREATESTRUCT& cs)
{
	if (g_pWorkspace != NULL)
	{
		CRect rectNormal (CPoint (cs.x, cs.y), CSize (cs.cx, cs.cy));
		int nFlags = 0;
		int nShowCmd = SW_SHOWNORMAL;

		if (!g_pWorkspace->LoadWindowPlacement (rectNormal, nFlags, nShowCmd))
		{
			return;
		}

		if (nShowCmd != SW_MAXIMIZE)
		{
			nShowCmd = SW_SHOWNORMAL;
		}

		switch (AfxGetApp()->m_nCmdShow)
		{
		case SW_MAXIMIZE:
		case SW_MINIMIZE:
			break;	// don't change!

		default:
			AfxGetApp()->m_nCmdShow = nShowCmd;
		}

		CRect rectDesktop;
		CRect rectInter;

		if (::SystemParametersInfo (SPI_GETWORKAREA, 0, &rectDesktop, 0) &&
			rectInter.IntersectRect (&rectDesktop, &rectNormal))
		{
			cs.x = rectNormal.left;
			cs.y = rectNormal.top;
			cs.cx = rectNormal.Width ();
			cs.cy = rectNormal.Height ();
		}
	}
}
//**************************************************************************************
void CBCGFrameImpl::OnLoadFrame()
{
	//---------------------------------------------------
	// Automatically load state if CBCGWorkspace is used:
	//---------------------------------------------------
	if (g_pWorkspace != NULL)
	{
		g_pWorkspace->LoadState (0, this);
	}
}
//**************************************************************************************
void CBCGFrameImpl::LoadUserToolbars ()
{
	if (m_uiUserToolbarFirst == (UINT) -1 ||
		m_uiUserToolbarLast == (UINT) -1)
	{
		return;
	}

	for (UINT uiNewToolbarID = m_uiUserToolbarFirst;
		uiNewToolbarID <= m_uiUserToolbarLast;
		uiNewToolbarID ++)
	{
		CBCGToolBar* pNewToolbar = new CBCGToolBar;
		if (!pNewToolbar->Create (m_pFrame, 
			dwDefaultToolbarStyle,
			uiNewToolbarID))
		{
			TRACE0 ("Failed to create a new toolbar!\n");
			delete pNewToolbar;
			continue;
		}

		if (!pNewToolbar->LoadState (m_strControlBarRegEntry))
		{
			pNewToolbar->DestroyWindow ();
			delete pNewToolbar;
		}
		else
		{
			pNewToolbar->SetBarStyleEx (pNewToolbar->GetBarStyle () |
				CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
			pNewToolbar->EnableDocking (CBRS_ALIGN_ANY);

			m_pFrame->DockControlBar (pNewToolbar);
			m_listUserDefinedToolbars.AddTail (pNewToolbar);
		}
	}
}
//**********************************************************************************************
void CBCGFrameImpl::SaveUserToolbars ()
{
	for (POSITION pos = m_listUserDefinedToolbars.GetHeadPosition (); pos != NULL;)
	{
		CBCGToolBar* pUserToolBar = 
			(CBCGToolBar*) m_listUserDefinedToolbars.GetNext (pos);
		ASSERT_VALID(pUserToolBar);

		pUserToolBar->SaveState (m_strControlBarRegEntry);
	}
}
//**********************************************************************************************
CBCGToolBar* CBCGFrameImpl::GetUserBarByIndex (int iIndex) const
{
	POSITION pos = m_listUserDefinedToolbars.FindIndex (iIndex);
	if (pos == NULL)
	{
		return NULL;
	}

	CBCGToolBar* pUserToolBar = 
		(CBCGToolBar*) m_listUserDefinedToolbars.GetAt (pos);
	ASSERT_VALID (pUserToolBar);

	return pUserToolBar;
}
//**********************************************************************************************
BOOL CBCGFrameImpl::IsUserDefinedToolbar (const CBCGToolBar* pToolBar) const
{
	ASSERT_VALID (pToolBar);

	UINT uiCtrlId = pToolBar->GetDlgCtrlID ();
	return	uiCtrlId >= m_uiUserToolbarFirst &&
			uiCtrlId <= m_uiUserToolbarLast;
}
//*******************************************************************************************
BOOL CBCGFrameImpl::IsDockStateValid (const CDockState& state)
{
	//----------------------------------------------------------------
	// This function helps to avoid GPF during CFrameWnd::LoadBarState
	// execution: when one of the previously saved toolbars is not
	// created, LoadBarState fails.
	//----------------------------------------------------------------

	for (int i = 0; i < state.m_arrBarInfo.GetSize (); i ++)
	{
		CControlBarInfo* pInfo = (CControlBarInfo*) state.m_arrBarInfo [i];
		ASSERT (pInfo != NULL);

		if (!pInfo->m_bFloating)
		{
			//---------------------------------------
			// Find the control bar with the same ID:
			//---------------------------------------
			if (m_pFrame->GetControlBar (pInfo->m_nBarID) == NULL)
			{
				TRACE (_T("CBCGFrameImpl::IsDockStateValid ControlBar %x is not valid!\n"), pInfo->m_nBarID);
				return FALSE;
			}
		}
	}

	return TRUE;
}
//**********************************************************************************
void CBCGFrameImpl::InitUserToobars (	LPCTSTR lpszRegEntry,
										UINT uiUserToolbarFirst, 
										UINT uiUserToolbarLast)
{
	ASSERT (uiUserToolbarLast >= uiUserToolbarFirst);

	if (uiUserToolbarFirst == (UINT) -1 ||
		uiUserToolbarFirst == (UINT) -1)
	{
		ASSERT (FALSE);
		return;
	}

	m_uiUserToolbarFirst = uiUserToolbarFirst;
	m_uiUserToolbarLast = uiUserToolbarLast;

	// ET: get Path automatically from workspace if needed
	m_strControlBarRegEntry = (lpszRegEntry == NULL) ? 
		( g_pWorkspace ? g_pWorkspace->GetRegSectionPath() : _T("") )
		: lpszRegEntry;
}
//**************************************************************************************
const CBCGToolBar* CBCGFrameImpl::CreateNewToolBar (LPCTSTR lpszName)
{
	ASSERT (lpszName != NULL);

	if (m_uiUserToolbarFirst == (UINT)-1 ||
		m_uiUserToolbarLast == (UINT)-1)
	{
		TRACE (_T("User-defined toolbars IDs are none defined!\n"));
		return NULL;
	}

	int iMaxToolbars = m_uiUserToolbarLast - m_uiUserToolbarFirst + 1;
	if (m_listUserDefinedToolbars.GetCount () == iMaxToolbars)
	{
		CBCGLocalResource locaRes;

		CString strError;
		strError.Format (IDS_BCGBARRES_TOO_MANY_TOOLBARS_FMT, iMaxToolbars);

		AfxMessageBox (strError, MB_OK | MB_ICONASTERISK);
		return NULL;
	}

	//-----------------------
	// Find a new toolbar id:
	//-----------------------
	UINT uiNewToolbarID;

	for (uiNewToolbarID = m_uiUserToolbarFirst;
		uiNewToolbarID <= m_uiUserToolbarLast;
		uiNewToolbarID ++)
	{
		BOOL bUsed = FALSE;

		for (POSITION pos = m_listUserDefinedToolbars.GetHeadPosition (); 
			!bUsed && pos != NULL;)
		{
			CBCGToolBar* pUserToolBar = 
				(CBCGToolBar*) m_listUserDefinedToolbars.GetNext (pos);
			ASSERT_VALID(pUserToolBar);

			bUsed = (pUserToolBar->GetDlgCtrlID () == (int) uiNewToolbarID);
		}

		if (!bUsed)
		{
			break;
		}
	}

	if (uiNewToolbarID >= m_uiUserToolbarLast)
	{
		TRACE0 ("Can't find non-used toolbar number!\n");
		return NULL;
	}

//[ay]
	CBCGToolBar* pNewToolbar = (CBCGToolBar* )g_pclassToolbarRuntime->CreateObject();
	if (!pNewToolbar->Create (m_pFrame,
		dwDefaultToolbarStyle,
		uiNewToolbarID,
		lpszName))
	{
		TRACE0 ("Failed to create a new toolbar!\n");
		delete pNewToolbar;
		return NULL;
	}

	pNewToolbar->SetWindowText (lpszName);

	pNewToolbar->SetBarStyleEx (pNewToolbar->GetBarStyle () |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	pNewToolbar->EnableDocking (CBRS_ALIGN_ANY);
	
	m_pFrame->FloatControlBar (pNewToolbar,
		CPoint (::GetSystemMetrics (SM_CXFULLSCREEN) / 2, 
				::GetSystemMetrics (SM_CYFULLSCREEN) / 2));
	m_pFrame->RecalcLayout ();

	m_listUserDefinedToolbars.AddTail (pNewToolbar);
	return pNewToolbar;
}
//**************************************************************************************
BOOL CBCGFrameImpl::DeleteToolBar (CBCGToolBar* pToolBar)
{
	ASSERT_VALID (pToolBar);
	
	POSITION pos = m_listUserDefinedToolbars.Find (pToolBar);
	if (pos == NULL)
	{
		return FALSE;
	}

	m_listUserDefinedToolbars.RemoveAt (pos);
	pToolBar->RemoveStateFromRegistry (m_strControlBarRegEntry);

	if( ::IsWindow( pToolBar->m_hWnd ) )
		pToolBar->DestroyWindow ();
	delete pToolBar;

	m_pFrame->RecalcLayout ();
	return TRUE;
}
//*******************************************************************************************
void CBCGFrameImpl::SetMenuBar (CBCGMenuBar* pMenuBar)
{
	ASSERT_VALID (pMenuBar);
	ASSERT (m_pMenuBar == NULL);	// Method should be called once!

	m_pMenuBar = pMenuBar;

	m_hDefaultMenu=*m_pFrame->GetMenu();

	// ET: Better support for dynamic menu
	m_pMenuBar->OnDefaultMenuLoaded (m_hDefaultMenu);
	m_pMenuBar->CreateFromMenu (m_hDefaultMenu, TRUE /* Default menu */);

	m_pFrame->SetMenu (NULL);
	
	m_pMenuBar->SetDefaultMenuResId (m_nIDDefaultResource);
}
//*******************************************************************************************
BOOL CBCGFrameImpl::ProcessKeyboard (int nKey)
{
	if (m_pActivePopupMenu != NULL &&
		::IsWindow (m_pActivePopupMenu->m_hWnd))
	{
		//andy
		if( !m_pActivePopupMenu->GetMenuBar()->IsApprenceDislpayed() )
		{
			m_pActivePopupMenu->SendMessage (WM_KEYDOWN, nKey);
			return TRUE;
		}
	}

	if (!CBCGToolBar::IsCustomizeMode () &&
		(::GetAsyncKeyState (VK_MENU) & 0x8000) &&	// ALT is pressed
		nKey!=VK_MENU ) // 28.07.2005 build 88: ~BT 4091 - при нажатии на Alt меню активизилось или деактивизилось, а надо только при отпускании
	{
		if (OnMenuChar (nKey))
		{
			return TRUE;
		}

		ASSERT_VALID (m_pFrame);
		m_pFrame->SetFocus ();
	}

	return FALSE;
}
//*******************************************************************************************
BOOL CBCGFrameImpl::ProcessMouseClick (UINT uiMsg, POINT pt, HWND hwnd)
{
	//------------------------------------------------
	// Maybe user start drag the button with control?
	//------------------------------------------------
	if (uiMsg == WM_LBUTTONDOWN &&
		(CBCGToolBar::IsCustomizeMode () ||
		(::GetAsyncKeyState (VK_MENU) & 0x8000)))	// ALT is pressed
	{
		for (POSITION posTlb = gAllToolbars.GetHeadPosition (); posTlb != NULL;)
		{
			CBCGToolBar* pToolBar = (CBCGToolBar*) gAllToolbars.GetNext (posTlb);
			ASSERT (pToolBar != NULL);

			if (CWnd::FromHandlePermanent (pToolBar->m_hWnd) != NULL)
			{
				ASSERT_VALID(pToolBar);

				CPoint ptToolBar = pt;
				pToolBar->ScreenToClient (&ptToolBar);

				int iHit = pToolBar->HitTest (ptToolBar);
				if (iHit >= 0)
				{
					CBCGToolbarButton* pButton = pToolBar->GetButton (iHit);
					ASSERT_VALID (pButton);

					if (pButton->GetHwnd () != NULL &&
						pButton->GetHwnd () == hwnd &&
						pButton->Rect ().PtInRect (ptToolBar))
					{
						pToolBar->SendMessage (WM_LBUTTONDOWN, 
							0, MAKELPARAM (ptToolBar.x, ptToolBar.y));
						return TRUE;
					}

					break;
				}
			}
		}
	}

	if (!CBCGToolBar::IsCustomizeMode () &&
		m_pActivePopupMenu != NULL &&
		::IsWindow (m_pActivePopupMenu->m_hWnd))
	{
		CRect rectMenu;
		m_pActivePopupMenu->GetWindowRect (rectMenu);

		if (!rectMenu.PtInRect (pt))
		{
			//--------------------------------------------
			// Maybe secondary click on the parent button?
			//--------------------------------------------
			CBCGToolbarMenuButton* pParentButton = 
				m_pActivePopupMenu->GetParentButton ();
			if (pParentButton != NULL)
			{
				CWnd* pWndParent = pParentButton->GetParentWnd ();
				if (pWndParent != NULL)
				{
					CPoint ptClient = pt;
					pWndParent->ScreenToClient (&ptClient);

					if (pParentButton->Rect ().PtInRect (ptClient))
					{
						//-----------------------------
						// Secondary click, do nothing!
						//-----------------------------
						if (!pWndParent->IsKindOf (RUNTIME_CLASS (CBCGPopupMenuBar)))
						{
							m_pActivePopupMenu->SendMessage (WM_CLOSE);
							m_pMenuBar->Deactivate(); // 28.07.2005 build 88, BT 4091
						}

						return TRUE;
					}
				}
			}

			m_pActivePopupMenu->SendMessage (WM_CLOSE);

			CWnd* pWndFocus = CWnd::GetFocus ();
			if (pWndFocus != NULL && pWndFocus->IsKindOf (RUNTIME_CLASS (CBCGToolBar)))
			{
				m_pFrame->SetFocus ();
			}
		}
	}

	return FALSE;
}
//*******************************************************************************************
BOOL CBCGFrameImpl::ProcessMouseMove (POINT pt)
{
	if (!CBCGToolBar::IsCustomizeMode () &&
		m_pActivePopupMenu != NULL)
	{
		CRect rectMenu;
		m_pActivePopupMenu->GetWindowRect (rectMenu);

		if (rectMenu.PtInRect (pt) ||
			m_pActivePopupMenu->GetMenuBar ()->FindDestBar (pt) != NULL)
		{
			return FALSE;	// Default processing
		}

		return TRUE;		// Active menu "capturing"
	}

	return FALSE;	// Default processing
}
//*******************************************************************************************
BOOL CBCGFrameImpl::OnShowPopupMenu (CBCGPopupMenu* pMenuPopup, CFrameWnd* /*pWndFrame*/)
{
	m_pActivePopupMenu = pMenuPopup;

	if (pMenuPopup != NULL && !CBCGToolBar::IsCustomizeMode ())
	{
		ASSERT_VALID (pMenuPopup);

		CControlBar* pTopLevelBar = NULL;

		for (CBCGPopupMenu* pMenu = pMenuPopup; pMenu != NULL;
			pMenu = pMenu->GetParentPopupMenu ())
		{
			CBCGToolbarMenuButton* pParentButton = pMenu->GetParentButton ();
			if (pParentButton == NULL)
			{
				break;
			}
		
			pTopLevelBar = 
				DYNAMIC_DOWNCAST (CControlBar, pParentButton->GetParentWnd ());
		}

		if (pTopLevelBar != NULL && 
			!pTopLevelBar->IsKindOf (RUNTIME_CLASS (CBCGPopupMenuBar)))
		{
			ASSERT_VALID (pTopLevelBar);

			if (!pTopLevelBar->IsFloating () &&
				::GetFocus () != pTopLevelBar->GetSafeHwnd ())
			{
				pTopLevelBar->SetFocus ();
			}
		}
	}

	return TRUE;
}
//*************************************************************************************
void CBCGFrameImpl::DockControlBarLeftOf(CControlBar* pBar, CControlBar* pLeftOf)
{
	ASSERT_VALID (m_pFrame);
	ASSERT_VALID (pBar);
	ASSERT_VALID (pLeftOf);

	CRect rect;
	DWORD dw;
	UINT n;
	
	// get MFC to adjust the dimensions of all docked ToolBars
	// so that GetWindowRect will be accurate
	m_pFrame->RecalcLayout(TRUE);
	
	pLeftOf->GetWindowRect(&rect);
	rect.OffsetRect(1,1);
	dw=pLeftOf->GetBarStyle();

	n = 0;
	n = (dw&CBRS_ALIGN_TOP) ? AFX_IDW_DOCKBAR_TOP : n;
	n = (dw&CBRS_ALIGN_BOTTOM && n==0) ? AFX_IDW_DOCKBAR_BOTTOM : n;
	n = (dw&CBRS_ALIGN_LEFT && n==0) ? AFX_IDW_DOCKBAR_LEFT : n;
	n = (dw&CBRS_ALIGN_RIGHT && n==0) ? AFX_IDW_DOCKBAR_RIGHT : n;
	
	// When we take the default parameters on rect, DockControlBar will dock
	// each Toolbar on a seperate line. By calculating a rectangle, we
	// are simulating a Toolbar being dragged to that location and docked.
	m_pFrame->DockControlBar (pBar,n,&rect);
}
//****************************************************************************************
void CBCGFrameImpl::SetupToolbarMenu (CMenu& menu, 
									  const UINT uiViewUserToolbarCmdFirst,
									  const UINT uiViewUserToolbarCmdLast)
{
	//---------------------------------------------------------------
	// Replace toolbar dummy items to the user-defined toolbar names:
	//---------------------------------------------------------------
	for (int i = 0; i < (int) menu.GetMenuItemCount ();)
	{
		UINT uiCmd = menu.GetMenuItemID (i);

		if (uiCmd >= uiViewUserToolbarCmdFirst && 
			uiCmd <= uiViewUserToolbarCmdLast)
		{
			//-------------------------------------------------------------------
			// "User toolbar" item. First check that toolbar number 'x' is exist:
			//-------------------------------------------------------------------
			CBCGToolBar* pToolBar = GetUserBarByIndex (uiCmd - uiViewUserToolbarCmdFirst);
			if (pToolBar != NULL)
			{
				ASSERT_VALID (pToolBar);

				//-----------------------------------------------------------
				// Modify the current menu item text to the toolbar title and
				// move next:
				//-----------------------------------------------------------
				CString strToolbarName;
				pToolBar->GetWindowText (strToolbarName);

				menu.ModifyMenu (i ++, MF_BYPOSITION | MF_STRING, uiCmd, strToolbarName);
			}
			else
			{
				menu.DeleteMenu (i, MF_BYPOSITION);
			}
		}
		else	// Not "user toolbar" item, move next
		{
			i ++;
		}
	}
}


// Copyright (C) 1997,'98 by Joerg Koenig
// All rights reserved
//

/////////////////////////////////////////////////////////////////////////////
// helpers for docking 
/////////////////////////////////////////////////////////////////////////////


// We need our own version of a dock bar, because the original
// MFC implementation overlapps toolbars. CBCGToolBar don't want
// such a overlapping, because this makes it impossible to draw
// a real 3d border ...

IMPLEMENT_DYNCREATE(CBCGToolDockBar, CDockBar);

CSize CBCGToolDockBar::CalcFixedLayout(BOOL bStretch, BOOL bHorz)
{
	ASSERT_VALID(this);

	CSize sizeFixed = CControlBar::CalcFixedLayout(bStretch, bHorz);

	// get max size
	CSize sizeMax;
	if (!m_rectLayout.IsRectEmpty())
		sizeMax = m_rectLayout.Size();
	else
	{
		CRect rectFrame;
		CFrameWnd* pFrame = GetParentFrame();
		pFrame->GetClientRect(&rectFrame);
		sizeMax = rectFrame.Size();
	}

	// prepare for layout
	AFX_SIZEPARENTPARAMS layout;
	layout.hDWP = m_bLayoutQuery ?
		NULL : ::BeginDeferWindowPos(m_arrBars.GetSize());
	int cxBorder = afxData.cxBorder2; 
	int cyBorder = afxData.cyBorder2;

	CPoint pt(-cxBorder, -cyBorder);
	int nWidth = 0;

	BOOL bWrapped = FALSE;
	BOOL bFirstBar = TRUE;


//remove empty spaces
	bool	bLastEmpty = false;
	for( int n = 0; n < m_arrBars.GetSize(); )
	{
		void* pVoid = m_arrBars[n];

		if( !pVoid )
		{
			if( bLastEmpty )
				m_arrBars.RemoveAt( n );
			else
			{
				bLastEmpty = true;
				n++;
			}
		}
		else
		{
			bLastEmpty = false;
			n++;
		}
	}

	// layout all the control bars
	for (int nPos = 0; nPos < m_arrBars.GetSize(); nPos++)
	{
		CControlBar* pBar = GetDockedControlBar(nPos);
		void* pVoid = m_arrBars[nPos];

		if (pBar != NULL)
		{
			if (pBar->IsKindOf (RUNTIME_CLASS(CBCGToolBar)) ||
				pBar->IsKindOf (RUNTIME_CLASS(CBCGSizingControlBar)) ||
				pBar->IsKindOf (RUNTIME_CLASS(CBCGDialogBar)))
			{
				cxBorder = cyBorder = 0;
			}
			else
			{
				cxBorder = afxData.cxBorder2;
				cyBorder = afxData.cyBorder2;
			}

			if (pBar->IsVisible() && pBar->m_pDockContext )
			{
				// get ideal rect for bar
				DWORD dwMode = 0;
				if ((pBar->m_dwStyle & CBRS_SIZE_DYNAMIC) &&
					(pBar->m_dwStyle & CBRS_FLOATING))
					dwMode |= LM_HORZ | LM_MRUWIDTH;
				else if (pBar->m_dwStyle & CBRS_ORIENT_HORZ)
					dwMode |= LM_HORZ | LM_HORZDOCK;
				else
					dwMode |=  LM_VERTDOCK;

				CSize sizeBar = pBar->CalcDynamicLayout(-1, dwMode);

				TRACE( "%s -> %d, %d\n", pBar->GetRuntimeClass()->m_lpszClassName, sizeBar.cx, sizeBar.cy );

				bool	bFixed = false;
				if( ( pBar->m_dwStyle & CBRS_SIZE_FIXED ) == CBRS_SIZE_FIXED )
					bFixed = true;

				BOOL bIsMenuBar = FALSE;

				if (pBar->IsKindOf (RUNTIME_CLASS(CBCGMenuBar)))
				{
					bIsMenuBar = TRUE;

					if (dwMode & LM_HORZDOCK)
					{
						sizeBar.cx = sizeMax.cx;
					}
					else if (dwMode & LM_VERTDOCK)
					{
						sizeBar.cy = sizeMax.cy;
					}
				}

				if (bFirstBar)
				{
					if (dwMode & LM_HORZDOCK)
					{
						pt.x = -cxBorder;
					}
					else if (dwMode & LM_VERTDOCK)
					{
						pt.y = -cyBorder;
					}
				}

				CRect rect(pt, sizeBar);

				// get current rect for bar
				CRect rectBar;
				pBar->GetWindowRect(&rectBar);
				ScreenToClient(&rectBar);

				BOOL bMenuIsCutted = FALSE;

				if (bHorz)
				{
					// Offset Calculated Rect out to Actual
					if (rectBar.left > rect.left && !m_bFloating)
						rect.OffsetRect(rectBar.left - rect.left, 0);

					// If ControlBar goes off the right, then right justify

					if (rect.right > sizeMax.cx && !m_bFloating&&!bFixed )
					{
						int x = rect.Width() - cxBorder;
						x = max(sizeMax.cx - x, pt.x);
						rect.OffsetRect(x - rect.left, 0);

						if (bIsMenuBar)
						{
							bMenuIsCutted = TRUE;
						}

						if( !bFixed )
							if (rect.right > sizeMax.cx)
								rect.right = sizeMax.cx;
					}

					// If ControlBar has been wrapped, then left justify
					if (bWrapped)
					{
						bWrapped = FALSE;
						rect.OffsetRect(-(rect.left + cxBorder), 0);
					}
					// If ControlBar is completely invisible, then wrap it
					//andy
					else if ((rect.left >= (sizeMax.cx - cxBorder) || bMenuIsCutted) &&
						(nPos > 0) && (m_arrBars[nPos - 1] != NULL))
					{
					//modified by andy
					/*	
						m_arrBars.InsertAt(nPos, (CObject*)NULL);
						pBar = NULL; pVoid = NULL;
						bWrapped = TRUE;*/
					}
					if (!bWrapped)
					{
						if (rect != rectBar)
						{
							if (!m_bLayoutQuery &&
								!(pBar->m_dwStyle & CBRS_FLOATING))
							{
								pBar->m_pDockContext->m_rectMRUDockPos = rect;
							}

							AfxRepositionWindow(&layout, pBar->m_hWnd, &rect);
						}
						pt.x = rect.left + sizeBar.cx - cxBorder;
						nWidth = max(nWidth, sizeBar.cy);
					}
				}
				else
				{
					// Offset Calculated Rect out to Actual
					if (rectBar.top > rect.top && !m_bFloating)
						rect.OffsetRect(0, rectBar.top - rect.top);

					// If ControlBar goes off the bottom, then bottom justify
					if (rect.bottom > sizeMax.cy && !m_bFloating)
					{
						int y = rect.Height() - cyBorder;
						y = max(sizeMax.cy - y, pt.y);
						rect.OffsetRect(0, y - rect.top);

						if (bIsMenuBar)
						{
							bMenuIsCutted = TRUE;
						}

						if (rect.bottom > sizeMax.cy)
						{
							rect.bottom = sizeMax.cy;
						}
					}

					// If ControlBar has been wrapped, then top justify
					if (bWrapped)
					{
						bWrapped = FALSE;
						rect.OffsetRect(0, -(rect.top + cyBorder));
					}
					//modified by andy
					// If ControlBar is completely invisible, then wrap it
					/*else if ((rect.top >= (sizeMax.cy - cyBorder) || bIsMenuBar) &&
						(nPos > 0) && (m_arrBars[nPos - 1] != NULL))
					{
						m_arrBars.InsertAt(nPos, (CObject*)NULL);
						pBar = NULL; pVoid = NULL;
						bWrapped = TRUE;
					}*/

					if (!bWrapped)
					{
						if (rect != rectBar)
						{
							if (!m_bLayoutQuery &&
								!(pBar->m_dwStyle & CBRS_FLOATING) &&
								pBar->m_pDockContext != NULL)
							{
								pBar->m_pDockContext->m_rectMRUDockPos = rect;
							}

							AfxRepositionWindow(&layout, pBar->m_hWnd, &rect);
						}
						pt.y = rect.top + sizeBar.cy - cyBorder;
						nWidth = max(nWidth, sizeBar.cx);
					}
				}

				bFirstBar = FALSE;
			}

			if (!bWrapped)
			{
				// handle any delay/show hide for the bar
				pBar->RecalcDelayShow(&layout);
			}
		}

		if (pBar == NULL && pVoid == NULL && nWidth != 0)
		{
			// end of row because pBar == NULL
			if (bHorz)
			{
				pt.y += nWidth - cyBorder;

				sizeFixed.cx = max(sizeFixed.cx, pt.x);
				sizeFixed.cy = max(sizeFixed.cy, pt.y);
				pt.x = -cxBorder;

				sizeFixed.cy --;	// By Sven Ritter

			}
			else
			{
				pt.x += nWidth - cxBorder;
				sizeFixed.cx = max(sizeFixed.cx, pt.x);
				sizeFixed.cy = max(sizeFixed.cy, pt.y);
				pt.y = -cyBorder;

				sizeFixed.cx --;	// By Sven Ritter
			}
			nWidth = 0;
		}
	}
	if (!m_bLayoutQuery)
	{
		// move and resize all the windows at once!
		if (layout.hDWP == NULL || !::EndDeferWindowPos(layout.hDWP))
			TRACE0("Warning: DeferWindowPos failed - low system resources.\n");
	}

	TRACE( "---\n" );

	// adjust size for borders on the dock bar itself
	CRect rect;
	rect.SetRectEmpty();
	CalcInsideRect(rect, bHorz);

	if ((!bStretch || !bHorz) && sizeFixed.cx != 0)
		sizeFixed.cx += -rect.right + rect.left;
	if ((!bStretch || bHorz) && sizeFixed.cy != 0)
		sizeFixed.cy += -rect.bottom + rect.top;

	return sizeFixed;
}

// dwDockBarMap
const DWORD dwDockBarMap[4][2] =
{
	{ AFX_IDW_DOCKBAR_TOP,      CBRS_TOP    },
	{ AFX_IDW_DOCKBAR_BOTTOM,   CBRS_BOTTOM },
	{ AFX_IDW_DOCKBAR_LEFT,     CBRS_LEFT   },
	{ AFX_IDW_DOCKBAR_RIGHT,    CBRS_RIGHT  },
};


// Unfortunataly a simple rewrite of CFrameWnd's EnableDocking() is not possible,
// because we have not enough permissions to access some data in this class.
// That's why we call CFrameWnd::EnableDocking() first and exchange all occurencies
// of CDockBar objects with our own version of a dock bar.
//andy
extern CRuntimeClass *g_pclassDockBarRuntime;

void CBCGFrameImpl::FrameEnableDocking(CFrameWnd * pFrame, DWORD dwDockStyle) 
{
	ASSERT_VALID(pFrame);

	// must be CBRS_ALIGN_XXX or CBRS_FLOAT_MULTI only
	ASSERT((dwDockStyle & ~(CBRS_ALIGN_ANY|CBRS_FLOAT_MULTI)) == 0);

	pFrame->EnableDocking(dwDockStyle);

	for (int i = 0; i < 4; i++) 
	{
		if (dwDockBarMap[i][1] & dwDockStyle & CBRS_ALIGN_ANY) 
		{
			CDockBar* pDock = (CDockBar*)pFrame->GetControlBar(dwDockBarMap[i][0]);
			
			// make sure the dock bar is of correct type
			if( pDock == 0 || ! pDock->IsKindOf(RUNTIME_CLASS(CBCGToolDockBar)) ) {
				BOOL bNeedDelete = ! pDock->m_bAutoDelete;
				pDock->m_pDockSite->RemoveControlBar(pDock);
				pDock->m_pDockSite = 0;	// avoid problems in destroying the dockbar
				pDock->DestroyWindow();
				if( bNeedDelete )
					delete pDock;
				pDock = 0;
			}

			if( pDock == 0 ) {
				pDock = (CBCGToolDockBar*)g_pclassDockBarRuntime->CreateObject();
				if (!pDock->Create(pFrame,
					WS_CLIPSIBLINGS|WS_CLIPCHILDREN|WS_CHILD|WS_VISIBLE |
						dwDockBarMap[i][1], dwDockBarMap[i][0])) {
					AfxThrowResourceException();
				}
			}
		}
	}
}
//********************************************************************************
BOOL CBCGFrameImpl::OnMenuChar (UINT nChar)
{
	if (m_pMenuBar != NULL &&
		(m_pMenuBar->GetStyle () & WS_VISIBLE) &&
		m_pMenuBar->TranslateChar (nChar))
	{
		return TRUE;
	}

	for (POSITION posTlb = gAllToolbars.GetHeadPosition (); posTlb != NULL;)
	{
		CBCGToolBar* pToolBar = (CBCGToolBar*) gAllToolbars.GetNext (posTlb);
		ASSERT (pToolBar != NULL);

		if (CWnd::FromHandlePermanent (pToolBar->m_hWnd) != NULL &&
			pToolBar != m_pMenuBar &&
			(pToolBar->GetStyle () & WS_VISIBLE) &&
			pToolBar->GetTopLevelFrame () == m_pFrame &&
			pToolBar->TranslateChar (nChar))
		{
			return TRUE;
		}
	}

	return FALSE;
}

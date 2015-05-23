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

// BCGPopupMenu.cpp : implementation file
//

#include "stdafx.h"

#pragma warning (disable : 4706)

#ifdef _AFXDLL
#define COMPILE_MULTIMON_STUBS
#endif // _AFXDLL

#include "multimon.h"

#pragma warning (default : 4706)

#include "BCGPopupMenu.h"
#include "BCGMenuBar.h"
#include "globals.h"
#include "BCGToolbarMenuButton.h"
#include "BCGMDIFrameWnd.h"
#include "BCGFrameWnd.h"
#include "BCGOleIPFrameWnd.h"
#include "BCGMenuBar.h"
#include "menupage.h"
#include "MenuHash.h"
#include "MenuImages.h"
#include "BCGShowAllButton.h"
#include "BCGUserToolsManager.h"
#include "BCGUserTool.h"
#include "bcgsound.h"

extern CMenuHash g_menuHash;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

inline static COLORREF PixelAlpha (COLORREF srcPixel, int percent);
inline static COLORREF PixelAlpha (COLORREF srcPixel, COLORREF dstPixel, int percent);
inline static void SetAlphaPixel (COLORREF* pBits, CRect rect, int x, int y, int percent, int iShadowSize);

/////////////////////////////////////////////////////////////////////////////
// CBCGPopupMenu

static const int iFadeStep = 10;
static const int iBorderSize = 2;
static const int iAnimTimerId = 1;
static const int iAnimTimerDuration = 30;
static const int iScrollTimerId = 2;
static const int iScrollTimerDuration = 80;
static const int iShadowSize = 7;

CString	CBCGPopupMenu::m_strClassName;
CBCGPopupMenu::ANIMATION_TYPE CBCGPopupMenu::m_AnimationType = NO_ANIMATION;

static clock_t nLastAnimTime = 0;

IMPLEMENT_SERIAL(CBCGPopupMenu, CMiniFrameWnd, VERSIONABLE_SCHEMA | 1)

CBCGPopupMenu::CBCGPopupMenu() :
	m_pMenuCustomizationPage (NULL)
{
	Initialize ();
}
//****************************************************************************************
CBCGPopupMenu::CBCGPopupMenu(CBCGMenuPage* pCustPage, LPCTSTR lpszTitle) :
	m_pMenuCustomizationPage (pCustPage),
	m_strCaption (lpszTitle)
{
	Initialize ();
}
//****************************************************************************************
void CBCGPopupMenu::Initialize ()
{
	m_hMenu = NULL;
	m_ptLocation = CPoint (0, 0);
	m_ptLocationAnim = CPoint (-1, -1);
	m_pParentBtn = NULL;
	m_bAutoDestroyParent = TRUE;
	m_bAutoDestroy = TRUE;
	m_FinalSize = CSize (0, 0);
	m_AnimSize = CSize (0, 0);
	m_bAnimationIsDone = (m_AnimationType == NO_ANIMATION);
	m_bScrollable = FALSE;
	m_bTobeDstroyed = FALSE;
	m_bShown = FALSE;

	m_iMaxWidth = -1;

	m_rectScrollUp.SetRectEmpty ();
	m_rectScrollDn.SetRectEmpty ();

	m_iScrollMode = 0;

	m_bIsAnimRight = TRUE;
	m_bIsAnimDown = TRUE;

	m_bSaveShadows = FALSE;

	m_iShadowSize = CBCGMenuBar::IsMenuShadows () &&
					!CBCGToolBar::IsCustomizeMode () &&
					globalData.m_nBitsPerPixel > 8 ? // Don't draw shadows in 256 colors or less
						iShadowSize : 0;

	m_iFadePercent = 0;
	if (m_AnimationType == FADE && globalData.m_nBitsPerPixel <= 8)
	{
		m_AnimationType = NO_ANIMATION;
		m_bAnimationIsDone = TRUE;
	}

   // By Alex Corazzin (2)
   m_pMessageWnd = NULL;
}
//****************************************************************************************
CBCGPopupMenu::~CBCGPopupMenu()
{
	if (m_bAutoDestroy && m_hMenu != NULL)
	{
		::DestroyMenu (m_hMenu);
	}
}

BEGIN_MESSAGE_MAP(CBCGPopupMenu, CMiniFrameWnd)
	//{{AFX_MSG_MAP(CBCGPopupMenu)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_MOUSEACTIVATE()
	ON_WM_DESTROY()
	ON_WM_KEYDOWN()
	ON_WM_ERASEBKGND()
	ON_WM_ACTIVATEAPP()
	ON_WM_TIMER()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBCGPopupMenu message handlers

BOOL CBCGPopupMenu::Create (CWnd* pWndParent, int x, int y, HMENU hMenu, BOOL bLocked, BOOL bOwnMessage)
{
	BCGPlaySystemSound (BCGSOUND_MENU_POPUP);

	ASSERT (pWndParent != NULL);

	if (m_strClassName.IsEmpty ())
	{
		m_strClassName = ::AfxRegisterWndClass (
			CS_SAVEBITS,
			::LoadCursor(NULL, IDC_ARROW),
			(HBRUSH)(COLOR_BTNFACE + 1), NULL);
	}

	m_hMenu = hMenu;

	if (x == -1 && y == -1)	// Undefined position
	{
		if (pWndParent != NULL)
		{
			CRect rectParent;
			pWndParent->GetClientRect (&rectParent);
			pWndParent->ClientToScreen (&rectParent);

			m_ptLocation = CPoint (rectParent.left + 5, rectParent.top + 5);
		}
		else
		{
			m_ptLocation = CPoint (0, 0);
		}
	}
	else
	{
		m_ptLocation = CPoint (x, y);
	}

	DWORD dwStyle = WS_POPUP;
	if (m_pMenuCustomizationPage != NULL)
	{
		dwStyle |= (WS_CAPTION | WS_SYSMENU);
	}

	BOOL bIsAnimate = (m_AnimationType != NO_ANIMATION) && 
					!CBCGToolBar::IsCustomizeMode ();

	CRect rect (x, y, x, y);
	BOOL bCreated = CMiniFrameWnd::CreateEx (
				0,
				m_strClassName, m_strCaption,
				dwStyle, rect,
				pWndParent->GetOwner () == NULL ? 
					pWndParent : pWndParent->GetOwner ());
	if (!bCreated)
	{
		return FALSE;
	}

	// By Ales Corazzin (2) ------
	if (bOwnMessage)
	{
		m_pMessageWnd = pWndParent;
	}
   // ---------------------------

	CBCGPopupMenuBar* pMenuBar = GetMenuBar ();
	ASSERT_VALID (pMenuBar);

	pMenuBar->m_bLocked = bLocked;

	if (bIsAnimate)
	{
		if (m_bIsAnimDown)
		{
			pMenuBar->SetOffset (pMenuBar->m_Buttons.GetCount () - 1);
		}

		//--------------------------
		// Adjust initial menu size:
		//--------------------------
		m_AnimSize = m_FinalSize;

		switch (m_AnimationType)
		{
		case UNFOLD:
			m_AnimSize.cx = pMenuBar->GetColumnWidth ();

		case SLIDE:
			m_AnimSize.cy = pMenuBar->GetRowHeight ();
			break;

		case FADE:
			m_AnimSize.cx += m_iShadowSize;
			m_AnimSize.cy += m_iShadowSize;
			
			if (pMenuBar->IsWindowVisible ())
			{
				pMenuBar->ShowWindow (SW_HIDE);
			}
			break;
		}

		//------------------------------
		// Adjust initial menu position:
		//------------------------------
		m_ptLocationAnim = m_ptLocation;

		if (m_AnimationType != FADE)
		{
			if (!m_bIsAnimRight)
			{
				m_ptLocationAnim.x += m_FinalSize.cx - m_AnimSize.cx;
			}

			if (!m_bIsAnimDown)
			{
				m_ptLocationAnim.y += m_FinalSize.cy - m_AnimSize.cy;
			}
		}

		SetWindowPos (NULL, m_ptLocationAnim.x, m_ptLocationAnim.y, 
					m_AnimSize.cx, m_AnimSize.cy,
					SWP_NOZORDER | SWP_NOACTIVATE);

		SetTimer (iAnimTimerId, iAnimTimerDuration, NULL);
		nLastAnimTime = clock ();
	}

	//--------------------------------------
	// Update windows covered by the shadow:
	//--------------------------------------
	UpdateBottomWindows (!m_bAnimationIsDone && m_AnimationType != FADE);
	if (m_iShadowSize == 0 && m_AnimationType == FADE)
	{
		m_AnimSize = m_FinalSize;
	}

	SetWindowPos (&wndTop, -1, -1, -1, -1,
		SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);

	if (CBCGToolBar::IsCustomizeMode ())
	{
		pMenuBar->Invalidate ();
		pMenuBar->UpdateWindow ();
	}

	return TRUE;
}
//****************************************************************************************

//-----------------------------------------------------
// My "classic " trick - how I can access to protected
// member m_pRecentFileList?
//-----------------------------------------------------
class CBCGApp : public CWinApp
{
	friend class CBCGPopupMenu;
};

int CBCGPopupMenu::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMiniFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	DWORD toolbarStyle = dwDefaultToolbarStyle;
	if (m_AnimationType == FADE && !CBCGToolBar::IsCustomizeMode ())
	{
		toolbarStyle &= ~WS_VISIBLE;
	}

	if (!m_wndMenuBar.Create (this, toolbarStyle | CBRS_TOOLTIPS | CBRS_FLYBY))
	{
		TRACE(_T("Can't create popup menu bar\n"));
		return FALSE;
	}

	CBCGPopupMenu* pParentPopupMenu = GetParentPopupMenu ();
	if (pParentPopupMenu != NULL)
	{
		m_iMaxWidth = pParentPopupMenu->m_iMaxWidth;
	}

	m_wndMenuBar.m_iMaxWidth = m_iMaxWidth;
	m_wndMenuBar.SetOwner (GetParent ());

	return InitMenuBar () ? 0 : 1;
}
//****************************************************************************************
void CBCGPopupMenu::OnSize(UINT nType, int cx, int cy) 
{
	CMiniFrameWnd::OnSize(nType, cx, cy);
	
	CBCGPopupMenuBar* pMenuBar = GetMenuBar ();
	ASSERT_VALID (pMenuBar);

	if (pMenuBar->GetSafeHwnd () == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect (rectClient);

	if ((m_AnimationType == FADE || m_bAnimationIsDone) 
		&& !CBCGToolBar::IsCustomizeMode ())
	{
		rectClient.right -= m_iShadowSize;
		rectClient.bottom -= m_iShadowSize;
	}

	rectClient.DeflateRect (iBorderSize, iBorderSize);

	if (m_bScrollable && (m_bAnimationIsDone || m_AnimationType == FADE))
	{
		int iScrollBtnHeight = CMenuImages::Size ().cy + 2 * iBorderSize;

		pMenuBar->SetWindowPos (NULL, rectClient.left, 
					rectClient.top + iScrollBtnHeight + iBorderSize,
					rectClient.Width (), 
					rectClient.Height () - 2 * iScrollBtnHeight - iBorderSize,
					SWP_NOZORDER | SWP_NOACTIVATE);
		
		m_rectScrollUp = rectClient;
		m_rectScrollUp.top += iBorderSize;
		m_rectScrollUp.bottom = m_rectScrollUp.top + iScrollBtnHeight;

		m_rectScrollDn = rectClient;
		m_rectScrollDn.top = m_rectScrollDn.bottom - iScrollBtnHeight;
	}
	else
	{
		UINT uiFlags = SWP_NOZORDER | SWP_NOACTIVATE;
		if (!m_bAnimationIsDone)
		{
			uiFlags |= SWP_NOREDRAW;
		}

		pMenuBar->SetWindowPos (NULL, rectClient.left, rectClient.top,
					rectClient.Width (), 
					rectClient.Height (),
					uiFlags);

		m_rectScrollUp.SetRectEmpty ();
		m_rectScrollDn.SetRectEmpty ();
	}
}
//****************************************************************************************
void CBCGPopupMenu::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	if (m_AnimationType == FADE && !m_bAnimationIsDone &&
		!CBCGToolBar::IsCustomizeMode ())
	{
		DrawFade (&dc);
	}
	else
	{
		DoPaint (&dc);
	}
}
//****************************************************************************************
int CBCGPopupMenu::OnMouseActivate(CWnd* /*pDesktopWnd*/, UINT /*nHitTest*/, UINT /*message*/) 
{
	return MA_NOACTIVATE;
}
//****************************************************************************************
void CBCGPopupMenu::RecalcLayout (BOOL /*bNotify*/) 
{
#ifdef _DEBUG
	if (m_pParentBtn != NULL)
	{
		ASSERT_VALID (m_pParentBtn);
		ASSERT (m_pParentBtn->m_pPopupMenu == this);
	}
#endif // _DEBUG

	CBCGPopupMenuBar* pMenuBar = GetMenuBar ();
	ASSERT_VALID (pMenuBar);

	if (!::IsWindow (m_hWnd) ||
		!::IsWindow (pMenuBar->m_hWnd))
	{
		return;
	}

	CRect rectScreen;

	// By Jan Vasina
	MONITORINFO mi;
	mi.cbSize = sizeof (MONITORINFO);
	if (GetMonitorInfo (MonitorFromPoint (m_ptLocation, MONITOR_DEFAULTTONEAREST),
		&mi))
	{
		rectScreen = mi.rcWork;
	}
	else
	{
		::SystemParametersInfo (SPI_GETWORKAREA, 0, &rectScreen, 0);
	}

	CSize size = pMenuBar->CalcSize ();
	size.cx += iBorderSize * 2;
	size.cy += iBorderSize * 2;

	if (m_pMenuCustomizationPage != NULL)
	{
		size.cy += ::GetSystemMetrics (SM_CYSMCAPTION);
		size.cy += 2 * ::GetSystemMetrics (SM_CYBORDER) + 5;
	}

	//---------------------------------------------
	// Adjust the menu position by the screen size:
	//---------------------------------------------
	if (m_ptLocation.x + size.cx > rectScreen.right)
	{
		//-----------------------------------------------------
		// Menu can't be overlapped with the parent popup menu!
		//-----------------------------------------------------
		CBCGPopupMenu* pParentMenu = GetParentPopupMenu ();
		CBCGMenuBar* pParentMenuBar = m_pParentBtn == NULL ? NULL :
			DYNAMIC_DOWNCAST (CBCGMenuBar, m_pParentBtn->m_pWndParent);

		if (pParentMenu != NULL)
		{
			CRect rectParent;
			pParentMenu->GetWindowRect (rectParent);

			m_ptLocation.x = rectParent.left - size.cx;
		}
		else if (pParentMenuBar != NULL && 
			(pParentMenuBar->m_dwStyle & CBRS_ORIENT_HORZ) == 0)
		{
			//------------------------------------------------
			// Parent menu bar is docked vertical, place menu 
			// in the left or right side of the parent frame:
			//------------------------------------------------
			CRect rectParent;
			pParentMenuBar->GetWindowRect (rectParent);

			m_ptLocation.x = rectParent.left - size.cx;
		}
		else
		{
			m_ptLocation.x = rectScreen.right - size.cx - 1;
		}

		if (m_ptLocation.x < rectScreen.left)
		{
			m_ptLocation.x = rectScreen.left;
		}

		if (m_AnimationType == UNFOLD)
		{
			m_bIsAnimRight = FALSE;
		}
		else if (m_AnimationType == FADE)
		{
			m_bIsAnimRight = FALSE;
			m_bIsAnimDown = FALSE;
		}
	}

	if (m_ptLocation.y + size.cy > rectScreen.bottom)
	{
		m_bIsAnimDown = FALSE;

		if (m_pParentBtn != NULL && m_pParentBtn->GetParentWnd () != NULL &&
			GetParentPopupMenu () == NULL)
		{
			CPoint ptRight (m_pParentBtn->Rect ().right, 0);
            m_pParentBtn->GetParentWnd ()->ClientToScreen (&ptRight);

            CPoint ptTop (0, m_pParentBtn->Rect ().top - size.cy);
			m_pParentBtn->GetParentWnd ()->ClientToScreen (&ptTop);

            if (ptTop.y < 0)
            {
				int yParentButtonTop = ptTop.y + size.cy;

				//-----------------------------------------------------
				// Where more space: on top or on bottom of the button?
				//-----------------------------------------------------
				if (rectScreen.bottom - yParentButtonTop < 
					yParentButtonTop - rectScreen.top)
				{
					m_ptLocation.y = rectScreen.top;
					size.cy += ptTop.y;
				}
				else
				{
					size.cy = rectScreen.bottom - m_ptLocation.y;
					m_bIsAnimDown = TRUE;
				}

				m_bScrollable = TRUE;
            }
            else
            {
                m_ptLocation.y = ptTop.y;
            }
		}
		else
		{
			m_ptLocation.y -= size.cy;
		}

		if (m_ptLocation.y < rectScreen.top)
		{
			m_ptLocation.y = rectScreen.top;
		}

		if (m_ptLocation.y + size.cy > rectScreen.bottom)
		{
			size.cy = rectScreen.bottom - m_ptLocation.y;
			m_bScrollable = TRUE;
		}
	}

	m_FinalSize = size;

	//----------------------------------------------------------------
	// If the final size is more, that half-screen (in either x or y),
	// CS_SAVEBITS may not save a screen content under the menu. So,
	// we need to save a area under the shadows:
	//---------------------------------------------------------------
	m_bSaveShadows =	(m_FinalSize.cx >= rectScreen.Width ()) ||
						(m_FinalSize.cy >= rectScreen.Height ()) ||
						(m_AnimationType == FADE);

	if (m_AnimationType == FADE || m_bAnimationIsDone || 
		CBCGToolBar::IsCustomizeMode ())
	{
		if (!CBCGToolBar::IsCustomizeMode ())
		{
			size.cx += m_iShadowSize;
			size.cy += m_iShadowSize;
		}

		if (m_pMenuCustomizationPage != NULL)
		{
			SetWindowPos (NULL, -1, -1, size.cx, size.cy,
						SWP_NOMOVE | SWP_NOZORDER |
						SWP_NOACTIVATE);
		}
		else
		{
			SetWindowPos (NULL, m_ptLocation.x, m_ptLocation.y, size.cx, size.cy,
						SWP_NOZORDER | SWP_NOACTIVATE);
		}
	}

	if (CBCGToolBar::IsCustomizeMode ())
	{
		pMenuBar->AdjustLocations ();
	}

	//---------------------------------------------------
	// Parent button maybe covered by shadow, repaint it:
	//---------------------------------------------------
	if (m_iShadowSize != 0 &&
		(m_AnimationType == FADE || m_bAnimationIsDone) && 
		!CBCGToolBar::IsCustomizeMode () &&
		m_pParentBtn != NULL &&
		m_pParentBtn->GetParentWnd () != NULL)
	{
		CWnd* pWndParent = m_pParentBtn->GetParentWnd ();

		CRect rectInter;

		CRect rectMenu;
		GetWindowRect (rectMenu);

		CRect rectShadowRight (CPoint (rectMenu.right + 1, rectMenu.top), 
					CSize (m_iShadowSize, rectMenu.Height () + m_iShadowSize));
		pWndParent->ScreenToClient (rectShadowRight);
		if (rectInter.IntersectRect (rectShadowRight, m_pParentBtn->m_rect))
		{
			pWndParent->InvalidateRect (m_pParentBtn->m_rect);
			pWndParent->UpdateWindow ();
		}
		else
		{
			CRect rectShadowBottom (CPoint (rectMenu.left, rectMenu.bottom + 1),
						CSize (rectMenu.Width () + m_iShadowSize, m_iShadowSize));
			pWndParent->ScreenToClient (rectShadowBottom);
			if (rectInter.IntersectRect (rectShadowBottom, m_pParentBtn->m_rect))
			{
				pWndParent->InvalidateRect (m_pParentBtn->m_rect);
				pWndParent->UpdateWindow ();
			}
		}
	}
}

// A.M. fix BT4609
extern DWORD g_nLastCommandID;
extern bool g_bWaitingForCommandFlag;


//****************************************************************************************
void CBCGPopupMenu::OnDestroy() 
{
	CBCGPopupMenuBar* pMenuBar = GetMenuBar ();
	ASSERT_VALID (pMenuBar);

// A.M. fix BT4609
	g_bWaitingForCommandFlag = false;


	//---------------------------------------------------------
	// First, maybe we have a dragged menu item. Remove it now:
	//---------------------------------------------------------
	if (pMenuBar->m_pDragButton != NULL &&
		!pMenuBar->m_bIsDragCopy)
	{
		pMenuBar->RemoveButton (
			pMenuBar->ButtonToIndex (pMenuBar->m_pDragButton));
		pMenuBar->m_pDragButton = NULL;
	}

	if (m_pParentBtn != NULL)
	{
		ASSERT (m_pParentBtn->m_pPopupMenu == this);

		SaveState ();

		m_pParentBtn->m_pPopupMenu = NULL;
		m_pParentBtn->m_bClickedOnMenu = FALSE;

		CBCGPopupMenu* pParentMenu = GetParentPopupMenu ();
		if (pParentMenu != NULL)
		{
			if (m_bAutoDestroyParent && !CBCGToolBar::IsCustomizeMode ())
			{
				//-------------------------------------------
				// Automatically close the parent popup menu:
				//-------------------------------------------
				pParentMenu->SendMessage (WM_CLOSE);
				m_pParentBtn = NULL;
			}
			else if (pParentMenu->m_iShadowSize > 0 &&
				(m_AnimationType == SLIDE || m_AnimationType == UNFOLD))
			{
				//--------------------
				// Redraw parent menu:
				//--------------------
				pParentMenu->ShowWindow (SW_HIDE);
				pParentMenu->UpdateBottomWindows ();
				pParentMenu->ShowWindow (SW_SHOWNOACTIVATE);
			}
		}
	}
	else
	{
		CBCGMenuBar::SetShowAllCommands (FALSE);
	}

	if (m_pMenuCustomizationPage != NULL)
	{
		m_pMenuCustomizationPage->CloseContextMenu (this);
	}
				
	//------------------------------------------------
	// Inform the main frame about the menu detsroyng:
	//------------------------------------------------
	CFrameWnd* pWndMain = GetTopLevelFrame ();

	CBCGMDIFrameWnd* pMainFrame = DYNAMIC_DOWNCAST (CBCGMDIFrameWnd, pWndMain);
	if (pMainFrame != NULL)
	{
		pMainFrame->OnClosePopupMenu (this);
	}
	else	// Maybe, SDI frame...
	{
		CBCGFrameWnd* pFrame = DYNAMIC_DOWNCAST (CBCGFrameWnd, pWndMain);
		if (pFrame != NULL)
		{
			pFrame->OnClosePopupMenu (this);
		}
		else	// Maybe, OLE frame...
		{
			CBCGOleIPFrameWnd* pOleFrame = 
				DYNAMIC_DOWNCAST (CBCGOleIPFrameWnd, pWndMain);
			if (pOleFrame != NULL)
			{
				pOleFrame->OnClosePopupMenu (this);
			}
		}
	}

	CMiniFrameWnd::OnDestroy();
}
//****************************************************************************************
void CBCGPopupMenu::PostNcDestroy() 
{
	if (m_pParentBtn != NULL)
	{
		m_pParentBtn->OnCancelMode ();
	}
		
	CMiniFrameWnd::PostNcDestroy();
}
//****************************************************************************************
void CBCGPopupMenu::SaveState ()
{
	if (!CBCGToolBar::IsCustomizeMode ())
	{
		return;
	}

	if (m_pParentBtn == NULL)
	{
		return;
	}

	ASSERT_VALID (m_pParentBtn);

	CBCGPopupMenuBar* pMenuBar = GetMenuBar ();
	ASSERT_VALID (pMenuBar);

	HMENU hmenu = pMenuBar->ExportToMenu ();
	ASSERT (hmenu != NULL);

	m_pParentBtn->CreateFromMenu (hmenu);
	::DestroyMenu (hmenu);

	CBCGPopupMenu* pParentMenu = GetParentPopupMenu ();
	if (pParentMenu != NULL)
	{
		pParentMenu->SaveState ();
	}
}
//****************************************************************************************
void CBCGPopupMenu::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	ASSERT_VALID (this);

	CBCGPopupMenuBar* pMenuBar = GetMenuBar ();
	ASSERT_VALID (pMenuBar);

#ifdef _DEBUG
	if (m_pParentBtn != NULL)
	{
		ASSERT_VALID (m_pParentBtn);
		ASSERT (m_pParentBtn->m_pPopupMenu == this);
	}
#endif // _DEBUG

	BOOL bHightlightWasChanged = FALSE;

	switch (nChar)
	{
	case VK_RIGHT:
		{
			//-------------------------------------------
			// Save animation type and disable animation:
			//-------------------------------------------
			ANIMATION_TYPE animType = m_AnimationType;
			m_AnimationType = NO_ANIMATION;

			//-------------------------------
			// Try to open next cascade menu:
			//-------------------------------
			CBCGToolbarMenuButton* pSelItem = GetSelItem ();
			if (pSelItem != NULL && 
				(pSelItem->m_nID == (UINT) -1 || pSelItem->m_nID == 0) &&
				pSelItem->OpenPopupMenu ())
			{
				if (pSelItem->m_pPopupMenu != NULL)
				{
					//--------------------------
					// Select a first menu item:
					//--------------------------
					if (GetSelItem () == pSelItem)
					{
						pSelItem->m_pPopupMenu->OnKeyDown (VK_HOME, 0, 0);
					}
				}
			}
			else
			{
				//------------------------------------------------------
				// No next menu, first try to go to the parent menu bar:
				//------------------------------------------------------
				CBCGToolBar* pToolBar = GetParentToolBar ();
				if (pToolBar != NULL && 
					!pToolBar->IsKindOf (RUNTIME_CLASS (CBCGPopupMenuBar)))
				{
					pToolBar->NextMenu ();
				}
				else
				{
					//------------------------------------------------------
					// Close the current menu and move control to the parent
					// popup menu:
					//------------------------------------------------------
					CBCGPopupMenu* pParenMenu = GetParentPopupMenu ();
					if (pParenMenu != NULL)
					{
						pParenMenu->SendMessage (WM_KEYDOWN, VK_RIGHT);
					}
				}
			}

			//------------------------
			// Restore animation type:
			//------------------------
			m_AnimationType = animType;
		}
		return;
				
	case VK_LEFT:
		{
			CBCGToolBar* pToolBar = GetParentToolBar ();
			if (pToolBar != NULL)
			{
				pToolBar->PrevMenu ();
			}
			else if (m_pParentBtn != NULL && m_pParentBtn->IsDroppedDown ())		
			{
				CloseMenu ();
			}
		}
		return;

	case VK_DOWN:
		if ((::GetAsyncKeyState (VK_CONTROL) & 0x8000) &&	// Ctrl+Down
			!pMenuBar->m_bAreAllCommandsShown)
		{
			ShowAllCommands ();
			break;
		}

	case VK_UP:
	case VK_HOME:
	case VK_END:
		bHightlightWasChanged = TRUE;

	case VK_RETURN:
		if (!CBCGToolBar::IsCustomizeMode ())
		{
			pMenuBar->OnKey (nChar);
		}
		break;

	case VK_ESCAPE:
		CloseMenu (TRUE);
		return;

	default:
		if (pMenuBar->OnKey (nChar))
		{
			return;
		}
		else
		{
			CMiniFrameWnd::OnKeyDown(nChar, nRepCnt, nFlags);
		}
	}

	if (bHightlightWasChanged &&
		m_bScrollable && pMenuBar->m_iHighlighted >= 0)
	{
		//---------------------------------------
		// Maybe, selected item is invisible now?
		//---------------------------------------
		CBCGToolbarButton* pItem = pMenuBar->GetButton (pMenuBar->m_iHighlighted);
		if (pItem == NULL && pMenuBar->GetRowHeight () == 0)
		{
			ASSERT (FALSE);
		}
		else
		{
			CRect rectBar;
			pMenuBar->GetClientRect (rectBar);

			int iOffset = pMenuBar->GetOffset ();
			int iOffsetDelta = 0;

			if (pItem->Rect ().top < rectBar.top)
			{
				//---------------------
				// Scroll up is needed!
				//---------------------
				iOffsetDelta = (pItem->Rect ().top - rectBar.top) / 
					pMenuBar->GetRowHeight () - 1;
			}
			else if (pItem->Rect ().bottom > rectBar.bottom)
			{
				//-----------------------
				// Scroll down is needed!
				//-----------------------
				iOffsetDelta = (pItem->Rect ().bottom - rectBar.bottom) / 
					pMenuBar->GetRowHeight () + 1;
			}

			if (iOffsetDelta != 0)
			{
				int iTotalRows = m_FinalSize.cy / pMenuBar->GetRowHeight () - 2;

				iOffset += iOffsetDelta;
				iOffset = min (max (0, iOffset), 
					pMenuBar->m_Buttons.GetCount () - iTotalRows - 1);

				pMenuBar->SetOffset (iOffset);
				pMenuBar->AdjustLayout ();

				InvalidateRect (m_rectScrollDn);
				InvalidateRect (m_rectScrollUp);
			}
		}
	}
}
//****************************************************************************************
CBCGPopupMenu* CBCGPopupMenu::GetParentPopupMenu () const
{
	if (m_pParentBtn == NULL)
	{
		return NULL;
	}

	CBCGPopupMenuBar* pParentBar = 
		DYNAMIC_DOWNCAST (CBCGPopupMenuBar, m_pParentBtn->m_pWndParent);
	if (pParentBar != NULL)
	{
		CBCGPopupMenu* pParentMenu =
			DYNAMIC_DOWNCAST (CBCGPopupMenu, pParentBar->GetParentFrame ());

		//andy
		if( !pParentMenu  )return 0;
		ASSERT_VALID (pParentMenu);

		return pParentMenu;
	}
	else
	{
		return NULL;
	}
}
//****************************************************************************************
CBCGToolBar* CBCGPopupMenu::GetParentToolBar () const
{
	if (m_pParentBtn == NULL)
	{
		return NULL;
	}

	CBCGToolBar* pParentBar = 
		DYNAMIC_DOWNCAST (CBCGToolBar, m_pParentBtn->m_pWndParent);
	return pParentBar;
}
//****************************************************************************************
CBCGToolbarMenuButton* CBCGPopupMenu::GetSelItem ()
{
	CBCGPopupMenuBar* pMenuBar = GetMenuBar ();
	ASSERT_VALID (pMenuBar);

	return DYNAMIC_DOWNCAST (CBCGToolbarMenuButton,
							pMenuBar->GetHighlightedButton ());
}
//****************************************************************************************
void CBCGPopupMenu::CloseMenu (BOOL bSetFocusToBar)
{
	m_bTobeDstroyed = TRUE;

	SaveState ();

	CBCGPopupMenu* pParentMenu = GetParentPopupMenu ();
	CBCGToolBar* pParentToolBar = GetParentToolBar ();

	CFrameWnd* pWndMain = GetTopLevelFrame ();
	if (pParentMenu != NULL)
	{
		m_bAutoDestroyParent = FALSE;
		ActivatePopupMenu (pWndMain, pParentMenu);
	}
	else if (pParentToolBar != NULL)
	{
		//ActivatePopupMenu (pWndMain, NULL);

		if (bSetFocusToBar)
		{
			pParentToolBar->SetFocus ();
		}
	}
	else
	{
		ActivatePopupMenu (pWndMain, NULL);
	}

	SendMessage (WM_CLOSE);
}
//****************************************************************************************
int CBCGPopupMenu::InsertItem (const CBCGToolbarMenuButton& button, int iInsertAt)
{
	CBCGPopupMenuBar* pMenuBar = GetMenuBar ();
	ASSERT_VALID (pMenuBar);

	return pMenuBar->InsertButton (button, iInsertAt);
}
//****************************************************************************************
int CBCGPopupMenu::InsertSeparator (int iInsertAt)
{
	CBCGPopupMenuBar* pMenuBar = GetMenuBar ();
	ASSERT_VALID (pMenuBar);

	return pMenuBar->InsertSeparator (iInsertAt);
}
//****************************************************************************************
int CBCGPopupMenu::GetMenuItemCount () const
{
	return m_wndMenuBar.m_Buttons.GetCount ();
}
//****************************************************************************************
CBCGToolbarMenuButton* CBCGPopupMenu::GetMenuItem (int iIndex) const
{
	return (CBCGToolbarMenuButton*) m_wndMenuBar.GetButton (iIndex);
}
//****************************************************************************************
BOOL CBCGPopupMenu::RemoveItem (int iIndex)
{
	CBCGPopupMenuBar* pMenuBar = GetMenuBar ();
	ASSERT_VALID (pMenuBar);

	return pMenuBar->RemoveButton (iIndex);
}
//****************************************************************************************
void CBCGPopupMenu::RemoveAllItems ()
{
	CBCGPopupMenuBar* pMenuBar = GetMenuBar ();
	ASSERT_VALID (pMenuBar);

	pMenuBar->RemoveAllButtons ();
}
//****************************************************************************************
BOOL CBCGPopupMenu::OnEraseBkgnd(CDC* /*pDC*/) 
{
	return TRUE;
}
//****************************************************************************************
BOOL CBCGPopupMenu::ActivatePopupMenu (CFrameWnd* pTopFrame, CBCGPopupMenu* pPopupMenu)
{
	ASSERT_VALID (pTopFrame);

	BOOL bRes = TRUE;

	CBCGMDIFrameWnd* pMainFrame = DYNAMIC_DOWNCAST (CBCGMDIFrameWnd, pTopFrame);
	if (pMainFrame != NULL)
	{
		bRes = pMainFrame->ShowPopupMenu (pPopupMenu);
	}
	else	// Maybe, SDI frame...
	{
		CBCGFrameWnd* pFrame = DYNAMIC_DOWNCAST (CBCGFrameWnd, pTopFrame);
		if (pFrame != NULL)
		{
			bRes = pFrame->ShowPopupMenu (pPopupMenu);
		}
		else	// Maybe, OLE frame
		{
			CBCGOleIPFrameWnd* pOleFrame = 
				DYNAMIC_DOWNCAST (CBCGOleIPFrameWnd, pTopFrame);
			if (pOleFrame != NULL)
			{
				bRes = pOleFrame->ShowPopupMenu (pPopupMenu);
			}
		}
	}

	if (!bRes)
	{
		if (pPopupMenu != NULL && !pPopupMenu->m_bTobeDstroyed)
		{
			pPopupMenu->CloseMenu ();
		}

		return FALSE;
	}

	if (pPopupMenu != NULL)
	{
		CBCGPopupMenuBar* pMenuBar = pPopupMenu->GetMenuBar ();
		ASSERT_VALID (pMenuBar);

		CBCGPopupMenu* pParentMenu = DYNAMIC_DOWNCAST (CBCGPopupMenu, pMenuBar->GetParent ());
		if (pParentMenu != NULL && pParentMenu->GetParentButton () != NULL &&
			!pMenuBar->m_bAreAllCommandsShown)
		{
			pMenuBar->InsertButton (CBCGShowAllButton ());
		}
	}

	return TRUE;
}
//************************************************************************************
#if _MSC_VER >= 1300
	void CBCGPopupMenu::OnActivateApp(BOOL bActive, DWORD /*hTask*/) 
#else
	void CBCGPopupMenu::OnActivateApp(BOOL bActive, HTASK /*hTask*/) 
#endif
{
	if (!bActive && !CBCGToolBar::IsCustomizeMode () &&
		!m_wndMenuBar.m_bInCommand)
	{
		SendMessage (WM_CLOSE);
	}
}
//*************************************************************************************
void CBCGPopupMenu::OnTimer(UINT_PTR nIDEvent) 
{
	CBCGPopupMenuBar* pMenuBar = GetMenuBar ();
	ASSERT_VALID (pMenuBar);

	switch (nIDEvent)
	{
	case iAnimTimerId:
		if (!m_bAnimationIsDone)
		{
			clock_t nCurrAnimTime = clock ();

			int nDuration = nCurrAnimTime - nLastAnimTime;
			int nSteps = (int) (.5 + (float) nDuration / iAnimTimerDuration);

			switch (m_AnimationType)
			{
			case UNFOLD:
				m_AnimSize.cx += nSteps * pMenuBar->GetColumnWidth ();
				// no break intentionally

			case SLIDE:
				m_AnimSize.cy += nSteps * pMenuBar->GetRowHeight ();
				break;

			case FADE:
				m_iFadePercent += iFadeStep;
				if (m_iFadePercent > 100 + nSteps * iFadeStep)
				{
					m_iFadePercent = 101;
				}
				break;
			}

			if ((m_AnimationType != FADE && m_AnimSize.cy + m_iShadowSize >= m_FinalSize.cy) ||
				(m_AnimationType == UNFOLD && m_AnimSize.cx + m_iShadowSize >= m_FinalSize.cx) ||
				(m_AnimationType == FADE && m_iFadePercent > 100))
			{
				m_bAnimationIsDone = TRUE;

				m_AnimSize.cx = m_FinalSize.cx + m_iShadowSize;
				m_AnimSize.cy = m_FinalSize.cy + m_iShadowSize;

				m_ptLocationAnim = m_ptLocation;

				pMenuBar->SetOffset (0);
				KillTimer (iAnimTimerId);
			}
			else
			{
				if (m_AnimationType != FADE)
				{
					if (m_bIsAnimDown)
					{
						pMenuBar->SetOffset (pMenuBar->GetOffset () - 1);
					}

					if (!m_bIsAnimRight && m_AnimationType == UNFOLD)
					{
						m_ptLocationAnim.x -= pMenuBar->GetColumnWidth ();
					}

					if (!m_bIsAnimDown)
					{
						m_ptLocationAnim.y -= pMenuBar->GetRowHeight ();
					}
				}
			}

			switch (m_AnimationType)
			{
			case SLIDE:
			case UNFOLD:
				{
					SetWindowPos (NULL, m_ptLocationAnim.x, m_ptLocationAnim.y, 
						m_AnimSize.cx, m_AnimSize.cy,
						SWP_NOZORDER | SWP_NOACTIVATE);

					//----------------
					// Redraw borders:
					//----------------
					CRect rectClient;
					GetClientRect (rectClient);

					InvalidateRect (CRect (	CPoint (rectClient.left, rectClient.top),
											CSize (rectClient.Width (), iBorderSize)));
					InvalidateRect (CRect (	CPoint (rectClient.left, rectClient.top),
											CSize (iBorderSize, rectClient.Height ())));
					InvalidateRect (CRect (	CPoint (rectClient.right - iBorderSize, rectClient.top),
											CSize (iBorderSize, rectClient.Height ())));

					//-----------------------
					// Redraw scroll buttons:
					//-----------------------
					InvalidateRect (m_rectScrollUp);
					InvalidateRect (m_rectScrollDn);
				}
				break;

			case FADE:
				if (!m_bAnimationIsDone)
				{
					Invalidate ();
					UpdateWindow ();
				}
				break;
			}

			nLastAnimTime = nCurrAnimTime;
		}
		break;

	case iScrollTimerId:
		{
			CPoint point;
			::GetCursorPos (&point);
			ScreenToClient (&point);

			CBCGToolbarMenuButton* pSelItem = GetSelItem ();
			if (pSelItem != NULL)
			{
				pSelItem->OnCancelMode ();
			}

			int iOffset = pMenuBar->GetOffset ();

			if (m_rectScrollUp.PtInRect (point) && m_iScrollMode < 0 &&
				IsScrollUpAvailable ())	// Scroll Up
			{
				pMenuBar->SetOffset (iOffset - 1);
				pMenuBar->AdjustLayout ();
			}
			else if (m_rectScrollDn.PtInRect (point) && m_iScrollMode > 0 &&
				IsScrollDnAvailable ())	// Scroll Down
			{
				pMenuBar->SetOffset (iOffset + 1);
				pMenuBar->AdjustLayout ();
			}
			else
			{
				KillTimer (iScrollTimerId);
				m_iScrollMode = 0;
				InvalidateRect (m_rectScrollDn);
				InvalidateRect (m_rectScrollUp);
			}
		}
		break;
	}

	CMiniFrameWnd::OnTimer(nIDEvent);
}
//****************************************************************************************
void CBCGPopupMenu::OnMouseMove(UINT nFlags, CPoint point) 
{
	CMiniFrameWnd::OnMouseMove(nFlags, point);

	if (!m_bScrollable || m_iScrollMode != 0)
	{
		return;
	}
	
	if (m_rectScrollUp.PtInRect (point) && IsScrollUpAvailable ())
	{
		m_iScrollMode = -1;
		InvalidateRect (m_rectScrollUp);
	}
	else if (m_rectScrollDn.PtInRect (point) && IsScrollDnAvailable ())
	{
		m_iScrollMode = 1;
		InvalidateRect (m_rectScrollDn);
	}
	else
	{
		m_iScrollMode = 0;
	}

	if (m_iScrollMode != 0)
	{
		SetTimer (iScrollTimerId, iScrollTimerDuration, NULL);
	}
}
//****************************************************************************************
BOOL CBCGPopupMenu::IsScrollUpAvailable ()
{
	CBCGPopupMenuBar* pMenuBar = GetMenuBar ();
	ASSERT_VALID (pMenuBar);

	return pMenuBar->GetOffset () > 0;
}
//****************************************************************************************
BOOL CBCGPopupMenu::IsScrollDnAvailable ()
{
	CBCGPopupMenuBar* pMenuBar = GetMenuBar ();
	ASSERT_VALID (pMenuBar);

	if (pMenuBar->GetRowHeight () == 0)
	{
		return FALSE;
	}

	int iTotalRows = m_FinalSize.cy / pMenuBar->GetRowHeight () - 1;
	return (pMenuBar->GetOffset () <= 
		pMenuBar->m_Buttons.GetCount () - iTotalRows);
}
//****************************************************************************************
void CBCGPopupMenu::CollapseSubmenus ()
{
	for (POSITION pos = m_wndMenuBar.m_Buttons.GetHeadPosition (); pos != NULL;)
	{
		CBCGToolbarButton* pButton = 
			(CBCGToolbarButton*) m_wndMenuBar.m_Buttons.GetNext (pos);
		ASSERT (pButton != NULL);

		pButton->OnCancelMode ();
	}
}
//****************************************************************************************
void CBCGPopupMenu::DrawImage (CDC* pDC, const CRect& rect, int iImage, BOOL bDrawFrame)
{
	CRect rectImage (CPoint (0, 0), CMenuImages::Size ());

	CRect rectFill = rect;
	rectFill.top -= 2;

	pDC->FillRect (rectFill, &globalData.brBtnFace);

	CPoint point (
		rect.left + (rect.Width () - rectImage.Width ()) / 2,
		rect.top + (rect.Height () - rectImage.Height ()) / 2);

	CMenuImages::Draw (pDC, (CMenuImages::IMAGES_IDS) iImage, point);

	if (bDrawFrame)
	{
		pDC->Draw3dRect (rect,
			globalData.clrBtnHilite,
			globalData.clrBtnShadow);
	}
}
//****************************************************************************************
void CBCGPopupMenu::ShowAllCommands ()
{
	CBCGToolbarMenuButton* pParentMenuButton =
		DYNAMIC_DOWNCAST (CBCGToolbarMenuButton, m_pParentBtn);
	if (pParentMenuButton != NULL)
	{
		CBCGMenuBar::SetShowAllCommands ();

		// Play standard menu popup sound!
		BCGPlaySystemSound (BCGSOUND_MENU_POPUP);

		ShowWindow (SW_HIDE);
		m_bShown = FALSE;

		if (m_bmpShadowRight.GetSafeHandle () != NULL)
		{
			m_bmpShadowRight.DeleteObject ();
		}
		
		if (m_bmpShadowBottom.GetSafeHandle () != NULL)
		{
			m_bmpShadowBottom.DeleteObject ();
		}

		InitMenuBar ();
		UpdateBottomWindows ();

		ShowWindow (SW_SHOWNOACTIVATE);

		if (pParentMenuButton->m_pWndParent != NULL 
			&& ::IsWindow (pParentMenuButton->m_pWndParent->m_hWnd))
		{
			pParentMenuButton->m_pWndParent->InvalidateRect (
				pParentMenuButton->Rect ());
			pParentMenuButton->m_pWndParent->UpdateWindow ();
		}
	}
}
//**************************************************************************************
void CBCGPopupMenu::SetMaxWidth (int iMaxWidth)
{
	if (iMaxWidth == m_iMaxWidth)
	{
		return;
	}

	m_iMaxWidth = iMaxWidth;
	if (GetSafeHwnd () == NULL)
	{
		return;
	}

	CBCGPopupMenuBar* pMenuBar = GetMenuBar ();
	ASSERT_VALID (pMenuBar);

	if (!::IsWindow (m_hWnd) ||
		!::IsWindow (pMenuBar->m_hWnd))
	{
		return;
	}

	pMenuBar->m_iMaxWidth = m_iMaxWidth;
	RecalcLayout ();
}
//*************************************************************************************
BOOL CBCGPopupMenu::InitMenuBar ()
{
	if (m_hMenu != NULL)
	{
		ASSERT (::IsMenu (m_hMenu));

		if (m_pParentBtn != NULL || 
			!g_menuHash.LoadMenuBar (m_hMenu, &m_wndMenuBar))
		{
			//-------------------------------------------
			// Failed to restore, load the default state:
			//-------------------------------------------
			if (!m_wndMenuBar.ImportFromMenu (m_hMenu, !HideRarelyUsedCommands ()))
			{
				TRACE(_T("Can't import menu\n"));
				return FALSE;
			}
		}
	}

	POSITION pos;

	//----------------------------------------
	// Maybe, we should process the MRU files:
	//----------------------------------------
	CRecentFileList* pMRUFiles = 
		((CBCGApp*) AfxGetApp ())->m_pRecentFileList;

	if (pMRUFiles != NULL && !CBCGToolBar::IsCustomizeMode ())
	{
		int iMRUItemIndex = 0;
		BOOL bIsPrevSeparator = FALSE;

		for (pos = m_wndMenuBar.m_Buttons.GetHeadPosition (); 
			pos != NULL; iMRUItemIndex ++)
		{
			POSITION posSave = pos;

			CBCGToolbarButton* pButton = 
				(CBCGToolbarButton*) m_wndMenuBar.m_Buttons.GetNext (pos);
			ASSERT (pButton != NULL);

			if (pButton->m_nID == ID_FILE_MRU_FILE1 &&
				pButton->m_strText == _T("Recent File"))
			{
				//------------------------------
				// Remove dummy item ("Recent"):
				//------------------------------
				m_wndMenuBar.m_Buttons.RemoveAt (posSave);
				delete pButton;

				TCHAR szCurDir [_MAX_PATH];
				::GetCurrentDirectory (_MAX_PATH, szCurDir);

				int nCurDir = lstrlen (szCurDir);
				ASSERT (nCurDir >= 0);

				szCurDir [nCurDir] = _T('\\');
				szCurDir [++ nCurDir] = _T('\0');

				//---------------
				// Add MRU files:
				//---------------
				int iNumOfFiles = 0;	// Actual added to menu
				for (int i = 0; i < pMRUFiles->GetSize (); i ++)
				{
					CString strName;

					if (pMRUFiles->GetDisplayName (strName, i, 
						szCurDir, nCurDir))
					{
						//---------------------
						// Add shortcut number:
						//---------------------
						CString strItem;
						strItem.Format (_T("&%d %s"), ++iNumOfFiles, strName);

						m_wndMenuBar.InsertButton (
							CBCGToolbarMenuButton (
								ID_FILE_MRU_FILE1 + i, NULL,
								-1, strItem),
							iMRUItemIndex ++);
					}
				}

				//------------------------------------------------------
				// Usualy, the MRU group is "covered" by two seperators.
				// If MRU list is empty, remove redandant separator:
				//------------------------------------------------------
				if (iNumOfFiles == 0 &&	// No files were added
					bIsPrevSeparator &&	// Prev. button was separator
					pos != NULL)		// Not a last button
				{
					posSave = pos;

					pButton = (CBCGToolbarButton*) 
						m_wndMenuBar.m_Buttons.GetNext (pos);
					ASSERT (pButton != NULL);

					if (pButton->m_nStyle & TBBS_SEPARATOR)
					{
						//---------------------------------------
						// Next button also separator, remove it:
						//---------------------------------------
						m_wndMenuBar.m_Buttons.RemoveAt (posSave);
						delete pButton;
					}
				}

				break;
			}

			bIsPrevSeparator = (pButton->m_nStyle & TBBS_SEPARATOR);
		}
	}

	//--------------------------
	// Setup user-defined tools:
	//--------------------------
	if (g_pUserToolsManager != NULL && !CBCGToolBar::IsCustomizeMode ())
	{
		BOOL bToolsAreReady = FALSE;
		int iToolItemIndex = 0;

		for (pos = m_wndMenuBar.m_Buttons.GetHeadPosition (); pos != NULL; iToolItemIndex ++)
		{
			POSITION posSave = pos;

			CBCGToolbarButton* pButton = 
				(CBCGToolbarButton*) m_wndMenuBar.m_Buttons.GetNext (pos);
			ASSERT (pButton != NULL);

			if (g_pUserToolsManager->GetToolsEntryCmd () == pButton->m_nID)
			{
				//----------------------------------------------------
				// Replace dummy tools command by the user tools list:
				//----------------------------------------------------
				m_wndMenuBar.m_Buttons.RemoveAt (posSave);
				delete pButton;

				if (!bToolsAreReady)
				{
					const CObList& lstTools = g_pUserToolsManager->GetUserTools ();
					for (POSITION posTool = lstTools.GetHeadPosition (); posTool != NULL;)
					{
						CBCGUserTool* pTool = (CBCGUserTool*) lstTools.GetNext (posTool);
						ASSERT_VALID (pTool);

						//----------------------------------------------
						// Is user tool associated with the user image?
						//----------------------------------------------
						int iUserImage = CMD_MGR.GetCmdImage (pTool->GetCommandId (), TRUE);

						m_wndMenuBar.InsertButton (
							CBCGToolbarMenuButton (
								pTool->GetCommandId (), NULL,
								iUserImage == -1 ? 0 : iUserImage, pTool->m_strLabel,
								iUserImage != -1),
							iToolItemIndex ++);
					}

					bToolsAreReady = TRUE;
				}
			}
		}
	}

	//-----------------------------------------------------------------------------
	// Maybe, main application frame should update the popup menu context before it
	// displayed (example - windows list):
	//-----------------------------------------------------------------------------
	if (!ActivatePopupMenu (GetTopLevelFrame (), this))
	{
		return FALSE;
	}

	RecalcLayout ();
	return TRUE;
}
//************************************************************************************
BOOL CBCGPopupMenu::HideRarelyUsedCommands () const
{
	return (m_pParentBtn != NULL &&
			m_pParentBtn->GetParentWnd ()->IsKindOf (RUNTIME_CLASS (CBCGMenuBar)));
}
//************************************************************************************
void CBCGPopupMenu::UpdateBottomWindows (BOOL bCheckOnly)
{
	if (m_iShadowSize > 0)
	{
		CWnd* pWndMain = GetTopLevelParent ();
		if (pWndMain != NULL)
		{
			//---------------------------------------------------------
			// If menu will be shown outside of the application window,
			// don't show shadows!
			//---------------------------------------------------------
			CRect rectMain;
			pWndMain->GetWindowRect (rectMain);

			CRect rectMenu (m_ptLocation, 
				CSize (m_FinalSize.cx + m_iShadowSize, m_FinalSize.cy + m_iShadowSize));

			CRect rectInter;
			rectInter.UnionRect (&rectMenu, &rectMain);

			if (rectInter != rectMain)
			{
				m_iShadowSize = 0;

				if (!bCheckOnly)
				{
					SetWindowPos (NULL, -1, -1, m_FinalSize.cx, m_FinalSize.cy,
								SWP_NOMOVE | SWP_NOZORDER |
								SWP_NOACTIVATE);
				}
			}
			else
			{
				pWndMain->UpdateWindow ();
			}
		}
	}
}

// ==================================================================
// 
// FUNCTION :  DrawShadows ()
// 
// * Description : Draws the shadow for a rectangular screen element
// 
// * Authors: [Stas Levin ]
//			  [Timo Hummel], Modified: [8/11/99 5:06:59 PM]
//			  
// * Function parameters : 
// [dc] -		The device context to draw into
// [rect] -		The CRect of the rectangular region to draw the
//			    shadow around (altough the CDC needs to be big enough
//				to hold the shadow)
// ==================================================================
void CBCGPopupMenu::DrawShadows (CDC& dc, const CRect& rect)
{
	const int iMinBrightness = 100;
	const int iMaxBrightness = 50;

	if (m_iShadowSize == 0)
	{
		return;
	}

	int cx = rect.Width ();
	int cy = rect.Height ();

	if (m_bmpShadowRight.GetSafeHandle () != NULL &&
		m_bmpShadowBottom.GetSafeHandle () != NULL)
	{
		//---------------------------------------------------
		// Shadows are already implemented, put them directly
		// to the DC:
		//---------------------------------------------------
		dc.DrawState (CPoint (rect.right, rect.top), 
					CSize (m_iShadowSize, cy + m_iShadowSize),
					&m_bmpShadowRight, DSS_NORMAL);

		dc.DrawState (CPoint (rect.left, rect.bottom),
					CSize (cx + m_iShadowSize, m_iShadowSize),
					&m_bmpShadowBottom, DSS_NORMAL);
		return;
	}

	ASSERT (m_bmpShadowRight.GetSafeHandle () == NULL);
	ASSERT (m_bmpShadowBottom.GetSafeHandle () == NULL);

	//--------------------------------------------
	// Copy screen content into the memory bitmap:
	//--------------------------------------------
	CDC dcMem;
	if (!dcMem.CreateCompatibleDC (&dc))
	{
		return;
	}

	//--------------------------------------------
	// Gets the whole menu and changes the shadow.
	//--------------------------------------------
	CBitmap	bmpMem;
	if (!bmpMem.CreateCompatibleBitmap (&dc, cx + m_iShadowSize, cy + m_iShadowSize))
	{
		return;
	}

	CBitmap* pOldBmp = dcMem.SelectObject(&bmpMem);
	ASSERT (pOldBmp != NULL);

	LPBITMAPINFO lpbi;

		// Fill in the BITMAPINFOHEADER
	lpbi = (LPBITMAPINFO) new BYTE[sizeof(BITMAPINFOHEADER) ];
	lpbi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	lpbi->bmiHeader.biWidth = cx + m_iShadowSize;
	lpbi->bmiHeader.biHeight = cy + m_iShadowSize;
	lpbi->bmiHeader.biPlanes = 1;
	lpbi->bmiHeader.biBitCount = 32;
	lpbi->bmiHeader.biCompression = BI_RGB;
	lpbi->bmiHeader.biSizeImage = (cx + m_iShadowSize) * (cy + m_iShadowSize);
	lpbi->bmiHeader.biXPelsPerMeter = 0;
	lpbi->bmiHeader.biYPelsPerMeter = 0;
	lpbi->bmiHeader.biClrUsed = 0;
	lpbi->bmiHeader.biClrImportant = 0;

	COLORREF* pBits = NULL;
	HBITMAP hmbpDib = CreateDIBSection (
		dcMem.m_hDC, lpbi, DIB_RGB_COLORS, (void **)&pBits,
		NULL, NULL);

	if (hmbpDib == NULL || pBits == NULL)
	{
		delete lpbi;
		return;
	}

	dcMem.SelectObject (hmbpDib);
	dcMem.BitBlt (0, 0, cx + m_iShadowSize, cy + m_iShadowSize, &dc, rect.left, rect.top, SRCCOPY);

	//----------------------------------------------------------------------------
	// Process shadowing:
	// For having a very nice shadow effect, its actually hard work. Currently,
	// I'm using a more or less "hardcoded" way to set the shadows (by using a
	// hardcoded algorythm):
	//
	// This algorythm works as follows:
	// 
	// It always draws a few lines, from left to bottom, from bottom to right,
	// from right to up, and from up to left). It does this for the specified
	// shadow width and the color settings.
	//-----------------------------------------------------------------------------

	// For speeding up things, iShadowOffset is the
	// value which is needed to multiply for each shadow step
	int iShadowOffset = (iMaxBrightness - iMinBrightness) / m_iShadowSize;

	// Loop for drawing the shadow
	// Actually, this was simpler to implement than I thought
	for (int c = 0; c < m_iShadowSize; c++)
	{
		// Draw the shadow from left to bottom
		for (int y = cy; y < cy + (m_iShadowSize - c); y++)
		{
			SetAlphaPixel (pBits, rect, c + m_iShadowSize, y, 
				iMaxBrightness - ((m_iShadowSize  - c) * (iShadowOffset)),m_iShadowSize);
		}

		// Draw the shadow from left to right
		for (int x = m_iShadowSize + (m_iShadowSize - c); x < cx + c; x++)
		{
			SetAlphaPixel(pBits, rect,x, cy + c,
				iMaxBrightness - ((c) * (iShadowOffset)),m_iShadowSize);
		}

		// Draw the shadow from top to bottom
		for (int y1 = m_iShadowSize + (m_iShadowSize - c); y1 < cy + c + 1; y1++)
		{
			SetAlphaPixel(pBits, rect, cx+c, y1, 
				iMaxBrightness - ((c) * (iShadowOffset)),
				m_iShadowSize);
		}
		
		// Draw the shadow from top to left
		for (int x1 = cx; x1 < cx + (m_iShadowSize - c); x1++)
		{
			SetAlphaPixel (pBits, rect, x1, c + m_iShadowSize,
				iMaxBrightness - ((m_iShadowSize - c) * (iShadowOffset)),
				m_iShadowSize);
		}
	}

	//-----------------------------------------
	// Copy shadowed bitmap back to the screen:
	//-----------------------------------------
	dc.BitBlt (rect.left, rect.top, cx + m_iShadowSize, cy + m_iShadowSize, 
				&dcMem, 0, 0, SRCCOPY);

	if (m_bSaveShadows)
	{
		//------------------------------------
		// Save shadows in the memory bitmaps:
		//------------------------------------
		m_bmpShadowRight.CreateCompatibleBitmap (&dc, m_iShadowSize+1, cy + m_iShadowSize);
		
		dcMem.SelectObject (&m_bmpShadowRight);
		dcMem.BitBlt (0, 0, m_iShadowSize, cy + m_iShadowSize,
						&dc, rect.right, rect.top, SRCCOPY);

		m_bmpShadowBottom.CreateCompatibleBitmap (&dc, cx + m_iShadowSize, m_iShadowSize+1);

		dcMem.SelectObject (&m_bmpShadowBottom);
		dcMem.BitBlt (0, 0, cx + m_iShadowSize, m_iShadowSize,
						&dc, rect.left, rect.bottom, SRCCOPY);
	}

	dcMem.SelectObject (pOldBmp);
	DeleteObject (hmbpDib);
	delete lpbi;
}

// ==================================================================
// 
// FUNCTION :  DoPaint ()
// 
// * Description : Actually draw the popup menu window
// 
// * Authors: [Guillaume Nodet ]
//			  
// * Function parameters : 
// [pPaintDC] -	Pointer to the device context to draw into
// ==================================================================
void CBCGPopupMenu::DoPaint (CDC* pPaintDC)
{
	CRect rectClient;	// Client area rectangle
	GetClientRect (&rectClient);

	BOOL bDrawShadows = m_iShadowSize != 0 &&
						(m_AnimationType == FADE || m_bAnimationIsDone) &&
						!CBCGToolBar::IsCustomizeMode ();

	if (bDrawShadows)
	{
		rectClient.right -= m_iShadowSize;
		rectClient.bottom -= m_iShadowSize;

		DrawShadows (*pPaintDC, rectClient);
	}

	pPaintDC->Draw3dRect (rectClient,
							globalData.clrBtnLight, 
							globalData.clrBtnDkShadow);
	rectClient.DeflateRect (1, 1);
	pPaintDC->Draw3dRect (rectClient,
							globalData.clrBtnHilite,
							globalData.clrBtnShadow);

	if (m_bScrollable)
	{
		DrawImage (pPaintDC, m_rectScrollUp, 
			IsScrollUpAvailable () ? CMenuImages::IdArowUp : CMenuImages::IdArowUpDisabled, 
					m_iScrollMode < 0);
		DrawImage (pPaintDC, m_rectScrollDn, 
					IsScrollDnAvailable () ? CMenuImages::IdArowDown : CMenuImages::IdArowDownDsbl,
					m_iScrollMode > 0);
	}

	m_bShown = TRUE;
}

// ==================================================================
// 
// FUNCTION :  DrawFade ()
// 
// * Description : Draw the popup menu window with fading effect
// 
// * Authors: [Guillaume Nodet ]
//			  
// * Function parameters : 
// [pPaintDC] -	Pointer to the device context to draw into
// ==================================================================
void CBCGPopupMenu::DrawFade (CDC* pPaintDC)
{
	CRect rectClient;
	GetClientRect (&rectClient);

	CDC dcMem;
	if (!dcMem.CreateCompatibleDC (pPaintDC))
	{
		return;
	}

	// create the three bitmaps if not done yet
	if( m_iFadeBitmapSrc.GetSafeHandle() == NULL ||
		m_iFadeBitmapDst.GetSafeHandle() == NULL ||
		m_iFadeBitmapTmp.GetSafeHandle() == NULL )
	{
		// Fill in the BITMAPINFOHEADER
		BITMAPINFOHEADER bih;
		bih.biSize = sizeof(BITMAPINFOHEADER);
		bih.biWidth = m_AnimSize.cx;
		bih.biHeight = m_AnimSize.cy;
		bih.biPlanes = 1;
		bih.biBitCount = 32;
		bih.biCompression = BI_RGB;
		bih.biSizeImage = m_AnimSize.cx * m_AnimSize.cy;
		bih.biXPelsPerMeter = 0;
		bih.biYPelsPerMeter = 0;
		bih.biClrUsed = 0;
		bih.biClrImportant = 0;

		HBITMAP hmbpDib;
		// Create a DIB section and attach it to the source bitmap
		hmbpDib = CreateDIBSection (
			dcMem.m_hDC, (LPBITMAPINFO)&bih, DIB_RGB_COLORS, (void **)&m_cFadeSrcBits,
			NULL, NULL);
		if (hmbpDib == NULL || m_cFadeSrcBits == NULL)
		{
			return;
		}
		m_iFadeBitmapSrc.Attach( hmbpDib );

		// Create a DIB section and attach it to the destination bitmap
		hmbpDib = CreateDIBSection (
			dcMem.m_hDC, (LPBITMAPINFO)&bih, DIB_RGB_COLORS, (void **)&m_cFadeDstBits,
			NULL, NULL);
		if (hmbpDib == NULL || m_cFadeDstBits == NULL)
		{
			return;
		}
		m_iFadeBitmapDst.Attach( hmbpDib );

		// Create a DIB section and attach it to the temporary bitmap
		hmbpDib = CreateDIBSection (
			dcMem.m_hDC, (LPBITMAPINFO)&bih, DIB_RGB_COLORS, (void **)&m_cFadeTmpBits,
			NULL, NULL);
		if (hmbpDib == NULL || m_cFadeTmpBits == NULL)
		{
			return;
		}
		m_iFadeBitmapTmp.Attach( hmbpDib );

		// get source image, representing the window below the popup menu
		CBitmap* oldBmp = dcMem.SelectObject (&m_iFadeBitmapSrc);
		dcMem.BitBlt (0, 0, m_AnimSize.cx, m_AnimSize.cy, pPaintDC, rectClient.left, rectClient.top, SRCCOPY);

		// copy it to the destination so that shadow will be ok
		memcpy (m_cFadeDstBits, m_cFadeSrcBits, sizeof(COLORREF)*m_AnimSize.cx*m_AnimSize.cy);

		// get final image
		CRect rect;
		dcMem.SelectObject (m_iFadeBitmapDst);

		DoPaint (&dcMem);

		GetMenuBar()->GetWindowRect (&rect);
		ScreenToClient (&rect);

		dcMem.SetViewportOrg (rect.TopLeft());
		GetMenuBar()->DoPaint (&dcMem);
		dcMem.SetViewportOrg (CPoint (0,0));

		dcMem.SelectObject (oldBmp);
	}
	
	COLORREF *src = m_cFadeSrcBits;
	COLORREF *dst = m_cFadeDstBits;
	COLORREF *tmp = m_cFadeTmpBits;
	for( int pixel=0; pixel<m_AnimSize.cx*m_AnimSize.cy; pixel++ )
	{
		*tmp++ = PixelAlpha (*src++, *dst++, 100-m_iFadePercent);
	}

	CBitmap* oldBmp = dcMem.SelectObject (&m_iFadeBitmapTmp);
	pPaintDC->BitBlt (rectClient.left, rectClient.top, m_AnimSize.cx, m_AnimSize.cy, &dcMem, 0, 0, SRCCOPY);
	dcMem.SelectObject( oldBmp );

	if (m_iFadePercent >= 100)
	{
		GetMenuBar()->SetWindowPos (NULL,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOREDRAW|SWP_NOZORDER|SWP_SHOWWINDOW);
		GetMenuBar()->ValidateRect (NULL);
	}
}

// ==================================================================
// 
// FUNCTION :  static void SetAlphaPixel ()
// 
// * Description : Draws an alpha blended pixel
// 
// * Author : [Timo Hummel], Created : [8/11/99 5:04:38 PM]
// 
// * Function parameters : 
// [pBits] -	The DIB bits
// [x] -		X-Coordinate
// [y] -		Y-Coordinate
// [percent] -	Percentage to blit (100 = hollow, 0 = solid)
// 
// ==================================================================
inline static void SetAlphaPixel (COLORREF* pBits, CRect rect, int x, int y, int percent, int m_iShadowSize)
{
	// Our direct bitmap access swapped the y coordinate...
	y = (rect.Height()+m_iShadowSize)- y;

	COLORREF* pColor = (COLORREF*) (pBits + (rect.Width () + m_iShadowSize) * y + x);
	*pColor = PixelAlpha (*pColor, percent);
}

// ==================================================================
// 
// FUNCTION :  PixelAlpha ()
// 
// * Description : Shades a color value with a specified percentage
// 
// * Author : [Timo Hummel], Created : [8/11/99 2:37:04 PM]
// 
// * Returns : [COLORREF] - The result pixel
// 
// * Function parameters : 
// [srcPixel] - The source pixel
// [percent] -  Percentage (amount of shadow)
//
// Example: percent = 10    makes the pixel around 10 times darker
//          percent = 50    makes the pixel around 2 times darker
// 
// ==================================================================
COLORREF PixelAlpha (COLORREF srcPixel, int percent)
{
	// My formula for calculating the transpareny is as
	// follows (for each single color):
	//
	//							   percent
	// destPixel = sourcePixel * ( ------- )
	//                               100
	//
	// This is not real alpha blending, as it only modifies the brightness,
	// but not the color (a real alpha blending had to mix the source and
	// destination pixels, e.g. mixing green and red makes yellow).
	// For our nice "menu" shadows its good enough.

	COLORREF clrFinal = RGB ( (GetRValue (srcPixel) * percent) / 100, 
							  (GetGValue (srcPixel) * percent) / 100, 
							  (GetBValue (srcPixel) * percent) / 100);

	return (clrFinal);

}

// ==================================================================
// 
// FUNCTION :  PixelAlpha ()
// 
// * Description : Shades a color value with a specified percentage
// 
// * Author : [Guillaume Nodet]
// 
// * Returns : [COLORREF] - The result pixel
// 
// * Function parameters : 
// [srcPixel] - The source pixel
// [dstPixel] - The destination pixel
// [percent] -  Percentage (amount of shadow)
//
// ==================================================================
COLORREF PixelAlpha (COLORREF srcPixel, COLORREF dstPixel, int percent)
{
	int ipercent = 100 - percent;
	COLORREF clrFinal = RGB ( (GetRValue (srcPixel) * percent + GetRValue (dstPixel) * ipercent) / 100, 
							  (GetGValue (srcPixel) * percent + GetGValue (dstPixel) * ipercent) / 100, 
							  (GetBValue (srcPixel) * percent + GetBValue (dstPixel) * ipercent) / 100);

	return (clrFinal);

}


BOOL CBCGPopupMenu::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	// Added by Alex Corazzin:

//modified by andy
//	if( g_bWaitingForCommandFlag )
//		return TRUE;
//end
	if (!CMiniFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
	{
		return m_pMessageWnd != NULL ? 
			m_pMessageWnd->OnCmdMsg (nID, nCode, pExtra, pHandlerInfo) : FALSE;
	}

	return TRUE;
}


BOOL CBCGPopupMenu::PostCommand (UINT commandID)
{
	// Added by Alex Corazzin
	if (m_pMessageWnd != NULL)
	{
//modified by andy
//		g_nLastCommandID = commandID;

//		if( !g_bWaitingForCommandFlag )
//end
		return m_pMessageWnd->PostMessage (WM_COMMAND, commandID);
	}

	return FALSE;
}

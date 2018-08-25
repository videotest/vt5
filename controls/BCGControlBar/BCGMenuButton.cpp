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

// BCGMenuButton.cpp : implementation file
//

#include "stdafx.h"
//#include "MenuImages.h"
#include "bcgcontrolbar.h"
#include "BCGMenuButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const int nImageHorzMargin = 10;

/////////////////////////////////////////////////////////////////////////////
// CBCGMenuButton

IMPLEMENT_DYNAMIC(CBCGMenuButton, CBCGButton)

CBCGMenuButton::CBCGMenuButton()
{
	m_bRightArrow = FALSE;
	m_hMenu = NULL;
	m_nMenuResult = 0;
	m_bMenuIsActive = FALSE;
}
//*****************************************************************************************
CBCGMenuButton::~CBCGMenuButton()
{
}


BEGIN_MESSAGE_MAP(CBCGMenuButton, CBCGButton)
	//{{AFX_MSG_MAP(CBCGMenuButton)
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_GETDLGCODE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CBCGMenuButton message handlers

CSize CBCGMenuButton::SizeToContent (BOOL bCalcOnly)
{
	CSize size = CBCGButton::SizeToContent (FALSE);
	size.cx += CMenuImages::Size ().cx;

	if (!bCalcOnly)
	{
		SetWindowPos (NULL, -1, -1, size.cx, size.cy,
			SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
	}

	return size;
}
//*****************************************************************************************
void CBCGMenuButton::OnDraw (CDC* pDC, const CRect& rect, UINT uiState)
{
	ASSERT_VALID (pDC);

	CSize sizeArrow = CMenuImages::Size ();

	CRect rectParent = rect;
	rectParent.right -= sizeArrow.cx + nImageHorzMargin;

	CBCGButton::OnDraw (pDC, rectParent, uiState);

	int iImage;
	if (!m_bRightArrow)
	{
		iImage = (uiState & ODS_DISABLED) ? CMenuImages::IdArowDownDsbl : CMenuImages::IdArowDown;
	}
	else
	{
		iImage = (uiState & ODS_DISABLED) ? CMenuImages::IdArowLeftDsbl : CMenuImages::IdArowLeft;
	}

	CRect rectArrow = rect;
	rectArrow.left = rectParent.right;

	CPoint pointImage (
		rectArrow.left + (rectArrow.Width () - sizeArrow.cx) / 2,
		rectArrow.top + (rectArrow.Height () - sizeArrow.cy) / 2);

	CMenuImages::Draw (pDC, (CMenuImages::IMAGES_IDS) iImage, pointImage);
}
//*****************************************************************************************
void CBCGMenuButton::OnShowMenu () 
{
	if (m_hMenu == NULL || m_bMenuIsActive)
	{
		return;
	}

	CRect rectWindow;
	GetWindowRect (rectWindow);

	int x, y;

	if (m_bRightArrow)
	{
		x = rectWindow.right;
		y = rectWindow.top;
	}
	else
	{
		x = rectWindow.left;
		y = rectWindow.bottom;
	}

	m_bMenuIsActive = TRUE;
	Invalidate ();

	m_nMenuResult = ::TrackPopupMenu (m_hMenu, 
		TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD, 
		x, y, 0, GetSafeHwnd (), NULL);

	if (m_nMenuResult != 0)
	{
		//-------------------------------------------------------
		// Trigger mouse up event (to button click notification):
		//-------------------------------------------------------
		CWnd* pParent = GetParent ();
		if (pParent != NULL)
		{
			pParent->SendMessage (	WM_COMMAND,
									MAKEWPARAM (GetDlgCtrlID (), BN_CLICKED),
									(LPARAM) m_hWnd);
		}
	}

	// -- By Thomas Freudenberg:
	m_bPushed = FALSE;
	m_bHighlighted = FALSE;
	m_bMenuIsActive = FALSE;
	
	Invalidate ();
	UpdateWindow ();

	if (m_bCaptured)
	{
		ReleaseCapture ();
		m_bCaptured = FALSE;
	}
	//--
}
//*****************************************************************************************
void CBCGMenuButton::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar == VK_SPACE || nChar == VK_DOWN)
	{
		OnShowMenu ();
		return;
	}
	
	CButton::OnKeyDown(nChar, nRepCnt, nFlags);
}
//*****************************************************************************************
void CBCGMenuButton::OnLButtonDown(UINT /*nFlags*/, CPoint /*point*/) 
{
	SetFocus ();
	OnShowMenu ();
}
//*****************************************************************************************
UINT CBCGMenuButton::OnGetDlgCode() 
{
	return DLGC_WANTARROWS;
}

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

// BCGMDIChildWnd.cpp : implementation file
//

#include "stdafx.h"
#include "bcgcontrolbar.h"
#include "BCGMDIFrameWnd.h"
#include "BCGMDIChildWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBCGMDIChildWnd

IMPLEMENT_DYNCREATE(CBCGMDIChildWnd, CMDIChildWnd)

CBCGMDIChildWnd::CBCGMDIChildWnd()
{
	m_pMDIFrame = NULL;
}

CBCGMDIChildWnd::~CBCGMDIChildWnd()
{
}


BEGIN_MESSAGE_MAP(CBCGMDIChildWnd, CMDIChildWnd)
	//{{AFX_MSG_MAP(CBCGMDIChildWnd)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBCGMDIChildWnd message handlers

BOOL CBCGMDIChildWnd::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST &&
		m_pMDIFrame != NULL &&
		m_pMDIFrame->GetActivePopup () != NULL)
	{
		// Don't process accelerators if popup window is active
		return FALSE;
	}

	return CMDIChildWnd::PreTranslateMessage(pMsg);
}
//*******************************************************************************
int CBCGMDIChildWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_pMDIFrame = DYNAMIC_DOWNCAST (CBCGMDIFrameWnd, GetMDIFrame ());
	ASSERT_VALID (m_pMDIFrame);
	
	return 0;
}
//*************************************************************************************
void CBCGMDIChildWnd::DockControlBarLeftOf(CControlBar* pBar, CControlBar* pLeftOf)
{
	ASSERT_VALID (this);
	ASSERT_VALID (pBar);
	ASSERT_VALID (pLeftOf);

	CRect rect;
	DWORD dw;
	UINT n;
	
	// get MFC to adjust the dimensions of all docked ToolBars
	// so that GetWindowRect will be accurate
	RecalcLayout(TRUE);
	
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
	DockControlBar (pBar,n,&rect);
}

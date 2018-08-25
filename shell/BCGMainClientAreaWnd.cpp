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

// BCGMainClientAreaWnd.cpp : implementation file
//

#include "stdafx.h"
#include "BCGMainClientAreaWnd.h"

/////////////////////////////////////////////////////////////////////////////
// CBCGMainClientAreaWnd

CBCGMainClientAreaWnd::CBCGMainClientAreaWnd()
{
}

CBCGMainClientAreaWnd::~CBCGMainClientAreaWnd()
{
}


BEGIN_MESSAGE_MAP(CBCGMainClientAreaWnd, CWnd)
	//{{AFX_MSG_MAP(CBCGMainClientAreaWnd)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_MDISETMENU,OnSetMenu)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CBCGMainClientAreaWnd message handlers

afx_msg LRESULT CBCGMainClientAreaWnd::OnSetMenu (WPARAM wp, LPARAM lp)
{
	CMFCMDIFrameWnd* pMainFrame = DYNAMIC_DOWNCAST(CMFCMDIFrameWnd, GetParentFrame());
	if (pMainFrame != NULL && ::IsWindow (pMainFrame->GetSafeHwnd ()))
	{
		if (pMainFrame->OnSetMenu ((HMENU) wp))
		{
			wp = NULL;
		}
	}
	else
	{
		wp = NULL;
	}

	return DefWindowProc (WM_MDISETMENU, wp, lp);
}
//*********************************************************************************
BOOL CBCGMainClientAreaWnd::OnEraseBkgnd(CDC* pDC) 
{
	CMDIFrameWndEx* pMainFrame = DYNAMIC_DOWNCAST (CMDIFrameWndEx, GetParentFrame ());
	if (pMainFrame != NULL && pMainFrame->OnEraseMDIClientBackground (pDC))
	{
		return TRUE;
	}

	return CWnd::OnEraseBkgnd(pDC);
}

// XPBarMgr.cpp : implementation file
//

#include "stdafx.h"
#include "shell.h"
#include "XPBarMgr.h"


// CXPBarMgr

IMPLEMENT_DYNAMIC(CXPBarMgr, CControlBar)
CXPBarMgr::CXPBarMgr()
{
	m_hWndXPbar = NULL;
	m_nWidth = 0;
}

CXPBarMgr::~CXPBarMgr()
{
}


BEGIN_MESSAGE_MAP(CXPBarMgr, CControlBar)
	ON_WM_WINDOWPOSCHANGED()
END_MESSAGE_MAP()



// CXPBarMgr message handlers

void CXPBarMgr::OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL /*bDisableIfNoHndler*/)
{
}

void CXPBarMgr::SetXPBarPos()
{
	CRect rcWnd;
	GetWindowRect(&rcWnd);
	if (m_hWndXPbar)
	{
		CWnd *pWndParent = CWnd::FromHandle(m_hWndXPbar)->GetParent();
		if (pWndParent)
			pWndParent->ScreenToClient(&rcWnd);
		::SetWindowPos(m_hWndXPbar, HWND_TOP, rcWnd.left, rcWnd.top, rcWnd.Width(), rcWnd.Height(), 0);
	}
}



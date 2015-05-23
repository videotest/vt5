// ContextView.cpp : implementation file
//

#include "stdafx.h"
#include "data.h"
#include "ContextView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CContextView

CContextView::CContextView()
{
}

CContextView::~CContextView()
{
}


BEGIN_MESSAGE_MAP(CContextView, CWnd)
	//{{AFX_MSG_MAP(CContextView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CContextView message handlers

BOOL CContextView::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID) 
{
	return CWnd::Create(AfxRegisterWndClass(CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW, 0, (HBRUSH)(COLOR_3DFACE + 1), 0), 
						"ContextView", dwStyle | WS_CHILD | WS_VISIBLE| WS_CLIPCHILDREN | WS_CLIPSIBLINGS, rect, pParentWnd, nID, NULL);
}

int CContextView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	
	return 0;
}

void CContextView::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	
}

void CContextView::OnSetFocus(CWnd* pOldWnd) 
{
	CWnd::OnSetFocus(pOldWnd);
	
	// TODO: Add your message handler code here
	
}

void CContextView::OnDestroy() 
{
	CWnd::OnDestroy();
	
	// TODO: Add your message handler code here
	
}

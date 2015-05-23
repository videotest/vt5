// WndSizer.cpp : implementation file
//
#include "stdafx.h"
#include "WndSizer.h"
#include "FileOpenDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWndSizer



CWndSizer::CWndSizer()
{
	sizeMode = smNone;
}

CWndSizer::~CWndSizer()
{
}


BEGIN_MESSAGE_MAP(CWndSizer, CWnd)
	//{{AFX_MSG_MAP(CWndSizer)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_SETCURSOR()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CWndSizer message handlers

void CWndSizer::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	CRect rc;
	GetClientRect(rc);
	rc.InflateRect(1,1,0,0);

	
	dc.DrawFrameControl(rc,DFC_SCROLL,DFCS_SCROLLSIZEGRIP);
}

void CWndSizer::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if(sizeMode == smSetCursor)
	{
		SetCapture();
		sizeMode = smSizing;
		m_pParent->OnStartSize();
	}
	
	CWnd::OnLButtonDown(nFlags, point);
}

BOOL CWndSizer::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	// TODO: Add your message handler code here and/or call default
	CPoint  pt;
	GetCursorPos( &pt );
	ScreenToClient( &pt );
	CRect rc;
	GetClientRect(&rc);
	rc.InflateRect(1,1,0,0);

	if( pt.x+pt.y >= rc.Width())
	{
		::SetCursor(LoadCursor(NULL, IDC_SIZENWSE));
		sizeMode = smSetCursor;
	}
	else
	{
		sizeMode = smNone;
        return CWnd::OnSetCursor(pWnd, nHitTest, message);
	}
	return true;
}

void CWndSizer::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	if (sizeMode == smSizing)
	{
		CRect rc;
		GetClientRect(&rc);
		point.x = rc.Width() - point.x;
		point.y = rc.Height() - point.y;
		m_pParent->OnOurSize(point);
		//::SendMessage(m_pParent->m_hWnd, WM_OURSIZE, (WPARAM)point.x, (LPARAM)point.y);
	}
	CWnd::OnMouseMove(nFlags, point);
}

void CWndSizer::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	ReleaseCapture();	
	sizeMode = smSetCursor;
	m_pParent->OnEndSize();
	CWnd::OnLButtonUp(nFlags, point);
}

int CWndSizer::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	m_pParent = (CFileOpenDlg*)GetParent();
	return 0;
}

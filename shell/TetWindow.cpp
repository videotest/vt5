// TetWindow.cpp : implementation file
//

#include "stdafx.h"
#include "shell.h"
#include "TetWindow.h"
#include "tetgame.h"

/////////////////////////////////////////////////////////////////////////////
// CTetWindow

CTetWindow::CTetWindow()
{
	m_pgame = new CTetGame( this );

}

CTetWindow::~CTetWindow()
{
	delete m_pgame;
}


BEGIN_MESSAGE_MAP(CTetWindow, CWnd)
	//{{AFX_MSG_MAP(CTetWindow)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	ON_WM_GETDLGCODE()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTetWindow message handlers


void CTetWindow::OnPaint() 
{

	static	bool	bInside = false;

	ASSERT( !bInside );
	bInside = true;
	CPaintDC dc(this); // device context for painting

	CDC	dcMemory;
	dcMemory.CreateCompatibleDC( &dc );

	CRect	rect;
	GetClientRect( &rect );
	CBitmap	bmp;
	int	cx = rect.Width();
	int	cy = rect.Height();

	bmp.CreateCompatibleBitmap( &dc, cx, cy );

	CBitmap	*pold = dcMemory.SelectObject( &bmp );

	dcMemory.FillRect( rect, &CBrush( ::GetSysColor( COLOR_3DFACE )) );
	m_pgame->OnDraw( &dcMemory );

	dc.BitBlt( 0, 0, cx, cy, &dcMemory, 0, 0, SRCCOPY );
	dcMemory.SelectObject( pold );
	bInside = false;
}

void CTetWindow::OnTimer(UINT_PTR nIDEvent) 
{
	m_pgame->OnTimer();
	//CWnd::OnTimer(nIDEvent);
}

int CTetWindow::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	SetTimer( 777, 100, 0 );
	
	return 0;
}

void CTetWindow::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
	CRect	rect;
	GetClientRect( &rect );
	m_pgame->SetRect( rect );
}

void CTetWindow::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if( nChar == VK_F2 )
		m_pgame->NewGame();
	else if( nChar == VK_LEFT )
		m_pgame->MoveLeft();
	else if( nChar == VK_RIGHT )
		m_pgame->MoveRight();
	else if( nChar == VK_UP )
		m_pgame->Rotate();
	else if( nChar == VK_SPACE )
		m_pgame->Drop();
	
//	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CTetWindow::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
//	if( nChar == VK_SPACE )
//		m_pgame->Drop();
	
	CWnd::OnChar(nChar, nRepCnt, nFlags);
}

UINT CTetWindow::OnGetDlgCode() 
{
	return DLGC_WANTARROWS;
}

BOOL CTetWindow::OnEraseBkgnd(CDC* pDC) 
{
	
	//return CWnd::OnEraseBkgnd(pDC);
	return true;
}

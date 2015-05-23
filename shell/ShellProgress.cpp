#include "stdafx.h"
#include "ShellProgress.h"

BEGIN_MESSAGE_MAP(CShellProgress, CWnd)
	//{{AFX_MSG_MAP(CConvertorChooser)
	//ON_WM_CREATE()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SETTEXT, OnSetText)
END_MESSAGE_MAP()



CShellProgress::CShellProgress()
{
	//m_paneSide = psRight;
	m_nCurPersent = 0;
	m_nPrevPersent = 0;
	m_bLockUpdate = false;
}

LRESULT CShellProgress::OnSetText( WPARAM wParam, LPARAM lParam )
{
	if( m_bLockUpdate != 0 )
		return 0;
	DefWindowProc( WM_SETTEXT, wParam, lParam );

	Invalidate();
	UpdateWindow();

	return 0;
}

void CShellProgress::SetPercent( int nNewPercent )
{
	if( nNewPercent == 0 )
	{
		m_nCurPersent = 0;
		m_nPrevPersent = 0;
		Invalidate();
	}
	else
	{
		m_nPrevPersent = m_nCurPersent;
		m_nCurPersent = nNewPercent;

		CRect	rect;
		GetClientRect( &rect );

		int	nWidth = rect.Width();
		rect.left = m_nPrevPersent*nWidth/100;
		rect.right = m_nCurPersent*nWidth/100;

		InvalidateRect( rect );
		UpdateWindow();
	}
}


void CShellProgress::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	CRect	rc;
	GetClientRect(&rc);

	HBRUSH	hbrush = (HBRUSH)GetParent()->SendMessage( WM_CTLCOLORSTATIC, (WPARAM)(HDC)dc, (LPARAM)GetSafeHwnd() );
	if( !hbrush )hbrush = ::GetSysColorBrush( COLOR_3DFACE );
	::FillRect( dc, rc, hbrush );

	CString	str;
	GetWindowText( str );

	dc.SelectObject( GetParent()->GetFont() );

	CRect	rcText = rc;
	rcText.InflateRect( -1, -1 );
	dc.SetBkMode( TRANSPARENT );
	dc.DrawText( str, rcText, DT_WORDBREAK );
	
	int	nWidth = rc.Width();
	rc.left = 0;
	rc.right = nWidth*m_nCurPersent/100;
	dc.InvertRect(&rc);
}

void CShellProgress::LockUpdateText( bool bLock)
{
	m_bLockUpdate = bLock;
}
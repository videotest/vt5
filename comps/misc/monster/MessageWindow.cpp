// MessageWindow.cpp : implementation file
//

#include "stdafx.h"
#include "monster.h"
#include "MessageWindow.h"
#include "MonsterWindow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMessageWindow

CMessageWindow::CMessageWindow( CActionMonsterMessage *pa )
{
	m_pParent = pa;

	//Defaults
	m_clrFrameColor = RGB(0, 0, 255);  //blue
	m_clrBkColor = RGB(249, 254, 188); //light yellow
	m_iWidth = 160;
	m_iHeight = 80;

	m_clrTextColor = RGB( 0, 0, 0 ); //black
	m_iFontHeight = 14;
	m_strFontName = "Arial Cyr";
}

CMessageWindow::~CMessageWindow()
{
	CMonsterWindow::s_pMonsterWindow->SetMessageWindow( 0 );
	m_pParent->Finalize();
}


BEGIN_MESSAGE_MAP(CMessageWindow, CWnd)
	//{{AFX_MSG_MAP(CMessageWindow)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_SETFOCUS()
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMessageWindow message handlers

BOOL CMessageWindow::Create( CWnd* pParentWnd, const char *szText ) 
{

	m_strMessage = szText;

	CRect	rcWindow;
	pParentWnd->GetWindowRect( &rcWindow );

	CMonsterWindow::s_pMonsterWindow->SetMessageWindow( this );

	if( !CWnd::CreateEx(WS_EX_TOPMOST|WS_EX_TOOLWINDOW, 
		AfxRegisterWndClass(0), 
		NULL, 
		WS_POPUP|WS_SYSMENU|WS_VISIBLE, rcWindow.right, rcWindow.CenterPoint().y, m_iWidth, m_iHeight, 
		0, NULL, NULL ) )
		return FALSE;

	GiveFocusToApp();

	return TRUE;

}


void CMessageWindow::OnPaint() 
{
   CPaintDC dc( this ); // device context for painting
   
   CRect rectCl;
   GetClientRect( &rectCl );
   
   CRgn rgnComb;	
   rgnComb.CreateRectRgn( rectCl.left+10,rectCl.top,rectCl.right,rectCl.bottom );
   
   int iRetComb = rgnComb.CombineRgn( &m_rgnTri, &m_rgn, RGN_OR );
   if ( iRetComb == ERROR )
   {
      AfxMessageBox( "ERROR in Combining Region" );
      return;
   }
   
   CBrush pBrush;
   pBrush.CreateSolidBrush( m_clrFrameColor );
   
   CBrush pBrush1;
   pBrush1.CreateSolidBrush( m_clrBkColor );
   
   dc.FillRgn( &rgnComb, &pBrush1 );
   dc.FrameRgn( &rgnComb, &pBrush, 2, 1 );
   
   dc.SetBkMode( TRANSPARENT );
   dc.SetTextColor( m_clrTextColor );
   
   CFont *pFont = dc.SelectObject( &m_fontText );
   
   CSize czTextWidth = dc.GetTextExtent( m_strMessage );
   
   if ( czTextWidth.cx < m_rectText.Width() )
      dc.DrawText( m_strMessage, m_rectText, DT_CENTER | DT_VCENTER | DT_SINGLELINE );
   else
      dc.DrawText( m_strMessage, m_rectText, DT_CENTER | DT_WORDBREAK );

   dc.SelectObject( pFont );
}

int CMessageWindow::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
   if ( CWnd::OnCreate( lpCreateStruct ) == -1 )
      return -1;
   
   CRect rectCl;
   GetClientRect( &rectCl );
   
   int x=0, y=0;
   CRect rectTemp;
   
   rectTemp = rectCl;
   rectTemp.left = rectTemp.left + 10;
   
   x = (int)( (float)( (float)rectTemp.Width() / 2.0 ) / 1.41421 );
   y = (int)( (float)( (float)rectTemp.Height() / 2.0 ) / 1.41421 );
   
   m_rectText.top = ( ( rectTemp.Height() / 2 ) - y );
   m_rectText.left = ( ( rectTemp.Width() / 2 ) - x ) + 10;
   m_rectText.right = ( ( rectTemp.Width() / 2 ) + x ) + 10;
   m_rectText.bottom = ( ( rectTemp.Height() / 2 ) + y );
   
   m_rgn.m_hObject = NULL;
   m_rgnTri.m_hObject = NULL;
   m_rgnComb.m_hObject = NULL;
   
   BOOL bRegRet = m_rgn.CreateEllipticRgn( rectCl.left+10,rectCl.top,rectCl.right,rectCl.bottom );
   
   CPoint ptTri[4];
   ptTri[0].x = rectCl.left;
   ptTri[0].y = ( rectCl.bottom / 2 ) - 10;
   
   ptTri[1].x = rectCl.left + 15;
   ptTri[1].y = ( rectCl.bottom / 2 ) - 5;
   
   ptTri[2].x = rectCl.left + 15;
   ptTri[2].y = ( rectCl.bottom / 2 ) + 5;
   
   ptTri[3].x = rectCl.left;
   ptTri[3].y = ( rectCl.bottom / 2 ) - 10;
   
   BOOL bRegTriRet = m_rgnTri.CreatePolygonRgn( ptTri, 3, ALTERNATE );
   
   m_rgnComb.CreateRectRgn( rectCl.left+10,rectCl.top,rectCl.right,rectCl.bottom );
   int iRetComb = m_rgnComb.CombineRgn( &m_rgnTri, &m_rgn, RGN_OR );
   
   if ( iRetComb == ERROR )
   {
      AfxMessageBox( "ERROR in Combining Region" );
      return -1;
   }
   
   int bRgnWnd = SetWindowRgn( m_rgnComb.operator HRGN( ), TRUE );	
   
   m_fontText.CreateFont( m_iFontHeight, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, m_strFontName );

   return 0;
}

void CMessageWindow::OnLButtonDown(UINT nFlags, CPoint point) 
{
	DestroyWindow();
	GiveFocusToApp();
}

void CMessageWindow::PostNcDestroy() 
{
	delete this;
}

void CMessageWindow::PlaceWindow()
{
	CRect	rcWindow;
	CMonsterWindow::s_pMonsterWindow->GetWindowRect( &rcWindow );

	CRect	rcThisWindow;
	GetWindowRect( &rcThisWindow );

	MoveWindow( rcWindow.right, rcWindow.CenterPoint().y, rcThisWindow.Width(), rcThisWindow.Height() );
}

void CMessageWindow::SetMessageText( const char *szMessage )
{	
	m_strMessage = szMessage;
	if( GetSafeHwnd() )
		Invalidate();
}

void CMessageWindow::OnSetFocus(CWnd* pOldWnd) 
{
	//CMonsterWindow::s_pMonsterWindow->SetFocus();
	/*CWnd::OnSetFocus( pOldWnd );

	HWND	hwndMainWindow;
	sptrIApplication	sptrA( GetAppUnknown() );

	sptrA->GetMainWindowHandle( &hwndMainWindow );
	::SetFocus( hwndMainWindow );*/
}

BOOL CMessageWindow::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	SetCursor( AfxGetApp()->LoadCursor( IDC_MONSTER_MOVE ) );
	return true;
}

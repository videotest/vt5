#include "stdafx.h"
#include "MdiClient.h"
#include "resource.h"
#include "MainFrm.h"
#include "shellframe.h"
#include "..\awin\render.h"
#include "shell.h"



CMdiClient::CMdiClient()
{
	m_strLabel = ::LanguageLoadCString( IDS_TITLE );
	//m_strLabel.LoadString( IDS_TITLE );
	m_prender = 0;
}

CMdiClient::~CMdiClient()
{
	delete m_prender;
}

IMPLEMENT_DYNCREATE(CMdiClient, CWnd)

BEGIN_MESSAGE_MAP(CMdiClient, CBCGMainClientAreaWnd)
	//{{AFX_MSG_MAP(CMdiClient)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CMdiClient::OnEraseBkgnd(CDC* pDC) 
{
	if( !m_prender )
		CBCGMainClientAreaWnd::OnEraseBkgnd(pDC);

	return TRUE;
}


void CMdiClient::OnSize(UINT nType, int cx, int cy) 
{	

	CBCGMainClientAreaWnd::OnSize(nType, cx, cy);

	if( m_prender )
	{
		_rect	rect( 0, 0, cx, cy );
		m_prender->layout( rect );
	}
	
	//if (m_pBmp != NULL)
		Invalidate();
}

void CMdiClient::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	if( m_prender )
		m_prender->paint( dc );
	else
	{
		CFont	font;
		CRect	rc;

		font.CreateFont( 30, 15, 0, 0, FW_BOLD, FALSE,
								 0, 0, 1, 0, 0,
								 0, 0, "Arial" );

		

		CFont *pfont = (CFont *)
		dc.SelectObject( &font );

		dc.DrawText( m_strLabel, rc, DT_CALCRECT );

		CRect	rcClient;
		GetClientRect( rcClient );

		CSize	size = rc.Size();

		rc.left = rcClient.right - 40 - size.cx;
		rc.top = rcClient.bottom - 40 - size.cy;

		rc.right = rcClient.right - 40;
		rc.bottom = rcClient.bottom - 40;
		
		dc.SetBkMode( TRANSPARENT );
		dc.SetTextColor( RGB( 255, 255, 0 ) );
		dc.DrawText( m_strLabel, rc, DT_LEFT|DT_SINGLELINE );

		dc.SelectObject( pfont );
	}
}


void CMdiClient::SetLabel( const char  *pszLabel )
{
	m_strLabel = pszLabel;
	Invalidate();
}

LRESULT CMdiClient::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	CWnd* pActiveWnd = NULL;

	CMainFrame	*pMainFrame = (CMainFrame*)GetParent();

	if( pMainFrame )pActiveWnd = pMainFrame->_GetActiveFrame();

	if( pActiveWnd && pActiveWnd->IsKindOf( RUNTIME_CLASS( CShellFrame ) ) )
	{
		// [vanek] SBT: 608 - 23.01.2004
		CShellFrame	*psf = (CShellFrame	*)pActiveWnd;
		if( psf->IsAlwaysZoomed() )
		{
           if( message == WM_MDIRESTORE || 
				message == WM_MDICASCADE ||
				//message == WM_MDIICONARRANGE ||
				message == WM_MDITILE) 
				return 1;

			if( message == WM_MDIGETACTIVE )
			{
				LRESULT lres = CBCGMainClientAreaWnd::WindowProc(message, wParam, lParam);
				LPBOOL lpBool = 0;
				lpBool = (LPBOOL)(lParam);
				if( lpBool )
					*lpBool = TRUE;
				if( (HWND)lres == psf->GetSafeHwnd( ) )
				{
					DWORD dwStyle = ::GetWindowLong( (HWND)lres, GWL_STYLE );
					if( !(dwStyle & WS_MAXIMIZE) )
						::ShowWindow( (HWND)lres, SW_MAXIMIZE );
				}
				
				return lres;
			} 

		}
		else if( message == WM_MDIACTIVATE )
		{

			HWND hwndActivate = (HWND)wParam; 
			if( hwndActivate )
			{
				POSITION pos = pMainFrame->GetFirstChildFramePos();
				while( pos )
				{
					CShellFrame *psf = pMainFrame->GetNextChildFrame( pos );
					if( hwndActivate == psf->GetSafeHwnd( ) && psf->IsAlwaysZoomed( ) )
					{
						LRESULT lres = CBCGMainClientAreaWnd::WindowProc(message, wParam, lParam);
						DWORD dwStyle = psf->GetStyle( );
						if( !(dwStyle & WS_MAXIMIZE) )
							psf->ShowWindow( SW_MAXIMIZE ); 
						return	lres;
					}
                                                                        						
				}
			}
		}

	}

	return CBCGMainClientAreaWnd::WindowProc(message, wParam, lParam);
}


	

void CMdiClient::OnMouseMove(UINT nFlags, CPoint point) 
{
	if( m_prender )
	{
		static bool	fLastSetByRender = false;
		bool	fSetByRender = false;

		fSetByRender = m_prender->mouse_move( point );
		if( fSetByRender == false && fLastSetByRender == true )
			::SendMessage( AfxGetMainWnd()->GetSafeHwnd(), WM_SETMESSAGESTRING, 0, 0 );
	
		fLastSetByRender = fSetByRender;
	}
	
	CBCGMainClientAreaWnd::OnMouseMove(nFlags, point);
}

void CMdiClient::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if( m_prender )m_prender->mouse_click( point );	
	
	CBCGMainClientAreaWnd::OnLButtonUp(nFlags, point);
}

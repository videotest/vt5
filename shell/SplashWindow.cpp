// SplashWindow.cpp : implementation file
//

#include "stdafx.h"
#include "shell.h"
#include "SplashWindow.h"
#include "mainfrm.h"
#include <math.h>


/////////////////////////////////////////////////////////////////////////////
// CSplashWindow dialog


CSplashWindow::CSplashWindow( CMainFrame *pfrm )
{
	//{{AFX_DATA_INIT(CSplashWindow)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pbi = 0;
	m_pdib = 0;
	m_pframe = pfrm;
}
CSplashWindow::~CSplashWindow()
{
	DeInit();
}


void CSplashWindow::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSplashWindow)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSplashWindow, CDialog)
	//{{AFX_MSG_MAP(CSplashWindow)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSplashWindow message handlers
bool CSplashWindow::LoadImage( const char *pszImage )
{
	DeInit();

	char	*pszExt = ::strrchr( (char*)pszImage, '.' );
	if( !pszExt )return false;

	IFileFilter2Ptr	ptrFileFilter;
	
	if( !stricmp( pszExt, ".jpg" ) )
		ptrFileFilter.CreateInstance( "FileFilters2.JPGFilter" );
	else if( !stricmp( pszExt, ".bmp" ) )
		ptrFileFilter.CreateInstance( "ImageDoc.BMPFileFilter" );

	if( ptrFileFilter == 0 )
		return false;	//unknown file filter or can't create instance

	IFileDataObjectPtr	ptrFile( ptrFileFilter );
	if( ptrFile == 0 )	//wrong filter
		return false;	
	if( ptrFile->LoadFile( _bstr_t( pszImage ) ) != S_OK )
		return false;	//bad file

	int	nObjCount = 0;
	ptrFileFilter->GetCreatedObjectCount( &nObjCount );

	if( nObjCount < 1 )
		return false;	//we didn't read it

	IUnknown	*punkImage = 0;
	ptrFileFilter->GetCreatedObject( 0, &punkImage );

	ASSERT( punkImage );

	IImage2Ptr	ptrImage( punkImage );
	punkImage->Release();

	if( ptrImage == 0 )	//object is not image
		return false;

	IUnknown	*punkCC = 0;
	ptrImage->GetColorConvertor( &punkCC );

	if( !punkCC )	//image has no cc 
		return false;

	IColorConvertorExPtr	ptrCC( punkCC );
	punkCC->Release();

	int	cx, cy;
	ptrImage->GetSize( &cx, &cy );
	int cx4 = (cx*3+3)/4*4;

	//create DIB
	m_pbi = new BITMAPINFOHEADER;
	m_pdib = new byte[cx4*cy];

	if( !m_pbi || !m_pdib )
		return false;	//no memory

	ZeroMemory( m_pbi, sizeof( BITMAPINFOHEADER ) );

	m_pbi->biBitCount = 24;
	m_pbi->biHeight = cy;
	m_pbi->biWidth = cx;
	m_pbi->biPlanes = 1;
	m_pbi->biSize = sizeof(BITMAPINFOHEADER);

	return ptrCC->ConvertImageToDIBRect( m_pbi, m_pdib, CPoint( 0, 0 ), ptrImage, CRect( 0, 0, cx, cy ), 
		CPoint(0,0), 1, CPoint( 0, 0 ), 0, 0 ) == S_OK;
}

void CSplashWindow::DeInit()
{
	if( m_pbi )delete m_pbi;m_pbi = 0;
	if( m_pdib )delete m_pdib;m_pdib = 0;
}

void CSplashWindow::Create()
{
	CDialog::Create( IDD, m_pframe );

	::SetWindowPos( *this, CWnd::wndTop/*CWnd::wndTopMost*/, 0, 0, m_pbi->biWidth, m_pbi->biHeight, SWP_NOMOVE|SWP_NOREDRAW );
	CenterWindow();
	ShowWindow( SW_SHOW );
	UpdateWindow();

	//SetTimer( 777, 2000, 0 );
}

void CSplashWindow::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	::SetDIBitsToDevice( dc, 0, 0, m_pbi->biWidth, m_pbi->biHeight, 
		0, 0, 0, m_pbi->biHeight, m_pdib, (BITMAPINFO*)m_pbi, DIB_RGB_COLORS );
}

BOOL CSplashWindow::PreTranslateMessage(MSG* pMsg) 
{
	if( pMsg->message == WM_KEYDOWN ||
		pMsg->message == WM_CHAR ||
		pMsg->message == WM_SYSKEYDOWN ||
		pMsg->message == WM_LBUTTONDOWN ||
		pMsg->message == WM_RBUTTONDOWN ||
		pMsg->message == WM_MBUTTONDOWN )
	{
		m_pframe->HideSplash();
		return true;
	}

	return false;
}

void CSplashWindow::OnTimer(UINT_PTR nIDEvent) 
{
	if( nIDEvent == 777 )
		m_pframe->HideSplash();
}

void CSplashWindow::OnDestroy() 
{
	KillTimer( 777 );
	CDialog::OnDestroy();
}



////////////////////////////////////////////////////////////////////////////////////////
HANDLE g_tread_logo = 0;

//////////////////////////////////////////////////////////////////////
DWORD WINAPI LogoFunction( LPVOID lpThreadParameter )
{
	CLogoWnd::ShowLogoWnd( 0 );

	MSG msg;
	while( CLogoWnd::m_pLogoWnd->GetSafeHwnd() )
	{
		if( ::PeekMessage( &msg, 0, 0, 0, PM_REMOVE ) )
		{
			::TranslateMessage( &msg );
			::DispatchMessage( &msg );
		}
	}

	return 0;
}

bool CreateLogoWnd()
{
	DWORD dwThreadID = 0;
	g_tread_logo = ::CreateThread( 0, 0, LogoFunction, 0, 0, &dwThreadID );
	
	if( g_tread_logo == 0 )
		return false;

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////
//   Logo class

CLogoWnd* CLogoWnd::m_pLogoWnd = 0;

CLogoWnd::CLogoWnd()
{
	m_hBitmap				= 0;
	
	m_rect_indicator		= CRect( 0, 0, 0, 0 );
	m_size_image			= CSize( 0, 0 );
	
	m_lindicator_type		= 0;
	m_benable_indicator		= false;
	
	m_lcur_stage			= 0;
	m_lmax_stage			= 0;

	m_lindicator_len		= 20;
	m_lindicator_tail_len	= 40;

	m_lstep					= 3;

	m_clr_back				= RGB( 0, 0, 0 );
	m_clr_high				= RGB( 255, 255, 255 );

	m_lstart_tick			= 0;
	m_lcan_destroy_by_click	= 0;

	m_llock					= 0;
}

CLogoWnd::~CLogoWnd()
{
	// Clear the static window pointer.
	ASSERT(m_pLogoWnd == this);
	m_pLogoWnd = NULL;

	if( m_hBitmap )
	{
		m_bitmap.Detach();
		::DeleteObject( m_hBitmap );
		m_hBitmap = 0;
	}
	
}

BEGIN_MESSAGE_MAP(CLogoWnd, CWnd)
	//{{AFX_MSG_MAP(CLogoWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CLogoWnd::ShowLogoWnd(CWnd* pParentWnd )
{
	if ( m_pLogoWnd != NULL)
		return;

	// Allocate a new splash screen, and create the window.
	m_pLogoWnd = new CLogoWnd;
	if (!m_pLogoWnd->Create(pParentWnd))
	{
		delete m_pLogoWnd;
	}
	else
		m_pLogoWnd->UpdateWindow();
}

void CLogoWnd::Lock( long llock )
{
	if ( !m_pLogoWnd )
		return;

	::InterlockedExchange( &m_pLogoWnd->m_llock, llock );
}

void CLogoWnd::CanDestroyByClick( long ldestroy )
{
	if ( !m_pLogoWnd )
		return;

	::InterlockedExchange( &m_pLogoWnd->m_lcan_destroy_by_click, ldestroy );
}


#define LOGO_SECTION	"Logo"
bool CLogoWnd::LoadSettings()
{
	//get shell data path
	char szCurPath[255];
	GetCurrentDirectory( 254, szCurPath );

	CString str_ini = CString( szCurPath ) + "\\shell.data";

	//is logo enable
	char sz_buf[1024];	sz_buf[0] = 0;
	::GetPrivateProfileString( LOGO_SECTION, "EnableLogo:Long", "0", sz_buf, sizeof(sz_buf), str_ini );
	if( strcmp( sz_buf, "1" ) )
		return false;

	//get image path
	::GetPrivateProfileString( LOGO_SECTION, "FileName:String", "", sz_buf, sizeof(sz_buf), str_ini );

	if( strchr( sz_buf, ':' ) )
		m_str_image_path = sz_buf;
	else
		m_str_image_path = CString( szCurPath ) + CString( "\\" ) + CString( sz_buf );

	//is indicator enable
	::GetPrivateProfileString( LOGO_SECTION, "EnableIndicator:Long", "", sz_buf, sizeof(sz_buf), str_ini );
	m_benable_indicator = !strcmp( sz_buf, "1" );

	//indicator type
	::GetPrivateProfileString( LOGO_SECTION, "IndicatorType:Long", "", sz_buf, sizeof(sz_buf), str_ini );
	m_lindicator_type = atoi( sz_buf );

	//indicator dimension
	::GetPrivateProfileString( LOGO_SECTION, "IndicatorX:Long", "", sz_buf, sizeof(sz_buf), str_ini );
	m_rect_indicator.left = atoi( sz_buf );

	::GetPrivateProfileString( LOGO_SECTION, "IndicatorY:Long", "", sz_buf, sizeof(sz_buf), str_ini );
	m_rect_indicator.top = atoi( sz_buf );

	::GetPrivateProfileString( LOGO_SECTION, "IndicatorWidth:Long", "", sz_buf, sizeof(sz_buf), str_ini );
	m_rect_indicator.right = m_rect_indicator.left + atoi( sz_buf );

	::GetPrivateProfileString( LOGO_SECTION, "IndicatorHeight:Long", "", sz_buf, sizeof(sz_buf), str_ini );
	m_rect_indicator.bottom = m_rect_indicator.top + atoi( sz_buf );

	m_lmax_stage = m_rect_indicator.Width();

	//indicator len
	::GetPrivateProfileString( LOGO_SECTION, "IndicatorLen:Long", "", sz_buf, sizeof(sz_buf), str_ini );
	m_lindicator_len = atoi( sz_buf );
	if( m_lindicator_len < 1 )
		m_lindicator_len = 20;

	//indicator tail len
	::GetPrivateProfileString( LOGO_SECTION, "IndicatorTailLen:Long", "", sz_buf, sizeof(sz_buf), str_ini );
	m_lindicator_tail_len = atoi( sz_buf );
	if( m_lindicator_tail_len < 1 )
		m_lindicator_tail_len = 40;

	//indicator tail len
	::GetPrivateProfileString( LOGO_SECTION, "IndicatorStep:Long", "", sz_buf, sizeof(sz_buf), str_ini );
	m_lstep = atoi( sz_buf );
	if( m_lstep < 1 )
		m_lstep = 5;

	

	//parse back color
	::GetPrivateProfileString( LOGO_SECTION, "BackColor:String", "", sz_buf, sizeof(sz_buf), str_ini );
	BYTE r, g, b;
	r = g = b = 0;
	char* psz_token = strtok( sz_buf, ", ()" );
	if( psz_token )
	{
		r = atoi( psz_token );
		psz_token = strtok( 0, ", ()" );
		if( psz_token )
		{
			g = atoi( psz_token );
			psz_token = strtok( 0, ", ()" );
			if( psz_token )
			{
				b = atoi( psz_token );
				m_clr_back = RGB( r, g, b );
			}
		}
	}

	//parse high color
	::GetPrivateProfileString( LOGO_SECTION, "HighColor:String", "", sz_buf, sizeof(sz_buf), str_ini );
	r = g = b = 0;
	psz_token = strtok( sz_buf, ", ()" );
	if( psz_token )
	{
		r = atoi( psz_token );
		psz_token = strtok( 0, ", ()" );
		if( psz_token )
		{
			g = atoi( psz_token );
			psz_token = strtok( 0, ", ()" );
			if( psz_token )
			{
				b = atoi( psz_token );
				m_clr_high = RGB( r, g, b );
			}
		}
	}


	return true;
}


bool CLogoWnd::LoadBitmap()
{
	m_hBitmap = (HBITMAP)::LoadImage(	0, (LPCSTR)m_str_image_path, IMAGE_BITMAP, 0, 0, 
										LR_DEFAULTSIZE|LR_LOADFROMFILE );

	if( !m_hBitmap )
		return false;
	
	m_bitmap.Attach( m_hBitmap );

	BITMAP bm;
	::ZeroMemory( &bm, sizeof(BITMAP) );
	m_bitmap.GetBitmap(&bm);

	m_size_image.cx = bm.bmWidth;
	m_size_image.cy = bm.bmHeight;

	return true;
}

BOOL CLogoWnd::Create(CWnd* pParentWnd /*= NULL*/)
{	
	if( !LoadSettings() )
		return false;

	if( !LoadBitmap() )
		return false;

	return CreateEx( 0/*WS_EX_TOPMOST*/,
		AfxRegisterWndClass(0, AfxGetApp()->LoadStandardCursor(IDC_ARROW)),
		NULL, WS_POPUP | WS_VISIBLE, 0, 0, m_size_image.cx, m_size_image.cy, pParentWnd->GetSafeHwnd(), NULL);
}

void CLogoWnd::HideLogoWnd()
{
	// Destroy the window, and update the mainframe.
	DestroyWindow();
}

void CLogoWnd::OnDestroy() 
{
	KillTimer( 1 );
	CWnd::OnDestroy();
}

void CLogoWnd::PostNcDestroy()
{
	if( g_tread_logo )
	{			
		::CloseHandle( g_tread_logo );
		g_tread_logo = 0;
	}
	
	delete this;
}

void CLogoWnd::DelayDestroy()
{
	if( m_pLogoWnd )
	{
		m_pLogoWnd->Lock( 0 );
		//::DestroyWindow( m_pLogoWnd->GetSafeHwnd() );
		m_pLogoWnd->StartDestroy();
	}
}

void CLogoWnd::StartDestroy()
{
	if( m_pLogoWnd )
	{
		long ltick = GetTickCount();
		::InterlockedExchange( &m_pLogoWnd->m_lstart_tick, ltick );
		::SetThreadPriority( g_tread_logo, THREAD_PRIORITY_TIME_CRITICAL );

	}
}

int CLogoWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Center the window.
	CenterWindow();

	// Set a timer to destroy the splash screen.
	SetTimer( 1, 50, NULL );

	// A.M. fix, BT 3732
//	SetFocus();

	return 0;
}

void CLogoWnd::OnPaint()
{
	CRect rcPaint;
	GetUpdateRect( &rcPaint );

	CPaintDC	dcPaint( this );
	CDC			dcMemory;

	dcMemory.CreateCompatibleDC( &dcPaint );

	int nImageWidth		= rcPaint.Width( );
	int nImageHeight	= rcPaint.Height( );

	if( nImageWidth < 1 || nImageHeight < 1 )
		return;	

	BITMAPINFOHEADER	bmih;
	ZeroMemory( &bmih, sizeof( bmih ) );

	bmih.biBitCount = 24;	
	bmih.biWidth	= nImageWidth;
	bmih.biHeight	= nImageHeight;
	bmih.biSize		= sizeof( bmih );
	bmih.biPlanes	= 1;

	byte	*pdibBits = 0;
	HBITMAP	hDIBSection = ::CreateDIBSection( dcPaint, (BITMAPINFO*)&bmih, DIB_PAL_COLORS, (void**)&pdibBits, 0, 0 );
	CBitmap *psrcBitmap = CBitmap::FromHandle( hDIBSection );

	CBitmap *poldBmp = dcMemory.SelectObject( psrcBitmap );


	dcMemory.SetMapMode( MM_TEXT );
	dcMemory.SetViewportOrg( -rcPaint.left, -rcPaint.top );

	ProcessDrawing( &dcMemory, rcPaint, &bmih, pdibBits );

	dcMemory.SetViewportOrg( 0, 0 );

	//paint to paint DC
	::SetDIBitsToDevice( dcPaint, rcPaint.left, rcPaint.top, nImageWidth, nImageHeight,
		0, 0, 0, nImageHeight, pdibBits, (BITMAPINFO*)&bmih, DIB_RGB_COLORS );

	//prepare to delete objects
	dcMemory.SelectObject( poldBmp );

	if( psrcBitmap )
		psrcBitmap->DeleteObject();


	dcMemory.SelectObject( poldBmp );
}

bool calc_func( double x1, double x2, double y1, double y2, double& a, double& b )
{	
	if( ( x1 - x2 ) == 0.0 )	return false;
	if( x1 == 0.0 ) x1 = 1;

	b = ( y2*x1 - y1*x2 ) / ( x1 - x2 );
	a = ( y1 - b ) / x1;
	
	return true;
}

void CLogoWnd::ProcessDrawing( CDC* pdc, CRect rcUpdate, BITMAPINFOHEADER* pbi, byte *pdib )
{
	//draw image to dc first
	{
		CDC dcImage;
		if( !dcImage.CreateCompatibleDC( pdc ) )
			return;
		// Paint the image.
		CBitmap* pOldBitmap = dcImage.SelectObject(&m_bitmap);
		pdc->BitBlt(	rcUpdate.left, rcUpdate.top, rcUpdate.Width(), rcUpdate.Height(), 
						&dcImage, rcUpdate.left, rcUpdate.top, SRCCOPY);

		dcImage.SelectObject( pOldBitmap );
	}

	if( !m_benable_indicator || m_lindicator_len < 1 || m_lindicator_tail_len < 1 || m_lstart_tick )
		return;

	//now draw indicator
	{

		int nleft	= m_lcur_stage - m_lindicator_len / 2;
		int nright	= m_lcur_stage + m_lindicator_len / 2;
		int ntail_len = m_lindicator_tail_len;

		int r_back = GetRValue( m_clr_back );
		int g_back = GetGValue( m_clr_back );
		int b_back = GetBValue( m_clr_back );

		int r_high = GetRValue( m_clr_high );
		int g_high = GetGValue( m_clr_high );
		int b_high = GetBValue( m_clr_high );

		CRect rc;
		if( rc.IntersectRect( &m_rect_indicator, &rcUpdate ) )
		{
			CRect rcFill = rc;
			rcFill -= rcUpdate.TopLeft();

			int cx	= pbi->biWidth;
			int cy	= pbi->biHeight;
			int	cx4	= (cx*3+3)/4*4;

			for( int _y=rcFill.top;_y<rcFill.bottom;_y++ )
			{
				int y = cy - _y - 1;
				long nRowOffset = ( y ) * cx4;
				for( int x=rcFill.left;x<rcFill.right;x++ )
				{
					int r_img = 0;
					int g_img = 0;
					int b_img = 0;

					if( x < nleft )
					{
						if( x > nleft - ntail_len )
						{
							double a, b;
							if( calc_func( nleft - ntail_len, nleft, r_back, r_high, a, b ) )
								r_img = int( a * (double)x + b );
							
							if( calc_func( nleft - ntail_len, nleft, g_back, g_high, a, b ) )
							g_img = int( a * (double)x + b );
							
							if( calc_func( nleft - ntail_len, nleft, b_back, b_high, a, b ) )
								b_img = int( a * (double)x + b );

						}
						else
						{
							r_img = r_back;
							g_img = g_back;
							b_img = b_back;
						}
					}
					else if( x > nright )
					{
						if( x < nright + ntail_len )
						{
							double a, b;
							if( calc_func( nright, nright + ntail_len, r_high, r_back, a, b ) )
								r_img = int( a * (double)x + b );
							
							if( calc_func( nright, nright + ntail_len, g_high, g_back, a, b ) )
								g_img = int( a * (double)x + b );
							
							if( calc_func( nright, nright + ntail_len, b_high, b_back, a, b ) )
								b_img = int( a * (double)x + b );
						}
						else
						{
							r_img = r_back;
							g_img = g_back;
							b_img = b_back;
						}
					}
					else
					{
						r_img = r_high;
						g_img = g_high;
						b_img = b_high;
					}

					if( r_img < 0 )		r_img = 0;
					if( r_img > 255 )	r_img = 255;

					if( g_img < 0 )		g_img = 0;
					if( g_img > 255 )	g_img = 255;

					if( b_img < 0 )		b_img = 0;
					if( b_img > 255 )	b_img = 255;

					pdib[ nRowOffset + 3 * x + 0 ] = (BYTE)b_img;
					pdib[ nRowOffset + 3 * x + 1 ] = (BYTE)g_img;
					pdib[ nRowOffset + 3 * x + 2 ] = (BYTE)r_img;
				}
			}			
		}
	}
}

void CLogoWnd::OnTimer(UINT_PTR nIDEvent)
{
	if( nIDEvent == 1 && m_benable_indicator )
	{
		if( m_lstart_tick != 0 /*&& ( GetTickCount() - m_lstart_tick ) > 1000 */ )
		{
			HideLogoWnd();
			return;
		}
		m_lcur_stage += m_lstep;

		if( m_lcur_stage >= m_lmax_stage )
			m_lcur_stage = 0;

		InvalidateRect( &m_rect_indicator );
	}
}

BOOL CLogoWnd::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

LRESULT CLogoWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	
	if( ( message == WM_KEYDOWN ||
	    message == WM_SYSKEYDOWN ||
	    message == WM_LBUTTONDOWN ||
	    message == WM_RBUTTONDOWN ||
	    message == WM_MBUTTONDOWN ||
	    message == WM_NCLBUTTONDOWN ||
	    message == WM_NCRBUTTONDOWN ||
	    message == WM_NCMBUTTONDOWN ) && m_pLogoWnd->m_lcan_destroy_by_click )
	{
		m_pLogoWnd->HideLogoWnd();
		return TRUE;
	}
	else if( message == WM_KILLFOCUS && (HWND)wParam != m_pLogoWnd->GetSafeHwnd() && !m_llock )
	{
		m_pLogoWnd->HideLogoWnd();
		return TRUE;	// message handled here
	}
	return CWnd::WindowProc(message, wParam, lParam);
}


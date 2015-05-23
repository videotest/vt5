// InputThread.cpp : implementation file
//

#include "stdafx.h"
//#include "fgdrv.h"
#include "InputThread.h"
#include "GrabImg.h"
#include "resource.h"

#include "vfw.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CRectTracker *crtFrame;
extern int iShowFrame;
extern int iShowFrame;
class CDrawDib
{
public:
	HDRAWDIB	m_hdd;

	CDrawDib()
	{m_hdd = ::DrawDibOpen();}
	~CDrawDib()
	{::DrawDibClose( m_hdd );}

	operator HDRAWDIB()
	{return m_hdd;}
};


CDrawDib	DrawDib;

extern void GrabImage( BITMAPINFOHEADER	*pbi );
/////////////////////////////////////////////////////////////////////////////
// CInputThread

IMPLEMENT_DYNAMIC(CInputThread, CWinThread)

CInputThread::CInputThread(
		BITMAPINFOHEADER	*pbi,
		HANDLE				hwnd,
		HANDLE				hEventDone,
		HANDLE				hEventComplete	)
{
	m_pbi = pbi;
	m_hwnd = hwnd;
	m_hEventDone = hEventDone;
	m_hEventComplete = hEventComplete;
}
		
	                
CInputThread::~CInputThread()
{
}

BOOL CInputThread::InitInstance()
{
	m_hEventStopper = ::CreateEvent( 0, true, true, 0 );
	return TRUE;
}

int CInputThread::ExitInstance()
{
	::CloseHandle( m_hEventStopper );

	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CInputThread, CWinThread)
	//{{AFX_MSG_MAP(CInputThread)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInputThread message handlers

int CInputThread::Run() 
{
	ASSERT( m_hwnd );
	ASSERT( m_pbi );

	while( 1 )
	{
		::GrabImage( m_pbi, m_hEventStopper, 1/*FAST*/ );	   // FAST SLOW

		::PostMessage( (HWND)m_hwnd, WM_USER+102, 0, 0 );

		PaintImage();

		if( WaitForSingleObject( m_hEventStopper, 0 ) == WAIT_TIMEOUT )
			::SetEvent( m_hEventStopper );

		if( ::WaitForSingleObject( m_hEventDone, 0 ) == WAIT_OBJECT_0 )
			break;
	}
	SetEvent( m_hEventComplete );

	return true;
}

CPoint	ptScroll=CPoint( 0, 0 );
CSize	sizeImage = CSize( 0, 0 );

void	 CInputThread::PaintImage( HDC hDCSample )
{
	HDC	hDC;
	
	if( hDCSample != 0 )
		hDC = hDCSample;
	else	
		hDC = ::GetDC( (HWND)m_hwnd );

	RECT	rc;
	   
	::GetWindowRect( ::GetDlgItem( (HWND)m_hwnd, IDC_IMAGE ), &rc );
	::ScreenToClient( (HWND)m_hwnd, (POINT*)&rc );
	::ScreenToClient( (HWND)m_hwnd, (POINT*)&rc+1 );

	//::InflateRect( &rc, -1, -1 );

	CRect	rcDib;

	
	rcDib.left = 0;
	rcDib.top = 0;
	rcDib.right = m_pbi->biWidth;
	rcDib.bottom = m_pbi->biHeight;

	sizeImage.cx = m_pbi->biWidth;
	sizeImage.cy = m_pbi->biHeight;


	DrawDibDraw( DrawDib, hDC, 
		rc.left,
		rc.top,
		rc.right-rc.left,
		rc.bottom-rc.top,
		m_pbi,
		LPSTR(m_pbi+1)+1024,
		0,//rcDib.left,
		0,//rcDib.top,
		rc.right-rc.left,//rcDib.Width(),
		rc.bottom-rc.top,//rcDib.Height(),
		0 );
/*

	::StretchDIBits( 
		hDC,
		rc.left,
		rc.top,
		rc.right-rc.left,
		rc.bottom-rc.top,
		rcDib.left,
		rcDib.top,
		rcDib.Width(),
		rcDib.Height(),
		LPSTR(m_pbi+1)+1024,
		(BITMAPINFO*)m_pbi,
		DIB_RGB_COLORS,
		SRCCOPY );

	DrawDibClose( hdd )*/
	
	if (hDC && iShowFrame)
	{
		CDC *dc = CDC::FromHandle(hDC);
		crtFrame->Draw(dc );	
	}
	
	if( hDCSample == 0 )
		::ReleaseDC( (HWND)m_hwnd, hDC );
}
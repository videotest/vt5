#include "stdafx.h"
#include "resource.h"
#include "wnd_misc.h"
#include "statusint.h"
#include "statusutils.h"
#include "docview5.h"


CObjectCountPage* g_pObjectCountPage = 0;

std_dll bool statusShowObjectCountPage( bool bShow )
{
	INewStatusBarPtr	ptrStatus( ::StatusGetBar(), false );
	if( ptrStatus == 0 )return false;

	HWND	hwnd = ::GetWindow( ptrStatus );

	ptrStatus->RemovePane( guidObjectCount );

	if( g_pObjectCountPage )
	{
		g_pObjectCountPage->DestroyWindow();
		delete g_pObjectCountPage;
		g_pObjectCountPage = 0;
	}

	if( bShow )
	{
		g_pObjectCountPage = new CObjectCountPage; 
		g_pObjectCountPage->Create( NULL, _T(""), WS_CHILD|WS_VISIBLE, NORECT, CWnd::FromHandle(hwnd), 9552);
		
		AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
		CString strTest;
		strTest.Format( IDS_OBJECT_COUNT_PANE, 1000 );
		int nWidth = ::StatusCalcWidth( strTest, true );

		ptrStatus->AddPane( guidObjectCount, nWidth, g_pObjectCountPage->GetSafeHwnd(), 0 );
	}

	return true;
}

////////////////////////////////////////////////////////////////
//
//	CObjectCountPane class 
//
////////////////////////////////////////////////////////////////
CObjectCountPage::CObjectCountPage()
{
	m_nTimerID		= -1;
	m_nLastCount	= 0;
	m_hIcon			= 0;
}

////////////////////////////////////////////////////////////////
CObjectCountPage::~CObjectCountPage()
{
	if( m_hIcon	)
		::DestroyIcon( m_hIcon );	m_hIcon = 0;
}


BEGIN_MESSAGE_MAP(CObjectCountPage, CWnd)
	//{{AFX_MSG_MAP(CObjectCountPage)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////
void CObjectCountPage::OnPaint() 
{
	CPaintDC	dc( this );
	CRect		rectClient;

	GetClientRect( &rectClient );

	HBRUSH	hbrush = (HBRUSH)GetParent()->SendMessage( WM_CTLCOLORSTATIC, (WPARAM)(HDC)dc, (LPARAM)GetSafeHwnd() );
	if( !hbrush )hbrush = ::GetSysColorBrush( COLOR_3DFACE );
	::FillRect( dc, rectClient, hbrush );

	rectClient.InflateRect( -1, -1 );

	::DrawIconEx( dc, rectClient.left, rectClient.top, m_hIcon, 16, 16, 0, 0, DI_NORMAL );

	CRect rectText = rectClient;
	rectText.left += 20;

	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	CString strText;
	strText.Format( IDS_OBJECT_COUNT_PANE, m_nLastCount );

	dc.SetBkMode( TRANSPARENT );

	dc.SelectObject( GetParent()->GetFont() );
	dc.DrawText( strText, rectText, DT_LEFT|DT_VCENTER|DT_SINGLELINE );
}


////////////////////////////////////////////////////////////////
int CObjectCountPage::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_nTimerID = 1;
	m_nTimerID = SetTimer( m_nTimerID, 500, NULL );

	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	m_hIcon = AfxGetApp()->LoadIcon( IDI_COUNT );

	return 0;
}

////////////////////////////////////////////////////////////////
void CObjectCountPage::OnDestroy() 
{
	CWnd::OnDestroy();

	if( m_nTimerID != -1)
		KillTimer( m_nTimerID );
}

////////////////////////////////////////////////////////////////
void CObjectCountPage::OnTimer(UINT_PTR nIDEvent) 
{	
	CWnd::OnTimer(nIDEvent);
	if( nIDEvent == m_nTimerID )
	{
		IApplicationPtr ptrApp( GetAppUnknown() );
		if( ptrApp == 0 )	return;

		IUnknown* punk = 0;
		ptrApp->GetActiveDocument( &punk );
		if( !punk )			return;

		IDocumentSitePtr ptrDS( punk );
		punk->Release();	punk = 0;
		if( ptrDS == 0 )	return;

		ptrDS->GetActiveView( &punk );
		if( !punk )	return;

		IUnknownPtr ptrView = punk;
		punk->Release();	punk = 0;

		punk = ::GetActiveObjectFromContext( ptrView, szTypeObjectList );
		if( !punk )			return;

		INamedDataObject2Ptr ptrNDO( punk );
		punk->Release();	punk = 0;
		if( ptrNDO == 0 )	return;

		long lcount = 0;
		ptrNDO->GetChildsCount( &lcount );				

		if( lcount != m_nLastCount )
		{
			m_nLastCount = lcount;
			Invalidate(); 
		}
	}
}

////////////////////////////////////////////////////////////////
BOOL CObjectCountPage::OnEraseBkgnd(CDC* pDC) 
{	
	return TRUE;
}

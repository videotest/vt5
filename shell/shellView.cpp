// shellView.cpp : implementation of the CShellView class
//

#include "stdafx.h"
#include "shell.h"

#include "shellDoc.h"
#include "shellView.h"
#include "shellFrame.h"


#include "targetman.h"
#include "docs.h"
#include "mainfrm.h"
#include "misc.h"
#include "statusutils.h"
#include "wndmisc5.h"
#include "\vt5\awin\profiler.h"


const	int	cyCaption = 12;
bool	g_bShowTitle = true;

GUID CShellView::m_guid_active_view = GUID_NULL;
/////////////////////////////////////////////////////////////////////////////
// CShellView

IMPLEMENT_DYNCREATE(CShellView, CView)

BEGIN_MESSAGE_MAP(CShellView, CView)
	//{{AFX_MSG_MAP(CShellView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_WM_DESTROY()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEACTIVATE()
	ON_WM_SHOWWINDOW()
	ON_WM_MOUSEWHEEL()
	ON_WM_SETCURSOR()	
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/*
	ON_WM_MOUSEMOVE()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_KEYDOWN()
	*/

BEGIN_DISPATCH_MAP(CShellView, CView)
	//{{AFX_DISPATCH_MAP(CShellView)
	DISP_PROPERTY_EX(CShellView, "Zoom", GetZoom, SetZoom, VT_R8)
	DISP_FUNCTION(CShellView, "GetViewDispatch", GetViewDispatch, VT_DISPATCH, VTS_NONE)
	DISP_FUNCTION(CShellView, "GetDataContextDispatch", GetDataContextDispatch, VT_DISPATCH, VTS_NONE)
	DISP_FUNCTION(CShellView, "GetViewName", GetViewName, VT_BSTR, VTS_NONE)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IShellViewDisp to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {28888205-51B2-47f2-9359-6EEA31DF0E47}
static const GUID IID_IShellViewDisp = 
{ 0x28888205, 0x51b2, 0x47f2, { 0x93, 0x59, 0x6e, 0xea, 0x31, 0xdf, 0xe, 0x47 } };

BEGIN_INTERFACE_MAP(CShellView, CView)
	INTERFACE_PART(CShellView, IID_IShellViewDisp, Dispatch)
	INTERFACE_PART(CShellView, IID_IViewSite, Site)
	INTERFACE_PART(CShellView, IID_INumeredObject, Num)
	INTERFACE_PART(CShellView, IID_IScrollZoomSite, ScrollSite)
	INTERFACE_PART(CShellView, IID_IScrollZoomSite2, ScrollSite)
	INTERFACE_AGGREGATE(CShellView, m_aggrs.m_pinterfaces[0])
	INTERFACE_AGGREGATE(CShellView, m_aggrs.m_pinterfaces[1])
	INTERFACE_AGGREGATE(CShellView, m_aggrs.m_pinterfaces[2])
	INTERFACE_AGGREGATE(CShellView, m_aggrs.m_pinterfaces[3])
	INTERFACE_AGGREGATE(CShellView, m_aggrs.m_pinterfaces[4])
	INTERFACE_AGGREGATE(CShellView, m_aggrs.m_pinterfaces[5])
	INTERFACE_AGGREGATE(CShellView, m_aggrs.m_pinterfaces[6])
	INTERFACE_AGGREGATE(CShellView, m_aggrs.m_pinterfaces[7])
	INTERFACE_AGGREGATE(CShellView, m_aggrs.m_pinterfaces[8])
	INTERFACE_AGGREGATE(CShellView, m_aggrs.m_pinterfaces[9])
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CShellView, Site);

HRESULT CShellView::XSite::GetFrameWindow( IUnknown **ppunkFrame )
{
	_try_nested(CShellView, Site, GetFrameWindow)
	{
		*ppunkFrame = pThis->m_pFrame->GetControllingUnknown();
		(*ppunkFrame)->AddRef();

		return S_OK;
	}
	_catch_nested;
}

HRESULT CShellView::XSite::InitView( IUnknown *punkView )
{
	_try_nested(CShellView, Site, InitView)
	{
		pThis->m_bEnableKeyScroll = (::GetValueInt(GetAppUnknown(), "\\IntelliMouse", "Enable key scroll", 1L)==1);
		return S_OK;
	}
	_catch_nested;
}

HRESULT CShellView::XSite::UpdateObjectInfo()
{
	METHOD_PROLOGUE_EX(CShellView, Site)
	if( pThis->GetSafeHwnd() && g_bShowTitle )
	{
		pThis->m_strViewObjects.Empty();

		TPOS	lpos = 0;
		IViewPtr	ptrV( pThis->GetControllingUnknown() );
		ptrV->GetFirstVisibleObjectPosition( &lpos );

		while( lpos )
		{
			IUnknownPtr	sptrO;
			ptrV->GetNextVisibleObject( &sptrO, &lpos );
			CString	strN = ::GetObjectName( sptrO );
			if( !pThis->m_strViewObjects.IsEmpty() )pThis->m_strViewObjects+=", ";
			pThis->m_strViewObjects+=strN;
		}


		CRect	rect;
		pThis->GetClientRect( &rect );
		rect.bottom = rect.top+cyCaption;
		pThis->InvalidateRect( rect );
	}
	return S_OK;
}

////////////////////////////////////////////////////////////////////////////
// CShellView construction/destruction

CShellView::CShellView( BSTR bstrViewProgID )
{
//	m_benable_map	= false;
	_OleLockApp( this );

	InitIntelliSettings();

	g_bShowTitle = GetValueInt( GetAppUnknown(), "\\Layout", "ShowObjectNames", 1 )!=0;

	m_sizeClient = CSize( 0, 0 );
	m_pFrame = 0;
	m_bDestroyCalled = false;

	EnableAggregation();

	EnableAutomation();

	m_aggrs.AttachComponentGroup( szPluginViewAggr );
	m_aggrs.SetOuterUnknown( GetControllingUnknown() );
	m_aggrs.Init();

	if( bstrViewProgID )
	{
		IUnknown	*punkView = 0;
		CLSID	clsid;

		::CLSIDFromProgID( bstrViewProgID, &clsid  );
		HRESULT hresult = ::CoCreateInstance( clsid, GetControllingUnknown(), CLSCTX_INPROC_SERVER, IID_IUnknown, (void **)&punkView );

		ASSERT(SUCCEEDED(hresult)); //Are you call EnableAggregation() in your view class constructor? ;-)

		if( !punkView )
		{
			::CLSIDFromProgID( _bstr_t( szSourceViewProgID ), &clsid  );
			hresult = ::CoCreateInstance( clsid, GetControllingUnknown(), CLSCTX_INPROC_SERVER, IID_IUnknown, (void **)&punkView );
		}
		
		//TODO:add error code checking here
				
		//sptrV.CreateInstance( bstrViewProgID, GetControllingUnknown() );

		if( punkView )
		{
			m_aggrs.AddComponent( punkView );
			punkView->Release();
		}
	}

}

CShellView::~CShellView()
{
	//TRACE( "View removed succedded\n" );

	m_benable_map	= false;

	DestroyIntelliSettings();

	if(  m_pFrame)
		m_pFrame->RemoveView( this );

	if( m_pDocument )
		GetDocument()->EnsureViewRemoved( this );
	_OleUnlockApp( this );
}


BOOL CShellView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	m_strClassName = ::AfxRegisterWndClass( CS_DBLCLKS|CS_OWNDC, ::LoadCursor(NULL, IDC_ARROW), (HBRUSH)(COLOR_WINDOW+1) );
	cs.lpszClass = m_strClassName;

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CShellView drawing

void CShellView::OnDraw(CDC* pDC)
{
	CShellDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if( g_bShowTitle )
	{
		CRect	rcClient;
		GetClientRect( rcClient );	

		bool	bActive = GetDocument()->GetActiveView() == this ;
		rcClient.bottom = rcClient.top+cyCaption;

		CBrush	brush( ::GetSysColor( bActive?COLOR_ACTIVECAPTION:COLOR_INACTIVECAPTION ));

		pDC->SetTextColor( ::GetSysColor( bActive?COLOR_CAPTIONTEXT:COLOR_INACTIVECAPTIONTEXT ) );

		pDC->FillRect( &rcClient, &brush );

		CFont	font;
		font.CreateFont( cyCaption-2, cyCaption-4, 0, 0, bActive?FW_BOLD:FW_BOLD, 0, 0, 0, 1, 0, 0, 0, 0, "Arial" );
		CFont	*poldFont = 
		pDC->SelectObject( &font );
		pDC->SetBkMode( TRANSPARENT );


		pDC->DrawText( m_strViewObjects, rcClient, DT_VCENTER|DT_SINGLELINE );

		pDC->SelectObject( poldFont );
		
	}
	
	/*CRect	rcClient;
	
	pDC->FillRect( rcClient, &CBrush( RGB( 255, 0, 0 ) ) );*/

}

/////////////////////////////////////////////////////////////////////////////
// CShellView printing

BOOL CShellView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CShellView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CShellView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CShellView diagnostics

#ifdef _DEBUG
void CShellView::AssertValid() const
{
	CView::AssertValid();
}

void CShellView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CShellDoc* CShellView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CShellDoc)));
	return (CShellDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CShellView message handlers

int CShellView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	m_pDocument = 0;	//reset early assigned document
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED);

	{
	sptrIWindow	sptrW( GetControllingUnknown() );
	sptrW->CreateWnd( 	GetSafeHwnd(), CRect( 0, 0, 100, 100 ), WS_CHILD|WS_VISIBLE, 1001 );
	}

	g_TargetManager.RegisterTarget( GetControllingUnknown(), szTargetViewSite );

	OnInitScrollSite();

	SetFocus();
	OnSetFocus( 0 );

	::FireEvent( GetAppUnknown(), szAppCreateView, GetControllingUnknown() );

	if( IsWindowVisible() )AddStatusBarPanes( true );

	return 0;
}

void CShellView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	CRect	rcClient;
	GetClientRect( rcClient );

	if( g_bShowTitle )
		rcClient.top += cyCaption;

	sptrIWindow	sptrW( GetControllingUnknown() );
	CWnd	*ptemp = ::GetWindowFromUnknown( sptrW );

	if( ptemp )
		ptemp->MoveWindow( rcClient, FALSE );

	DWORD	dwRedrawFlags = /*RDW_ERASE|*/RDW_INVALIDATE|RDW_ALLCHILDREN/*|RDW_UPDATENOW*/;
	if( m_sizeClient.cx < cx )
		RedrawWindow( CRect( m_sizeClient.cx, 0, cx, cy), 0, dwRedrawFlags );
	if( m_sizeClient.cy < cy )
		RedrawWindow( CRect( 0, m_sizeClient.cy, cx, cy ), 0, dwRedrawFlags );
	
	m_sizeClient = CSize( cx, cy );

	CScrollZoomSiteImpl::OnSize();
}

void CShellView::OnSetFocus(CWnd* pOldWnd) 
{
	if( m_bDestroyCalled )
		return;
	CView::OnSetFocus(pOldWnd);

	if( m_pFrame )
		if( m_pFrame->GetActiveView() != this )
		m_pFrame->SetActiveView( this, true );

		
	sptrIWindow	sptrW( GetControllingUnknown() );
	CWnd	*ptemp = ::GetWindowFromUnknown( sptrW );

	if( ptemp )
		ptemp->SetFocus();

	
}


void CShellView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	/*
	{
		long l = ::GetTickCount();
		char sz_buf[100];
		sprintf( sz_buf, "%d", l );
		CShellFrame* pframe = GetFrame();
		CString str_text;
		pframe->GetWindowText( str_text );
		TRACE( "\n");
		TRACE( sz_buf );
		TRACE( " " );
		if( bActivate )
			TRACE( "-------------Activate view '" );
		else
			TRACE( "-------------Deactivate view '" );
		TRACE( str_text );
		TRACE( "'=========================================" );		
	}
	*/

	if( g_bShowTitle )
	{
		CRect	rect;
		GetClientRect( &rect );
		rect.bottom = rect.top+cyCaption;
		InvalidateRect( rect );
	}

	CShellDoc *pshellDoc = GetDocument();

	CWnd* pChildWnd = NULL;	 
	CShellFrame* pChildFrame = NULL;


	pChildWnd = GetParent()->GetParent();


	pChildFrame = (CShellFrame*)pChildWnd;

	ASSERT_KINDOF(CShellFrame, pChildFrame);		


	/*
	if (bActivate)
	{
		if( pActivateView == pDeactiveView )
		{
			TRACE( "The activate view same as deactivate\n" );
		}
	}		
	*/

	
	if( pChildFrame && !(
		(pChildFrame->GetFrameType() == ftDDI) &&
		(pChildFrame->IsFloatingDDIFrame() )
		) )
	{	
		CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
	}
	

	if( bActivate )
	{	
		//TRACE( "\nActivate VIEW-------------------------------" );
			//activate document
		g_TargetManager.ActivateTarget( pshellDoc->GetControllingUnknown() );
		
			//..and view
		g_TargetManager.ActivateTarget( GetControllingUnknown() );

		
		CShellDocTemplate *pTempl = static_cast<CShellDocTemplate*>( pshellDoc->GetDocTemplate() );

		if( pTempl )
			pTempl->SetActiveDocument( pshellDoc, false );

		pshellDoc->SetActiveView( this );

		::FireEvent( pshellDoc->GetControllingUnknown(), szActivateView, GetControllingUnknown(), pshellDoc->GetControllingUnknown() );

		if( m_pFrame && m_pFrame->GetSplitterWnd() )
			m_pFrame->GetSplitterWnd()->SendMessage( WM_ACTIVATEPANE );

		::FireEvent( ::GetAppUnknown(), szAppActivateView, GetControllingUnknown(), pshellDoc->GetControllingUnknown() );
	}

	IUnknown* punkViewOther = 0;

	if( bActivate )	
	{
		if( pDeactiveView && ::IsWindow( pDeactiveView->GetSafeHwnd() ) && 
			pDeactiveView->IsKindOf(RUNTIME_CLASS(CShellView)) )
		{
			punkViewOther = pDeactiveView->GetControllingUnknown();
			//pViewOther = pDeactiveView;	
		}
	}
	else
	{
		if( pActivateView && ::IsWindow( pActivateView->GetSafeHwnd() ) &&
			pActivateView->IsKindOf(RUNTIME_CLASS(CShellView)) )
			punkViewOther = pActivateView->GetControllingUnknown();
			//pViewOther = pActivateView;	
	}


	//add view panes here
	IUnknown* punkCU = GetControllingUnknown();



	//if( pViewOther != NULL && pViewOther != this )
	if( ::IsWindow( GetSafeHwnd() ) && 
		( punkViewOther != GetControllingUnknown() || pChildFrame->IsInMDIActivateState() ||
		bActivate
		)
		)		
	{
		sptrIView spView( GetControllingUnknown() );		
		if( spView )
		{
			spView->OnActivateView( bActivate, punkViewOther );
			::SendMessage( AfxGetMainWnd()->GetSafeHwnd(), WM_SETMESSAGESTRING, 0, 0 );
		}
	}
	theApp.OnIdle( 0 );

	AddStatusBarPanes( bActivate != 0 );

	//paul 28.05.2003
	static long lFireEvent = -1;
	if( lFireEvent == -1 )
		lFireEvent = ::GetValueInt( ::GetAppUnknown(), "\\MainFrame", "FireOnActivateView", 1L );

	if( lFireEvent == 1 )
	{
		CShellDocManager* pman = theApp.GetDocManager();
		if( pman )
		{
			int ncol = 0;
			int nrow = 0;

			if( m_pFrame )
			{
				IFrameWindowPtr ptr_frame( m_pFrame->GetControllingUnknown() ) ;
				if( ptr_frame )
				{
					IUnknown* punk_splitter = 0;
					ptr_frame->GetSplitter( &punk_splitter );

					if( punk_splitter )
					{					
						ISplitterWindowPtr ptr_splitter( punk_splitter );
						punk_splitter->Release();	punk_splitter = 0;
						if( ptr_splitter )
						{
							ptr_splitter->GetActivePane( &nrow, &ncol );
						}
					}
				}
			}

			pman->GetDispatchImpl()->FireOnActivateView( GetDocument()->GetIDispatch( false ), nrow, ncol, bActivate );		
		}
	}
}

BOOL CShellView::PreTranslateMessage(MSG* pMsg) 
{
	BOOL	bRet = FALSE;

	if(pMsg->message == WM_KEYDOWN)
	{
		if( m_pFrame && m_pFrame->GetSplitterWnd() )		
			m_pFrame->GetSplitterWnd()->SendMessage(WM_KEYDOWN,  pMsg->wParam, pMsg->lParam);
	}

	if(pMsg->message == WM_MOUSEMOVE )
	{
		if( GetDocument()->GetActiveView() == this )
		{
			CPoint	point = pMsg->pt;
			CWnd	*pwnd = GetWindowFromUnknown( GetControllingUnknown() );
			pwnd->ScreenToClient( &point );
			point = ConvertToClient( GetControllingUnknown(), point );
			CSize	size = _GetClientSize();
			point.x = min( point.x, size.cx );
			point.y = min( point.y, size.cy );
			::StatusSetPaneText( guidPaneMousePos, "[%d, %d]", point.x, point.y );
		}

		
	}

	//This is used for filtering messages in child views (for example, in axeditor)
	sptrIWindow	sptrW( GetControllingUnknown() );
	if (sptrW)sptrW->PreTranslateMsg( pMsg, &bRet );

	if( bRet )
		return TRUE;

	//не разобрали пока?
	if( pMsg->message == WM_KEYDOWN )
	{
		if( _OnKeyDown( pMsg->wParam, LOWORD( pMsg->lParam ), HIWORD( pMsg->lParam ) ) )
			return TRUE;
	}	
	else		
	if( pMsg->message == WM_MOUSEMOVE )
	{
		if( _OnMouseMove( pMsg->wParam, CPoint( (DWORD)pMsg->lParam ) ) )
			return TRUE;
	}
	else
	if( pMsg->message == WM_MBUTTONDOWN )
	{
		if( _OnMButtonDown( pMsg->wParam, CPoint( (DWORD)pMsg->lParam ) ) )
			return TRUE;
	}
	else
	if( pMsg->message == WM_MBUTTONUP )
	{
		if( _OnMButtonUp( pMsg->wParam, CPoint( (DWORD)pMsg->lParam ) ) )
			return TRUE;
	}
	if( pMsg->message == WM_RBUTTONDOWN )
	{
		if( _OnRButtonDown( pMsg->wParam, CPoint( (DWORD)pMsg->lParam ) ) )
			return TRUE;
	}
	else
	if( pMsg->message == WM_RBUTTONUP )
	{
		if( _OnRButtonUp( pMsg->wParam, CPoint( (DWORD)pMsg->lParam ) ) )
			return TRUE;
	}			

	return CView::PreTranslateMessage(pMsg);
}


bool CShellView::Init( CShellFrame *pfrm, CShellDoc *pdoc, IUnknown *punkView )
{	

	m_pFrame = 0;
	m_pDocument = 0;

	sptrIView	sptrView( GetControllingUnknown() );
	if( sptrView == 0 )
		return false; 

	m_pFrame = pfrm;
	m_pDocument = pdoc;

	m_pFrame->AddView( this );

	

	IUnknown	*punkDoc = pdoc->GetDocumentInterface( false );

	if( !CheckInterface( pdoc->GetControllingUnknown(), IID_INamedData ) )
	{
		AfxMessageBox( IDS_NONAMEDDATA2 );
		return false;
	}
	if( !CheckInterface( GetControllingUnknown(), IID_IDataContext ) )
	{
		AfxMessageBox( IDS_NOCONTEXT );
		return false;
	}

	{
		sptrIDataContext	sptrDC( GetControllingUnknown() );
		sptrDC->AttachData( pdoc->GetControllingUnknown() );
	}

	sptrView->Init( punkDoc, GetControllingUnknown() );

	return true;
}

IUnknown *CShellView::GetViewUnknown()
{
	IUnknown	*punk = GetControllingUnknown();
	punk->AddRef();
	return punk;
}

//void CShellView::PostNcDestroy() 
//{
////	_ReportCounter( GetControllingUnknown() );
//	m_benable_map	= false;
//	CView::PostNcDestroy();
//}

void CShellView::OnDestroy() 
{
	if( IsWindowVisible() )AddStatusBarPanes( false );
	m_bDestroyCalled = true;
	::FireEvent( GetAppUnknown(), szAppDestroyView, GetControllingUnknown() );

	g_TargetManager.UnRegisterTarget( GetControllingUnknown() );

	if( CheckInterface( GetControllingUnknown(), IID_IDataContext ) )
	{
		sptrIDataContext	sptrDC( GetControllingUnknown() );
		sptrDC->AttachData( 0 );
	}

	//CView::OnDestroy();


	if( m_pFrame->GetActiveView() == this )
		m_pFrame->SetActiveView( 0 );
	

	CMainFrame* pMainFrame = NULL;
	pMainFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	CShellFrame* pActiveFrame = NULL;
	if( pMainFrame )
	{
		pActiveFrame = pMainFrame->GetActiveChildFrame();	
		if( pActiveFrame )
		{
			CShellView* pView = (CShellView*)pActiveFrame->GetActiveView();
			if( pView )pView->OnActivateView(TRUE, pView, this );
		}
		//pActiveFrame->SendMessage( WM_NCACTIVATE, TRUE, 0L );
	}



	/*
	if( GetMainFrame()->GetActiveView() == this )
		GetMainFrame()->SetActiveView( 0 );
		*/

//	_ReportCounter( GetControllingUnknown() );

	
}

void CShellView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if( !m_bAutoScrollMode )
	{
		CWnd	*pwnd = ::GetWindowFromUnknown( GetControllingUnknown() );
		pwnd->SendMessage( WM_HSCROLL, MAKELONG(nSBCode, nPos), (LPARAM)pScrollBar->m_hWnd );
	}

	CScrollZoomSiteImpl::OnScroll( SB_HORZ, nSBCode, nPos );
}

void CShellView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if( !m_bAutoScrollMode )
	{
		CWnd	*pwnd = ::GetWindowFromUnknown( GetControllingUnknown() );
		pwnd->SendMessage( WM_VSCROLL, MAKELONG(nSBCode, nPos), (LPARAM)pScrollBar->m_hWnd );
	}

	CScrollZoomSiteImpl::OnScroll( SB_VERT, nSBCode, nPos );
}

void CShellView::OnFinalRelease() 
{
	//m_aggrs.DeInit();
	delete this;
}

CScrollBar *CShellView::GetScrollBarCtrl( int sbCode )
{
	HANDLE	hwnd = 0;
	m_pFrame->m_sptrSplitter->GetScrollBarCtrl( GetControllingUnknown(), sbCode, &hwnd );

	return (CScrollBar*)CScrollBar::FromHandle( (HWND)hwnd );
}

BOOL CShellView::OnEraseBkgnd(CDC* pDC) 
{
	return false;//CView::OnEraseBkgnd(pDC);
}


int CShellView::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	//TRACE( "OnMouseActivate " );
	//_ReportCounter( GetControllingUnknown() );

	
	CShellFrame* pChildFrame = NULL;
	pChildFrame = (CShellFrame*)GetParent()->GetParent();	
	if( pChildFrame )
	{
		ASSERT_KINDOF(CShellFrame, pChildFrame);
		if( !( (pChildFrame->GetFrameType() == ftDDI) &&
			(pChildFrame->IsFloatingDDIFrame() ) ) )
		{
			return CView::OnMouseActivate(pDesktopWnd, nHitTest, message);
		}
	}



	int nResult = CWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);
	pChildFrame->SetActiveView(this, FALSE );
	pChildFrame->SetActive( TRUE );
	
	OnActivateView(TRUE, this, this);
	
	return nResult;
}

CWnd *CShellView::GetWindow()
{
	return GetWindowFromUnknown( GetControllingUnknown() );
}

double CShellView::GetZoom() 
{
	// TODO: Add your property handler here
	return ::GetZoom(GetControllingUnknown());
}

void CShellView::SetZoom(double newValue) 
{
	// TODO: Add your property handler here
	if(newValue > 0)
	{
		IScrollZoomSitePtr	sptrSZ(GetControllingUnknown());
		if(sptrSZ != 0)
			sptrSZ->SetZoom(newValue);
	}
}

void CShellView::AddStatusBarPanes( bool bAdd )
{
	INewStatusBarPtr	ptrStatus( StatusGetBar(), false );
	if( ptrStatus == 0 )return;

	if( bAdd )
	{
		ptrStatus->RemovePane( guidPaneClientSize );
		ptrStatus->RemovePane( guidPaneMousePos );

		

		if( GetValueInt( GetAppUnknown(), "\\StatusBar", "ShowMousePos", 1 )  )
		{
			ptrStatus->AddPane( guidPaneMousePos, ::StatusCalcWidth( "(9999, 9999)", true ), 0, 0 );
			::StatusSetPaneIcon( guidPaneMousePos, (HICON)::LoadImage( AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_STATUS_MOUSEPOS), IMAGE_ICON, 16, 16, 0 ) );
		}

		if( GetValueInt( GetAppUnknown(), "\\StatusBar", "ShowClientSize", 1 )  )
		{
			ptrStatus->AddPane( guidPaneClientSize, ::StatusCalcWidth( "9999x9999", true ), 0, 0 );
			::StatusSetPaneIcon( guidPaneClientSize, (HICON)::LoadImage( AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_STATUS_CLIENTSIZE), IMAGE_ICON, 16, 16, 0 ) );
		}

		UpdateStatusBar();
	}
	else
	{
		ptrStatus->RemovePane( guidPaneClientSize );
		ptrStatus->RemovePane( guidPaneMousePos );
	}
}

void CShellView::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CView::OnShowWindow(bShow, nStatus);
	AddStatusBarPanes( bShow != 0 );
	g_TargetManager.EnableTarget( GetControllingUnknown(), bShow!= 0 );
	g_TargetManager.EnableTarget( GetDocument()->GetControllingUnknown(), bShow!= 0 );
}

void CShellView::_SetClientSize( SIZE size )
{
	CScrollZoomSiteImpl::_SetClientSize( size );
	UpdateStatusBar();
}

void CShellView::UpdateStatusBar()
{
	if( GetDocument()->GetActiveView() == this )
	{
		CSize	size = _GetClientSize();
		::StatusSetPaneText( guidPaneClientSize, "%dx%d", size.cx, size.cy );
	}
}

#define MOUSE_SECTION				"IntelliMouse"

//mouse wheel
#define MOUSE_WHEEL_ENABLE			"Wheel enable"
#define MOUSE_WHEEL_HORZ_KEY		"Wheel horz scroll key"
#define MOUSE_WHEEL_SCROLL_STEP		"Wheel step"
#define MOUSE_WHEEL_INVERT_HORZ		"Invert wheel horz direction"
#define MOUSE_WHEEL_INVERT_VERT		"Invert wheel vert direction"

//mouse scroll
#define MOUSE_SCROLL_ENABLE			"Scroll enable"
#define MOUSE_SCROLL_BTN			"Scroll key"

#define MOUSE_SCROLL_INVERT_HORZ	"Invert scroll horz direction"
#define MOUSE_SCROLL_INVERT_VERT	"Invert scroll invert vert direction"

#define KEY_SCROLL_ENABLE			"Enable key scroll"
#define MOUSE_KEY_SCROLL_STEP		"Key step"

/////////////////////////////////////////////////////////////////////////////
void CShellView::InitIntelliSettings()
{

	//mouse wheel
	long l = ::GetValueInt( ::GetAppUnknown(), MOUSE_SECTION, MOUSE_WHEEL_ENABLE, 0 );
	m_bUseMouseWheel	= ( l == 1L ? true : false );

	m_wheelHorzSwitch = whsNone;
	CString str	= ::GetValueString( ::GetAppUnknown(), MOUSE_SECTION, MOUSE_WHEEL_HORZ_KEY, "Ctrl" );
	if( str == "Ctrl" )
		m_wheelHorzSwitch = whsCtrl;
	else
	if( str == "Shift" )
		m_wheelHorzSwitch = whsShift;

	m_nWheelStep = ::GetValueInt( ::GetAppUnknown(), MOUSE_SECTION, MOUSE_WHEEL_SCROLL_STEP, 10 );	

	l = ::GetValueInt( ::GetAppUnknown(), MOUSE_SECTION, MOUSE_WHEEL_INVERT_HORZ, 0 );
	m_bInvertWheelHorzDirection = ( l == 1L ? true : false );

	l = ::GetValueInt( ::GetAppUnknown(), MOUSE_SECTION, MOUSE_WHEEL_INVERT_VERT, 0 );
	m_bInvertWheelVertDirection = ( l == 1L ? true : false );

	//mouse scroll

	l = ::GetValueInt( ::GetAppUnknown(), MOUSE_SECTION, MOUSE_SCROLL_ENABLE, 0 );
	m_bUseMouseScroll			 = ( l == 1L ? true : false );


	m_mouseScrollBtn = msbNone;
	str = ::GetValueString( ::GetAppUnknown(), MOUSE_SECTION, MOUSE_SCROLL_BTN, "Middle" );
	if( str == "Middle" )
		m_mouseScrollBtn = msbMiddle;
	else
	if( str == "Right" )
		m_mouseScrollBtn = msbRight;


	l = ::GetValueInt( ::GetAppUnknown(), MOUSE_SECTION, MOUSE_SCROLL_INVERT_HORZ, 0 );
	m_bInvertScrollHorzDirection = ( l == 1L ? true : false );

	l = ::GetValueInt( ::GetAppUnknown(), MOUSE_SECTION, MOUSE_SCROLL_INVERT_VERT, 0 );
	m_bInvertScrollVertDirection = ( l == 1L ? true : false );


	l = ::GetValueInt( ::GetAppUnknown(), MOUSE_SECTION, KEY_SCROLL_ENABLE, 0 );
	m_bEnableKeyScroll = ( l == 1L ? true : false );

	m_nKeyStep = ::GetValueInt( ::GetAppUnknown(), MOUSE_SECTION, MOUSE_KEY_SCROLL_STEP, 10 );	

	

	m_hCurWhellHorz		= 0;
	m_hCurWhellVert		= 0;
	m_hCurMiddleSet		= 0;
	m_hCurMiddleHorz	= 0;
	m_hCurMiddleVert	= 0;
	m_hCurSave			= 0;

	m_bScrollBtnDown	= false;

}

/////////////////////////////////////////////////////////////////////////////
void CShellView::DestroyIntelliSettings()
{
	if( m_hCurWhellHorz )
		::DestroyCursor( m_hCurWhellHorz );

	if( m_hCurWhellVert )
		::DestroyCursor( m_hCurWhellVert );

	if( m_hCurMiddleSet )
		::DestroyCursor( m_hCurMiddleSet );

	if( m_hCurMiddleHorz )
		::DestroyCursor( m_hCurMiddleHorz );

	if( m_hCurMiddleVert )
		::DestroyCursor( m_hCurMiddleVert );
}

/////////////////////////////////////////////////////////////////////////////
void CShellView::ScrollDelta( CPoint ptDelta )
{
	CSize size = _GetClientSize( );	

	CPoint ptScrll = _GetScrollPos();

	double fZoom = _GetZoom( );
	size.cx = long( double(size.cx) * fZoom );
	size.cy = long( double(size.cy) * fZoom );

	ptScrll += ptDelta;

	if( ptScrll.x >= size.cx ) ptScrll.x = size.cx-1;
	if( ptScrll.y >= size.cy ) ptScrll.y = size.cy-1;

	if( ptScrll.x < 0 ) ptScrll.x = 0;
	if( ptScrll.y < 0 ) ptScrll.y = 0;

	_SetScrollPos( ptScrll );

}

/////////////////////////////////////////////////////////////////////////////
BOOL CShellView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	if( m_bUseMouseWheel )
	{
		long lStep = m_nWheelStep;
		if( lStep < 0 )
			lStep = 1;

		long lStepCount =- zDelta * lStep / 120;
		bool bHorzScroll = false;

		if( nFlags & MK_CONTROL && m_wheelHorzSwitch == whsCtrl )
			bHorzScroll = true;
		else
		if( nFlags & MK_SHIFT && m_wheelHorzSwitch == whsShift )
			bHorzScroll = true;

		CPoint ptDelta( 0, 0 );

		if( bHorzScroll )
			ptDelta.x = ( lStepCount * ( m_bInvertWheelHorzDirection ? -1 : 1 ) );
		else
			ptDelta.y = ( lStepCount * ( m_bInvertWheelVertDirection ? -1 : 1 ) );

		ScrollDelta( ptDelta );
	}


	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

/////////////////////////////////////////////////////////////////////////////
bool CShellView::ProcessScrollButtonUp( UINT nFlags, CPoint point )
{
	if( m_bScrollBtnDown )
	{
		ReleaseCapture();
	}
	m_bScrollBtnDown = false;
	m_ptMousePosSave = CPoint( 0, 0 );

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CShellView::ProcessScrollButtonDown( UINT nFlags, CPoint point )
{
	m_bScrollBtnDown = true;
	m_ptMousePosSave = point;

	SetCapture();

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CShellView::_OnMouseMove(UINT nFlags, CPoint point) 
{		
	if( m_bScrollBtnDown && m_bUseMouseScroll && m_mouseScrollBtn != msbNone )
	{
		CPoint ptDelta = m_ptMousePosSave - point;

		if( m_bInvertScrollHorzDirection )
			ptDelta.x = -ptDelta.x;

		if( m_bInvertScrollVertDirection )
			ptDelta.y = -ptDelta.y;
		
		ScrollDelta( ptDelta );

		m_ptMousePosSave = point;

		return true;
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CShellView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{	
	return CView::OnSetCursor(pWnd, nHitTest, message);
}

/////////////////////////////////////////////////////////////////////////////
bool CShellView::_OnMButtonDown(UINT nFlags, CPoint point) 
{	
	if( m_bUseMouseScroll && m_mouseScrollBtn == msbMiddle )
	{
		return ProcessScrollButtonDown( nFlags, point );
	}

	return false;	
}

/////////////////////////////////////////////////////////////////////////////
bool CShellView::_OnRButtonDown(UINT nFlags, CPoint point) 
{
	if( m_bUseMouseScroll && m_mouseScrollBtn == msbRight )
	{
		return ProcessScrollButtonDown( nFlags, point );
	}

	return false;
}


/////////////////////////////////////////////////////////////////////////////
bool CShellView::_OnMButtonUp(UINT nFlags, CPoint point) 
{	
	if( m_bScrollBtnDown )
	{
		return ProcessScrollButtonUp( nFlags, point );		
	}

	return false;	
}


/////////////////////////////////////////////////////////////////////////////
bool CShellView::_OnRButtonUp(UINT nFlags, CPoint point) 
{
	if( m_bScrollBtnDown )
	{
		return ProcessScrollButtonUp( nFlags, point );	
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////
bool CShellView::_OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{	

	if( !m_bEnableKeyScroll )
		return false;

	if( m_bEnableKeyScroll)
	{
		if ((GetAsyncKeyState( VK_CONTROL ) & 0x8000) == 0 &&
			(GetAsyncKeyState( VK_SHIFT ) & 0x8000) == 0&&
			(GetAsyncKeyState( VK_MENU ) & 0x8000) == 0)
		{
			int dx, dy;
			dx = dy = 0;
			if( nChar == VK_LEFT )
				dx = -m_nKeyStep;
			else
			if( nChar == VK_RIGHT )
				dx = m_nKeyStep;
			else
			if( nChar == VK_UP )
				dy = -m_nKeyStep;
			else
			if( nChar == VK_DOWN )
				dy = m_nKeyStep;
			if( !( dx == 0 && dy == 0 ) )
			{
				ScrollDelta( CPoint( dx, dy ) );
				return true;
			}
		}
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////
LPDISPATCH CShellView::GetViewDispatch( )
{
	IDispatch* pi_disp = 0;		

	IViewDispatchPtr ptr_vd( GetControllingUnknown() );
	if( ptr_vd == 0 )
		return 0;

	GUID guid = INVALID_KEY;
	ptr_vd->GetDispIID( &guid );
	if( guid == INVALID_KEY )
		return 0;

	GetControllingUnknown()->QueryInterface( guid, (void**)&pi_disp );

	return pi_disp;
}

/////////////////////////////////////////////////////////////////////////////
BSTR CShellView::GetViewName()
{
	_bstr_t bstrName;
	INamedObject2Ptr sptrNO2(GetControllingUnknown());
	if (sptrNO2 != 0)
		sptrNO2->GetName(bstrName.GetAddress());
	return bstrName.Detach();
}


/////////////////////////////////////////////////////////////////////////////
LPDISPATCH CShellView::GetDataContextDispatch( )
{
	// {DCEE4109-9579-46d3-9E57-576CA16D60B4}
	IID IID_IDataContextDisp =
	{ 0xdcee4109, 0x9579, 0x46d3, { 0x9e, 0x57, 0x57, 0x6c, 0xa1, 0x6d, 0x60, 0xb4 } };

	IDispatch* pi_disp = 0;		
	GetControllingUnknown()->QueryInterface( IID_IDataContextDisp, (void**)&pi_disp );

	return pi_disp;
}

/////////////////////////////////////////////////////////////////////////////
IUnknown* CShellView::GetInterfaceHook( const void* p )
{	
	return get_fast_interface( (const GUID*)p );
}

/////////////////////////////////////////////////////////////////////////////
IUnknown* CShellView::raw_get_interface( const GUID* piid )
{	
	IUnknown* punk = GetInterface( piid );
	if( !punk )
	{
		punk = QueryAggregates( piid );
		if( punk )
			punk->Release();
	}	
	return punk;
}

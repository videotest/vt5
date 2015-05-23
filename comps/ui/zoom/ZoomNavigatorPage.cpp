// ZoomNavigatorPage.cpp : implementation file
//

#include "stdafx.h"
#include "zoom.h"
#include "ZoomNavigatorPage.h"
#include "thumbnail.h"

#include "PropPage.h"
#include "window5.h"
#include "math.h"

#include "method_int.h"

#include "SewLI_int.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ZOOM_NAVIGATOR_SECTION	"ZoomNavigator"
#define ZOOM_USE_ZOOM_10		"UseZoomInOut10"



/////////////////////////////////////////////////////////////////////////////
// CZoomNavigatorPage

IMPLEMENT_DYNCREATE(CZoomNavigatorPage, CDialog);

// {6D036BFB-74B0-48e6-B503-0F38E132FE0A}
GUARD_IMPLEMENT_OLECREATE(CZoomNavigatorPage, szZoomNavigatorPropPageProgID, 
0x6d036bfb, 0x74b0, 0x48e6, 0xb5, 0x3, 0xf, 0x38, 0xe1, 0x32, 0xfe, 0xa);


BEGIN_INTERFACE_MAP(CZoomNavigatorPage, CPropertyPageBase)
	INTERFACE_PART(CZoomNavigatorPage, IID_IEventListener, EvList)
	INTERFACE_PART(CZoomNavigatorPage, IID_IZoomPage, ZoomPage)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CZoomNavigatorPage, ZoomPage);

/////////////////////////////////////////////////////////////////////////////
CZoomNavigatorPage::CZoomNavigatorPage()
	: CPropertyPageBase(CZoomNavigatorPage::IDD)
{
	//{{AFX_DATA_INIT(CZoomNavigatorPage)
	m_nSliderPos = 0;
	//}}AFX_DATA_INIT
	_OleLockApp( this );
	m_sName = c_szCZoomNavigatorPage;
	m_sUserName.LoadString( IDS_ZOOM_NAVIGATOR );	

	m_rcImage = m_rcVisible = NORECT;
	m_bStartDrag = false;
	
	m_pThumbnailCache = NULL;

	m_hMovePaneCur = AfxGetApp()->LoadCursor(IDC_MOVE_PANE);
	
	m_bPageFireScrollEvent	= false;	
}

/////////////////////////////////////////////////////////////////////////////
CZoomNavigatorPage::~CZoomNavigatorPage()
{
	if( m_pThumbnailCache )
	{
		::ThumbnailFree( m_pThumbnailCache );
		m_pThumbnailCache = NULL;
	}

	if( m_hMovePaneCur )
		::DestroyCursor( m_hMovePaneCur );

	_OleUnlockApp( this );
}


/////////////////////////////////////////////////////////////////////////////
IUnknown* CZoomNavigatorPage::GetActiveDocument()
{
	IUnknown* punkView = NULL;
	punkView = GetActiveView();
	if( punkView == NULL )
		return NULL;

	IViewPtr ptrView(punkView);
	punkView->Release();
	if( ptrView == NULL )
		return NULL;

	IUnknown* punkDoc = NULL;
	ptrView->GetDocument( &punkDoc );

	return punkDoc;		
}

/////////////////////////////////////////////////////////////////////////////
BOOL CZoomNavigatorPage::OnInitDialog() 
{
	CDialog::OnInitDialog();		
	Register( ::GetAppUnknown() );
	IUnknownPtr punkMethodMan( _GetOtherComponent( GetAppUnknown(), IID_IMethodMan ), false); 
	Register( punkMethodMan );

	IUnknown* punkActiveDoc = NULL;
	punkActiveDoc = GetActiveDocument();
	if( punkActiveDoc  )
	{
		RegisterDoc( punkActiveDoc );
		punkActiveDoc->Release();
	}

	m_crtlCombo.ResetContent();

	for( int i=1;i<10;i++ )
	{
		CString str = FormatZoomFromD2S( i * 10 );
		m_crtlCombo.AddString( str );
	}

	for( i=1;i<=10;i++ )
	{
		CString str = FormatZoomFromD2S( i * 100 );
		m_crtlCombo.AddString( str );
	}



	{
		CSliderCtrl* pSlider = (CSliderCtrl*)GetDlgItem( IDC_ZOOM_SLIDER );
		if( pSlider )
		{
			pSlider->SetRange( ConvertFromDoubleToTick( 1/16. ), ConvertFromDoubleToTick( 16. ) );
			pSlider->SetPageSize( ZOOM_TICK_STEP/10 );
			
			pSlider->SetPos( ConvertFromDoubleToTick( 5.0 ) );
			
			/*
			pSlider->SetRange( 0, 1000 );
			pSlider->SetLineSize( 1 );
			pSlider->SetPageSize( 50 );
			*/
					
		}				
	}

	// A.M. BT5278
	IUnknownPtr punkMtdMan(_GetOtherComponent( GetAppUnknown(), IID_IMethodMan2 ), false);
	IMethodMan2Ptr	sptrMtdMan = punkMtdMan;
	if (sptrMtdMan != 0)
	{
		BOOL bRecording,bRunning;
		if (SUCCEEDED(sptrMtdMan->IsRecording(&bRecording)) && bRecording ||
			SUCCEEDED(sptrMtdMan->IsRunning(&bRunning)) && bRunning)
		{
			CWnd *pWnd = GetDlgItem(IDC_FEET_TO_DOC);
			if (pWnd) pWnd->SetProperty(0x26, VT_BOOL, TRUE);
		}
	}

	UpdatePage( 0 );	


	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CZoomNavigatorPage::RegisterDoc( IUnknown* punkDoc )
{
	if( ::GetObjectKey(m_ptrActiveDoc) == ::GetObjectKey(punkDoc) )
		return;

	if( m_ptrActiveDoc )
	{
		UnRegisterDoc( );		
	}

	m_ptrActiveDoc = punkDoc;
	Register( m_ptrActiveDoc );
}

/////////////////////////////////////////////////////////////////////////////
void CZoomNavigatorPage::UnRegisterDoc( )
{
	if( m_ptrActiveDoc )
	{
		UnRegister( m_ptrActiveDoc );
		m_ptrActiveDoc = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////
void CZoomNavigatorPage::OnDestroy() 
{
	UnRegisterDoc( );
	IUnknownPtr punkMethodMan( _GetOtherComponent( GetAppUnknown(), IID_IMethodMan ), false); 
	UnRegister( punkMethodMan );
	UnRegister( ::GetAppUnknown() );
	CDialog::OnDestroy();
}

/////////////////////////////////////////////////////////////////////////////
bool CZoomNavigatorPage::UpdatePage( DWORD dwOptions )
{
	if( !GetSafeHwnd() )
		return false;

	IUnknown* punkView = NULL;
	punkView = GetActiveView();
	if( punkView == NULL )
		return false;

	IScrollZoomSitePtr ptrSZS( punkView );
	punkView->Release();

	if( ptrSZS == NULL )
		return false;
	
	double fZoom = 1.0;
	ptrSZS->GetZoom( &fZoom );
	
	SetZoomToCombo( fZoom );
	SetZoomToSlider( fZoom );


	CSliderCtrl* pSlider = (CSliderCtrl*)GetDlgItem( IDC_ZOOM_SLIDER );
	if( pSlider )
	{
		pSlider->SetPos( ConvertFromDoubleToTick(fZoom) );
	}				

	return true;
}

/////////////////////////////////////////////////////////////////////////////
void CZoomNavigatorPage::SetZoomToCombo( double fZoom )
{
	CString strZoom = FormatZoomFromD2S( fZoom * 100.0 );	
	int nIndex = m_crtlCombo.FindString( 0, strZoom );
	if( nIndex != -1 )
		m_crtlCombo.SetCurSel( nIndex );
	else
		m_crtlCombo.SetWindowText( strZoom );
}

/////////////////////////////////////////////////////////////////////////////
void CZoomNavigatorPage::SetZoomToSlider( double fZoom )
{
	CSliderCtrl* pSlider = (CSliderCtrl*)GetDlgItem( IDC_ZOOM_SLIDER );
	if( pSlider )
	{		
		pSlider->SetPos( ConvertFromDoubleToTick(fZoom) );
	}
}


/////////////////////////////////////////////////////////////////////////////
void CZoomNavigatorPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CZoomNavigatorPage)
	DDX_Control(pDX, IDC_ZOOM_SLIDER, m_ctrlSlider);
	DDX_Control(pDX, IDC_ZOOM, m_crtlCombo);
	DDX_Slider(pDX, IDC_ZOOM_SLIDER, m_nSliderPos);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CZoomNavigatorPage, CPropertyPageBase)
	//{{AFX_MSG_MAP(CZoomNavigatorPage)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_HSCROLL()
	ON_CBN_SELCHANGE(IDC_ZOOM, OnSelchangeZoom)	
	ON_CBN_EDITCHANGE(IDC_ZOOM, OnEditchangeZoom)
	ON_WM_SETCURSOR()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CZoomNavigatorPage message handlers

/*
#define FontPtr 
//Push button referense
#ifdef _DEBUG
#import "..\\..\\..\\vt5\\debug\\comps\\VTControls.ocx" no_namespace 
#else
#import "..\\..\\..\\vt5\\release\\comps\\VTControls.ocx" no_namespace 
#endif //_DEBUG
*/


/////////////////////////////////////////////////////////////////////////////
IUnknown* CZoomNavigatorPage::GetActiveView()
{
	sptrIApplication sptrApp( ::GetAppUnknown() );
	if( sptrApp == NULL )
		return NULL;

	IUnknown* punkDoc = NULL;
	sptrApp->GetActiveDocument( &punkDoc );
	
	if( punkDoc == NULL )
		return NULL;
	

	sptrIDocumentSite sptrDS( punkDoc );
	punkDoc->Release();
	if( sptrDS == NULL )
		return NULL;

	IUnknown* punkView = NULL;
	sptrDS->GetActiveView( &punkView );

	return punkView;

}

/////////////////////////////////////////////////////////////////////////////
IUnknown* CZoomNavigatorPage::GetActveObject()
{
	IUnknown* punkView = NULL;
	punkView = GetActiveView();
	if( punkView == NULL )
		return NULL;

	IUnknownPtr ptrView = punkView;
	punkView->Release();

	IUnknown* punkObject = NULL;
	if (CheckInterface(ptrView,IID_ISewLIView))
		punkObject = ::GetActiveParentObjectFromContext(punkView, szTypeSewLI);	
	else
		punkObject = ::GetActiveParentObjectFromContext(punkView, szTypeImage);	

	return punkObject;
}

/////////////////////////////////////////////////////////////////////////////
BYTE* CZoomNavigatorPage::GetThumbnail( IUnknown* punkObject, CSize sizeThumbnail )
{

	sptrIThumbnailManager spThumbMan( ::GetAppUnknown() );
	if( spThumbMan == NULL )
		return NULL;

	IUnknown* punkRenderObject = NULL;

	spThumbMan->GetRenderObject( punkObject, &punkRenderObject );
	if( punkRenderObject == NULL )
		return NULL;


	sptrIRenderObject spRenderObject( punkRenderObject );
	punkRenderObject->Release();

	if( spRenderObject == NULL ) 
		return NULL;


	BYTE* pbi = NULL;

	short nSupported;
	COLORREF clr = ::GetSysColor(COLOR_3DHILIGHT);//COLOR_3DFACE);

	spRenderObject->GenerateThumbnail( punkObject, 24, RF_BACKGROUND, (LPARAM)clr, 
				sizeThumbnail, &nSupported, &pbi );

	return pbi;
	
}

/////////////////////////////////////////////////////////////////////////////
bool CZoomNavigatorPage::GetViewRectOnPage( IUnknown* punkView, BYTE* pbi, CRect& rcImage, CRect& rcVisible )
{
	CWnd* pWnd = GetDlgItem( IDC_VIEW );	
	if( pWnd == NULL )
		return false;

	CRect rcWnd;
	pWnd->GetWindowRect( &rcWnd );
	ScreenToClient( &rcWnd );

	if( punkView == NULL || pbi == NULL )
		return false;

	IScrollZoomSitePtr ptrSZS( punkView );
	if( ptrSZS == NULL )
		return false;

	
	CSize sizeV;
	ptrSZS->GetClientSize( &sizeV );

	CPoint ptScrollV;
	ptrSZS->GetScrollPos( &ptScrollV );		

	//ASSERT( ptScrollV.x >= 0 && ptScrollV.y >= 0 );
	if( ptScrollV.x < 0 )
		ptScrollV.x = 0;
	
	if( ptScrollV.y < 0 )
		ptScrollV.y = 0;

	double fZoomV = 1;
	ptrSZS->GetZoom( &fZoomV );	

	CRect rcVisibleV;
	ptrSZS->GetVisibleRect( &rcVisibleV );

	BITMAPINFOHEADER* pBitmapInfoHeader = (BITMAPINFOHEADER*)pbi;
	CSize sizeThumbnail( pBitmapInfoHeader->biWidth, pBitmapInfoHeader->biHeight );

	rcImage.left = rcWnd.left + rcWnd.Width()  / 2 - sizeThumbnail.cx / 2;
	rcImage.top  = rcWnd.top  + rcWnd.Height() / 2 - sizeThumbnail.cy / 2;
	rcImage.right  = rcImage.left + sizeThumbnail.cx;
	rcImage.bottom = rcImage.top  + sizeThumbnail.cy;


	double fZoomV2T = 1.0;
	if( sizeV.cx > 0 )
		fZoomV2T = (double)rcImage.Width() / (double)sizeV.cx;

	m_fZoomV2T = fZoomV2T;

	double fZoom = 1.0;
	if( fZoom > 0.0 )
		fZoom = fZoomV2T / fZoomV;

	CSize sizeVisible( rcVisibleV.Width(), rcVisibleV.Height() );
	CPoint ptOffset( ptScrollV.x, ptScrollV.y );


	sizeVisible.cx = (double)sizeVisible.cx * (double)fZoom;
	sizeVisible.cy = (double)sizeVisible.cy * (double)fZoom;

	ptOffset.x = (double)ptOffset.x * (double)fZoom;
	ptOffset.y = (double)ptOffset.y * (double)fZoom;



	rcVisible.left = rcImage.left + ptOffset.x;
	rcVisible.top  = rcImage.top  + ptOffset.y;
	
	rcVisible.right  = rcVisible.left + sizeVisible.cx;
	rcVisible.bottom = rcVisible.top  + sizeVisible.cy;

	
	if( rcVisible.left < rcImage.left )
		rcVisible.left = rcImage.left;

	if( rcVisible.top < rcImage.top )
		rcVisible.top = rcImage.top;

	if( rcVisible.right > rcImage.right )
		rcVisible.right = rcImage.right;

	if( rcVisible.bottom > rcImage.bottom )
		rcVisible.bottom = rcImage.bottom;
		

	return true;
}

/////////////////////////////////////////////////////////////////////////////
void CZoomNavigatorPage::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	bool bProcessDrawing = false;

	CRect rcClient;
	GetClientRect( &rcClient );
	
	CWnd* pWnd = GetDlgItem( IDC_VIEW );	
	if( pWnd == NULL )
	{		
		ASSERT( FALSE );
		return;
	}

	CRect rcWnd;
	pWnd->GetWindowRect( &rcWnd );
	ScreenToClient( &rcWnd );

	//


	//Get active view
	IUnknown* punkView = NULL;
	punkView = GetActiveView();
	if( punkView != NULL )
	{
		IUnknownPtr ptrView = punkView;
		punkView->Release();

		//Get active object
		IUnknown* punkObject = NULL;
		punkObject = GetActveObject();
		if( punkObject != NULL )
		{
			IUnknownPtr ptrObject = punkObject;
			punkObject->Release();

			CSize sizeCtrl( rcWnd.Width() + 1, rcWnd.Height() + 1 );

			if( ::GetObjectKey(ptrObject) != m_guidActiveObject )
			{
				m_guidActiveObject = ::GetObjectKey(ptrObject);

				if( m_pThumbnailCache )
					::ThumbnailFree( m_pThumbnailCache );
				m_pThumbnailCache = NULL;
				m_pThumbnailCache = GetThumbnail( punkObject, sizeCtrl );
			}

			if( m_pThumbnailCache == NULL )
			{		
				return;
			}

			
			CRect rcImage, rcVisible;
			if( !GetViewRectOnPage( ptrView, m_pThumbnailCache, rcImage, rcVisible ) )
			{		
				ASSERT( FALSE );
				return;
			}
			
			m_rcImage	= rcImage;
			m_rcVisible = rcVisible;


			CDC dcImage;
			dcImage.CreateCompatibleDC( &dc );

			CBitmap Bitmap;
			Bitmap.CreateCompatibleBitmap( &dc, rcClient.Width() + 1, rcClient.Height() + 1 );
			CBitmap* pOldBitmap = dcImage.SelectObject( &Bitmap );

			CRect rcFill = CRect( 0, 0, rcClient.Width() + 1, rcClient.Height() + 1 );

			dcImage.FillRect( &rcFill, &CBrush( ::GetSysColor(COLOR_BTNFACE) ) );
				
			::ThumbnailDraw( m_pThumbnailCache, &dcImage, rcWnd, 0 );
			
			dcImage.FrameRect( &rcVisible, &CBrush( RGB( 0, 0, 0 ) ) );
			rcVisible.DeflateRect( 1, 1, 1, 1 );
			dcImage.FrameRect( &rcVisible, &CBrush( RGB( 255, 255, 255 ) ) );	

			dc.BitBlt( 0, 0, rcClient.Width() + 1, rcClient.Height() + 1,
					&dcImage, 0, 0, SRCCOPY );

			dcImage.SelectObject( pOldBitmap );
			
			bProcessDrawing = true;

		}
	}
	else
		m_guidActiveObject = GUID_NULL; // A.M. fix SBT2254


	if( !bProcessDrawing )
		dc.FillRect( &rcClient, &CBrush( ::GetSysColor(COLOR_BTNFACE) ) );

	
}

/////////////////////////////////////////////////////////////////////////////
void CZoomNavigatorPage::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	bool bUpdatePropPage = false;

	if( !strcmp(pszEvent, szAppActivateView) )
	{	
		IUnknown* punkDoc = NULL;
		punkDoc = GetActiveDocument();
		if( punkDoc )
		{
			RegisterDoc( punkDoc );
			punkDoc->Release();
		}				

		InvalidatePreviewWnd();
		bUpdatePropPage = true;
	}
	else
	if( !strcmp(pszEvent, szEventScrollPosition) ||		
		!strcmp(pszEvent, szEventActivateObject) ||
		!strcmp(pszEvent, szActivateView)
		)
	{
		InvalidatePreviewWnd();
		bUpdatePropPage = true;
	}
	else
	if( !strcmp(pszEvent, szNeedUpdateView) )
	{
/*		IUnknown* punkView = NULL;
		punkView = GetActiveView();
		if( punkView )
		{
			if( ::GetObjectKey(punkView) == ::GetObjectKey(punkHit) )*/
			{
				m_guidActiveObject = INVALID_KEY;
				InvalidatePreviewWnd();	
				bUpdatePropPage = true;
			}

/*			punkView->Release();
		}*/
	}
	else
	if(!strcmp(pszEvent, szEventChangeObject))	
	{
		//if( ::GetObjectKey( punkFrom ) == m_guidActiveObject )
		if( GetObjectKind( punkFrom ) == szTypeImage );
		{
			m_guidActiveObject = INVALID_KEY;//force regenerate thumbnail;
			InvalidatePreviewWnd();				
			bUpdatePropPage = true;
		}
	}
	else
	if( !strcmp(pszEvent, "AfterClose"/*szEventBeforeClose*/) )
	{
		if( ::GetObjectKey(m_ptrActiveDoc) == ::GetObjectKey(punkFrom) )
			UnRegisterDoc();
	}
	else
	if( !strcmp(pszEvent, szEventChangeObjectList ) )
	{
		IUnknownPtr ptr_active( GetActveObject(), false );
		if( ::GetObjectKey( punkFrom ) == ::GetObjectKey( ptr_active ) )		
		{
			m_guidActiveObject = INVALID_KEY;
			InvalidatePreviewWnd();				
			bUpdatePropPage = true;
		}
	}
	else
	if( !strcmp(pszEvent, szEventMethodRun ) || !strcmp(pszEvent, szEventMethodRecord ) )
	{
		CWnd *pWnd = GetDlgItem(IDC_FEET_TO_DOC);
		if (pWnd) pWnd->SetProperty(0x26, VT_BOOL, TRUE);
	}
	else
	if( !strcmp(pszEvent, szEventMethodStop ) )
	{
		CWnd *pWnd = GetDlgItem(IDC_FEET_TO_DOC);
		if (pWnd) pWnd->SetProperty(0x26, VT_BOOL, FALSE);
	}

	if( bUpdatePropPage && !m_bPageFireScrollEvent )
		UpdatePage( 0 );

}

/////////////////////////////////////////////////////////////////////////////
void CZoomNavigatorPage::InvalidatePreviewWnd()
{	
	if( m_bPageFireScrollEvent )
		return;

	CWnd* pWnd = GetDlgItem( IDC_VIEW );	
	if( pWnd == NULL )
		return;

	CRect rcWnd;
	pWnd->GetWindowRect( &rcWnd );
	ScreenToClient( &rcWnd );	

	rcWnd.InflateRect( 1, 1, 1, 1 );

	InvalidateRect( &rcWnd );
	//UpdateWindow();
	
	//Invalidate();
	
}

/////////////////////////////////////////////////////////////////////////////
void CZoomNavigatorPage::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if( !m_rcVisible.PtInRect( point ) )
	{
		if( m_rcImage.PtInRect(point))
		{
			CPoint ptOffset( point.x - m_rcImage.left - m_rcVisible.Width() / 2, 
							 point.y - m_rcImage.top - m_rcVisible.Height() / 2 );

			IUnknown* punkView = NULL;
			punkView = GetActiveView();
			if( punkView )
			{
				IScrollZoomSitePtr ptrSZS( punkView );
				punkView->Release();
				if( ptrSZS )
				{					
					double fZoomV;
					ptrSZS->GetZoom( &fZoomV );
					
					
					if( m_fZoomV2T > 0.0 )
						ptOffset.x = (double)ptOffset.x / m_fZoomV2T * fZoomV;

					
					if( m_fZoomV2T > 0.0 )
						ptOffset.y = (double)ptOffset.y / m_fZoomV2T * fZoomV;
					
					m_bPageFireScrollEvent = true;
					ptrSZS->SetScrollPos( ptOffset );
					m_bPageFireScrollEvent = false;					
					InvalidatePreviewWnd();
				}
			}				
		}			
	}

	if( m_rcVisible.PtInRect( point ) )
	{
		::SetCursor( m_hMovePaneCur );

		m_bStartDrag = true;		
		m_ptStartDragMousePos = point;

		m_viewSrollPosBeforDrag = CPoint( 0, 0 );
		IUnknown* punkView = NULL;
		punkView = GetActiveView();
		if( punkView )
		{
			IScrollZoomSitePtr ptrSZS( punkView );
			punkView->Release();
			if( ptrSZS )
			{
				ptrSZS->GetScrollPos( &m_viewSrollPosBeforDrag );
			}
		}				
		

	}

	CDialog::OnLButtonDown(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////
void CZoomNavigatorPage::OnLButtonUp(UINT nFlags, CPoint point) 
{	
	if( m_bStartDrag )
	{		
		m_bStartDrag = false;
	}

	::SetCursor(::LoadCursor(NULL,IDC_ARROW));

	CDialog::OnLButtonUp(nFlags, point);
}


/////////////////////////////////////////////////////////////////////////////
void CZoomNavigatorPage::OnMouseMove(UINT nFlags, CPoint point) 
{	
	
	//m_rcImage = m_rcVisible
	if( m_bStartDrag && /*m_rcImage.PtInRect( point ) &&*/
		(::GetAsyncKeyState( VK_LBUTTON ) & 0x8000) == 0x8000  )
	{

		CPoint ptDragOffset = point - m_ptStartDragMousePos;
		IUnknown* punkView = NULL;
		punkView = GetActiveView();
		if( punkView )
		{
			IScrollZoomSitePtr ptrSZS( punkView );
			if( ptrSZS )
			{

				double fZoomV;
				ptrSZS->GetZoom( &fZoomV );

				CPoint ptOffset( 0, 0 );

				if( m_fZoomV2T > 0.0 )
				{
					ptOffset.x = m_viewSrollPosBeforDrag.x + (double)ptDragOffset.x / m_fZoomV2T * fZoomV;
					ptOffset.y = m_viewSrollPosBeforDrag.y + (double)ptDragOffset.y / m_fZoomV2T * fZoomV;				
				}

				m_bPageFireScrollEvent = true;
				ptrSZS->SetScrollPos( ptOffset );				
				m_bPageFireScrollEvent = false;
				InvalidatePreviewWnd();				
				
			}

			punkView->Release();
		}
	}

	CDialog::OnMouseMove(nFlags, point);
}


HRESULT CZoomNavigatorPage::XZoomPage::Update(DWORD dwOptions)
{
	_try_nested(CZoomNavigatorPage, ZoomPage, Update)
	{
		pThis->UpdatePage( dwOptions );
		return S_OK;
	}
	_catch_nested;
}



BEGIN_EVENTSINK_MAP(CZoomNavigatorPage, CDialog)
    //{{AFX_EVENTSINK_MAP(CZoomNavigatorPage)
	ON_EVENT(CZoomNavigatorPage, IDC_ZOOM_OUT, -600 /* Click */, OnClickZoomOut, VTS_NONE)
	ON_EVENT(CZoomNavigatorPage, IDC_ZOOM_100, -600 /* Click */, OnClickZoom100, VTS_NONE)
	ON_EVENT(CZoomNavigatorPage, IDC_ZOOM_IN, -600 /* Click */, OnClickZoomIn, VTS_NONE)
	ON_EVENT(CZoomNavigatorPage, IDC_FEET_TO_SCREEN, -600 /* Click */, OnClickFeetToScreen, VTS_NONE)
	ON_EVENT(CZoomNavigatorPage, IDC_FEET_TO_DOC, -600 /* Click */, OnClickFeetToDoc, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

/////////////////////////////////////////////////////////////////////////////
void CZoomNavigatorPage::OnClickZoomOut() 
{	
	m_bPageFireScrollEvent = true;
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	::ExecuteAction( GetActionNameByResID( ID_ACTION_ZOOM_OUT ) );	
	m_bPageFireScrollEvent = false;
	
	InvalidatePreviewWnd();
	UpdatePage( 0 );
}


/////////////////////////////////////////////////////////////////////////////
void CZoomNavigatorPage::OnClickZoom100() 
{
	m_bPageFireScrollEvent = true;
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	::ExecuteAction( GetActionNameByResID( ID_ACTION_ZOOM_1_1 ) );	
	m_bPageFireScrollEvent = false;
	
	InvalidatePreviewWnd();
	
	UpdatePage( 0 );
}

/////////////////////////////////////////////////////////////////////////////
void CZoomNavigatorPage::OnClickZoomIn() 
{
	m_bPageFireScrollEvent = true;
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	::ExecuteAction( GetActionNameByResID( ID_ACTION_ZOOM_IN ) );	
	m_bPageFireScrollEvent = false;

	InvalidatePreviewWnd();

	UpdatePage( 0 );

}

/////////////////////////////////////////////////////////////////////////////
void CZoomNavigatorPage::OnClickFeetToScreen() 
{	
	m_bPageFireScrollEvent = true;
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	::ExecuteAction( GetActionNameByResID( ID_ACTION_FIT_DOC2SCR ) );	
	m_bPageFireScrollEvent = false;

	InvalidatePreviewWnd();

	UpdatePage( 0 );
}

/////////////////////////////////////////////////////////////////////////////
void CZoomNavigatorPage::OnClickFeetToDoc() 
{	
	m_bPageFireScrollEvent = true;
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	::ExecuteAction( GetActionNameByResID( ID_ACTION_FIT_2IMAGE ) );	
	m_bPageFireScrollEvent = false;

	InvalidatePreviewWnd();

	UpdatePage( 0 );
}

/////////////////////////////////////////////////////////////////////////////
void CZoomNavigatorPage::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	
	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}

/////////////////////////////////////////////////////////////////////////////
double CZoomNavigatorPage::FormatZoomFromS2D( CString strZoom )
{
	int nPos = strZoom.Find( "%" );	
	if( nPos != -1 )
		strZoom.Delete( nPos );

	_variant_t var = (LPCSTR)strZoom;
	double fZoom = 100.0;
	try{
		var.ChangeType( VT_R4 );
		fZoom = (double)var.fltVal;
	}
	catch(...)
	{
		fZoom = 100.0;
	}

	return fZoom;
}

/////////////////////////////////////////////////////////////////////////////
CString CZoomNavigatorPage::FormatZoomFromD2S( double fZoom )
{
	double fRoundZoom = (double)( ( (double)(long)(fZoom * 100) ) / 100.0 ); 
	_variant_t var = (double)fRoundZoom;
	CString strZoom = "100";
	try{
		var.ChangeType( VT_BSTR );
		strZoom = var.bstrVal;
	}
	catch(...)
	{}
	
	strZoom += "%";	

	return strZoom;
	
}

/////////////////////////////////////////////////////////////////////////////
int CZoomNavigatorPage::ConvertFromDoubleToTick( double fZoom )
{
	//return (int)fZoom * 100.0;
	
	/*if( fZoom < 1.0 )
	{
		return (16 - 1.0 / fZoom) * ZOOM_TICK_STEP;
	}
	
	
	return (fZoom + 14) * ZOOM_TICK_STEP;*/
	int nTick = 0;

	if( fZoom > 0.0 ) 
		nTick = log10( fZoom )*ZOOM_TICK_STEP;

	return nTick;
	
}

/////////////////////////////////////////////////////////////////////////////
double CZoomNavigatorPage::ConvertFromTickToDouble( int nTick )
{
	//return (double)nTick / 100.0;
	
	/*if( nTick < 15 * ZOOM_TICK_STEP )
	{
		int nDev = 16 - nTick / ZOOM_TICK_STEP;

		double fZoom = 1.0 / (double)nDev;
		return fZoom;
	}


	return ( (double)nTick / (double)ZOOM_TICK_STEP - 14 ) ;*/
	return pow( 10, 1.*nTick/ZOOM_TICK_STEP );
}


/////////////////////////////////////////////////////////////////////////////
bool CZoomNavigatorPage::SetZoom( double fZoom )
{	
	IUnknown* punkView = NULL;
	punkView = GetActiveView();
	if( punkView == NULL )
		return false;

	sptrIScrollZoomSite2	sptrSite( punkView );
	punkView->Release();

	if( sptrSite == NULL )
		return false;	
	

	m_bPageFireScrollEvent = true;	

	CPoint	pointScroll;
	sptrSite->GetScrollPos( &pointScroll );
	
	CRect	rectClient;
	CWnd	*pwnd = GetWindowFromUnknown( sptrSite );
	pwnd->GetClientRect( rectClient );

	CPoint	pointCenterClient = ::ConvertToClient( sptrSite, rectClient.CenterPoint() );
	
	sptrSite->SetFitDoc(FALSE);
	sptrSite->SetZoom( fZoom );

	CPoint	pointCenterClientNew = ::ConvertToClient( sptrSite, rectClient.CenterPoint() );
	CSize	size = ::ConvertToWindow( sptrSite, pointCenterClientNew - pointCenterClient );

	sptrSite->GetScrollPos( &pointScroll );

	pointScroll.x -= size.cx;
	pointScroll.y -= size.cy;
	
	sptrSite->SetScrollPos( pointScroll );	
	
	m_bPageFireScrollEvent = false;	

	InvalidatePreviewWnd();

	return true;


}

/////////////////////////////////////////////////////////////////////////////
void CZoomNavigatorPage::OnSelchangeZoom() 
{
	int nIndex = m_crtlCombo.GetCurSel();
	if( nIndex != -1 )
	{		
		CString strText;
		m_crtlCombo.GetLBText( nIndex, strText );			
		double fZoom = FormatZoomFromS2D(strText) / 100;
		SetZoomToSlider( fZoom );
		SetZoom( fZoom );				
	}				
}

/////////////////////////////////////////////////////////////////////////////
void CZoomNavigatorPage::OnEditchangeZoom() 
{	
	CString strZoom;
	m_crtlCombo.GetWindowText( strZoom );
	double fZoom = FormatZoomFromS2D(strZoom) / 100;
	SetZoomToSlider( fZoom );
	SetZoom( fZoom );	
}

/////////////////////////////////////////////////////////////////////////////
void CZoomNavigatorPage::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{	
	if( GetDlgItem(IDC_ZOOM_SLIDER) == (CWnd*)pScrollBar)
	{
		UpdateData( TRUE );
		double fZoom = ConvertFromTickToDouble((int)m_nSliderPos);
		SetZoomToCombo( fZoom );
		//SetZoomToSlider( fZoom );
		SetZoom( fZoom );
		UpdateData( FALSE );
		return;

	}
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

/*
/////////////////////////////////////////////////////////////////////////////
void CZoomNavigatorPage::DrawFrameRect(CDC * pDC, CRect &rect)
{
	CRect rcFrame = rect;

    CPen pen;
    int nOldDrawMode = pDC->SetROP2( R2_NOTXORPEN ); //XOR mode, always have to erase what's drawn.    
    
	pen.CreatePen( PS_SOLID,  2, RGB( 255, 0, 0 ) ); // 0 width = 1 device unit	

    CPen* pOldPen = (CPen*)pDC->SelectObject( &pen );
    pDC->Rectangle( &rcFrame );

	pDC->SelectObject( pOldPen );
	pDC->SetROP2( nOldDrawMode );
    
}
*/



/////////////////////////////////////////////////////////////////////////////
BOOL CZoomNavigatorPage::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if( m_bStartDrag && m_hMovePaneCur )
	{
		::SetCursor(m_hMovePaneCur);
		return TRUE;
	}
	
	return CDialog::OnSetCursor(pWnd, nHitTest, message);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CZoomNavigatorPage::OnEraseBkgnd(CDC* pDC) 
{	
	return true;
}


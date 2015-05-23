// GalleryView.cpp : implementation file
//

#include "stdafx.h"
#include "DBaseDoc.h"
#include "GalleryView.h"
#include "constant.h"
#include "types.h"
#include "dbmacros.h"
#include "obj_types.h"

#include "MenuRegistrator.h"

/////////////////////////////////////////////////////////////////////////////
// CGalleryView
IMPLEMENT_DYNCREATE(CGalleryView, CCmdTarget)

/////////////////////////////////////////////////////////////////////////////
CGalleryView::CGalleryView()
{
	EnableAutomation();
	EnableAggregation(); 

	m_sName = c_szCGalleryView;//"GalleryView";
	m_sUserName.LoadString(IDS_GALLERY_NAME);	

	m_sizeScroll	= CSize(1,1);
	m_sizeItem		= CSize(0,0);
	m_sizePicture	= CSize(0,0);	

	m_nItemCount = 0;

	m_nSelectedItem = -1;

	m_pIDocument = NULL;

	m_nFontHeight = 10;

	//Timer
	m_lCurrentTimerRecord = -1;
	m_nTimerID = -1;
	m_lCurrentTimerRecord = -1;
	m_bTimerActivate = false;	

	InitColor();

	m_pframeRuntime = 0;

	_OleLockApp( this );
	
}

/////////////////////////////////////////////////////////////////////////////
CGalleryView::~CGalleryView()
{
	_OleUnlockApp( this );
}

/////////////////////////////////////////////////////////////////////////////
void CGalleryView::OnDestroy() 
{

	StopTimer();

	//UnRegister listener
	if( m_pIDocument )
	{
		sptrIDBaseDocument spDoc(m_pIDocument);
		if( spDoc )
			spDoc->UnRegisterDBaseListener( GetControllingUnknown() );				

		m_pIDocument->Release();
	}

	CViewBase::OnDestroy();		
}


/////////////////////////////////////////////////////////////////////////////
DWORD CGalleryView::GetMatchType( const char *szType )
{
	if( !strcmp(szType, szTypeQueryObject )  )
		return mvFull;
	return mvNone;
}


BEGIN_MESSAGE_MAP(CGalleryView, CViewBase)
	//{{AFX_MSG_MAP(CGalleryView)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_ERASEBKGND()
	ON_WM_KEYDOWN()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CONTEXTMENU()
	ON_WM_MOUSEWHEEL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BEGIN_DISPATCH_MAP(CGalleryView, CViewBase)
	//{{AFX_DISPATCH_MAP(CGalleryView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IGalleryView to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {F97A3846-BF78-40FF-9E4B-FFDEC7E6A033}
static const IID IID_IGalleryView =
{ 0xf97a3846, 0xbf78, 0x40ff, { 0x9e, 0x4b, 0xff, 0xde, 0xc7, 0xe6, 0xa0, 0x33 } };

BEGIN_INTERFACE_MAP(CGalleryView, CViewBase)
	INTERFACE_PART(CGalleryView, IID_IGalleryView, Dispatch)
	INTERFACE_PART(CGalleryView, IID_IDBaseListener, DBaseListener)
	INTERFACE_PART(CGalleryView, IID_IPrintView, PrintView)
	INTERFACE_PART(CGalleryView, IID_IDBGalleryView, GalleryVw)
	INTERFACE_PART(CGalleryView, IID_IDBFilterView, FilterVw)
	INTERFACE_PART(CGalleryView, IID_IScrollZoomView, ScrollZoomView)
	INTERFACE_PART(CGalleryView, IID_IClipboardProvider, ClipboardProvider)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CGalleryView, GalleryVw);
IMPLEMENT_UNKNOWN(CGalleryView, ScrollZoomView);
IMPLEMENT_UNKNOWN(CGalleryView, ClipboardProvider);



// {60789AB3-6E3A-4569-9E81-FAAE3F8E8DDB}
GUARD_IMPLEMENT_OLECREATE(CGalleryView, szGalleryViewProgID, 
0x60789ab3, 0x6e3a, 0x4569, 0x9e, 0x81, 0xfa, 0xae, 0x3f, 0x8e, 0x8d, 0xdb);

HRESULT CGalleryView::XGalleryVw::Stub()
{
	_try_nested(CGalleryView, GalleryVw, Stub())
	{
		return S_OK;
	}
	_catch_nested;
}

/////////////////////////////////////////////////////////////////////////////
// CScrollZoomView
HRESULT CGalleryView::XScrollZoomView::SetZoom( double fZoom, BOOL* pbWasProcess, double* pReturnZoom )
{
	_try_nested(CGalleryView, ScrollZoomView, SetZoom)
	{
		*pReturnZoom = 1.0;
		*pbWasProcess = TRUE;
		return S_OK;
	}
	_catch_nested;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CGalleryView::XScrollZoomView::SetScrollPos( POINT ptPos,	BOOL* pbWasProcess, POINT* pptReturnPos )
{
	_try_nested(CGalleryView, ScrollZoomView, SetScrollPos)
	{
		*pbWasProcess = FALSE;
		return S_OK;
	}
	_catch_nested;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CGalleryView::XScrollZoomView::SetClientSize( SIZE sizeNew, BOOL* pbWasProcess, SIZE* pReturnsizeNew )
{
	_try_nested(CGalleryView, ScrollZoomView, SetClientSize)
	{
		*pbWasProcess = FALSE;
		return S_OK;
	}
	_catch_nested;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CGalleryView::XClipboardProvider::GetActiveObject( IUnknown** ppunkObject )
{
	_try_nested(CGalleryView, ClipboardProvider, GetActiveObject)
	{
		IQueryObjectPtr ptrQ( pThis->GetActiveQuery() );
		if( ptrQ == 0 )
			return S_FALSE;

		int nfield = 0;
		ptrQ->GetActiveField( &nfield );

		BSTR bstrTableName = 0;
		BSTR bstrFieldName = 0;		

		if( S_OK == ptrQ->GetField( nfield, 0, &bstrTableName, &bstrFieldName ) )
		{
			CString strTable = bstrTableName;
			CString strField = bstrFieldName;

			if( bstrTableName )
				::SysFreeString( bstrTableName );


			if( bstrFieldName  )
				::SysFreeString( bstrFieldName );

			CString strName;
			strName.Format( "%s.%s", (LPCSTR)strTable, (LPCSTR)strField );

			IDocument* pIDoc = pThis->GetDocument();
			if( pIDoc )
			{
				*ppunkObject = ::GetObjectByName( pIDoc, strName, 0 );
				pIDoc->Release();
			}
		}

		return S_OK;
	}
	_catch_nested;
}


/////////////////////////////////////////////////////////////////////////////
// CGalleryView message handlers
void CGalleryView::OnPaint() 
{

	CRect rcUpdate;
	GetUpdateRect( &rcUpdate );
	CRect rcUpdateSource = rcUpdate;
	rcUpdate = ConvertToClient( rcUpdate );		

	
	CPaintDC dc(this);	

	
	CDC dcImage;
	dcImage.CreateCompatibleDC( &dc );

	CBitmap Bitmap;
	Bitmap.CreateCompatibleBitmap( &dc, rcUpdateSource.Width(), rcUpdateSource.Height() );
	CBitmap* pOldBitmap = dcImage.SelectObject( &Bitmap );

	dcImage.SetWindowOrg( CPoint( rcUpdateSource.left, rcUpdateSource.top ) );
	
	CDC* pDC = &dcImage;


	pDC->FillRect( &rcUpdateSource, &CBrush( m_colorBk ) );


	sptrISelectQuery spQuery = GetActiveQuery();
	if( spQuery != NULL )
	{		
		BOOL bOpen = FALSE;
		spQuery->IsOpen( &bOpen );

		long nRecordCount = 0;
		spQuery->GetRecordCount( &nRecordCount );

		long nCurrentRecord = -1;
		if( bOpen )
			nCurrentRecord = GetCurrentRecord();

		if( bOpen && nRecordCount >= 1 )
		{
		
			SetSelectedItem( nCurrentRecord - 1, false );

			spQuery->EnterPrivateMode();

			int nItem = GetNextVisibleItem( rcUpdate, -1 );


			while( nItem != -1 )
			{
				long nRealRecord = 0;
				spQuery->SetCurrentRecord( nItem+1, &nRealRecord );		
				

				/*
				CRect rcClient;
				GetClientRect( &rcClient );

				CString strDebug;
				strDebug.Format( "Item:%d;Update:%d %d %d %d;Item:%d %d %d %d\n", nItem,
					rcUpdate.left, rcUpdate.top, rcUpdate.right, rcUpdate.bottom, 
					GetItemRect(nItem).left, GetItemRect(nItem).top, GetItemRect(nItem).right, GetItemRect(nItem).bottom 
					);
				TRACE( strDebug );
				*/
  

				void* pTumbnail = NULL;
				
				spQuery->GetTumbnail( &pTumbnail );
				
				BSTR bstrTumbnailCaption = 0;
				spQuery->GetTumbnailCaption( &bstrTumbnailCaption );
				CString strCaption = bstrTumbnailCaption;
				if( bstrTumbnailCaption )
					::SysFreeString( bstrTumbnailCaption ); 
				

				DrawItem( nItem, pDC, GetSelectedItem() == nItem, strCaption, pTumbnail );		

				nItem = GetNextVisibleItem( rcUpdate, nItem );
			}

			spQuery->LeavePrivateMode( TRUE, FALSE );
		}
		else
		{			
			CString strErrorMessage;
			if( !bOpen )
			{
				strErrorMessage.LoadString( IDS_WARNING_RS_CLOSE );
			}
			else
			if( nRecordCount < 1 )
			{
				strErrorMessage.LoadString( IDS_WARNING_NO_RECORD );				
			}

			CFont font;
			VERIFY( CreateFont( font ) );
			int nOldMode = pDC->SetBkMode( TRANSPARENT );
			CFont* pOldFont = (CFont*)pDC->SelectObject( &font );
			VERIFY( pDC->TextOut( 10, 10, (LPCSTR)strErrorMessage ) );
			pDC->SelectObject( pOldFont );			
			pDC->SetBkMode( nOldMode );
		}
	}

	
	dc.BitBlt( rcUpdateSource.left, rcUpdateSource.top, rcUpdateSource.Width(), rcUpdateSource.Height(),
					&dcImage, rcUpdateSource.left, rcUpdateSource.top, SRCCOPY );

	dcImage.SelectObject( pOldBitmap );
	
}

/////////////////////////////////////////////////////////////////////////////
bool CGalleryView::CreateFont( CFont& font )
{
	LOGFONT lf;
	::ZeroMemory( &lf, sizeof(LOGFONT) );

	HFONT hfont = (HFONT)GetStockObject(ANSI_VAR_FONT);

	::GetObject( hfont, sizeof(LOGFONT), &lf );

	::DeleteObject( hfont );
	

	return ( font.CreateFontIndirect( &lf ) == TRUE );
}

/////////////////////////////////////////////////////////////////////////////
int CGalleryView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CViewBase::OnCreate(lpCreateStruct) == -1)
		return -1;		

	StartTimer();

	sptrIScrollZoomSite	spScrool( GetControllingUnknown() );
	ASSERT( spScrool != NULL );

	if( spScrool )
		spScrool->SetAutoScrollMode( TRUE );		

	//CSize sizeTumbnail = CSize(50,50);
	//long nRecortdCount = 

	//Register listener	
	
	m_pIDocument = GetDocument();
	
	if( m_pIDocument )
	{
		sptrIDBaseDocument spDoc(m_pIDocument);		
		if( spDoc )
			spDoc->RegisterDBaseListener( GetControllingUnknown() );				
	}


	OnRecordsetChange();	
	
	ActivateGalleryObjectInContext();

	{
		IWindowPtr ptrWnd = GetControllingUnknown();
		DWORD dwFlags = 0;
		ptrWnd->GetWindowFlags( &dwFlags );
		ptrWnd->SetWindowFlags( ( dwFlags & ~wfZoomSupport ) | wfSupportFitDoc2Scr );
	}

	
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
bool CGalleryView::ActivateGalleryObjectInContext()
{			
	sptrISelectQuery spQuery = GetActiveQuery();
	if( spQuery == NULL )
		return false;

	BOOL m_bOpen = FALSE;
	spQuery->IsOpen( &m_bOpen );
	if( !m_bOpen )
		return false;

	BSTR bstrTable, bstrField;
	bstrTable = bstrField = 0;
	spQuery->GetTumbnailLocation( &bstrTable, &bstrField );

	::ActivateDBObjectInContext( bstrTable, bstrField, GetControllingUnknown() );	

	// A.M. fix BT5491
	IDataContext2Ptr sptrDC(GetControllingUnknown());
	sptrDC->SetObjectSelect(spQuery, 1);
	
		
	if( bstrTable )
		::SysFreeString( bstrTable );	bstrTable = 0;

	if( bstrField )		
		::SysFreeString( bstrField );	bstrField = 0;

	return true;

}


/////////////////////////////////////////////////////////////////////////////
sptrISelectQuery CGalleryView::GetActiveQuery()
{
	IDocument* pDoc = NULL;
	pDoc = GetDocument();
	if( pDoc == NULL )
		return NULL;
	
	sptrIDBaseDocument spDoc(pDoc);
	pDoc->Release();
	if( spDoc == NULL )
		return NULL;

	IUnknown* pUnkQuery = NULL;
	spDoc->GetActiveQuery( &pUnkQuery );
	if( pUnkQuery == NULL )
		return NULL;

	sptrISelectQuery spSelectQuery( pUnkQuery );
	pUnkQuery->Release();

	return spSelectQuery;

}

/////////////////////////////////////////////////////////////////////////////
void CGalleryView::OnRecordsetChange()
{

	int nOldItemCount = GetItemCount();
	CSize sizeOld = GetPictureSize();

	sptrISelectQuery spQuery = GetActiveQuery();
	if( spQuery == NULL )
		return;

	long nRecordCount = 0;
	spQuery->GetRecordCount( &nRecordCount );

	CSize sizeTumbnail;
	spQuery->GetTumbnailViewSize( &sizeTumbnail );

	bool bNeedItemCountChange	= ( nRecordCount != nOldItemCount );
	bool bNeedItemSizeChange	= ( (sizeTumbnail != sizeOld) == TRUE );
	
/*	if( !bNeedItemCountChange && !bNeedItemSizeChange )	
		return;
		*/

	//if( bNeedItemCountChange )
	SetItemCount( nRecordCount, false );

	//if( bNeedItemSizeChange )
	SetPictureSize( sizeTumbnail.cx, sizeTumbnail.cy, false );	


	CRect rcClient;
	GetClientRect( &rcClient );
	//rcClient = ConvertToWindow( rcClient );		
	RedrawWindow( &rcClient, NULL, RDW_INVALIDATE );		
}

/////////////////////////////////////////////////////////////////////////////
void CGalleryView::OnFinalRelease() 
{	
	if( GetSafeHwnd() )
		DestroyWindow();

	delete this;
}


/////////////////////////////////////////////////////////////////////////////
void CGalleryView::OnDBaseNotify( const char *pszEvent, IUnknown *punkObject, IUnknown *punkDBaseDocument, BSTR bstrTableName, BSTR bstrFieldName, const _variant_t var )
{

	if( !strcmp( pszEvent, szDBaseEventAfterDeleteRecord) ||
		!strcmp( pszEvent, szDBaseEventAfterInsertRecord) ||
		!strcmp( pszEvent, szDBaseEventGalleryOptionsChange) )
	{
		if( !::IsWindow( GetSafeHwnd() ) )
			return;
		OnRecordsetChange();	
		ScrollToCurrentRecord();
	}
	
	if( !strcmp( pszEvent, szDBaseEventAfterNavigation)   ||
		!strcmp( pszEvent, szDBaseEventNewTumbnailGenerate )
		)
	{

		if( !strcmp( pszEvent, szDBaseEventAfterNavigation) )
			ScrollToCurrentRecord();

		sptrISelectQuery spQuery = GetActiveQuery();
		if( spQuery == NULL )
			return;

		CRect rcItem;
		rcItem = GetItemRect( GetSelectedItem() );
		rcItem = ConvertToWindow( rcItem );		
		RedrawWindow( &rcItem, NULL, RDW_INVALIDATE );	


		long nRecord = GetCurrentRecord();

		CRect rcClient;
		GetClientRect( &rcClient );
		
		

		int nItem = GetNextVisibleItem( rcClient, -1 );
		bool bFound = false;

		while( !( (nItem == -1) || bFound) )
		{	
			if( nItem == (nRecord - 1) )
				bFound = true;

			nItem = GetNextVisibleItem( rcClient, nItem );
		}

		if( bFound )
		{
			CRect rcItemRect = GetItemRect( nRecord - 1 );
			rcItemRect = ConvertToWindow( rcItemRect );												
			RedrawWindow( &rcItemRect, NULL, RDW_INVALIDATE );	
		}
	}
	else
	if( !strcmp( pszEvent, szDBaseEventAfterQueryOpen ) )
	{
		OnRecordsetChange();		
		CalcScrollSize();
		ScrollToCurrentRecord();		
		Invalidate();		
		UpdateWindow();
	}	
	else
	if( !strcmp( pszEvent, szDBaseEventAfterQueryClose ) )
	{
		OnRecordsetChange();		
		CalcScrollSize();			
		Invalidate();		
		UpdateWindow();
	}		
	else
	if( !strcmp( pszEvent, szDBaseEventFieldChange ) )
	{
		if( bstrTableName != NULL && bstrFieldName != NULL )
		{
			sptrISelectQuery spQuery = GetActiveQuery();
			if( spQuery )
			{
				CString strTableNameChange, strFieldNameChange;
				strTableNameChange = bstrTableName;
				strFieldNameChange = bstrFieldName;

				BSTR bstrTableCaption, bstrFieldCaption;
				CString strTableCaption, strFieldCaption;
				spQuery->GetTumbnailCaptionLocation( &bstrTableCaption, &bstrFieldCaption );
				strTableCaption = bstrTableCaption;
				strFieldCaption = bstrFieldCaption;

				if( bstrTableCaption )
					::SysFreeString( bstrTableCaption );

				if( bstrFieldCaption )
					::SysFreeString( bstrFieldCaption );
				
				if( strTableNameChange == strTableCaption &&
					strFieldNameChange == strFieldCaption)
				{
					long nRecord = GetCurrentRecord();					
					CRect rcItemRect = GetItemRect( nRecord - 1 );
					rcItemRect = ConvertToWindow( rcItemRect );												
					RedrawWindow( &rcItemRect, NULL, RDW_INVALIDATE );						
				}
			}
		}			
	}	

}


/////////////////////////////////////////////////////////////////////////////
void CGalleryView::ScrollToCurrentRecord()
{
	sptrISelectQuery spQuery = GetActiveQuery();
	if( spQuery == NULL )
		return;

	BOOL m_bOpen = FALSE;
	spQuery->IsOpen( &m_bOpen );
	if( !m_bOpen )
		return;


	long nCurrentRecord = GetCurrentRecord();	

	CRect rcItem = GetItemRect( nCurrentRecord - 1 );	

	sptrIScrollZoomSite	sptrS( GetControllingUnknown() );
	if( sptrS )
		sptrS->EnsureVisible( rcItem );	

	rcItem = ConvertToWindow( rcItem );												
	RedrawWindow( &rcItem, NULL, RDW_INVALIDATE );	
	
	//support toolbar update in ShellFrame
	::ForceAppIdleUpdate();


	ActivateGalleryObjectInContext();	
}


/////////////////////////////////////////////////////////////////////////////
void CGalleryView::DrawItem( int nIndex, CDC* pDC, bool bSelected, CString strCaption, void* pTumbnail, bool bPrinting )
{
	if( !IsPermissibleIndex(nIndex) )
		return;

	CDibWrapper* pDIB = (CDibWrapper*)pTumbnail;	


	CRect rcItem = GetItemRect( nIndex );

	//Cashe
	//rcItem = ConvertToWindow( rcItem );
	
	CSize sizePicture = GetPictureSize();

	CSize sizeBorder = GetBorderSize();
	CSize sizeText   = GetTextSize();	

	
	CRect rcBorder = rcItem;	
	rcBorder.bottom = rcBorder.top + sizePicture.cy + 2 * sizeBorder.cy;

	CRect rcPictureSelection = rcBorder;
	rcPictureSelection.DeflateRect( 1, 1, 1, 1 );

	

	CRect rcPicture;
	rcPicture.left = rcItem.left + sizeBorder.cx;
	rcPicture.top  = rcItem.top  + sizeBorder.cy;

	rcPicture.right  = rcPicture.left + sizePicture.cx;
	rcPicture.bottom = rcPicture.top  + sizePicture.cy;




	CRect rcTextBox = rcItem;
	rcTextBox.top = rcTextBox.bottom - sizeText.cy - 2;	
	
	CRect rcText = rcTextBox;
	CRect rcBorderAroundText = rcTextBox;

	rcText.DeflateRect( 2, TEXT_TO_PICTURE_DISTANCE + 2, 2, 2 );
	rcBorderAroundText.DeflateRect( 1, TEXT_TO_PICTURE_DISTANCE + 1, 1, 1 );


	rcItem					= ConvertToWindow( rcItem );

	rcPictureSelection		= ConvertToWindow( rcPictureSelection );
	rcBorder				= ConvertToWindow( rcBorder );
	
	rcPicture				= ConvertToWindow( rcPicture );

	rcBorderAroundText		= ConvertToWindow( rcBorderAroundText );
	rcText					= ConvertToWindow( rcText );


	pDC->FillRect( rcItem, &CBrush( m_colorBk ) );	
	

	if( bSelected )
	{
		pDC->FillRect( rcPictureSelection, &CBrush( m_colorItemBkSelection ) );	
	}


	pDC->DrawEdge( &rcBorder, EDGE_RAISED, BF_RECT );



	if( !bPrinting )
	{	
		if( pDIB )
			pDIB->Draw( pDC, rcPicture, (sizeBorder.cx > 0 && sizeBorder.cy > 0) );
	}
	else
	if( GetSafeHwnd() )
	{
	
		int nOldMM = pDC->GetMapMode();
		CSize sizeOldVE = pDC->GetViewportExt();
		CSize sizeOldWE = pDC->GetWindowExt();
		CPoint ptOldVO = pDC->GetViewportOrg();
		CPoint ptOldWO = pDC->GetWindowOrg();
		
		
		CPoint ptOffset( ptOldVO.x, ptOldVO.y );
		//pDC->LPtoDP( &ptOffset );
		

		CRect rcThumbnail = rcPicture;
		pDC->LPtoDP( &rcThumbnail );

		//rcPicture += ptOffset;

		pDC->SetMapMode( MM_TEXT );
		pDC->SetWindowOrg( ptOffset );

		if( pDIB )
			pDIB->Draw( pDC, rcThumbnail, (sizeBorder.cx > 0 && sizeBorder.cy > 0) );


		pDC->SetMapMode( nOldMM );

		pDC->SetViewportExt( sizeOldVE );
		pDC->SetWindowExt( sizeOldWE );
		pDC->SetViewportOrg( ptOldVO );
		pDC->SetWindowOrg( ptOldWO );
	}

	if( bSelected )
		pDC->FillRect( rcBorderAroundText, &CBrush( m_colorTextBkSelection ) );	
	else
		pDC->FillRect( rcBorderAroundText, &CBrush( m_colorTextBk ) );	




	COLORREF colorBack;
	COLORREF colorText;

	COLORREF colorOldBack = pDC->GetBkColor();
	COLORREF colorOldText = pDC->GetTextColor();
	
	if( bSelected )
	{
		colorBack = m_colorTextBkSelection;
		colorText = m_colorTextSelection;
	}
	else	  
	{
		colorBack = m_colorTextBk;
		colorText = m_colorText;
	}

	pDC->SetTextColor( colorText );
	pDC->SetBkColor( colorBack );
	

	CFont font;
	VERIFY( CreateFont( font ) );

	CFont* pOldFont = pDC->SelectObject( &font );

	CRect rcTextCalc = NORECT;
	pDC->DrawText( strCaption, &rcTextCalc, DT_SINGLELINE | DT_CALCRECT );
	


	//pDC->DrawText( strCaption, &rcText, DT_PATH_ELLIPSIS | DT_MODIFYSTRING );
	if( rcTextCalc.Width() > rcText.Width() )		
		DrawText( pDC->GetSafeHdc(), (LPCSTR)strCaption, strCaption.GetLength(), &rcText, DT_PATH_ELLIPSIS | DT_MODIFYSTRING );
	else
		pDC->DrawText( strCaption, &rcText, DT_CENTER | DT_SINGLELINE );
	
	pDC->SelectObject( pOldFont );

	pDC->SetBkColor( colorOldBack );
	pDC->SetTextColor( colorOldText );


	pDC->DrawEdge( &rcBorderAroundText, EDGE_SUNKEN, BF_RECT );	

}


/////////////////////////////////////////////////////////////////////////////
//Scroll size
/////////////////////////////////////////////////////////////////////////////
void CGalleryView::SetScrollSize( CSize size )
{
	m_sizeScroll = size;	
	sptrIScrollZoomSite	spScrool( GetControllingUnknown() );
	ASSERT( spScrool != NULL );

	if( spScrool )
	{
		spScrool->SetClientSize( m_sizeScroll );
		

		HWND	hwndSB = 0;
		spScrool->GetScrollBarCtrl( SB_VERT, (HANDLE*)&hwndSB );
		CScrollBar	*pctrl = (CScrollBar*)CScrollBar::FromHandle( hwndSB );
		if( pctrl )
		{
			SCROLLINFO si;
			::ZeroMemory( &si, sizeof(si) );

			pctrl->GetScrollInfo( &si );

			CSize sizeItem = GetItemSize( );
			CRect rcClient;
			GetClientRect( &rcClient );

			int nPageItemScroll =  GetHorzItemCount( ) * (int)( 1 + ( (double)rcClient.Height()  / 
									(double)( ITEM_DISTANCE + sizeItem.cy ) ) );

			si.nMin	= 0;
			si.nMax	= m_sizeScroll.cy;
			
			si.fMask = SIF_TRACKPOS | SIF_PAGE;			
			si.nTrackPos = ITEM_DISTANCE + sizeItem.cy;
			si.nPage = nPageItemScroll;

			pctrl->SetScrollInfo( &si, TRUE );

			spScrool->UpdateScroll( SB_VERT );

		}
		
		
	}
}

/////////////////////////////////////////////////////////////////////////////
void CGalleryView::CalcScrollSize()
{
	sptrIScrollZoomSite	spScrool( GetControllingUnknown() );
	ASSERT( spScrool != NULL );


	sptrISelectQuery spQuery = GetActiveQuery();
	if( spQuery )
	{
		BOOL bOpen = FALSE;
		spQuery->IsOpen( &bOpen );

		long lRecCount = -1;
		spQuery->GetRecordCount( &lRecCount );

		if( !bOpen || lRecCount < 1 )
		{
			SetScrollSize( CSize(0,0) );
			return;
		}
	}
		

	CSize sizeNew;
	CSize sizeItem = GetItemSize( );
	int nCount = GetItemCount( );

	int nHorzItemCount = GetHorzItemCount();

	int nVertItemCount = nCount / nHorzItemCount;
	if( (nCount % nHorzItemCount) != 0 )
		nVertItemCount += 1;

	sizeNew.cx = nHorzItemCount * ( sizeItem.cx + ITEM_DISTANCE ) + ITEM_DISTANCE;
	sizeNew.cy = nVertItemCount * ( sizeItem.cy + ITEM_DISTANCE ) + ITEM_DISTANCE;

	SetScrollSize( sizeNew );
		

}


/////////////////////////////////////////////////////////////////////////////
//Item dimension
/////////////////////////////////////////////////////////////////////////////
void CGalleryView::SetPictureSize( int nX, int nY, bool bForceRepain )
{
	m_sizePicture = CSize( nX, nY );
	
	m_sizeItem = m_sizePicture;
	m_sizeItem.cy += GetTextSize().cy;	

	CSize sizeBorder = GetBorderSize();

	m_sizeItem.cx += 2 * sizeBorder.cx;
	m_sizeItem.cy += 2 * sizeBorder.cy;

	CalcScrollSize();

	if( bForceRepain && GetSafeHwnd() )
		Invalidate();

}

/////////////////////////////////////////////////////////////////////////////
CSize CGalleryView::GetBorderSize()
{
	CSize sizeBorder( 0, 0 );
	sptrISelectQuery spQuery = GetActiveQuery();
	if( spQuery == NULL )
		return sizeBorder;
		

	spQuery->GetTumbnailBorderSize( &sizeBorder );

	return sizeBorder;
}

/////////////////////////////////////////////////////////////////////////////
CSize CGalleryView::GetTextSize()
{
	CClientDC dc(0);

	CFont font;
	VERIFY( CreateFont( font ) );

	CFont* pOldFont = dc.SelectObject( &font );
	
	TEXTMETRIC tm;
	::ZeroMemory( &tm, sizeof(TEXTMETRIC) );
	dc.GetTextMetrics( &tm );
	dc.SelectObject( pOldFont );

	return CSize( 0, tm.tmHeight + TEXT_TO_PICTURE_DISTANCE + 2 );
}


/////////////////////////////////////////////////////////////////////////////
int CGalleryView::GetHorzItemCount( )
{
	CRect rcClient;
	GetClientRect( &rcClient );
	CSize sizeItem = GetItemSize();
	
	int nOccuppedWidth = ITEM_DISTANCE + sizeItem.cx;

	int nCount = rcClient.Width() / nOccuppedWidth;
	if( nCount <= 0 )
		nCount = 1;

	return nCount;	
}

/////////////////////////////////////////////////////////////////////////////
int CGalleryView::GetVertItemCount( )
{
	CRect rcClient;
	GetClientRect( &rcClient );
	CSize sizeItem = GetItemSize();
	
	int nOccuppedHeight = ITEM_DISTANCE + sizeItem.cy;

	return (rcClient.Height() / nOccuppedHeight + 1);

}

/////////////////////////////////////////////////////////////////////////////
void CGalleryView::SetItemCount( int nCount, bool bForceRepain )	
{
	m_nItemCount = nCount;

	//Now set scroll size
	CalcScrollSize();

	if( bForceRepain && GetSafeHwnd() )
		Invalidate();
}


/////////////////////////////////////////////////////////////////////////////
//helper functions
/////////////////////////////////////////////////////////////////////////////
int CGalleryView::GetItemNumberFromPoint( CPoint pt )
{
	CPoint ptOffset = ConvertToClient( pt );

	CSize sizeItem = GetItemSize();

	int nHorzIndex, nVertIndex;
	nHorzIndex = nVertIndex = 0;

	nHorzIndex = ptOffset.x / ( sizeItem.cx + ITEM_DISTANCE );
	nVertIndex = ptOffset.y / ( sizeItem.cy + ITEM_DISTANCE );

	int nHorzItemCount = GetHorzItemCount(  );
	int nVertItemCount = GetVertItemCount(  );

	if( nHorzIndex >= nHorzItemCount )
		return -1;


	CRect rcItem;

	rcItem.left = nHorzIndex * (sizeItem.cx + ITEM_DISTANCE) + ITEM_DISTANCE;
	rcItem.top  = nVertIndex * (sizeItem.cy + ITEM_DISTANCE) + ITEM_DISTANCE;

	rcItem.right  = rcItem.left + sizeItem.cx;
	rcItem.bottom = rcItem.top  + sizeItem.cy;

	if( !rcItem.PtInRect(ptOffset) )
		return -1;
	

	int nIndex = ConvertToLinearIndex( nHorzIndex, nVertIndex );


	if( !IsPermissibleIndex( nIndex ) )
		return -1;

	return nIndex;
}

/////////////////////////////////////////////////////////////////////////////
int CGalleryView::GetNextVisibleItem( CRect rect, int nPrevIndex )
{	
	CRect rcPane = rect;
	
	CSize sizeItem = GetItemSize();

	//Vert & horz index of first visible item
	int nHorz1, nVert1;
	

	nHorz1 = rcPane.left / (sizeItem.cx + ITEM_DISTANCE);
	nVert1 = rcPane.top  /  (sizeItem.cy + ITEM_DISTANCE);

	if( nHorz1 >= GetHorzItemCount( ) )
		nHorz1 = GetHorzItemCount( ) - 1;


	//Vert & horz index of last visible item
	int nHorz2, nVert2;

	nHorz2 = ( rcPane.right - ITEM_DISTANCE ) / (sizeItem.cx + ITEM_DISTANCE);
	nVert2 = ( rcPane.bottom - ITEM_DISTANCE ) /  (sizeItem.cy + ITEM_DISTANCE);

	if( nHorz2 >= GetHorzItemCount( ) )
		nHorz2 = GetHorzItemCount( ) - 1;

	if( nPrevIndex == -1 )
		return ConvertToLinearIndex( nHorz1, nVert1 );

	int nPrevHorz, nPrevVert;
	nPrevHorz = nPrevVert = -1;

	int nNewHorz, nNewVert;
	nNewHorz = nNewVert = -1;

	ConvertToHorzVertIndex( nPrevIndex, nPrevHorz, nPrevVert );

	if( nPrevHorz == -1 || nPrevVert == -1 ) 
		return -1;

	if( nPrevHorz < nHorz1 || nPrevHorz > nHorz2 )
		return -1;

	if( nPrevVert < nVert1 || nPrevVert > nVert2 )
		return -1;

	nNewHorz = nPrevHorz;
	nNewVert = nPrevVert;

	nNewHorz++;
	
	if( nNewHorz > nHorz2 )
	{
		nNewHorz = nHorz1;
		nNewVert++;		
	}

	if( nNewVert > nVert2 )
		return -1;

	return ConvertToLinearIndex( nNewHorz, nNewVert );
}

/////////////////////////////////////////////////////////////////////////////
int CGalleryView::ConvertToLinearIndex( int nHorzPos, int nVertPos )
{
	int nHorzItemCount = GetHorzItemCount(  );	
	int nIndex = nVertPos * nHorzItemCount + nHorzPos;
	
	if( nIndex >= GetItemCount() || nIndex < 0 )		
		return -1;

	return nIndex;
}

/////////////////////////////////////////////////////////////////////////////
void CGalleryView::ConvertToHorzVertIndex( int nLinearIndex, int& nHorzPos, int& nVertPos )
{
	int nHorzItemCount = GetHorzItemCount(  );	

	if( nLinearIndex >= GetItemCount() || nLinearIndex < 0 )		
	{
		nHorzPos = nVertPos = -1;
		return;
	}		

	nVertPos = nLinearIndex / nHorzItemCount;
	nHorzPos = nLinearIndex - nVertPos * nHorzItemCount;

}

/////////////////////////////////////////////////////////////////////////////
CRect CGalleryView::GetItemRect( int nIndex )
{
	CRect rcItem = CRect(0,0,0,0);
	/*
	if( nIndex >= GetItemCount() || nIndex < 0 )		
		return rcItem;
		*/

	int nHorzItemCount = GetHorzItemCount(  );	

	CSize sizeItem = GetItemSize();
				 //( sizeItem.cx + ITEM_DISTANCE );
	int nVertIndex = nIndex / nHorzItemCount;
	int nHorzIndex = nIndex - nVertIndex * nHorzItemCount;
	
	rcItem.left = nHorzIndex * (sizeItem.cx + ITEM_DISTANCE) + ITEM_DISTANCE;
	rcItem.top  = nVertIndex * (sizeItem.cy + ITEM_DISTANCE) + ITEM_DISTANCE;

	rcItem.right  = rcItem.left + sizeItem.cx;
	rcItem.bottom = rcItem.top  + sizeItem.cy;
	

	return rcItem;
}

/////////////////////////////////////////////////////////////////////////////
bool CGalleryView::IsPermissibleIndex( int nIndex )
{
	if( nIndex >= GetItemCount() || nIndex < 0 )
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
void CGalleryView::OnSize(UINT nType, int cx, int cy) 
{
	CViewBase::OnSize(nType, cx, cy);	
	CalcScrollSize();	
	
	ScrollToCurrentRecord();
	
	Invalidate( );	
	
}

/////////////////////////////////////////////////////////////////////////////
void CGalleryView::SetSelectedItem( int nSelectedItem, bool bForceRepaint )
{
	int nOldSelectedItem = m_nSelectedItem;

	CRect rcUpdate = NORECT;

	CRect rcItem = GetItemRect(m_nSelectedItem);

	if( bForceRepaint )
	{
		rcUpdate = rcItem;
		rcUpdate = ConvertToWindow( rcUpdate );

		RedrawWindow( &rcUpdate, NULL, RDW_INVALIDATE );		
	}
		

	if( !IsPermissibleIndex( nSelectedItem ) )
	{
		m_nSelectedItem = -1;
	}
	else
		m_nSelectedItem = nSelectedItem;


	rcItem = GetItemRect(m_nSelectedItem);

	if( bForceRepaint )		
	{
		rcUpdate = rcItem;
		rcUpdate = ConvertToWindow( rcUpdate );

		RedrawWindow( &rcUpdate, NULL, RDW_INVALIDATE );		

	}	


}

/////////////////////////////////////////////////////////////////////////////
int CGalleryView::GetSelectedItem()
{
	if( IsPermissibleIndex(m_nSelectedItem) )
		return m_nSelectedItem;

	return -1;
}


#define VK_PAGE_DOWN	34
#define VK_PAGE_UP		33



/////////////////////////////////////////////////////////////////////////////
void CGalleryView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{	
	CViewBase::OnKeyDown(nChar, nRepCnt, nFlags);


	if( !( nChar == VK_LEFT || nChar == VK_RIGHT || nChar == VK_UP || nChar == VK_DOWN || 
		 nChar == VK_PAGE_DOWN || nChar == VK_PAGE_UP ) )
		return;

	sptrISelectQuery spQuery = GetActiveQuery();
	if( spQuery == NULL )
		return;

	BOOL bOpen = FALSE;
	spQuery->IsOpen( &bOpen );
	if( !bOpen )
		return;

	long lRecordCount, lCurrentRecord;
	lRecordCount = lCurrentRecord = 0;

	lCurrentRecord = GetCurrentRecord();
	
	spQuery->GetRecordCount( &lRecordCount );

	if( lRecordCount < 1 )
		return;

	if( lCurrentRecord < 1 || lCurrentRecord > lRecordCount )
		return;

	bool bProcessNavigate = false;

	long lDelta = 0;

	int nHorzItemCount = GetHorzItemCount( );

	CSize sizeItem = GetItemSize( );
	CRect rcClient;
	GetClientRect( &rcClient );

	int nPageItemScroll =  nHorzItemCount  * (int)( 1 + ( (double)rcClient.Height()  / 
							(double)( ITEM_DISTANCE + sizeItem.cy ) ) );

	if( nChar == VK_LEFT )
		lDelta = -1;
	else
	if( nChar == VK_RIGHT )
		lDelta = 1;
	else
	if( nChar == VK_UP )
		lDelta = -nHorzItemCount;
	else
	if( nChar == VK_DOWN )
		lDelta = +nHorzItemCount;
	else
	if( nChar == VK_PAGE_DOWN )
		lDelta = +nPageItemScroll;
	else
	if( nChar == VK_PAGE_UP )
		lDelta = -nPageItemScroll;


	long nNewRecord = lCurrentRecord + lDelta;

	if( nNewRecord < 1 )
		nNewRecord = 1;

	if( nNewRecord > lRecordCount )
		nNewRecord = lRecordCount;


	if( nNewRecord < 1 || nNewRecord > lRecordCount )
		return;

	GotoRecordSlow( nNewRecord );
	SetSelectedItem( nNewRecord - 1 , true );

	ScrollToCurrentRecord();

}

/////////////////////////////////////////////////////////////////////////////
void CGalleryView::StartTimer()
{
	m_nTimerID = 1;
	m_nTimerID = SetTimer( m_nTimerID, 100, NULL );

}

/////////////////////////////////////////////////////////////////////////////
void CGalleryView::StopTimer()
{
	if( m_nTimerID != -1)
		KillTimer( m_nTimerID );

	m_nTimerID = -1;

}

/////////////////////////////////////////////////////////////////////////////
void CGalleryView::OnTimer(UINT_PTR nIDEvent) 
{
	if( nIDEvent == m_nTimerID && m_bTimerActivate )
	{
		long nCurTickCount = ::GetTickCount();

		long lNavigatePeriod = ::_GetValueInt( ::GetAppUnknown(), 
								SHELL_DATA_DB_SECTION, SHELL_DATA_DB_NAVIGATE_PERIOD, 500 );

		if( nCurTickCount - m_lRecordStartTickCount >= lNavigatePeriod )
		{	
			m_bTimerActivate = false;
			ForceGoToRecord( );			
			m_lCurrentTimerRecord = -1;
		}

	}

	CViewBase::OnTimer(nIDEvent);
}

/////////////////////////////////////////////////////////////////////////////
bool CGalleryView::ForceGoToRecord( )
{
	/*
	if( !m_bTimerActivate )
		return false;
		*/

	sptrISelectQuery spQuery = GetActiveQuery();
	if( spQuery == NULL )
		return false;

	long lRecordCount, lCurrentRecord;
	lRecordCount = lCurrentRecord = 0;

	spQuery->GetCurrentRecord( &lCurrentRecord );
	spQuery->GetRecordCount( &lRecordCount );

	if( lRecordCount >= 1 &&
		m_lCurrentTimerRecord >= 1 && m_lCurrentTimerRecord <= m_lCurrentTimerRecord &&
		m_lCurrentTimerRecord != lCurrentRecord
		)
	{
		spQuery->GoToRecord( m_lCurrentTimerRecord );
		return true;
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////
bool CGalleryView::GotoRecordSlow( long lNewCurrentRecord )
{
	sptrISelectQuery spQuery = GetActiveQuery();
	if( spQuery )
		spQuery->UpdateInteractive( TRUE );

	m_lCurrentTimerRecord = lNewCurrentRecord;
	m_lRecordStartTickCount = ::GetTickCount();
	m_bTimerActivate = true;

	return true;		
}

void CGalleryView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CViewBase::OnLButtonDblClk(nFlags, point);

	sptrISelectQuery spQuery = GetActiveQuery();
	if( spQuery == NULL )
		return;

	BOOL bOpen = FALSE;
	spQuery->IsOpen( &bOpen );
	if( !bOpen )
		return;

	if( m_bTimerActivate )
		ForceGoToRecord( );

	int nSelectedItem = GetItemNumberFromPoint( point );
	if( nSelectedItem != -1 )
	{
		GotoRecordSlow( nSelectedItem + 1 );

		if( m_bTimerActivate )
			ForceGoToRecord( );

		BSTR bstrTable, bstrField;
		bstrTable = bstrField = 0;
		spQuery->GetTumbnailLocation( &bstrTable, &bstrField );

		CString strObjName = ::GenerateFieldPath( CString(bstrTable), CString(bstrField), false );
		IUnknown* punkObj = NULL;
		punkObj = ::FindObjectByName( m_pIDocument, strObjName );
		if( punkObj == NULL )
			return;

		ChangeViewType( punkObj );
		punkObj->Release();

	}
	
}

/////////////////////////////////////////////////////////////////////////////
void CGalleryView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CViewBase::OnLButtonDown(nFlags, point);

	sptrISelectQuery spQuery = GetActiveQuery();
	if( spQuery == NULL )
		return;

	BOOL bOpen = FALSE;
	spQuery->IsOpen( &bOpen );
	if( !bOpen )
		return;

	int nSelectedItem = GetItemNumberFromPoint( point );
	if( nSelectedItem != -1 )
	{
		GotoRecordSlow( nSelectedItem + 1 );
		SetSelectedItem( nSelectedItem , true );		
	}
	
	
}

/////////////////////////////////////////////////////////////////////////////
BOOL CGalleryView::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
long CGalleryView::GetRecordCount()
{
	sptrISelectQuery spQuery = GetActiveQuery();
	if( spQuery == NULL )
		return -1;

	BOOL m_bOpen = FALSE;
	spQuery->IsOpen( &m_bOpen );
	if( !m_bOpen )
		return -1;


	long nRecordCount = 0;
	spQuery->GetRecordCount( &nRecordCount );

	
	if( nRecordCount < 1 ) 
		return -1;

	return nRecordCount;
}

/////////////////////////////////////////////////////////////////////////////
//
//	print support
//
/////////////////////////////////////////////////////////////////////////////
/*
	long nWidth = _GetMaxWidth();
	bool bContinue = false;
	if(nWidth > nUserPosX + nMaxWidth)
	{
		nReturnWidth = nMaxWidth;
		nNewUserPosX +=  nReturnWidth;
		bContinue = true;
	}
	else
	{
		nReturnWidth = nWidth - nUserPosX;
		nNewUserPosX = nWidth;
	}
	return bContinue;

*/

/////////////////////////////////////////////////////////////////////////////
int CGalleryView::GetRowCount()
{
	int nHorzItemCount = GetHorzItemCount( );
	int nRecordCount = GetRecordCount();
	if( nHorzItemCount < 1 || nRecordCount < 1 )
		return 0;

	if( (nRecordCount % nHorzItemCount) == 0 )
		return (nRecordCount / nHorzItemCount);

	return nRecordCount / nHorzItemCount + 1;

}

/////////////////////////////////////////////////////////////////////////////
int CGalleryView::GetColumnCount()
{
	return GetHorzItemCount( );
}

//Print calculation
/////////////////////////////////////////////////////////////////////////////
/*
int CGalleryView::GetColumnToPrint( int nColumn, int& nReturnWidth )
{
	CSize sizeItem = GetItemSize();
	int nItemCount = GetItemCount();

	int nItemWidth		= sizeItem.cx;
	int nItemHorzCount	= nReturnWidth / nItemWidth;

	int nItemPerWidth	= nReturnWidth / nItemWidth;

	if( nColumn + nItemPerWidth > nItemHorzCount )
		nItemPerWidth = nItemHorzCount - nColumn;

	nReturnWidth		= nItemWidth * nItemHorzCount;

	return nItemPerWidth;
}

/////////////////////////////////////////////////////////////////////////////
int CGalleryView::GetRowToPrint( int nRow, int& nReturnHeight )
{
	CSize sizeItem = GetItemSize();

	int nItemHeight		= sizeItem.cy;
	int nItemVertCount	= GetRowCount();

	int nItemPerHeight	= nReturnHeight / nItemHeight;
	if( nRow + nItemPerHeight > nItemVertCount )
		nItemPerHeight = nItemVertCount - nRow;

	nReturnHeight		= nItemHeight * nItemPerHeight;

	return nItemPerHeight;
}
*/
/////////////////////////////////////////////////////////////////////////////



bool CGalleryView::GetPrintWidth( int nMaxWidth, int& nReturnWidth, int nUserPosX, int& nNewUserPosX )
{	
	nReturnWidth = nMaxWidth;
	nNewUserPosX = nUserPosX;
	bool bContinue = true;

	int nItemCount = GetItemCount();

	CSize sizeItem = GetItemSize();
	int nItemWidth = sizeItem.cx;
	
	
	int nItemHorzCount	= GetColumnCount();
	int nItemPerWidth	= nMaxWidth / nItemWidth;

	if( nItemPerWidth > nItemHorzCount )
		nItemPerWidth = nItemHorzCount;

	if( nItemPerWidth < 1 )
		nItemPerWidth = 1;

	nNewUserPosX = nUserPosX + nItemPerWidth;

	if( nNewUserPosX >= (nItemHorzCount - 1) )
	{
		nNewUserPosX = nItemHorzCount;
		bContinue = false;
	}

	nReturnWidth = ( nNewUserPosX - nUserPosX ) * nItemWidth;

	return bContinue;
}

/////////////////////////////////////////////////////////////////////////////
bool CGalleryView::GetPrintHeight( int nMaxHeight, int& nReturnHeight, int nUserPosY, int& nNewUserPosY )
{	
		
	nReturnHeight = nMaxHeight;
	nNewUserPosY = nUserPosY;
	bool bContinue = true;

	int nItemCount = GetItemCount();

	CSize sizeItem = GetItemSize();
	int nItemHeight = sizeItem.cy;

	int nItemVertCount	= GetRowCount();
	int nItemPerHeight  = nMaxHeight / nItemHeight;

	nNewUserPosY = nUserPosY + nItemPerHeight;

	if( nNewUserPosY >= (nItemVertCount - 1) )
	{
		nNewUserPosY = nItemVertCount;
		bContinue = false;
	}

	nReturnHeight = ( nNewUserPosY - nUserPosY ) * nItemHeight;
	
	return bContinue;	
}

/////////////////////////////////////////////////////////////////////////////
void CGalleryView::Print( HDC hdc, CRect rectTarget, int nUserPosX, int nUserPosY, int nUserPosDX, int nUserPosDY, bool bUseScrollZoom, DWORD dwFlags )
{	
	
	sptrISelectQuery spQuery = GetActiveQuery();
	if( spQuery == NULL )
		return;

	BOOL bOpen = FALSE;
	spQuery->IsOpen( &bOpen );
	if( !bOpen )
		return;

	CDC* pdc = CDC::FromHandle(hdc);	
	if( pdc == NULL)
		return;

	
	int nTopLeftIndex = ConvertToLinearIndex( nUserPosX, nUserPosY );
	int nRighBottomIndex = ConvertToLinearIndex(    nUserPosX + nUserPosDX - 1, 
													nUserPosY + nUserPosDY - 1 );

	if( nTopLeftIndex == -1 )
		ASSERT( FALSE );

	if( nRighBottomIndex == -1 )
	{
		nRighBottomIndex = (nUserPosY + nUserPosDY) * GetHorzItemCount()  - 1;
	}



	CRect rcTopLeft		= GetItemRect( nTopLeftIndex );
	CRect rcRighBottom	= GetItemRect( nRighBottomIndex );

	rcTopLeft.left -= ITEM_DISTANCE;
	rcTopLeft.top  -= ITEM_DISTANCE;
	
	CRect rcUpdate = CRect( rcTopLeft.left, rcTopLeft.top,
							rcRighBottom.right, rcRighBottom.bottom
							);


	int nOldMM = pdc->GetMapMode();
	CSize sizeOldVE = pdc->GetViewportExt();
	CSize sizeOldWE = pdc->GetWindowExt();
	CPoint ptOldVO = pdc->GetViewportOrg();

	double fViewZoom = 1.0;
	//fViewZoom = ::GetZoom( GetControllingUnknown( ) );
	//In millimeters
	CSize sizeWindowExt = CSize( rcUpdate.Width() * fViewZoom, rcUpdate.Height() * fViewZoom );
	//In device unit
	CSize sizeViewPortExt = CSize( rectTarget.Width(), rectTarget.Height() );	
	CPoint ptViewPortOrg;
	

	double fZoomX = (double)rectTarget.Width()  / (double)( rcUpdate.Width()  * fViewZoom );
	double fZoomY = (double)rectTarget.Height() / (double)( rcUpdate.Height() * fViewZoom );


	CRect rcFirstItem = GetItemRect( nTopLeftIndex );	

	ptViewPortOrg.x = rectTarget.left - rcUpdate.left * fZoomX;
	ptViewPortOrg.y = rectTarget.top  - rcUpdate.top * fZoomY;


	pdc->SetMapMode( MM_ANISOTROPIC );


	pdc->SetWindowExt( sizeWindowExt );					
	pdc->SetViewportExt( sizeViewPortExt );
	pdc->SetViewportOrg( ptViewPortOrg );

	CPoint ptTest( rcRighBottom.right, rcRighBottom.bottom );
	pdc->LPtoDP( &ptTest );


	spQuery->EnterPrivateMode();

	int nItem = GetNextVisibleItem( rcUpdate, -1 );

	while( nItem != -1 )
	{		
		long nRealRecord = 0;
		spQuery->SetCurrentRecord( nItem+1, &nRealRecord );		
		
		void* pTumbnail = NULL;
		
		spQuery->GetTumbnail( &pTumbnail );
		
		BSTR bstrTumbnailCaption;
		spQuery->GetTumbnailCaption( &bstrTumbnailCaption );
		CString strCaption = bstrTumbnailCaption;

		if( bstrTumbnailCaption )
			::SysFreeString( bstrTumbnailCaption ); 

		DrawItem( nItem, pdc, 0, strCaption, pTumbnail, true );
		
		nItem = GetNextVisibleItem( rcUpdate, nItem );
	}

	spQuery->LeavePrivateMode( TRUE, FALSE );

	pdc->SetMapMode( nOldMM );
	pdc->SetViewportExt( sizeOldVE );
	pdc->SetWindowExt( sizeOldWE );
	pdc->SetViewportOrg( ptOldVO );

	/*
	CBrush brush;
	brush.CreateSolidBrush( RGB(255,0,0) );
	pdc->FillRect( &rectTarget, &brush );	
	*/
}



/////////////////////////////////////////////////////////////////////////////
void CGalleryView::InitColor()
{

	m_colorBk				 = /*::GetValueColor( GetAppUnknown(), SHELL_DATA_DB_GALLERY,
								SHELL_DATA_GALLERY_BACK_COLOR, */
								::GetSysColor( COLOR_BTNFACE );// );
	


	m_colorItemBk			 = /*::GetValueColor( GetAppUnknown(), SHELL_DATA_DB_GALLERY,
									SHELL_DATA_GALLERY_ITEM_BACK_COLOR, */
									::GetSysColor( COLOR_BTNFACE );// );

	m_colorText				 = /*::GetValueColor( GetAppUnknown(), SHELL_DATA_DB_GALLERY,
									SHELL_DATA_GALLERY_TEXT_COLOR, */
									::GetSysColor( COLOR_WINDOWTEXT );// );

	m_colorTextBk			 = /*::GetValueColor( GetAppUnknown(), SHELL_DATA_DB_GALLERY,
									SHELL_DATA_GALLERY_TEXT_BACK_COLOR, */
									::GetSysColor( COLOR_BTNFACE );// );



	m_colorItemBkSelection	 = /*::GetValueColor( GetAppUnknown(), SHELL_DATA_DB_GALLERY,
									SHELL_DATA_GALLERY_SELECTION_ITEM_BACK_COLOR, */
									::GetSysColor( COLOR_HIGHLIGHT );// );

	m_colorTextSelection	 = /*::GetValueColor( GetAppUnknown(), SHELL_DATA_DB_GALLERY,
									SHELL_DATA_GALLERY_SELECTION_TEXT_COLOR, */
									::GetSysColor( COLOR_HIGHLIGHTTEXT );// );

	m_colorTextBkSelection	 = /*::GetValueColor( GetAppUnknown(), SHELL_DATA_DB_GALLERY,
									SHELL_DATA_GALLERY_SELECTION_TEXT_BACK_COLOR, */
									::GetSysColor( COLOR_HIGHLIGHT );// );
}

/////////////////////////////////////////////////////////////////////////////
CPoint CGalleryView::GetScrollPos()
{
	CPoint pt = ::GetScrollPos( GetControllingUnknown() );
	//double fZoom = GetZoom( GetControllingUnknown() );
	//return CPoint( (double)pt.x / fZoom, (double)pt.y / fZoom );	
	return pt;
}

/////////////////////////////////////////////////////////////////////////////
CRect CGalleryView::ConvertToWindow( CRect rc )
{
	double fZoom = 1.0;//GetZoom( punkSSite );
	CPoint ptScroll = GetScrollPos( );	

	rc.left = _AjustValue( fZoom, rc.left*fZoom-ptScroll.x );
	rc.right = _AjustValue( fZoom, rc.right*fZoom-ptScroll.x );
	rc.top = _AjustValue( fZoom, rc.top*fZoom-ptScroll.y );
	rc.bottom = _AjustValue( fZoom, rc.bottom*fZoom-ptScroll.y );

	return rc;
	
}

/////////////////////////////////////////////////////////////////////////////
CRect CGalleryView::ConvertToClient( CRect rc )
{
	double fZoom = 1.0;//GetZoom( punkSSite );
	CPoint ptScroll = GetScrollPos(  );	

	rc.left = int( (rc.left+ptScroll.x) / fZoom );//_AjustValueEx( 1./fZoom, (rc.left+ptScroll.x) / fZoom );
	rc.right = int( (rc.right+ptScroll.x) / fZoom );
	rc.top = int( (rc.top+ptScroll.y) / fZoom );
	rc.bottom = int( (rc.bottom+ptScroll.y) / fZoom );

	return rc;	
	
}

/////////////////////////////////////////////////////////////////////////////
CPoint CGalleryView::ConvertToClient( CPoint pt )
{
	double fZoom = 1.0;//GetZoom( punkSSite );
	CPoint ptScroll = GetScrollPos( );
	

	pt.x = int( (pt.x+ptScroll.x) / fZoom );
	pt.y = int( (pt.y+ptScroll.y) / fZoom );

	return pt;
	
}
POSITION CGalleryView::GetFisrtVisibleObjectPosition()
{
	return (POSITION)1;
}

IUnknown *CGalleryView::GetNextVisibleObject( POSITION &rPos )
{
	sptrISelectQuery spSelectQuery( GetActiveQuery() );
	if( spSelectQuery == NULL )
	{
		rPos = 0;
		return 0;
	}

	if( (int )rPos == 1 )
	{
		rPos = 0;
		spSelectQuery.AddRef();
		return spSelectQuery;
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
//
//Filter Support
//
/////////////////////////////////////////////////////////////////////////////

bool CGalleryView::GetActiveFieldValue( CString& strTable, CString& strField, CString& strValue )
{
	if( !GetActiveField( strTable, strField ) )
		return false;

	CString strKey;
	strKey.Format( "%s\\%s.%s",
						(LPCTSTR)SECTION_DBASEFIELDS, 
						(LPCTSTR)strTable, (LPCTSTR)strField );						

	sptrINamedData spND( m_pIDocument );
	if( spND == NULL ) 
		return false;

	_variant_t var;
	spND->GetValue( _bstr_t((LPCSTR)strKey), &var );
	if( !(var.vt == VT_R8 || var.vt == VT_BSTR || var.vt == VT_DATE ) )
		return false;

	var.ChangeType( VT_BSTR );
	strValue = var.bstrVal;
	return true;

}

/////////////////////////////////////////////////////////////////////////////
bool CGalleryView::GetActiveField( CString& strTable, CString& strField )
{	

	sptrISelectQuery spSelectQuery( GetActiveQuery() );

	if( spSelectQuery == NULL )
		return false;
	
	
	BSTR bstrTable, bstrField;
	bstrTable = bstrField = NULL;

	spSelectQuery->GetTumbnailCaptionLocation( &bstrTable, &bstrField );

	strTable = bstrTable;
	strField = bstrField;

	
	if( bstrTable )
		::SysFreeString( bstrTable );

	if( bstrField )
		::SysFreeString( bstrField );
		
	
	if( !IsAvailableFieldInQuery( spSelectQuery, strTable, strField ) )
		return false;

	{

		BOOL bOpen = FALSE;
		spSelectQuery->IsOpen( &bOpen );
		if( !bOpen )
			return false;

		long nRecordCount = -1;
		spSelectQuery->GetRecordCount( &nRecordCount );
		if( nRecordCount < 1 )
			return false;

	}

	return true;
}


/////////////////////////////////////////////////////////////////////////////
void CGalleryView::OnActivateView( bool bActivate, IUnknown *punkOtherView )
{
	ShowFilterPropPage( bActivate );

	/*
	if( bActivate )
		CreateRecordPaneInfo();
	else
		DestroyRecordPaneInfo();	
		*/


}

/////////////////////////////////////////////////////////////////////////////
bool CGalleryView::ShowFilterPropPage( bool bShow )
{
	ShowDBPage( bShow );
	return true;
}

/////////////////////////////////////////////////////////////////////////////
long CGalleryView::GetCurrentRecord()
{
	sptrISelectQuery spQuery( GetActiveQuery() );
	if( spQuery == NULL )
		return -1;

	long nCurRecord = -1;

	if( m_bTimerActivate )
	{
		nCurRecord = m_lCurrentTimerRecord;
	}
	else
	{
		spQuery->GetCurrentRecord( &nCurRecord );
	}

	return nCurRecord;

}


void CGalleryView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CPoint	pointClient = point;
	ScreenToClient( &pointClient );
	OnLButtonDown(0, pointClient );
	
	IUnknown *punk = _GetOtherComponent( GetAppUnknown(), IID_ICommandManager );

	if( !punk )
		return;

	sptrICommandManager2 sptrCM = punk;
	punk->Release();

	if( sptrCM == NULL )
		return;
	
	CMenuRegistrator rcm;	

	_bstr_t	bstrMenuName = rcm.GetMenu( szDBGalleryMenu, 0 );
	sptrCM->ExecuteContextMenu2( bstrMenuName, point, 0 );	
	
}


CString CGalleryView::FindMatchViewProgID( IUnknown* punkObj )
{
	CString strObjType = ::GetObjectKind( punkObj );
	CStringArray arViewList;
	GetAvailableViewProgID( arViewList );

	for( int i=0;i<arViewList.GetSize();i++ )
	{
		DWORD dw_match = GetViewMatchType( arViewList[i], strObjType );
		if( dw_match & mvFull )
			return arViewList[i];
	}

	return "";
}


bool CGalleryView::ChangeViewType( IUnknown* punkObj )
{
	CString strProgID = FindMatchViewProgID( punkObj );
	if( strProgID.IsEmpty() )
		return false;


	IDocument* pIDoc = GetDocument();
	if( !pIDoc )
		return false;

	IDocumentSitePtr ptrDS( pIDoc );
	pIDoc->Release();	pIDoc = 0;

	if( ptrDS == 0 )	return false;

	IUnknown* punk = 0;
	ptrDS->GetActiveView( &punk );
	if( !punk )			return false;


	IViewSitePtr ptrVS( punk );
	punk->Release();	punk = 0;


	ptrVS->GetFrameWindow( &punk );
	if( !punk )			return false;

	sptrIFrameWindow	sptrF( punk );
	punk->Release();	punk = 0;


	sptrF->GetSplitter( &punk );
	if( !punk )			return false;

	sptrISplitterWindow	sptrS( punk );
	punk->Release();	punk = 0;	
		
	int	nRow, nCol;
	nRow = nCol = 0;

	sptrS->GetActivePane( &nRow, &nCol );
	sptrS->ChangeViewType( nRow, nCol, _bstr_t( (LPCSTR)strProgID ) );

	sptrS->GetViewRowCol( nRow, nCol, &punk );
	if( !punk )			return false;

	IDataContext2Ptr	ptrContext( punk );
	punk->Release();	punk = 0;

	if( ptrContext == 0 )
		return false;

	ptrContext->SetObjectSelect( punkObj, TRUE );

	return true;
}

BOOL CGalleryView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	IScrollZoomSitePtr ptr_szs( GetControllingUnknown() );
	if( ptr_szs )
	{
		CSize size_client = CSize( 0, 0 );
		ptr_szs->GetClientSize( &size_client );
		CPoint pt_scroll = CPoint( 0, 0 );
		ptr_szs->GetScrollPos( &pt_scroll );

		CSize size_thumbnail = GetItemSize();

		size_thumbnail.cy += ITEM_DISTANCE;

		if( zDelta > 0 )
			size_thumbnail.cy *= -1;

		if( pt_scroll.y + size_thumbnail.cy < size_client.cy )
		{
			pt_scroll.y += size_thumbnail.cy;
			ptr_szs->SetScrollPos( pt_scroll );
		}		
	}

	return CViewBase::OnMouseWheel( nFlags, zDelta, pt );
}
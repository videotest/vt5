// AviGallery.cpp: implementation of the CAviGallery class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AviGallery.h"
#include "AviBase.h"
#include "misc_utils.h"
#include "misc_templ.h"
#include "avibase.h"
#include "resource.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CAviGallery::CAviGallery()
{
	m_bstrName		= "Avi View";

	char sz_buf[256];
	sz_buf[0] = 0;

	::LoadString( App::handle(), IDS_AVI_GALLERY_VIEW_NAME, sz_buf, sizeof(sz_buf) );
	
	m_bstrUserName	= sz_buf;

	if( !m_bstrUserName.length() )
		m_bstrUserName = m_bstrName;


	m_pDibBits = 0;
	m_lBufSize = 0;

	m_lPrevKeyFrame	= m_lPrevFrame = -1;

	m_hDrawDib		= 0;

	ReadSettings();
}

//////////////////////////////////////////////////////////////////////
void CAviGallery::ReadSettings()
{
	m_clrBk				= ::GetSysColor( COLOR_BTNFACE );
	m_clrItem			= ::GetSysColor( COLOR_BTNFACE );
	m_clrSelectItem		= ::GetSysColor( COLOR_HIGHLIGHT );

	m_size_thumbnail.cx	= ::GetValueInt( ::GetAppUnknown(), szAviSection, szThumbnailWidth, 100 );
	m_size_thumbnail.cy	= ::GetValueInt( ::GetAppUnknown(), szAviSection, szThumbnailHeight, 100 );

	if( m_size_thumbnail.cx < 10 )
		m_size_thumbnail.cx = 10;

	if( m_size_thumbnail.cy < 10 )
		m_size_thumbnail.cy = 10;

	m_lframe_step		= ::GetValueInt( ::GetAppUnknown(), szAviSection, szGalleryFrameStep, 1 );
	if( m_lframe_step < 1 )
		m_lframe_step = 1;
}

//////////////////////////////////////////////////////////////////////
long CAviGallery::GetNearestFrame( long lframe )
{
	if( m_ptrAvi == 0 )
		return lframe;

	long lreal_frame = lframe;
	if( m_lframe_step != 1 )
	{
		IAviDibImagePtr ptr_dim_img( m_ptrAvi );
		if( ptr_dim_img )
		{
			ptr_dim_img->GetNearestKeyFrame( lframe * m_lframe_step, &lreal_frame );
		}
	}

	return lreal_frame;
}

//////////////////////////////////////////////////////////////////////
RECT CAviGallery::GetItemRect( long lindex )
{
	RECT rcItem = {0};
	long lCol = -1;
	long lRow = -1;
	
	ConvertToHorzVertIndex( lindex, lCol, lRow );

	if( lCol < 0 || lRow < 0 )
		return rcItem;

	SIZE sizeThumbnail = GetThumbnailSize();

	rcItem.left = lCol * ( sizeThumbnail.cx + ITEM_DISTANCE ) + ITEM_DISTANCE;
	rcItem.top  = lRow * ( sizeThumbnail.cy + ITEM_DISTANCE ) + ITEM_DISTANCE;

	rcItem.right  = rcItem.left + sizeThumbnail.cx;
	rcItem.bottom = rcItem.top  + sizeThumbnail.cy;

	return rcItem;
}

//////////////////////////////////////////////////////////////////////
CAviGallery::~CAviGallery()
{
	if( m_pDibBits )
	{
		delete [] m_pDibBits;
		m_pDibBits = 0;
		
		m_lBufSize = 0;
	}

}

//////////////////////////////////////////////////////////////////////
LRESULT	CAviGallery::DoMessage( UINT nMsg, WPARAM wParam, LPARAM lParam )
{
	switch( nMsg )
	{
		case WM_PAINT:			return OnPaint();				
		case WM_ERASEBKGND:		return OnEraseBackground( (HDC)wParam );
		case WM_SIZE:			return OnSize( (int)LOWORD(lParam), (int)HIWORD(lParam) );
		case WM_DESTROY:		return OnDestroy();
		case WM_LBUTTONDOWN:	return OnLButtonDown( wParam, (int)LOWORD(lParam), (int)HIWORD(lParam) );
		case WM_LBUTTONDBLCLK:	return OnLButtonDblClick( (int)LOWORD(lParam), (int)HIWORD(lParam) );		
	}

	return CWinViewBase::DoMessage( nMsg, wParam, lParam );
}


//////////////////////////////////////////////////////////////////////
SIZE CAviGallery::GetThumbnailSize()
{
	return m_size_thumbnail;
}


//////////////////////////////////////////////////////////////////////
long	CAviGallery::GetColCount()
{
	SIZE sizeThumbnail = GetThumbnailSize();
	RECT rc;
	::GetClientRect( m_hwnd, &rc );

	if( sizeThumbnail.cx < 1 )
		return 0;
	
	long lCount = ( rc.right - rc.left ) / ( sizeThumbnail.cx + ITEM_DISTANCE );
	if( lCount < 1 )
		lCount = 1;

	return lCount;

}

//////////////////////////////////////////////////////////////////////
long CAviGallery::GetRowCount()
{
	long lCols			= GetColCount();
	long lFrameCount	= GetEndFrame() - GetStartFrame();

	if( lCols < 1 ) 
		return 0;

	long lRow = lFrameCount / lCols;
	lRow /= m_lframe_step;

	if( lFrameCount % lCols != 0 )
		lRow++;

	if( lRow < 1 ) 
		lRow = 1;

	return lRow; 
}

//////////////////////////////////////////////////////////////////////
void CAviGallery::BuildView()
{
	if( m_ptrAvi == 0 )
		return;

	ReadSettings();

	SIZE sizeThumbnail = GetThumbnailSize();

	long lCols			= GetColCount();
	long lRows			= GetRowCount();

	SIZE sizeClient;

	sizeClient.cx = lCols * ( ITEM_DISTANCE + sizeThumbnail.cx ) + ITEM_DISTANCE;
	sizeClient.cy = lRows * ( ITEM_DISTANCE + sizeThumbnail.cy ) + ITEM_DISTANCE;

	SetClientSize( sizeClient );

	InvalidateRect( m_hwnd, 0, 0 );
	UpdateWindow( m_hwnd );
}



//////////////////////////////////////////////////////////////////////
long CAviGallery::GetStartFrame()
{
	return 0;
}

//////////////////////////////////////////////////////////////////////
long CAviGallery::GetEndFrame()
{
	if( m_ptrAvi == 0 )
		return -1;

	_variant_t var_count, var_cur;		
	if( S_OK == m_ptrAvi->get_TotalFrames( &var_count ) )
		return (long)var_count;
	
	return -1;
}

//////////////////////////////////////////////////////////////////////
long CAviGallery::GetActiveFrame()
{
	if( m_ptrAvi == 0 )
		return -1;

	_variant_t var_cur;
	if( S_OK == m_ptrAvi->get_CurFrame( &var_cur ) )
		return (long)var_cur;
	
	return -1;
}



//////////////////////////////////////////////////////////////////////
LRESULT CAviGallery::OnPaint()
{
	PAINTSTRUCT	ps;	

	RECT	rectPaint;
	::GetUpdateRect( m_hwnd, &rectPaint, false );
	
	if( rectPaint.right - rectPaint.left <= 0 )
		return 1;

	if( rectPaint.bottom - rectPaint.top <= 0 )
		return 1;

	HDC hdcPaint = ::BeginPaint( m_hwnd, &ps );	

	ProcessDrawing( hdcPaint, rectPaint );

 	::EndPaint( m_hwnd, &ps );
	

	return 1;

}



//////////////////////////////////////////////////////////////////////
void CAviGallery::ReAttachObject( bool bForceReattach )
{
	IAviImagePtr ptrAvi = ::GetActiveAviFromContext( Unknown() );	

	if( ptrAvi == 0 )
	{
		SIZE size;
		size.cx = size.cy = 0;
		SetClientSize( size ); 
	}

	if( ::GetKey( m_ptrAvi ) != ::GetKey( ptrAvi ) || bForceReattach )
	{
		m_ptrAvi = ptrAvi;
		m_lPrevKeyFrame	= m_lPrevFrame = -1;

		{
			IPropertySheetPtr ptrSheet;
			IOptionsPagePtr ptrPage;
			int nPage = -1;

			if( GetPropPageStuff( ptrSheet, ptrPage, nPage, clsidAviGalleryPage ) )
			{
				IAviGalleryPropPagePtr ptrInfoPP( ptrPage );
				ptrInfoPP->ReadInfo( m_ptrAvi );
			}
				
		}

		BuildView();
	}

	if( ::IsWindow( m_hwnd ) )
	{
		InvalidateRect( m_hwnd, 0, 0 );
		UpdateWindow( m_hwnd );
	}

}

//////////////////////////////////////////////////////////////////////
void CAviGallery::DoAttachObjects()
{	 
	ReAttachObject( false );	
}

//////////////////////////////////////////////////////////////////////
void CAviGallery::ProcessDrawing( HDC hdc, RECT rectPaint )
{
	IScrollZoomSitePtr ptrZ( Unknown() );
	if( ptrZ == 0 )
		return;

	POINT ptScroll;
	ptrZ->GetScrollPos( &ptScroll );

	RECT rcUpdate;
	::memcpy( &rcUpdate, &rectPaint, sizeof(RECT) );

	
	rectPaint.left	+= ptScroll.x;
	rectPaint.right += ptScroll.x;

	rectPaint.top	+= ptScroll.y;
	rectPaint.bottom+= ptScroll.y;
	


	int nWidth	= rcUpdate.right - rcUpdate.left + 1;
	int nHeight	= rcUpdate.bottom - rcUpdate.top + 1;


	HDC	hdcCache = ::CreateCompatibleDC( hdc );
	if( !hdcCache )
		return;

	HBITMAP	hBitmap = ::CreateCompatibleBitmap( hdc, nWidth, nHeight );
	if( !hBitmap )
	{
		::DeleteDC( hdcCache );	hdcCache = 0;
		return;
	}

	HBITMAP hOldBmp = (HBITMAP)::SelectObject( hdcCache, hBitmap );

	POINT ptOrgPrev;	
	::SetWindowOrgEx( hdcCache, rectPaint.left, rectPaint.top, &ptOrgPrev );
	


	_brush brush( ::CreateSolidBrush( m_clrBk ) );
	::FillRect( hdcCache, &rectPaint, brush );
	

	long lIndex = GetNextVisibleIndex( rectPaint, -1 );
/*
	char szBuf[1024];
	sprintf( szBuf, "\nRect:%d, %d, %d, %d", rectPaint.left, rectPaint.top, rectPaint.right, rectPaint.bottom );
	*/

	while( lIndex != -1 )
	{	
		/*
		char szBuf[100];
		sprintf( szBuf, " ,=%d", lIndex );
		OutputDebugString( szBuf );
		*/
		DrawFrame( hdcCache, lIndex );
		lIndex = GetNextVisibleIndex( rectPaint, lIndex );		
	}	

	//OutputDebugString( szBuf );


	::BitBlt(	hdc, rcUpdate.left, rcUpdate.top, nWidth, nHeight, 
				hdcCache, rectPaint.left, rectPaint.top, SRCCOPY
				);

	::SelectObject( hdcCache, hOldBmp );

	::DeleteObject( hBitmap );

	::DeleteDC( hdcCache );	hdcCache = 0;
}



//////////////////////////////////////////////////////////////////////
void CAviGallery::DrawFrame( HDC hdc, long lIndex )
{

	SIZE sizeThumbnail = GetThumbnailSize();
	

	long lCol = -1;
	long lRow = -1;
	
	ConvertToHorzVertIndex( lIndex, lCol, lRow );

	if( lCol < 0 || lRow < 0 )
		return;

	RECT rcItem;

	rcItem.left = lCol * ( sizeThumbnail.cx + ITEM_DISTANCE ) + ITEM_DISTANCE;
	rcItem.top  = lRow * ( sizeThumbnail.cy + ITEM_DISTANCE ) + ITEM_DISTANCE;

	rcItem.right  = rcItem.left + sizeThumbnail.cx;
	rcItem.bottom = rcItem.top  + sizeThumbnail.cy;

	if( m_ptrAvi == 0 )
		return;

	_variant_t var_cur;
	long lSelectedFrame = -1;
	m_ptrAvi->get_CurFrame( &var_cur );
	lSelectedFrame=  (long)var_cur;	

	long lreal_frame = GetNearestFrame( lIndex );
	_brush brushBk( ::CreateSolidBrush( lreal_frame == lSelectedFrame ? m_clrSelectItem : m_clrItem ) );
	
	::FillRect( hdc, &rcItem, brushBk );
	::DrawEdge( hdc, &rcItem, EDGE_RAISED, BF_RECT );


	RECT rcImage;
	memcpy( &rcImage, &rcItem, sizeof(RECT) );

	int nDelta = 7;
	rcImage.left	+= nDelta;
	rcImage.top		+= nDelta;
	rcImage.right	-= nDelta;
	rcImage.bottom	-= nDelta;


	IAviDibImagePtr ptrDib = m_ptrAvi;
	if( ptrDib == 0 )
		return;

	long lInfoSize = 0;

	if( S_OK != ptrDib->GetBitmapInfoHeader( lreal_frame, &lInfoSize, 0 ) )
		return;

	BYTE* pInfo = new BYTE[lInfoSize];
	_ptrKiller_t<BYTE> pk1(pInfo);

	if( S_OK != ptrDib->GetBitmapInfoHeader( lreal_frame, &lInfoSize, pInfo ) )
		return;

	BITMAPINFOHEADER* pbi = (BITMAPINFOHEADER*)pInfo;

	pbi->biCompression = BI_RGB;

	long lDibBitsSize = 0;
	if( S_OK != ptrDib->GetDibBitsSize( (BYTE*)pbi, &lDibBitsSize ) )
		return;

	if( lDibBitsSize != m_lBufSize )
	{
		m_lBufSize = lDibBitsSize;
		if( m_pDibBits )
		{
			delete m_pDibBits;
			m_pDibBits = 0;
		}

		m_pDibBits = new BYTE[m_lBufSize];
	}

	if( S_OK != ptrDib->GetDibBits( lreal_frame, m_pDibBits, &m_lPrevKeyFrame, &m_lPrevFrame ) )
		return;		

	if( !m_hDrawDib )
		m_hDrawDib = ::DrawDibOpen();

	if( !m_hDrawDib )
		return;

	SIZE sizeTarget;
	sizeTarget.cx = rcImage.right - rcImage.left + 1;
	sizeTarget.cy = rcImage.bottom - rcImage.top + 1;

	SIZE sizeDraw;

	if( sizeTarget.cx < 1 || sizeTarget.cy < 1 || pbi->biWidth < 1 || pbi->biHeight < 1 )
		return;
	
	sizeDraw.cx = sizeDraw.cy = 0;
	{
		double fThumbWidth	= (double)sizeTarget.cx;
		double fThumbHeight	= (double)sizeTarget.cy;
		double fThumbZoom	= fThumbWidth / fThumbHeight;

		double fImageWidth	= (double)pbi->biWidth;
		double fImageHeight	= (double)pbi->biHeight;
		double fImageZoom	= fImageWidth / fImageHeight;


		double fZoom = min( fThumbWidth / fImageWidth, fThumbHeight / fImageHeight );

		double fWidth, fHeight;

		fWidth = fThumbWidth;
		fHeight = fThumbHeight;
		
		if( fWidth / fImageZoom > fThumbHeight )
		{		
			fWidth = fThumbHeight * fImageZoom;
			fHeight = fThumbHeight;
		}

		if( fHeight * fImageZoom > fThumbWidth )
		{
			fHeight = fThumbWidth / fImageZoom;
			fWidth = fThumbWidth;
		}

		sizeDraw.cx = long(fWidth);
		sizeDraw.cy = long(fHeight);
	}

	int nX = rcImage.left + (sizeTarget.cx - sizeDraw.cx) / 2;
	int nY = rcImage.top + (sizeTarget.cy - sizeDraw.cy) / 2;

	RECT rcBorder;
	
	rcBorder.left	= nX - 1;
	rcBorder.top	= nY - 1;
	rcBorder.right	= rcBorder.left + sizeDraw.cx + 2;
	rcBorder.bottom	= rcBorder.top + sizeDraw.cy + 2;

	::DrawEdge( hdc, &rcBorder, EDGE_SUNKEN, BF_RECT );


	
	BOOL bRes = ::DrawDibDraw( m_hDrawDib, hdc,
									nX, nY, sizeDraw.cx, sizeDraw.cy,
									pbi, m_pDibBits, 0, 0, pbi->biWidth, pbi->biHeight, 0 );

}

//////////////////////////////////////////////////////////////////////
long CAviGallery::GetNextVisibleIndex( RECT rcVisible, long lPrevFrame )
{

	long lAreaWidth		= rcVisible.right - rcVisible.left + 1;
	long lAreaHeight	= rcVisible.bottom - rcVisible.top + 1;

	if( lAreaWidth < 1 || lAreaHeight < 1 )
		return -1;


	SIZE sizeThumbnail = GetThumbnailSize();

	int nItemFullWidth	= sizeThumbnail.cx + ITEM_DISTANCE;
	int nItemFullHeight	= sizeThumbnail.cy + ITEM_DISTANCE;

	long lCols = GetColCount();
	long lRows = GetRowCount();

	if( lCols < 1 || lRows < 1 )
		return -1;


	//Vert & horz index of first visible item
	long lCol1, lRow1;

	lCol1 = rcVisible.left / nItemFullWidth;
	lRow1 = rcVisible.top  / nItemFullHeight;

	//Vert & horz index of last visible item
	long lCol2, lRow2;
	lCol2 = ( rcVisible.right  - ITEM_DISTANCE ) / nItemFullWidth;
	lRow2 = ( rcVisible.bottom - ITEM_DISTANCE ) / nItemFullHeight;

	if( lCol1 >= lCols )
		lCol1 = lCols - 1;

	if( lCol2 >= lCols )
		lCol2 = lCols - 1;


	if( lPrevFrame == -1 )
		return ConvertToLinearIndex( lCol1, lRow1 );

	long lPrevCol, lPrevRow;
	lPrevCol = lPrevRow = -1;

	long lNewCol, lNewRow;
	lNewCol = lNewRow = -1;

	ConvertToHorzVertIndex( lPrevFrame, lPrevCol, lPrevRow );

	if( lPrevCol == -1 || lPrevRow == -1 ) 
		return -1;

	if( lPrevCol < lCol1 || lPrevCol > lCol2 )
		return -1;

	if( lPrevRow < lRow1 || lPrevRow > lRow2 )
		return -1;

	lNewCol = lPrevCol;
	lNewRow = lPrevRow;

	lNewCol++;
	
	if( lNewCol > lCol2 )
	{
		lNewCol = lCol1;
		lNewRow++;		
	}

	if( lNewRow > lRow2 )
		return -1;

	long lNewFrame = ConvertToLinearIndex( lNewCol, lNewRow );

	return lNewFrame;
}


//////////////////////////////////////////////////////////////////////
long CAviGallery::ConvertToLinearIndex( long lCol, long lRow )
{
	int lCols = GetColCount();
	long lIndex = lCols * lRow + lCol;

	long lFrameCount = GetEndFrame() - GetStartFrame();

	if( lIndex >= lFrameCount || lIndex < 0 )
		return -1;

	return lIndex;
}

//////////////////////////////////////////////////////////////////////
void CAviGallery::ConvertToHorzVertIndex( long lLinearIndex, long& lCol, long& lRow )
{
	int lCols = GetColCount();
	
	long lFrameCount = GetEndFrame() - GetStartFrame();

	if( lLinearIndex >= lFrameCount || lLinearIndex < 0 )		
	{
		lCol = lRow = -1;		
	}		

	lRow = lLinearIndex / lCols;
	lCol = lLinearIndex - lRow * lCols;

}

//////////////////////////////////////////////////////////////////////
void CAviGallery::SetClientSize( SIZE size )
{
	if( !::IsWindow( m_hwnd ) )
		return;


	IScrollZoomSitePtr pSite( Unknown() );
	if( pSite != 0 && size.cx >= 0 && size.cy >= 0 ) 
	{
		SIZE sizeOld;
		pSite->GetClientSize( &sizeOld );

		if( sizeOld.cx != size.cx || sizeOld.cy != size.cy )
			pSite->SetClientSize( size );
	}

}

//////////////////////////////////////////////////////////////////////
HRESULT CAviGallery::OnActivateView( BOOL bActivate, IUnknown *punkOtherView )
{
	ShowPropPage( bActivate == TRUE );
	
	return S_OK;	
}


//////////////////////////////////////////////////////////////////////
LRESULT CAviGallery::OnLButtonDown(long lflags, int x, int y )
{
	IScrollZoomSitePtr ptrZ( Unknown() );
	if( ptrZ == 0 )
		return 0;

	POINT ptScroll;
	ptrZ->GetScrollPos( &ptScroll );

	RECT rcClient = {0};
	::GetClientRect( m_hwnd, &rcClient );
	
	rcClient.left	+= ptScroll.x;
	rcClient.right += ptScroll.x;

	rcClient.top	+= ptScroll.y;
	rcClient.bottom+= ptScroll.y;

	long lIndex = GetNextVisibleIndex( rcClient, -1 );

	while( lIndex != -1 )
	{	
		long lreal_frame = GetNearestFrame( lIndex );
		RECT rcItem = GetItemRect( lIndex );

		if( (x + ptScroll.x ) >= rcItem.left && 
			(x + ptScroll.x ) <= rcItem.right && 
			(y + ptScroll.y ) >= rcItem.top && 
			(y + ptScroll.y ) <= rcItem.bottom )
		{
			if( m_ptrAvi )
			{
				m_ptrAvi->MoveTo( lreal_frame, 0 );
				::InvalidateRect( m_hwnd, 0, true );
			}
			return 0;
		}

		lIndex = GetNextVisibleIndex( rcClient, lIndex );

	}
	return 0;
}

//////////////////////////////////////////////////////////////////////
LRESULT CAviGallery::OnLButtonDblClick( int x, int y )
{
	return 0; 
}

//////////////////////////////////////////////////////////////////////
LRESULT CAviGallery::OnDestroy( )
{
	if( m_hDrawDib )
		DrawDibClose( m_hDrawDib );	m_hDrawDib = 0;
	return 0;
}
		
//////////////////////////////////////////////////////////////////////
HRESULT CAviGallery::GetMatchType( BSTR bstrObjectType, DWORD *pdwMatch )
{
	return S_FALSE;//CWinViewBase::GetObjectFlags( bstrObjectType, pdwMatch );

	_bstr_t	bstr( bstrObjectType );
	if( !strcmp( bstr, szTypeImage ) )
		*pdwMatch = mvFull;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CAviGallery::OnEraseBackground( HDC hDC )
{
	if( !::IsWindow( m_hwnd ) )
		return 0;

	return 1;
}

//////////////////////////////////////////////////////////////////////
LRESULT CAviGallery::OnSize( int cx, int cy )
{
	if( !::IsWindow( m_hwnd ) )
		return 0;

	BuildView();

	return 0;
}

//////////////////////////////////////////////////////////////////////
void CAviGallery::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	CWinViewBase::OnNotify( pszEvent, punkHit, punkFrom, pdata, cbSize );
	
	if( !strcmp( pszEvent, szEventNewSettings ) )
	{		
		BuildView();
	}
}


//////////////////////////////////////////////////////////////////////
IUnknown* CAviGallery::DoGetInterface( const IID &iid )
{	
	//else if( iid == IID_IPrintView ) return (IPrintView*)this;
	if( iid == IID_IPersist ) return (IPersist*)this;
	else if( iid == IID_INamedObject2 ) return (INamedObject2*)this;	
	else 
		return CWinViewBase::DoGetInterface( iid );
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviGallery::GetClassID(CLSID *pClassID )
{
	memcpy( pClassID, &clsidAviGalleryView, sizeof(CLSID) );
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviGallery::GetFirstVisibleObjectPosition(TPOS *plpos)
{
	*plpos = 0;

	if( m_ptrAvi != 0 )
		*plpos = (TPOS)1;
	
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CAviGallery::GetNextVisibleObject( IUnknown ** ppunkObject, TPOS *plPos )
{	
	TPOS lPos		= *plPos;
	*ppunkObject	= 0;

	*plPos			= 0;

	if( lPos == (TPOS)1 )
	{
		if( m_ptrAvi )
		{
			m_ptrAvi->AddRef();
			*ppunkObject = m_ptrAvi;
			return S_OK;
		}
	}

	return S_OK;
}


//////////////////////////////////////////////////////////////////////
void CAviGallery::ShowPropPage( bool bShow )
{
	IPropertySheetPtr ptrSheet;
	IOptionsPagePtr ptrPage;
	int nPage = -1;

	if( !GetPropPageStuff( ptrSheet, ptrPage, nPage, clsidAviGalleryPage ) )
		return;

	if( bShow )
	{
		ptrSheet->IncludePage( nPage );
	}
	else
	{
		ptrSheet->ExcludePage( nPage );
	}

}

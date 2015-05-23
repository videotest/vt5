// ActionCGHPanesCorrectionA.cpp: implementation of the CActionPanesCorrectionA class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ActionCGHPanesCorrectionA.h"
#include "resource.h"
#include <math.h>
#include "thumbnail.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/*****************************************************************************************************/
IMPLEMENT_DYNCREATE( CActionPanesCorrectionA, CCmdTargetEx );

// {8C232849-F138-41d7-988D-8DD4C7409D82}
GUARD_IMPLEMENT_OLECREATE( CActionPanesCorrectionA, "CGH_Chomos.AutoCorrectionPanes", 
0x8c232849, 0xf138, 0x41d7, 0x98, 0x8d, 0x8d, 0xd4, 0xc7, 0x40, 0x9d, 0x82);

// {58D3388F-44CF-48eb-9F89-38BBC43F162E}
static const GUID GUID_AutoCorrectionCGHPanes = 
{ 0x58d3388f, 0x44cf, 0x48eb, { 0x9f, 0x89, 0x38, 0xbb, 0xc4, 0x3f, 0x16, 0x2e } };

ACTION_INFO_FULL( CActionPanesCorrectionA, IDS_ACTION_AUTO_CORRECT, -1, -1, GUID_AutoCorrectionCGHPanes );
ACTION_TARGET( CActionPanesCorrectionA, szTargetViewSite );

ACTION_ARG_LIST( CActionPanesCorrectionA )
	ARG_STRING("TargetImage", 0 )
END_ACTION_ARG_LIST()

BEGIN_INTERFACE_MAP(CActionPanesCorrectionA, CInteractiveActionBase)
	INTERFACE_PART(CActionPanesCorrectionA, IID_ILongOperation, Long)
END_INTERFACE_MAP()

CActionPanesCorrectionA::CActionPanesCorrectionA() { }
CActionPanesCorrectionA::~CActionPanesCorrectionA() { }

void CActionPanesCorrectionA::Finalize()   {	CActionPanesCorrectionBase::Finalize(); }
bool CActionPanesCorrectionA::Initialize() {	CActionPanesCorrectionBase::Initialize(); Finalize(); return true; }
bool CActionPanesCorrectionA::Invoke()
{
	CString strDestName = GetArgumentString( _T( "TargetImage" ) );
	if( StoreDestImage( strDestName ) != S_OK )
			return false;

	_combine();

	if( StoreToContext() != S_OK )
		return false;

	StoreToNamedData( _bstr_t( KARYO_ACTIONROOT ), _bstr_t( CGH_AUTOCORRECTION ), _variant_t( true ) );
	return true;
}

void CActionPanesCorrectionA::_combine()
{
	StartNotification( 7, 1, 1 );

	CSize offset[3] = { CSize( -1, -1), CSize( -1, -1), CSize( -1, -1) };

	int nNotify = 0;

	int **pXDensity = new int *[3];
	int **pYDensity = new int *[3];

	TPtrKiller2 <int> _pXDensity( pXDensity, 3 );
	TPtrKiller2 <int> _pYDensity( pYDensity, 3 );

	for( int i = 0; i < 3; i++ )
	{
		_get_offset( i, &pXDensity[i], &pYDensity[i] );
		Notify( ++nNotify );
	}

	for( int z = 0; z < 3; z++ )
	{
		for( i = 0; i < m_nImageW; i++ )
		{
			if( pXDensity[z][i] >= nEqual )
			{
				offset[z].cx = i;
				break;
			}
		}
		for( i = 0; i < m_nImageH; i++ )
		{
			if( pYDensity[z][i] >= nEqual )
			{
				offset[z].cy = i;
				break;
			}
		}
		Notify( ++nNotify );
	}

	// См другой вариант версия 2 


	for( i = 2; i >= 0; i-- )
		offset[i] -= offset[0];

	FinishNotification();

	if( ReCreateDest() != S_OK )
		return;

	for( i = 0; i < 3; i++ )
		CopyImagePane( m_sptrPrevImg, i, i, offset[i].cx, offset[i].cy );


}

void CActionPanesCorrectionA::_get_offset( int nPaneID,int **pXDensity, int **pYDensity )
{
	IImage3Ptr ptrImage = m_sptrImage;


	IUnknownPtr	pUnkCC;
	ptrImage->GetColorConvertor( &pUnkCC );

	if( pUnkCC == 0 )
		return;

	IColorConvertorPtr	ptrCC = pUnkCC;

	double MiddleColor = 0;

	long **Image;

	Image = new long*[m_nImageH];
	for( int j = 0; j < m_nImageH; j++ )
		Image[j] = new long[m_nImageW];

	TPtrKiller2 <long> _Image( Image, m_nImageH );

	for( j = 0; j < m_nImageH; j++ )
	{
		color *pColor = 0;
		ptrImage->GetRow( j, nPaneID, &pColor );

		for( int i = 0; i < m_nImageW; i++ )
		{
			Image[j][i] = ColorAsByte( pColor[i] );
			MiddleColor += Image[j][i];
		}
	}

	MiddleColor /= m_nImageH * m_nImageW;
	
	long Good = 0;
	for( j = 0; j < m_nImageH; j++ )
	{
		for( int i = 0; i < m_nImageW; i++ )
		{
			if( Image[j][i] >= MiddleColor )
				Good++;
		}
	}

	BOOL bInner = Good > ( m_nImageW * m_nImageH);
	for( j = 0; j < m_nImageH; j++ )
	{
		for( int i = 0; i < m_nImageW; i++ )
		{
			if( Image[j][i] >= MiddleColor )
				Image[j][i] = !bInner;
			else
				Image[j][i] = bInner;
		}
	}

	_get_density( pXDensity, pYDensity, Image );
}

void CActionPanesCorrectionA::_get_density( int **pXDensity, int **pYDensity, long **pSrcImage )
{
	(*pXDensity) = new int[m_nImageW];
	(*pYDensity) = new int[m_nImageH];

	for( int j = 0; j < m_nImageW; j++ )
		(*pXDensity)[j] = 0;

	for( j = 0; j < m_nImageH; j++ )
		(*pYDensity)[j] = 0;

	for( j = 0; j < m_nImageH; j++ )
	{
		for( int i = 0; i < m_nImageW; i++ )
		{
			(*pXDensity)[i] += pSrcImage[j][i];
			(*pYDensity)[j] += pSrcImage[j][i];
		}
	}
	// См отладку версия 2 
}


/*****************************************************************************************************/
COLORREF GetBackgroundColor()
{
	COLORREF	cr = ::GetValueColor( GetAppUnknown(), "\\Colors", "Background", RGB( 128, 128, 255 ) );
	return cr;
}
/*****************************************************************************************************/
IMPLEMENT_DYNCREATE( CActionPanesCorrectionM, CCmdTargetEx );

// {BF62792D-50F8-46b0-92E3-76E32E03D14F}
GUARD_IMPLEMENT_OLECREATE( CActionPanesCorrectionM, "CGH_Chomos.ManualCorrectionPanes", 
0xbf62792d, 0x50f8, 0x46b0, 0x92, 0xe3, 0x76, 0xe3, 0x2e, 0x3, 0xd1, 0x4f);


// {C7A1DCA8-A0BE-41bd-BA3E-A7BFD9D0CF7E}
static const GUID GUID_ManualCorrectionCGHPanes = 
{ 0xc7a1dca8, 0xa0be, 0x41bd, { 0xba, 0x3e, 0xa7, 0xbf, 0xd9, 0xd0, 0xcf, 0x7e } };


ACTION_INFO_FULL( CActionPanesCorrectionM, IDS_ACTION_MANUAL_CORRECT, -1, -1, GUID_ManualCorrectionCGHPanes );
ACTION_TARGET( CActionPanesCorrectionM, szTargetViewSite );

ACTION_ARG_LIST( CActionPanesCorrectionM )
	ARG_STRING("TargetImage", 0 )
	ARG_INT("PaneToSet", -1)
	ARG_INT("xOffest", 0)
	ARG_INT("yOffest", 0)
END_ACTION_ARG_LIST()

BEGIN_INTERFACE_MAP( CActionPanesCorrectionM, CInteractiveActionBase)
	INTERFACE_PART(  CActionPanesCorrectionM, IID_IScriptNotifyListner, ScriptNotify )
END_INTERFACE_MAP()		

IMPLEMENT_UNKNOWN_BASE( CActionPanesCorrectionM, ScriptNotify )

CActionPanesCorrectionM::CActionPanesCorrectionM()  
{ 
	m_bDrag = false;
	CWinApp *pApp = AfxGetApp();
	if( pApp )
	{
		m_hcurCatch = pApp->LoadCursor( IDC_CURSOR_CATCH );
		m_hcurFree  = pApp->LoadCursor( IDC_CURSOR_FREE );
	}
	m_nActivePane = 0;

	::ZeroMemory( m_xOffsets, sizeof( int ) * 32 );
	::ZeroMemory( m_yOffsets, sizeof( int ) * 32 );
}
CActionPanesCorrectionM::~CActionPanesCorrectionM() 
{ 
	if( m_hcurCatch )
		::DestroyCursor( m_hcurCatch );
	if( m_hcurFree )
		::DestroyCursor( m_hcurFree );
}

void CActionPanesCorrectionM::Finalize()  
{ 	
	COperationBase::Finalize(); 	
	::SetCursor( m_hcurOld );
}

bool CActionPanesCorrectionM::Initialize()
{
	m_hcurOld = ::GetCursor();

	COperationBase::Initialize();

	CString strDestName = GetArgumentString( _T( "TargetImage" ) );
	if( StoreDestImage( strDestName ) != S_OK )
	{
		Terminate();
		return false;
	}

	int nPaneID = GetArgumentInt( _T( "PaneToSet" ) );
	if( nPaneID >= 0 )
		Finalize();
	else
	{
		CWnd *pwnd = GetWindowFromUnknown( m_punkTarget );
		if( pwnd )
		{
			if( ::GetFocus() != pwnd->m_hWnd )
				pwnd->SetFocus();
		}

		CreateDestByImage( strDestName, m_sptrImage, GetCCName( m_sptrImage ) );
		CopyImagePanesEx( m_sptrPrevImg );
		_invalidate();
		m_hcurActive = m_hcurFree;
	}

	return true;
}

bool CActionPanesCorrectionM::Invoke()
{
	int nPaneID = GetArgumentInt( _T( "PaneToSet" ) );

	if( nPaneID >= 0 )
	{
		int xOffset = -GetArgumentInt( _T( "xOffest" ) );
		int yOffset = -GetArgumentInt( _T( "yOffest" ) );

		if( ReCreateDest() != S_OK )
			return false;

		CopyImagePane( m_sptrPrevImg, nPaneID, nPaneID, xOffset, yOffset );

		if( StoreToContext() != S_OK )
			return false;
	}
	else
	{
		_copy_named_data( m_sptrImage, m_sptrPrevImg );

		if( RemoveFromContext( m_sptrPrevImg ) != S_OK )
			return false;

		if( StoreToContext() != S_OK )
			return false;
	}

	LeaveMode();

	return true;
}

bool CActionPanesCorrectionM::DoLButtonDown(CPoint pt)
{
	m_bDrag = true;
	m_ptStart = pt;
	m_hcurActive = m_hcurCatch;
	::SetCursor( m_hcurActive );

	_fire_ch_event( "ManualCorrection_OnBeforeChange" );

	return true;
}

bool CActionPanesCorrectionM::DoLButtonUp(CPoint pt)
{
	m_bDrag = false;
	m_hcurActive = m_hcurFree;
	::SetCursor( m_hcurActive );
	
	_fire_ch_event( "ManualCorrection_OnAfterChange" );

	return true;
}

bool CActionPanesCorrectionM::DoMouseMove(CPoint point)
{
	if( m_bDrag )
	{
		m_xOffsets[m_nActivePane] -= point.x - m_ptStart.x;
		m_yOffsets[m_nActivePane] -= point.y - m_ptStart.y;
		
		m_ptStart = point;
		
		CopyImagePane( m_sptrPrevImg, m_nActivePane, m_nActivePane, m_xOffsets[m_nActivePane], m_yOffsets[m_nActivePane] );
		_invalidate();
		_fire_ch_event( "ManualCorrection_OnChange" );
	}

	return true;
}

bool CActionPanesCorrectionM::DoVirtKey(UINT nVirtKey)
{
	if( nVirtKey >= VK_LEFT && nVirtKey <= VK_DOWN )
	{
		int dx = 0, dy = 0;

		int t = 1;
		if( GetAsyncKeyState( VK_CONTROL ) & 0x8000 )
			t = 10;

		if( nVirtKey == VK_LEFT || nVirtKey == VK_RIGHT )
			dx = t * ( nVirtKey == VK_LEFT ? -1 : 1 );
		if( nVirtKey == VK_UP || nVirtKey == VK_DOWN )
			dy = t * ( nVirtKey == VK_UP ? -1 : 1 );


		m_xOffsets[m_nActivePane] -= dx;
		m_yOffsets[m_nActivePane] -= dy;
		
		CopyImagePane( m_sptrPrevImg, m_nActivePane, m_nActivePane, m_xOffsets[m_nActivePane], m_yOffsets[m_nActivePane] );
		_invalidate();

		_fire_ch_event( "ManualCorrection_OnBeforeChange" );
		_fire_ch_event( "ManualCorrection_OnChange" );
		_fire_ch_event( "ManualCorrection_OnAfterChange" );
	}
	else if( nVirtKey == VK_ESCAPE )
		Terminate();
	else if( nVirtKey == VK_RETURN )
	{
		Finalize();
		LeaveMode();
	}
	return true;
}

void CActionPanesCorrectionM::_invalidate()
{
	CRect rc;
	_get_img_rect( rc );

	CWnd *pwnd = GetWindowFromUnknown( m_punkTarget );
	if( pwnd )
		pwnd->InvalidateRect( ::ConvertToWindow( m_punkTarget, rc ) );
}

void CActionPanesCorrectionM::_get_img_rect( CRect &rc )
{
	IImage3Ptr ptrImage = m_sptrImage;

	POINT ptImgOffset;
	ptrImage->GetOffset( &ptImgOffset );

	rc.left = ptImgOffset.x;
	rc.top = ptImgOffset.y;

	rc.right = rc.left + m_nImageW;
	rc.bottom = rc.top + m_nImageH;
}

void CActionPanesCorrectionM::DoDraw(CDC &dc)
{
	IScrollZoomSitePtr	ptrScrollZoom = m_punkTarget;

	CRect rcImage,rc;
	_get_img_rect( rcImage );
	rc = rcImage;
	
	double fZoom;
	POINT pointScroll;
	
	ptrScrollZoom->GetZoom( &fZoom );
	ptrScrollZoom->GetScrollPos( &pointScroll );

	CRect rcVisible;
	ptrScrollZoom->GetVisibleRect( &rcVisible );

	CSize	size_cur = CSize( rcImage.Width(), rcImage.Height() );

	CRect	rectImage;
	rectImage.left   = int( ceil(double(- pointScroll.x )));
	rectImage.right  = int( floor( ( size_cur.cx ) * fZoom - pointScroll.x + .5) );
	rectImage.top    = int( ceil(double(- pointScroll.y )));
	rectImage.bottom = int( floor( ( size_cur.cy ) * fZoom - pointScroll.y + .5) );

	CRect	rectPaint = ::ConvertToWindow( m_punkTarget, rc );


	CDC dcMemory;
	dcMemory.CreateCompatibleDC( &dc );

	
	rectPaint.left = min( max( rectPaint.left, rectImage.left ), rectImage.right );
	rectPaint.right = min( max( rectPaint.right, rectImage.left ), rectImage.right );
	rectPaint.top = min( max( rectPaint.top, rectImage.top ), rectImage.bottom );
	rectPaint.bottom = min( max( rectPaint.bottom, rectImage.top ), rectImage.bottom );

	//init the DIB hearer
	BITMAPINFOHEADER	bmih;
	ZeroMemory( &bmih, sizeof( bmih ) );

	bmih.biBitCount = 24;
	bmih.biHeight = rcVisible.Height();
	bmih.biWidth = rcVisible.Width();
	bmih.biSize = sizeof( bmih );
	bmih.biPlanes = 1;
	bmih.biSizeImage=((int)(bmih.biWidth*3+3)/4)*4*bmih.biHeight;
	
	
	if( bmih.biHeight == 0 ||bmih.biWidth == 0 )
		return;

	//create a DIB section
	byte	*pdibBits = 0;
	HBITMAP	hDIBSection = ::CreateDIBSection( dc, (BITMAPINFO*)&bmih, DIB_PAL_COLORS, (void**)&pdibBits, 0, 0 );

	ASSERT( hDIBSection );

	CBitmap *psrcBitmap = CBitmap::FromHandle( hDIBSection  );

	ASSERT( psrcBitmap );
	
	//propare the memory DC to drawing
	CBitmap *poldBmp = dcMemory.SelectObject( psrcBitmap );
	dcMemory.SetMapMode( MM_TEXT );
	dcMemory.SetViewportOrg( -rectPaint.TopLeft() );

	COLORREF cr = GetBackgroundColor();
	dcMemory.FillRect( rectPaint, &CBrush( cr ) );


	DWORD dwDrawFlags = 0;

	{
		IUnknown *punkColorCnv = 0;

		IImage3Ptr image = m_sptrImage;
		image->GetColorConvertor( &punkColorCnv );

		IColorConvertorExPtr	ptrCC( punkColorCnv );
		punkColorCnv->Release();

		if( pdibBits)
		{
			ptrCC->ConvertImageToDIBRect( &bmih, pdibBits, CPoint( 0, 0 ), (IImage2*)image, 
				rcVisible,  CPoint( 0, 0 ),
				fZoom, pointScroll, cr, dwDrawFlags, m_punkTarget );		

		}
	}


	dcMemory.SetViewportOrg( 0, 0 );

	//paint to paint DC
	::SetDIBitsToDevice( dc, 0, 0, rcVisible.Width(), rcVisible.Height(),
		0, 0, 0, rcVisible.Height(), pdibBits, (BITMAPINFO*)&bmih, DIB_RGB_COLORS );

	//prepare to delete objects
	dcMemory.SelectObject( poldBmp );

	if( psrcBitmap )
		psrcBitmap->DeleteObject();

	dc.SetROP2( R2_COPYPEN );
	dc.SelectStockObject( BLACK_PEN );

/*
	IThumbnailManagerPtr sptrThumbMan = ::GetAppUnknown();
	IUnknown *punkRO = 0;
	sptrThumbMan->GetRenderObject( m_sptrImage , &punkRO );
	IRenderObjectPtr spRenderObject = punkRO;
	punkRO->Release();
	punkRO = 0;

	if( spRenderObject == 0 ) 
		return;

	BYTE *pbi = 0;
	short nSupported;
	spRenderObject->GenerateThumbnail( m_sptrImage, 24, RF_BACKGROUND, 0,
									CSize( rcImage.Width(), rcImage.Height() ), 
									&nSupported, &pbi, m_punkTarget );

	if (pbi)
	{
		::ThumbnailDraw( pbi, (CDC*)&dc, rcImage, m_sptrImage );
		::ThumbnailFree( pbi );
		pbi = 0;
	}
*/

}

HRESULT  CActionPanesCorrectionM::XScriptNotify::ScriptNotify( BSTR bstrNotifyDesc, VARIANT varValue )
{
	METHOD_PROLOGUE_BASE( CActionPanesCorrectionM, ScriptNotify );

	CString strNotifyDesc = bstrNotifyDesc;

	if( strNotifyDesc == "ActionNotify_Correct_Terminate" )
		pThis->Terminate();
	else if( strNotifyDesc == "ActionNotify_Correct_Finalize" )
	{
		pThis->Finalize();
		//pThis->LeaveMode();
	}
	else if( strNotifyDesc == "ActionNotify_Correct_PaneChanged" )
		pThis->m_nActivePane = (long)_variant_t( varValue );
	else if( strNotifyDesc == "ActionNotify_Correct_PaneMoved" )
	{
		pThis->m_xOffsets[pThis->m_nActivePane] = -::GetValueInt( ::GetAppUnknown(),KARYO_ACTIONROOT "\\PanesCorrection", "XOffset" );
		pThis->m_yOffsets[pThis->m_nActivePane] = -::GetValueInt( ::GetAppUnknown(),KARYO_ACTIONROOT "\\PanesCorrection", "YOffset" );
		pThis->CopyImagePane( pThis->m_sptrPrevImg, pThis->m_nActivePane, pThis->m_nActivePane, pThis->m_xOffsets[pThis->m_nActivePane], pThis->m_yOffsets[pThis->m_nActivePane] );
		pThis->_invalidate();
	}

	return S_OK;
}

HRESULT  CActionPanesCorrectionM::_fire_ch_event( CString strNotify )
{
	IScriptSitePtr	sptrScriptSite = ::GetAppUnknown();
	
	if( sptrScriptSite == 0 )
		return S_FALSE;

	::SetValue( ::GetAppUnknown(),KARYO_ACTIONROOT "\\PanesCorrection", "XOffset", long( -m_xOffsets[m_nActivePane] ) );
	::SetValue( ::GetAppUnknown(),KARYO_ACTIONROOT "\\PanesCorrection", "YOffset", long( -m_yOffsets[m_nActivePane] ) );

	return sptrScriptSite->Invoke( _bstr_t( strNotify ), 0, 0, 0, fwFormScript | fwAppScript );
}
/*****************************************************************************************************/

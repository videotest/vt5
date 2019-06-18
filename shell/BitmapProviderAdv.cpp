#include "stdafx.h"
#include "BitmapProviderAdv.h"


/////////////////////////////////////////////////////////////////////////////////////////
// CAdvancedBitmapProvider class implementation

/////////////////////////////////////////////////////////////////////////////////////////
CAdvancedBitmapProvider::CAdvancedBitmapProvider()
{
	m_crback = RGB( 0, 0, 0 );
}

/////////////////////////////////////////////////////////////////////////////////////////
CAdvancedBitmapProvider::~CAdvancedBitmapProvider()
{
}

BEGIN_INTERFACE_MAP(CAdvancedBitmapProvider, CCmdTargetEx)
    INTERFACE_PART(CAdvancedBitmapProvider, IID_IBitmapProvider, AdvBmps)
	INTERFACE_PART(CAdvancedBitmapProvider, IID_IAdvancedBitmapProvider, AdvBmps)
	INTERFACE_PART(CAdvancedBitmapProvider, IID_INumeredObject, Num)
END_INTERFACE_MAP();

IMPLEMENT_UNKNOWN(CAdvancedBitmapProvider, AdvBmps)

/////////////////////////////////////////////////////////////////////////////////////////
void CAdvancedBitmapProvider::SetBackColor( COLORREF crBack )
{
	m_crback = crBack;
}

/////////////////////////////////////////////////////////////////////////////////////////
COLORREF CAdvancedBitmapProvider::GetBackColor( )
{
	return m_crback;
}

/////////////////////////////////////////////////////////////////////////////////////////
bool CAdvancedBitmapProvider::Load( )
{
	// clear
	free( );

	if( 0 == GetValueInt( GetAppUnknown(), "\\General", "UseOverloadedIcons", 0 ) )
		return true;

	CString str_path( _T("") );            
	str_path = GetValueString( GetAppUnknown(), "\\Paths", "OverloadedIcons", "" );
	if( str_path.IsEmpty() )
		return false;

	CString str_filename( _T("") );      		
	str_filename = GetValueString( GetAppUnknown(), "\\OverloadedIcons", "ActionsIcons", "" );
	if( str_filename.IsEmpty() )
		return false;

	str_path += str_filename;

	// load icons in image container
	CImageContainer images;
	if( !images.Load( str_path ) )
		return false;

	long limg_count = 0;
	limg_count = images.GetImageCount( );
	if( !limg_count )
		return false;

	SIZE size_max = {0};
	images.GetMaxSize( &size_max );
	if( !size_max.cx || !size_max.cy )
		return false;

	// create bitmap
	CDC dc_memory;
	{
		CClientDC dcScreen( 0 );						
		dc_memory.CreateCompatibleDC( &dcScreen );
	}
	
	BITMAPINFOHEADER	bmih;
	::ZeroMemory( &bmih, sizeof( bmih ) );
	bmih.biBitCount = 24;
	bmih.biHeight = size_max.cy;
	bmih.biWidth = size_max.cx * limg_count;
	bmih.biSize = sizeof( bmih );
	bmih.biPlanes = 1;

	byte	*pdibBits = 0;
	HBITMAP	hDIBSection = ::CreateDIBSection( dc_memory.m_hDC, (BITMAPINFO*)&bmih, DIB_PAL_COLORS, 
		(void**)&pdibBits, 0, 0 );

	ASSERT( hDIBSection );            
	m_bmp.Attach( hDIBSection );

	if( !m_bmp.GetSafeHandle() )
		return false;

	CBitmap *poldbmp = 0;
	poldbmp = dc_memory.SelectObject( &m_bmp );

	RECT rc = { 0, 0, bmih.biWidth, bmih.biHeight };

	// fill background
	dc_memory.FillRect( &rc, &CBrush( m_crback ) );

	// draw icons
	double	fheight_half = (double)(size_max.cy)/2.,
			fwidth_half = (double)(size_max.cx)/2.;
	for( long limg_idx = 0; limg_idx < limg_count; limg_idx ++ )
	{           
		CString str_name( _T("") );     
		SIZE size_icon = {0};
		if( !images.GetImageInfoByIdx( limg_idx, &str_name, &size_icon ) )
			continue;
		
		HICON hicon = 0;
		images.GetIconByIdx( limg_idx, &hicon );
		if( !hicon )
			continue;

		// store index
		XCmdInfo *pinfo = 0;
		long lpos = 0;
		lpos = m_mapname2info.find( str_name );
		if( lpos )
			pinfo = m_mapname2info.get( lpos );
		else
		{
			pinfo = new XCmdInfo;
			m_mapname2info.set( pinfo, str_name );
		}
        pinfo->m_lContIdx = limg_idx;
		
        // draw icon
		POINT pt = {0};
		pt.x = (long)(size_icon.cx * limg_idx + fwidth_half - (double)(size_icon.cx)/2. + 0.5);
		pt.y = (long)(fheight_half - (double)(size_icon.cy)/2. + 0.5);
		::DrawIconEx( dc_memory.m_hDC, 
			pt.x , pt.y, 
			hicon,
			size_icon.cx,
			size_icon.cy,
			0, 0, DI_NORMAL );

		::DestroyIcon( hicon );  hicon = 0;
	}

	if( poldbmp )
		dc_memory.SelectObject( poldbmp ); poldbmp = 0;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
long CAdvancedBitmapProvider::GetBitmapCount()
{
	return m_bmp.GetSafeHandle() ? 1 : 0;
}

/////////////////////////////////////////////////////////////////////////////////////////
HANDLE CAdvancedBitmapProvider::GetBitmap( int idx )
{
    if( !idx )	
		return m_bmp.GetSafeHandle();
    
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////
long CAdvancedBitmapProvider::GetBmpIndexFromName( LPCTSTR lpstr_name, long *plBitmap )
{
    if( !lpstr_name || !plBitmap )
		return -1;

	// reset results
	long lidx = -1;
	*plBitmap = -1;

	long lpos = 0;
	lpos = m_mapname2info.find( CString( lpstr_name ) );
	if( lpos )
	{
		XCmdInfo *pinfo = 0;
		pinfo = m_mapname2info.get( lpos );
		if( pinfo )
		{
			lidx = pinfo->m_lContIdx;
			*plBitmap = 0;
		}
	}
    
	return lidx;
}

/////////////////////////////////////////////////////////////////////////////////////////
bool CAdvancedBitmapProvider::CatchActionInfo( IUnknown *punkActionInfo )
{
    if( !punkActionInfo )
		return false;

	IActionInfoPtr sptr_ai = punkActionInfo;
	if( sptr_ai == 0 )
		return false;

	_bstr_t bstr_name;
	sptr_ai->GetCommandInfo( bstr_name.GetAddress(), 0, 0, 0 );

    long lpos = 0;
	lpos = m_mapname2info.find( CString( (LPCTSTR)(bstr_name) ) );
	if( !lpos )
		return false;

	XCmdInfo *pinfo = 0;
	pinfo = m_mapname2info.get( lpos );
    
	IUnknownPtr sptr_unk;
	sptr_ai->GetTBInfo( 0, &sptr_unk );
	if( sptr_unk == GetControllingUnknown() )
		return true; // already caught

	IActionInfo3Ptr sptr_ai3 = sptr_ai;
	if( sptr_ai3 == 0 )
		return false;

	// catching
    if( S_OK != sptr_ai3->SetBitmapHelper( GetControllingUnknown() ) )
		return false;
    
	// store module and resource indetifiers
	sptr_ai->GetRuntimeInformaton( &pinfo->m_dwDllCode, &pinfo->m_dwResCmd );
	return true;
}

void	CAdvancedBitmapProvider::free( )
{
	m_mapname2info.clear( );
	m_bmp.DeleteObject();
}



/////////////////////////////////////////////////////////////////////////////////////////
// IBitmapProvider

/////////////////////////////////////////////////////////////////////////////////////////
HRESULT CAdvancedBitmapProvider::XAdvBmps::GetBitmapCount( long *piCount )
{
	_try_nested(CAdvancedBitmapProvider, AdvBmps, GetBitmapCount)
	{
		*piCount = pThis->GetBitmapCount();
		return S_OK;
	}
	_catch_nested;
}

/////////////////////////////////////////////////////////////////////////////////////////
HRESULT CAdvancedBitmapProvider::XAdvBmps::GetBitmap( int idx, HANDLE *phBitmap )
{
    _try_nested(CAdvancedBitmapProvider, AdvBmps, GetBitmap)
	{
		if( !phBitmap )
			return E_INVALIDARG;

		*phBitmap = pThis->GetBitmap( idx );
		return S_OK;
	}
	_catch_nested;
}

/////////////////////////////////////////////////////////////////////////////////////////
HRESULT CAdvancedBitmapProvider::XAdvBmps::GetBmpIndexFromCommand( UINT nCmd, DWORD dwDllCode, long *plIndex, long *plBitmap )
{
	_try_nested(CAdvancedBitmapProvider, AdvBmps, GetBmpIndexFromCommand)
	{
		if( !plIndex || !plBitmap )
			return E_INVALIDARG;

		// find name by nCmd and dwDllCode
		CString str_name( _T("") );
		for( long lpos  = pThis->m_mapname2info.head(); lpos; lpos = pThis->m_mapname2info.next( lpos ) )
		{
			CAdvancedBitmapProvider::XCmdInfo *pinfo = 0;
			pinfo = pThis->m_mapname2info.get( lpos );
			if( pinfo->m_dwResCmd == nCmd && pinfo->m_dwDllCode == dwDllCode )
			{
				str_name = pThis->m_mapname2info.get_key( lpos );
				break;
			}
		}
		
		if( str_name.IsEmpty() )
			return S_FALSE;
        
		// get indexes by name
		*plIndex = pThis->GetBmpIndexFromName( str_name, plBitmap );
		return S_OK;
	}
	_catch_nested;
}


/////////////////////////////////////////////////////////////////////////////////////////
// IBitmapProvider2

/////////////////////////////////////////////////////////////////////////////////////////
HRESULT CAdvancedBitmapProvider::XAdvBmps::SetBackColor( COLORREF crBack )
{
	_try_nested(CAdvancedBitmapProvider, AdvBmps, SetBackColor)
	{
		pThis->SetBackColor( crBack );
		return S_OK;
	}
	_catch_nested;
}

/////////////////////////////////////////////////////////////////////////////////////////
HRESULT CAdvancedBitmapProvider::XAdvBmps::GetBackColor( COLORREF *pcrBack )
{
	_try_nested(CAdvancedBitmapProvider, AdvBmps, GetBackColor)
	{
		if( !pcrBack )
			return E_INVALIDARG;

		*pcrBack = pThis->GetBackColor( );
		return S_OK;
	}
	_catch_nested;
}

/////////////////////////////////////////////////////////////////////////////////////////
HRESULT CAdvancedBitmapProvider::XAdvBmps::Load( )
{
	_try_nested(CAdvancedBitmapProvider, AdvBmps, Load)
	{
		pThis->Load( );
		return S_OK;
	}
	_catch_nested;
}

/////////////////////////////////////////////////////////////////////////////////////////
HRESULT CAdvancedBitmapProvider::XAdvBmps::GetBmpIndexFromName( BSTR bstrName, long *plIndex, long *plBitmap )
{
    _try_nested(CAdvancedBitmapProvider, AdvBmps, GetBmpIndexFromName)
	{
		if( !plIndex || !plBitmap )
			return E_INVALIDARG;
        
		*plIndex = pThis->GetBmpIndexFromName( _bstr_t(bstrName), plBitmap );
		return S_OK;
	}
	_catch_nested;
}

/////////////////////////////////////////////////////////////////////////////////////////
HRESULT CAdvancedBitmapProvider::XAdvBmps::CatchActionInfo( IUnknown *punkActionInfo )
{
    _try_nested(CAdvancedBitmapProvider, AdvBmps, CatchActionInfo)
	{
		if( !punkActionInfo )
			return E_INVALIDARG;

		return pThis->CatchActionInfo( punkActionInfo ) ? S_OK : S_FALSE;
	}
	_catch_nested;
}
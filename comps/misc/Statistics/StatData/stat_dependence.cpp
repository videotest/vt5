#include "StdAfx.h"

#include "resource.h"

#include "stat_consts.h"
#include "statui_int.h"

#include "params.h"
#include "menuconst.h"
#include "misc_utils.h" 
#include "misc_str.h"
#include "MenuRegistrator.h"

#include "measure_misc.h"
#include "stat_dependence.h"

#include "stat_utils.h"

#include <math.h>
#include <float.h>

#include "axint.h"

using namespace STAT_DEPENDENCE;

#define SZ_HOT_BTN 15
#define SZ_HOT_BTN_OFFSET 2
#define SZ_HOT_BTN_FULL ( SZ_HOT_BTN + 2 * SZ_HOT_BTN_OFFSET )

#define SZ_BORDER 5

#define SZ_PARAM_TEXT_OFFSET 5
#define SZ_PARAM_TEXT_VALUE_OFFSET 15

#define MAX_SCALE_FACTOR_X					600
#define MAX_SCALE_FACTOR_VALUES_X		400
#define MAX_SCALE_FACTOR_GRID_X			600

#define MAX_SCALE_FACTOR_Y				   600
#define MAX_SCALE_FACTOR_VALUES_Y	   400
#define MAX_SCALE_FACTOR_GRID_Y		   600

namespace STAT_DEPENDENCE
{

	void error_message( HWND hwnd, UINT nID )
	{
		CString str, str2;
		str.LoadString( nID );
		str2.LoadString( IDS_ERROR_TITLE );

		::MessageBox( hwnd, str, str2, MB_OK | MB_ICONERROR );
	}

	inline RECT scale_rect( RECT rc, double fscale )
	{
		if( fscale == 1 )
			return rc;

		rc.left = long( rc.left * fscale );
		rc.top = long( rc.top * fscale );
		rc.right = long( rc.right * fscale );
		rc.bottom = long( rc.bottom * fscale );

		return rc;
	}

	inline CString change_chars( CString str, CString strA, CString strB )
	{
		CString strRet;

		long lPrev = 0;
		long lPos = str.Find( strA, lPrev );

		while( lPos != -1 )
		{
			strRet += str.Mid( lPrev, lPos - lPrev ); 
			strRet +=  strB;
			lPos += strA.GetLength();
			lPrev = lPos;

			lPos = str.Find( strA, lPrev );
		}

		if( lPos == -1 )
			strRet += str.Right( str.GetLength() - lPrev );

		return strRet;
	}

	template<typename TData, typename TData2> static void sort_t( TData *pArr, TData2 *pParam, long lSz, unsigned uiFlag )
	{
		for( long i = 0; i < lSz; i++ )
		{
			long lID = -1;
			TData2 Extr = pParam[i];

			for( long j = i; j < lSz; j++ )
			{
				if( uiFlag == 0 ) // Desc
				{
					if( pParam[j] > Extr )
					{
						Extr = pParam[j];
						lID = j;
					}
				}
				else if( uiFlag == 1 ) // Asc
				{
					if( pParam[j] < Extr )
					{
						Extr = pParam[j];
						lID = j;
					}
				}
			}

			if( lID != -1 )
			{
				TData tmp = pArr[i];
				pArr[i] = pArr[lID];
				pArr[lID] = tmp;

				TData2 tmp2 = pParam[i];
				pParam[i] = pParam[lID];
				pParam[lID] = tmp2;
			}
		}
	}


	inline CString find_in_meas_param( INamedDataPtr sptrNData, CString strSect,long ParamKey, CString strName )
	{
		CString strRet;
		if( sptrNData == 0 )
			return strRet;

		sptrNData->SetupSection( _bstr_t( strSect ) );

		long lCount = 0;
		sptrNData->GetEntriesCount( &lCount );

		for( long i = 0; i < lCount; i++ )
		{
			_bstr_t bstr;
			sptrNData->GetEntryName( i, bstr.GetAddress() );

			if( CString( (char*)bstr ) == "Key" )
			{
				_variant_t var;
				sptrNData->GetValue( _bstr_t( "Key" ), &var );

				if( var.vt == VT_I4 )
				{
					if( var.lVal == ParamKey )
					{
						_variant_t var;
						sptrNData->GetValue( _bstr_t( strName ), &var );

						if( var.vt == VT_BSTR )
							strRet = var.bstrVal;

						return strRet;
					}
				}
			}
		}

		for( long i = 0; i < lCount; i++ )
		{
			_bstr_t bstr;
			sptrNData->GetEntryName( i, bstr.GetAddress() );
			strRet = find_in_meas_param( sptrNData, strSect + "\\" + (char*)bstr, ParamKey, strName );

			sptrNData->SetupSection( _bstr_t( strSect ) );			

			if( !strRet.IsEmpty() )
				break;
		}
		return strRet;
	}


};

namespace{
	bool _get_loga_scale( dblArray& Vals, double fMinVal, double fMaxVal, double dStep )
	{
		int nStep;
		if(dStep <1.)
			nStep=1;
		else if(dStep>10.)
			nStep=10;
		else
			nStep=(int)dStep;

		double a = pow( 10., floor( log10( fMinVal ) ) );
		vector<double> vals(1);
		vals[0]=a;		
		double c=a;
		for(int i=0;; ++i, c *= 10.)
		{
			for( int k = 0; k < 10; k += nStep )
			{
				double d = c * (0==k?1:k);
				if(d<=fMinVal){
					vals[0]=d;
				}else{
					vals.push_back(d);
					if(d>=fMaxVal)
						goto EndFill;
				}
			}
		}
EndFill:
		Vals.resize(vals.size()); 
		std::copy(vals.begin(),vals.end(),&Vals[0]);
		return true;
	}
}

//////////////////////////////////////////////////////////////////////
stat_dependence_view::stat_dependence_view(void)
{
	m_bstrName		= "StatDependence View";

	// [vanek] BT2000:4208 - 21.12.2004
	m_dwWindowFlags |= wfSupportFitDoc2Scr;

	m_bPrintMode = false;

	char sz_buf[256];
	sz_buf[0] = 0;

	::LoadString( App::handle(), IDS_STAT_DEPENDENCE_VIEW_NAME, sz_buf, sizeof( sz_buf ) );

	m_bstrUserName	= sz_buf;

	if( !m_bstrUserName.length() )
		m_bstrUserName = m_bstrName;

	IUnknown *punk = 0;
	::CoCreateInstance( CLSID_ChartDrawer, 0, CLSCTX_INPROC_SERVER, IID_IUnknown, (LPVOID *)&punk );
	m_sptr_chart = punk;
	if( punk )
		punk->Release();


	_init();
}
//////////////////////////////////////////////////////////////////////
stat_dependence_view::~stat_dependence_view()
{
	_deinit();
}
//////////////////////////////////////////////////////////////////////
void stat_dependence_view::_init()
{
	NULLING( m_rc_title			);
	NULLING( m_rc_xparam		);
	NULLING( m_rc_yparam		);
	NULLING( m_rc_chart		);

	NULLING( m_rc_name_text );
	NULLING( m_rc_count_text );
	NULLING( m_rc_model_text );
	NULLING( m_rc_corel_text );

	NULLING( m_rc_min_rect	);

	m_chart_params.init();

	m_sz_xname_param.Empty();
	m_sz_yname_param.Empty();

	m_sz_name_text = CString((LPCTSTR)IDS_NAME);
	m_sz_count_text = CString((LPCTSTR)IDS_COUNT);
	m_sz_model_text = CString((LPCTSTR)IDS_MODEL);
	m_sz_corel_text = CString((LPCTSTR)IDS_COREL);

	m_lst_legend.clear();
	m_wnd_layout.m_lst_legend_layout.clear();

	m_nchart_values_y = 0;
}
//////////////////////////////////////////////////////////////////////
void stat_dependence_view::_deinit()
{
}
//////////////////////////////////////////////////////////////////////
IUnknown* stat_dependence_view::DoGetInterface( const IID &iid )
{
	if( iid == IID_IPrintView ) return (IPrintView*)this;
	else if( iid == IID_IPersist ) return (IPersist*)this;
	else if( iid == IID_INamedObject2 ) return (INamedObject2*)this;
	else if( iid == IID_IHelpInfo )return (IHelpInfo*)this;	
	else if( iid == IID_INamedPropBag ) return (INamedPropBag*)this;	
	else if( iid == IID_INamedPropBagSer ) return (INamedPropBag*)this;	
	else return CWinViewBase::DoGetInterface( iid );
}

//////////////////////////////////////////////////////////////////////
void stat_dependence_view::DoAttachObjects()
{
	IDataContext3Ptr ptr_context = Unknown();

	if( ptr_context == 0 )
		return;

	for( long lpos_lst = m_list_attached.head(); lpos_lst; lpos_lst = m_list_attached.next( lpos_lst ) )
	{
		IUnknown *punk_lst = m_list_attached.get( lpos_lst );

		_store_to_ndata( punk_lst );
		break;
	}

	_clear_attached();


	long lpos_selected = 0;
	_bstr_t bstr_type( szTypeStatTable );

	ptr_context->GetFirstSelectedPos( bstr_type, &lpos_selected );

	while( lpos_selected )
	{
		IUnknown *punk_object = 0;

		ptr_context->GetNextSelected( bstr_type, &lpos_selected, &punk_object );

		if( punk_object )
		{
			_add_attach_data( punk_object );

			INamedDataPtr sptr_nd = punk_object;
			if( sptr_nd == 0 )
			{
				INamedDataObject2Ptr ptrNDO = punk_object;
				if( ptrNDO != 0 )
					ptrNDO->InitAttachedData();
			}

			punk_object->Release();
		}
	}

	if( m_list_attached.count() )
	{
		for( long lpos_lst = m_list_attached.head(); lpos_lst; lpos_lst = m_list_attached.next( lpos_lst ) )
		{
			IUnknownPtr ptr_object = m_list_attached.get( lpos_lst );

			if( _is_ndata_empty( ptr_object ) )
			{
				_load_from_ndata( ::GetAppUnknown() );
				_store_to_ndata( ptr_object );
			}
			else
			{
				_load_from_ndata( ptr_object );
				break;
			}
		}
		_attach_data();
	}else{
		::InvalidateRect( m_hwnd, 0, false );
	}
}

//////////////////////////////////////////////////////////////////////
LRESULT	stat_dependence_view::DoMessage( UINT nMsg, WPARAM wParam, LPARAM lParam )
{
	LRESULT	lResult = 0;
	switch( nMsg )
	{
	case WM_CREATE:			lResult = on_create( (CREATESTRUCT*)lParam );
		break;
	case WM_DESTROY:		lResult = on_destroy();
		break;
	case WM_PAINT:			lResult = on_paint();				
		break;
	case WM_ERASEBKGND:		lResult = on_erase_background( (HDC)wParam );
		break;
	case WM_SIZE:			lResult = on_size( (int)LOWORD(lParam), (int)HIWORD(lParam) );
		break;
	case WM_CONTEXTMENU:	lResult = on_context_menu( HWND(wParam), (int)GET_X_LPARAM(lParam), (int)GET_Y_LPARAM(lParam) );
		break;
	case WM_SHOWWINDOW: lResult = 0;
		if( wParam )
			m_wnd_data.bshowen = true;
		break;
	}
	if( lResult )return lResult;

	return CWinViewBase::DoMessage( nMsg, wParam, lParam );
}

//////////////////////////////////////////////////////////////////////
long stat_dependence_view::on_create( CREATESTRUCT *pcs )
{
	RECT rect = { 0,   0  , 1, 1 };

	m_wnd_hot_btn_chart.create_ex( WS_CHILD | WS_VISIBLE , rect, 0, m_hwnd, 0, "BUTTON" );
	m_wnd_hot_btn_xparam.create_ex( WS_CHILD | WS_VISIBLE , rect, 0, m_hwnd, 0, "BUTTON" );
	m_wnd_hot_btn_yparam.create_ex( WS_CHILD | WS_VISIBLE , rect, 0, m_hwnd, 0, "BUTTON" );
	m_wnd_hot_btn_axis.create_ex(		 WS_CHILD | WS_VISIBLE , rect, 0, m_hwnd, 0, "BUTTON" ); 

	INotifyControllerPtr ptr_nc = GetAppUnknown();
	if( ptr_nc )
		ptr_nc->RegisterEventListener( 0, Unknown() );

	ptr_nc = Unknown();
	if( ptr_nc )
		ptr_nc->RegisterEventListener( 0, Unknown() );

	// [vanek] - 23.12.2004 
	IVtActiveXCtrl2Ptr sptr =  Unknown();
	m_bPrintMode = sptr != 0;

	DoAttachObjects();
	return 1L;
}

//////////////////////////////////////////////////////////////////////
long stat_dependence_view::on_destroy()
{
	for( long lpos_lst = m_list_attached.head(); lpos_lst; lpos_lst = m_list_attached.next( lpos_lst ) )
	{
		IUnknownPtr ptr_object = m_list_attached.get( lpos_lst );
		if( ptr_object )
		{
			_store_to_ndata( ptr_object );
			break;
		}
	}

	_clear_attached();


	INotifyControllerPtr ptr_nc = GetAppUnknown();
	if( ptr_nc )
		ptr_nc->UnRegisterEventListener( 0, Unknown() );

	return 1L;
}

//////////////////////////////////////////////////////////////////////
long stat_dependence_view::on_paint()
{
	RECT rc_update = { 0 }, rc_client = { 0 };

	::GetUpdateRect( m_hwnd, &rc_update, false );
	::GetClientRect( m_hwnd, &rc_client );

	PAINTSTRUCT	ps = {0};

	HDC hdc_paint = ::BeginPaint( m_hwnd, &ps );	
	HDC hdc_memory = ::CreateCompatibleDC( hdc_paint );

	int nimage_width		= rc_update.right  - rc_update.left  + 1;
	int nimage_height		= rc_update.bottom - rc_update.top  + 1;

	BITMAPINFOHEADER	bmih = {0};

	bmih.biBitCount = 24;	
	bmih.biWidth	= nimage_width;
	bmih.biHeight	= nimage_height;							   
	bmih.biSize		= sizeof( bmih );
	bmih.biPlanes	= 1;							   

	byte	*pdib_bits = 0;
	HBITMAP hdib_section = ::CreateDIBSection( hdc_memory, (BITMAPINFO*)&bmih, DIB_PAL_COLORS, (void**)&pdib_bits, 0, 0 );

	HBITMAP hold_bitmap = (HBITMAP)::SelectObject( hdc_memory, hdib_section );

	::SetViewportOrgEx( hdc_memory, -rc_update.left, -rc_update.top, 0 );
	::SetBrushOrgEx( hdc_memory, -rc_update.left % 8, -rc_update.top % 8, 0 );

	HBRUSH hbrush = ::CreateSolidBrush( ::GetSysColor( COLOR_BTNFACE ) );
	::FillRect( hdc_memory, &rc_update, hbrush );
	::DeleteObject( hbrush );	hbrush = 0;

	::InflateRect( &rc_client, -SZ_BORDER, -SZ_BORDER );

	if( m_list_attached.count() > 0 )
		_do_draw( hdc_memory, rc_client );
	else{
		CString sNoDate((LPCTSTR)IDS_NO_DATA);
		::SetBkMode(hdc_memory,TRANSPARENT);
		::DrawText( hdc_memory, sNoDate, sNoDate.GetLength(), &rc_client, DT_CENTER | DT_VCENTER | DT_SINGLELINE );
	}

	::SetViewportOrgEx( hdc_memory, 0, 0, 0 );

	::SetDIBitsToDevice( hdc_paint, rc_update.left, rc_update.top, nimage_width, nimage_height,
		0, 0, 0, nimage_height, pdib_bits, (BITMAPINFO*)&bmih, DIB_RGB_COLORS );

	::SelectObject( hdc_memory, hold_bitmap );
	::DeleteObject( hdib_section );
	::DeleteObject( hdc_memory );

	::EndPaint( m_hwnd, &ps );

	return 1;
}

//////////////////////////////////////////////////////////////////////
long stat_dependence_view::on_erase_background( HDC hDC )
{
	return 1L;
}

//////////////////////////////////////////////////////////////////////
long stat_dependence_view::on_size( int cx, int cy )
{

	if( m_wnd_data.cx == cx && m_wnd_data.cy == cy )
		return 0L;

	if( m_wnd_data.bshowen )
	{
		RECT rc = {0};
		GetClientRect( m_hwnd, &rc );

		m_wnd_data.cx = rc.right;
		m_wnd_data.cy = rc.bottom;

		if( rc.right == rc.left && rc.bottom == rc.top )
			return 0;
	}  
	_recalc_layout();
	return 1L;
}

//////////////////////////////////////////////////////////////////////
long stat_dependence_view::on_context_menu( HWND hWnd, int x, int y )
{
	POINT pt;
	pt.x = x;
	pt.y = y;

	IUnknown *punk = _GetOtherComponent( GetAppUnknown(), IID_ICommandManager );

	if( !punk )
		return 0;

	ICommandManager2Ptr ptrCM = punk;
	punk->Release();	punk = 0;

	if( ptrCM == NULL )
		return 0;

	CMenuRegistrator rcm;	

	_bstr_t	bstrMenuName = rcm.GetMenu( szStatTableDependenceViewNameMenu, 0 );
	ptrCM->ExecuteContextMenu2( bstrMenuName, pt, 0 );	

	return 1L;
}

//////////////////////////////////////////////////////////////////////
HRESULT stat_dependence_view::GetMatchType( BSTR bstrObjectType, DWORD *pdwMatch )
{
	_bstr_t	bstr( bstrObjectType );

	if( !strcmp( bstr, szTypeStatTable ) )
		*pdwMatch = mvFull;
	else
		*pdwMatch = mvNone;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
void stat_dependence_view::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	CWinViewBase::OnNotify( pszEvent, punkHit, punkFrom, pdata, cbSize );

	// [vanek] : не реагируем на UpdateSystemSettings - 21.12.2004
	//if( !strcmp( pszEvent, szEventNewSettings ) )
	//{
	//	m_wnd_data.berror = false;
	//	_load_from_ndata( ::GetAppUnknown() );
	//  
	//	for( long lpos_lst = m_list_attached.head(); lpos_lst; lpos_lst = m_list_attached.next( lpos_lst ) )
	//	{
	//		IUnknownPtr ptr_object = m_list_attached.get( lpos_lst );
	//		if( ptr_object )
	//		{
	//			_store_to_ndata( ptr_object );
	//			break;
	//		}
	//	}
	//
	//	_attach_data();
	//	_set_modify();
	//}

	if( !strcmp( pszEvent, szEventNewSettingsView ) || !strcmp( pszEvent, szEventChangeObject ) && m_wnd_data.bprint_mode ||  !strcmp( pszEvent, szEventActiveContextChange ) )
	{
		if( !strcmp( pszEvent, szEventChangeObject ) )
		{
			bool b_ok = false;
			for( long lpos_lst = m_list_attached.head(); lpos_lst; lpos_lst = m_list_attached.next( lpos_lst ) )
			{
				IUnknownPtr ptr_object = m_list_attached.get( lpos_lst );
				if( ::GetKey( ptr_object ) == ::GetKey( punkFrom ) )
				{
					b_ok = true;
					break;
				}
			}
			if( !b_ok ) return;
		}

		m_wnd_data.berror = false;
		_clear_attached();
		DoAttachObjects();
	}


	if( !strcmp( pszEvent, szEventScrollPosition ) )
		if( ::GetKey( punkFrom ) == ::GetKey( Unknown() ) )
			_recalc_layout();
}

//////////////////////////////////////////////////////////////////////
HRESULT stat_dependence_view::GetPrintWidth( int nMaxWidth, int *pnReturnWidth, BOOL *pbContinue, int nUserPosX, int *pnNewUserPosX )
{
	//	m_wnd_data.block_recalc = true;
	bool bcontinue = false;

	IScrollZoomSitePtr ptr_zoom = Unknown();
	double fzoom = 1;
	ptr_zoom->GetZoom( &fzoom );

	SIZE sz_client = {0};
	ptr_zoom->GetClientSize( &sz_client );
	RECT rc = { 0, 0, long( sz_client.cx * fzoom ), long( sz_client.cy * fzoom ) };

	int nwidth = rc.right - rc.left;

	if( nwidth > nUserPosX + nMaxWidth )
	{
		*pnReturnWidth	 = nMaxWidth;
		*pnNewUserPosX	+= *pnReturnWidth;

		bcontinue = true;
	}
	else
	{
		*pnReturnWidth = nwidth - nUserPosX;
		*pnNewUserPosX = nwidth;
	}

	*pbContinue = bcontinue;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT stat_dependence_view::GetPrintHeight( int nMaxHeight, int *pnReturnHeight, BOOL *pbContinue, int nUserPosY, int *pnNewUserPosY )
{
	//	m_wnd_data.block_recalc = true;
	bool bcontinue = false;

	IScrollZoomSitePtr ptr_zoom = Unknown();
	double fzoom = 1;
	ptr_zoom->GetZoom( &fzoom );

	SIZE sz_client = {0};
	ptr_zoom->GetClientSize( &sz_client );
	RECT rc = { 0, 0, long( sz_client.cx * fzoom ), long( sz_client.cy * fzoom ) };
	int nheight = rc.bottom - rc.top;

	if( nheight > nUserPosY + nMaxHeight )
	{
		*pnReturnHeight	 = nMaxHeight;
		*pnNewUserPosY	+= *pnReturnHeight;
		bcontinue = true;
	}
	else
	{
		*pnReturnHeight = nheight - nUserPosY;
		*pnNewUserPosY = nheight;
	}

	*pbContinue = bcontinue;


	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT stat_dependence_view::Print( HDC hdc, RECT rectTarget, int nUserPosX, int nUserPosY, int nUserPosDX, int nUserPosDY, BOOL bUseScrollZoom, DWORD dwFlags )
{
	IScrollZoomSitePtr ptr_zoom = Unknown();
	double fzoom_view = 1;
	ptr_zoom->GetZoom( &fzoom_view );

	double fzoom = 1;

	if( ( rectTarget.right - rectTarget.left ) > ( rectTarget.bottom - rectTarget.top ) ) 
		fzoom = ( rectTarget.right - rectTarget.left ) / double( nUserPosDX );
	else
		fzoom = ( rectTarget.bottom - rectTarget.top ) / double( nUserPosDY );

	HRGN hrgn = ::CreateRectRgn( rectTarget.left, rectTarget.top, rectTarget.right + 1, rectTarget.bottom + 1 );

	::SelectClipRgn( hdc, hrgn );

	RECT rc_paint = { 0, 0, nUserPosX + nUserPosDX, nUserPosY + nUserPosDY };

	if( m_wnd_data.berror )
	{
		RECT rc_pt = rectTarget;

		::SetBkColor( hdc, ::GetSysColor( COLOR_BTNFACE ) );
		::SetBkMode(hdc,TRANSPARENT);
		::SetTextColor( hdc, ::GetSysColor( COLOR_BTNTEXT ) );
		::DrawText( hdc, m_wnd_data.m_str_error_descr, strlen( m_wnd_data.m_str_error_descr ), &rc_pt, DT_CENTER | DT_VCENTER | DT_SINGLELINE );
	}
	else
	{
		POINT pt_old = {0};
		::SetViewportOrgEx( hdc, int(rectTarget.left - nUserPosX * fzoom), int(rectTarget.top - nUserPosY * fzoom ), &pt_old );
		_do_draw( hdc, scale_rect( rc_paint, fzoom ), 0,  fzoom_view * fzoom, true );
		::SetViewportOrgEx( hdc, pt_old.x, pt_old.y, 0 );
	}

	::DeleteObject( hrgn );

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT stat_dependence_view::GetClassID(CLSID *pClassID )
{
	if( !pClassID )	
		return E_POINTER;

	memcpy( pClassID, &clsidStatTableDependenceView, sizeof( CLSID ) );

	return S_OK;
}


//////////////////////////////////////////////////////////////////////
HRESULT stat_dependence_view::GetFirstVisibleObjectPosition( long *plpos )
{
	if( !plpos )
		return S_FALSE;

	if( m_list_attached.count() == 0 )
		return S_FALSE;

	*plpos = m_list_attached.head();

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT stat_dependence_view::GetNextVisibleObject( IUnknown ** ppunkObject, long *plPos )
{
	if( !plPos )
		return S_FALSE;

	if( !ppunkObject )
		return S_FALSE;

	if( m_list_attached.count() == 0 )
		return S_FALSE;

	*ppunkObject = m_list_attached.get( *plPos );
	(*ppunkObject)->AddRef();

	*plPos = m_list_attached.next( *plPos );


	return S_OK;
}
//////////////////////////////////////////////////////////////////////
bool stat_dependence_view::_is_ndata_empty( IUnknownPtr sptr )
{
	INamedDataPtr sptr_nd = sptr;

	if( sptr_nd == 0 )
		return true;

	sptr_nd->SetupSection( _bstr_t( SECT_DEPENDENCE_ROOT ) );

	_variant_t var;
	sptr_nd->GetValue( _bstr_t( "Loaded" ), &var );

	if( var.vt == VT_EMPTY )
	{
		sptr_nd->SetValue( _bstr_t( "Loaded" ), _variant_t( 1L ) );
		return true;
	}

	return false;
}
//////////////////////////////////////////////////////////////////////
void stat_dependence_view::_set_modify()
{
	for( long lpos_lst = m_list_attached.head(); lpos_lst; lpos_lst = m_list_attached.next( lpos_lst ) )
	{
		IUnknownPtr ptr_object = m_list_attached.get( lpos_lst );
		if( ptr_object )
		{
			INamedDataObject2Ptr ptr_ndo = ptr_object;

			if( ptr_ndo == 0 )
				continue;

			ptr_ndo->SetModifiedFlag( TRUE );

			//set modified flag to doc
			IUnknown* punk_doc = 0;
			ptr_ndo->GetData( &punk_doc );

			if( punk_doc )
			{
				IFileDataObject2Ptr ptr_fdo2 = punk_doc;
				punk_doc->Release();	punk_doc = 0;

				if( ptr_fdo2 )
					ptr_fdo2->SetModifiedFlag( TRUE );
			}
		}
	}
}

namespace {
	struct pnt{double x,y;};
	bool less_pnt(const pnt& e1, const pnt& e2)
	{
		return e1.x<e2.x || 
			e1.x==e2.x && e1.y<e2.y;
	}
}

//////////////////////////////////////////////////////////////////////
void stat_dependence_view::_attach_data()
{
	m_lst_legend.clear();
	m_wnd_layout.m_lst_legend_layout.clear();

	m_sptr_chart->ClearAll();
	IChartAttributesPtr ptr_chart_atrr = m_sptr_chart;

	ptr_chart_atrr->SetPixelUnit( 2 );

	ptr_chart_atrr->SetConstParams( 0, ccfScaleDirectionY ) ;
	ptr_chart_atrr->SetConstParams( 2, ccfScaleDirectionX ) ;

	ptr_chart_atrr->SetConstParams( 2, ccfTextOffsetX ) ;
	ptr_chart_atrr->SetConstParams( 0, ccfTextOffsetY ) ;

	IStatTablePtr sptr_table = m_list_attached.get(m_list_attached.head() );
	map_meas_params		map_meas_params;
	CreateAllParamDescrCache( map_meas_params, sptr_table );

	double fcoef_x = 1;
	double fcoef_y = 1;

	map_meas_params::const_iterator it=map_meas_params.find(  m_chart_params.nx_param_key );
	if( it!=map_meas_params.end() )
	{
		ParameterDescriptor_ex* pmp = it->second;
		fcoef_x = pmp->fCoeffToUnits;
		m_sz_xname_param= pmp->bstrUserName && *pmp->bstrUserName?pmp->bstrUserName:pmp->bstrName;
		(m_sz_xname_param +=L",") += pmp->bstrUnit;
	}
	it=map_meas_params.find(m_chart_params.ny_param_key );
	if( it!=map_meas_params.end() )
	{
		ParameterDescriptor_ex* pmp = it->second;
		fcoef_y = pmp->fCoeffToUnits;
		m_sz_yname_param= pmp->bstrUserName && *pmp->bstrUserName?pmp->bstrUserName:pmp->bstrName;
		(m_sz_yname_param +=L",") += pmp->bstrUnit;
	}

	ptr_chart_atrr->SetStyle( CommonChartViewParam( cfAxisOnBorder | cfAbsolute ) );

	if( m_chart_params.bx_grid || m_chart_params.by_grid )
	{
		ptr_chart_atrr->SetColor( m_chart_params.cl_grid_color, ccfGridColor );
		ptr_chart_atrr->EnableGridLines( CommonChartGrid(	( m_chart_params.bx_grid ? ccgXGrid : 0 ) | 
			( m_chart_params.by_grid ? ccgYGrid : 0 ) |
			( m_chart_params.bdraw_end_chart_grid_line_x ? ccgXEndGrid : 0 ) | 
			( m_chart_params.bdraw_end_chart_grid_line_y ? ccgYEndGrid : 0 ) ) );
	}

	int nmodel = m_chart_params.nmodel_type;

	double f_abs_min_value_x = 1e308;
	double f_abs_max_value_x = -1e307;

	double f_abs_min_value_y = 1e308;
	double f_abs_max_value_y = -1e307;

	bool *pbbdepend_error = new bool[m_list_attached.count()];

	long lcurr_count = 0; // number of valid pair of points

	// [vanek] BT2000:4118 - 13.12.2004
	long lcount_sum = 0;
	for( long lpos = m_list_attached.head(), lid = 0; lpos; lpos = m_list_attached.next( lpos ), lid++  )
	{
		pbbdepend_error[lid] = false;

		IStatTablePtr sptr_table = m_list_attached.get( lpos );

		long lpos_param_x = 0, lpos_param_y = 0;

		sptr_table->GetParamPosByKey( m_chart_params.nx_param_key, &lpos_param_x );	
		sptr_table->GetParamPosByKey( m_chart_params.ny_param_key, &lpos_param_y );	

		if( !lpos_param_x || !lpos_param_y )
		{
			m_wnd_data.berror = true;
			break;
		}

		x_legend_data legend_data;

		// [vanek] BT2000:4071 get object's name - 09.12.2004
		INamedObject2Ptr sptrNO = sptr_table;
		_bstr_t bstr_name;
		if( sptrNO != 0 )
			sptrNO->GetName( bstr_name.GetAddress() );
		legend_data.sz_name = (LPCTSTR)(bstr_name);                

		long lcount = 0;
		sptr_table->GetRowCount( &lcount );

		// [vanek] BT2000:4118 - 13.12.2004
		lcurr_count = lcount;

		double fmin_value_x = 1e308;
		double fmax_value_x = -1e307;

		double fmin_value_y = 1e308;
		double fmax_value_y = -1e307;

		long lpos_row = 0; 
		sptr_table->GetFirstRowPos( &lpos_row ); 

		while( lpos_row ) 
		{ 
			stat_value *pvalue_x = 0;

			sptr_table->GetValue( lpos_row, lpos_param_x, &pvalue_x );

			stat_value *pvalue_y = 0;
			sptr_table->GetValue( lpos_row, lpos_param_y, &pvalue_y );

			// [vanek] BT2000:4118 - 13.12.2004
			if( pvalue_x && pvalue_x->bwas_defined && pvalue_y && pvalue_y->bwas_defined)
			{
				if( pvalue_x->fvalue < fmin_value_x )
					fmin_value_x = pvalue_x->fvalue;
				if( pvalue_x->fvalue > fmax_value_x )
					fmax_value_x = pvalue_x->fvalue;

				if( pvalue_y->fvalue < fmin_value_y )
					fmin_value_y = pvalue_y->fvalue;
				if( pvalue_y->fvalue > fmax_value_y )
					fmax_value_y = pvalue_y->fvalue;
			}
			else
				lcurr_count --;

			stat_row *prow = 0;
			sptr_table->GetNextRow( &lpos_row, &prow );
		}

		// [vanek] BT2000:4118 - 13.12.2004
		::SetValue( sptr_table, SECT_DEPENDENCE_RESULT_ROOT, RESULT_ALL_COUNT, lcount );
		::SetValue( sptr_table, SECT_DEPENDENCE_RESULT_ROOT, RESULT_CURR_COUNT, lcurr_count );

		if( lcurr_count != lcount )
			legend_data.sz_count_value.Format( "%d(%d)", lcurr_count, lcount );
		else
			legend_data.sz_count_value.Format( "%d", lcurr_count );

		double fa = 0, fb = 0; 
		double fcoef_correl = 0;

		if( m_chart_params.ndependence_type == 1 )
			fcoef_correl = _get_correl( sptr_table, nmodel, &fa, &fb, &pbbdepend_error[lid] );
		else if( m_chart_params.ndependence_type == 2 )
		{
			if( !lid )
			{
				double fmax_correl = -1e307;
				nmodel=-1;
				for( int i = 0; i <= 11; i++ )
				{
					double a = 0,b = 0;

					fcoef_correl = _get_correl( sptr_table, i, &a, &b,&pbbdepend_error[lid] );

					if( 0.!=fcoef_correl && fcoef_correl > fmax_correl )
					{
						nmodel = i;
						fmax_correl = fcoef_correl;
						fa = a;
						fb = b;
					}
				}
				fcoef_correl = fmax_correl;
			}
			else
				fcoef_correl = _get_correl( sptr_table, nmodel, &fa, &fb, &pbbdepend_error[lid]  );

			if( nmodel>=0 )
				pbbdepend_error[lid] = false;
		}

		__trace("nmodel=%d, fa=%g, fb=%g, fcoef_correl=%g", nmodel, fa, fb, fcoef_correl);

		::SetValue( sptr_table, SECT_DEPENDENCE_RESULT_ROOT, RESUL_MODEL, (long)nmodel );
		::SetValue( sptr_table, SECT_DEPENDENCE_RESULT_ROOT, RESUL_CORREL, (double)fcoef_correl );

		if( !pbbdepend_error[lid]  )
		{
			if( m_chart_params.ndependence_type )
				// [vanek] BT2000:4071 set precision - 09.12.2004
				legend_data.sz_corel_value.Format( "%.2lf", fcoef_correl );
			else
				legend_data.sz_corel_value = "---";
		}
		else
			legend_data.sz_corel_value = m_wnd_data.m_str_error_descr2;


		if( m_chart_params.ndependence_type )
		{
			switch( nmodel )
			{
			case 3:
			case 8:     
				fa = exp( fa );
				break;                  
			case 4:     
				fa = pow( 10, fa );
				break;			
			case 5:     
			case 9:     
				fa = pow( 10, fa );
				break;			
			}

			::SetValue( sptr_table, SECT_DEPENDENCE_RESULT_ROOT, RESUL_COEF_A, (double)fa );
			::SetValue( sptr_table, SECT_DEPENDENCE_RESULT_ROOT, RESUL_COEF_B, (double)fb );

			double fstep = ( fmax_value_x - fmin_value_x ) / 100;
			long lcount_dt = 100 + 1;

			double *pdata_x = new double[lcount_dt];
			double *pdata_y = new double[lcount_dt];

			if( !pbbdepend_error[lid] )
			{
				for( long i = 0; i < lcount_dt; i++ )
				{
					pdata_x[i] = fmin_value_x + i * fstep;
					pdata_y[i] = _get_f( nmodel, fa, fb, pdata_x[i] );
				}

				m_sptr_chart->SetData( lid, pdata_x, lcount_dt, cdfDataX );
				m_sptr_chart->SetData( lid, pdata_y, lcount_dt, cdfDataY );
			}
			else
			{
				double tmp_x = -(!m_chart_params.buser_range_x ? f_abs_max_value_x : m_chart_params.frange_max_x );
				double tmp_y = -(!m_chart_params.buser_range_y ? f_abs_max_value_y : m_chart_params.frange_max_y );

				m_sptr_chart->SetData( lid, &tmp_x, 1, cdfDataX );
				m_sptr_chart->SetData( lid, &tmp_y, 1, cdfDataY );
			}

			DWORD dw_digs  = 0;

			if( m_chart_params.bshow_axis_value_x )
				dw_digs |= cfAxisDigitsX;
			if( m_chart_params.bshow_axis_value_y )
				dw_digs |= cfAxisDigitsY;

			DWORD dw_digs2  = 0;

			if( m_chart_params.bshow_axis_labels_x )
				dw_digs2 |= cfAxisScaleX;
			if( m_chart_params.bshow_axis_labels_y )
				dw_digs2 |= cfAxisScaleY;

			ptr_chart_atrr->SetChartStyle( lid, ChartViewParam( cfAxisX | cfAxisY | dw_digs | dw_digs2 ) );

			for( long lp_lst = m_chart_params.m_lst_chart_color.head(), id = 0; lp_lst; lp_lst = m_chart_params.m_lst_chart_color.next( lp_lst ), id++ )
			{
				if( id == lid )
				{
					ptr_chart_atrr->SetChartColor( lid, m_chart_params.m_lst_chart_color.get( lp_lst ), ccfChartColor );
					break;
				}
			}

			ptr_chart_atrr->SetChartColor( lid, m_chart_params.cltext_color_axis, ccfAxisText );

			LOGFONT lf = m_chart_params.lf_font_axis_chart;
			HDC hdc = ::CreateDC( "DISPLAY", 0, 0, 0 );
			lf.lfHeight = -MulDiv( lf.lfHeight, GetDeviceCaps( hdc, LOGPIXELSY), 72);
			::DeleteDC( hdc );

			ptr_chart_atrr->SetAxisTextFont( lid, (BYTE *)&lf, ChartAxis( cafAxisX | cafAxisY ), ChartFont( cfnDigit | cfnAxisName ) );

			delete []pdata_x;
			delete []pdata_y;
		}

		if( fmin_value_x < f_abs_min_value_x )
			f_abs_min_value_x = fmin_value_x; 

		if( fmax_value_x > f_abs_max_value_x )
			f_abs_max_value_x = fmax_value_x; 

		if( fmin_value_y < f_abs_min_value_y )
			f_abs_min_value_y = fmin_value_y; 

		if( fmax_value_y > f_abs_max_value_y )
			f_abs_max_value_y = fmax_value_y; 

		if( !pbbdepend_error[lid] )
		{
			if( m_chart_params.ndependence_type )
				legend_data.sz_model_value = _get_f_str( nmodel, fa, fb );
			else
				legend_data.sz_model_value = "---";
		}
		else
			legend_data.sz_model_value = m_wnd_data.m_str_error_descr2;

		m_lst_legend.add_tail( legend_data );

		x_wnd_layout::x_legend_layout legend_layout;
		m_wnd_layout.m_lst_legend_layout.add_tail( legend_layout );

		lcount_sum += lcurr_count;
	}

	if( m_chart_params.buse_loga_scale_x ) {
		if( f_abs_min_value_x <= 0 ) {
			f_abs_min_value_x = 0.0001;
			m_wnd_data.berror = true;
			m_wnd_data.m_str_error_descr.LoadString( IDS_VAL_LOG_ERROR_X ); 
		}
		if( f_abs_max_value_x <= 0 ) {
			f_abs_max_value_x = 100.;
			m_wnd_data.berror = true;
			m_wnd_data.m_str_error_descr.LoadString( IDS_VAL_LOG_ERROR_X ); 
		}
		if( m_chart_params.buser_range_x )
		{
			if(m_chart_params.frange_min_x > 0) {
				f_abs_min_value_x = m_chart_params.frange_min_x;
			}
			if(m_chart_params.frange_max_x > 0) {
				f_abs_max_value_x = m_chart_params.frange_max_x;
			}
		}
		double proc2=pow(f_abs_max_value_x/f_abs_min_value_x, 0.02);
		f_abs_min_value_x /= proc2;
		f_abs_max_value_x *= proc2;
	}
	if( m_chart_params.buse_loga_scale_y ) {
		if( f_abs_min_value_y <= 0 ) {
			f_abs_min_value_y = 0.0001;
			m_wnd_data.berror = true;
			m_wnd_data.m_str_error_descr.LoadString( IDS_VAL_LOG_ERROR_Y ); 
		}
		if( f_abs_max_value_y <= 0 ) {
			f_abs_max_value_y = 100.;
			m_wnd_data.berror = true;
			m_wnd_data.m_str_error_descr.LoadString( IDS_VAL_LOG_ERROR_Y ); 
		}
		if( m_chart_params.buser_range_y )
		{
			if(m_chart_params.frange_min_y > 0) {
				f_abs_min_value_y = m_chart_params.frange_min_y;
			}
			if(m_chart_params.frange_max_y > 0) {
				f_abs_max_value_y = m_chart_params.frange_max_y;
			}
		}
		double proc2=pow(f_abs_max_value_y/f_abs_min_value_y, 0.02);
		f_abs_min_value_y /= proc2;
		f_abs_max_value_y *= proc2;
	}


	std::valarray<pnt> points(lcurr_count);
	size_t iPnt=0;

	// [vanek] BT2000:4118 нечего отображать - 13.12.2004
	if( !lcount_sum )
		m_wnd_data.berror = true;      
	// Draw experimental points
	for( long lpos = m_list_attached.head(), lid = 0; lpos; lpos = m_list_attached.next( lpos ), lid++  )
	{
		IStatTablePtr sptr_table = m_list_attached.get( lpos );

		long lpos_param_x = 0, lpos_param_y = 0;

		sptr_table->GetParamPosByKey( m_chart_params.nx_param_key, &lpos_param_x );	
		sptr_table->GetParamPosByKey( m_chart_params.ny_param_key, &lpos_param_y );	

		if( !lpos_param_x || !lpos_param_y )
		{
			m_wnd_data.berror = true;
			break;
		}

		long lpos_row = 0; 
		sptr_table->GetFirstRowPos( &lpos_row ); 


		while( lpos_row ) 
		{ 
			stat_value *pvalue_x = 0;

			sptr_table->GetValue( lpos_row, lpos_param_x, &pvalue_x );

			stat_value *pvalue_y = 0;
			sptr_table->GetValue( lpos_row, lpos_param_y, &pvalue_y );

			// [vanek] BT2000:4118 - 13.12.2004
			if( pvalue_x && pvalue_x->bwas_defined && pvalue_y && pvalue_y->bwas_defined)
			{
				for( long lp_lst = m_chart_params.m_lst_marker.head(), id = 0; lp_lst; lp_lst = m_chart_params.m_lst_marker.next( lp_lst ), id++ )
				{
					if( id == lid )
					{
						x_chart_params::x_marker_type marker = m_chart_params.m_lst_marker.get( lp_lst );

						double x = pvalue_x->fvalue;
						double y = pvalue_y->fvalue;

						points[iPnt].x=x;
						points[iPnt++].y=y;

						if( m_chart_params.buse_loga_scale_x )
						{
							x = ( log10( pvalue_x->fvalue / f_abs_min_value_x ) ) / ( log10( f_abs_max_value_x / f_abs_min_value_x ) );
						}

						if( m_chart_params.buse_loga_scale_y )
						{
							y = ( log10( pvalue_y->fvalue / f_abs_min_value_y ) ) / ( log10( f_abs_max_value_y / f_abs_min_value_y ) );
						}

						m_sptr_chart->AddMarker( x, y, marker.nmarker_size, marker.clmarker_color, ChartMarker( marker.nmarker_type ) );
						break;
					}
				}
			}

			stat_row *prow = 0;
			sptr_table->GetNextRow( &lpos_row, &prow );
		}
	}

	_ASSERTE(iPnt==lcurr_count);

	std::sort(&points[0], &points[lcurr_count], less_pnt );

	dblArray xs(lcurr_count), ys(lcurr_count);

	for(long ind=0; ind < lcurr_count; ++ind)
	{
		xs[ind]=points[ind].x;
		ys[ind]=points[ind].y;
	}

	if(m_chart_params.bConnectingLines){
		m_sptr_chart->SetData( 77, &xs[0], lcurr_count, cdfDataX );
		m_sptr_chart->SetData( 77, &ys[0], lcurr_count, cdfDataY );
		IChartAttributesPtr ptr_chart_at=m_sptr_chart;
		ptr_chart_at->SetChartColor( 77, m_chart_params.clr_conn_lines, ccfChartColor);
		ptr_chart_atrr->SetRangeState( 77, true, ChartRange( crfRangeX ) );
		if( m_chart_params.buse_loga_scale_x )
		{
			ptr_chart_atrr->SetChartRange( 77, 0., 1., crfRangeX );
		}
		ptr_chart_atrr->SetRangeState( 77, true, ChartRange( crfRangeY ) );
		if( m_chart_params.buse_loga_scale_y )
		{
			ptr_chart_atrr->SetChartRange( 77, 0., 1., crfRangeY );
		}
	}

	bool berror = false;
	for( long lpos = m_list_attached.head(), lid = 0; lpos; lpos = m_list_attached.next( lpos ), lid++  )
		if( pbbdepend_error[lid] )
		{
			berror = true;
			break;
		}

		if( !m_wnd_data.berror )
		{
			if( m_chart_params.ndependence_type && !berror )
			{
				for( long lpos = m_list_attached.head(), lid = 0; lpos; lpos = m_list_attached.next( lpos ), lid++  )
				{
					if( !m_chart_params.buse_loga_scale_x )
					{
						if( !m_chart_params.buser_range_x )
						{
							double frange = ( f_abs_max_value_x - f_abs_min_value_x ) * 0.02;
							ptr_chart_atrr->SetChartRange( lid, f_abs_min_value_x - frange, f_abs_max_value_x + frange, crfRangeX );
						}
						else
						{
							double frange = ( m_chart_params.frange_max_x - m_chart_params.frange_min_x ) * 0.02;
							ptr_chart_atrr->SetChartRange( lid, m_chart_params.frange_min_x - frange, m_chart_params.frange_max_x + frange, crfRangeX );
						}
					}else{
						ptr_chart_atrr->SetChartRange( lid, f_abs_min_value_x , f_abs_max_value_x, crfRangeX );
					}

					ptr_chart_atrr->SetRangeState( lid, true, ChartRange( crfRangeX ) );

					if( m_chart_params.buse_loga_scale_x )
					{
						m_sptr_chart->LogarifmScale( cdfDataX );
						ptr_chart_atrr->SetChartRange( lid, 0., 1., crfRangeX );
					}


					if( !m_chart_params.buse_loga_scale_y )
					{
						if( !m_chart_params.buser_range_y )
						{
							double frange = ( f_abs_max_value_y - f_abs_min_value_y ) * 0.02;
							ptr_chart_atrr->SetChartRange( lid, f_abs_min_value_y - frange, f_abs_max_value_y + frange, crfRangeY );
						}
						else
						{
							double frange = ( m_chart_params.frange_max_y - m_chart_params.frange_min_y ) * 0.02;
							ptr_chart_atrr->SetChartRange( lid, m_chart_params.frange_min_y - frange, m_chart_params.frange_max_y + frange, crfRangeY );
						}
					}else{
						ptr_chart_atrr->SetChartRange( lid, f_abs_min_value_y , f_abs_max_value_y, crfRangeY );
					}

					ptr_chart_atrr->SetRangeState( lid, true, ChartRange( crfRangeY ) );

					if( m_chart_params.buse_loga_scale_y )
					{
						m_sptr_chart->LogarifmScale( cdfDataY );
						ptr_chart_atrr->SetChartRange( lid, 0., 1., crfRangeY );
					}
				}
			}
			else
			{
				double tmp_x = -(!m_chart_params.buser_range_x ? f_abs_max_value_x : m_chart_params.frange_max_x );
				double tmp_y = -(!m_chart_params.buser_range_y ? f_abs_max_value_y : m_chart_params.frange_max_y );

				m_sptr_chart->SetData( -1, &tmp_x, 1, cdfDataX );
				m_sptr_chart->SetData( -1, &tmp_y, 1, cdfDataY );

				DWORD dw_digs  = 0;

				if( m_chart_params.bshow_axis_value_x )
					dw_digs |= cfAxisDigitsX;
				if( m_chart_params.bshow_axis_value_y )
					dw_digs |= cfAxisDigitsY;

				DWORD dw_digs2  = 0;

				if( m_chart_params.bshow_axis_labels_x )
					dw_digs2 |= cfAxisScaleX;
				if( m_chart_params.bshow_axis_labels_y )
					dw_digs2 |= cfAxisScaleY;

				ptr_chart_atrr->SetChartStyle( -1, ChartViewParam( cfAxisX | cfAxisY | dw_digs | dw_digs2 ) );
				ptr_chart_atrr->SetChartColor( -1, m_chart_params.cltext_color_axis, ccfAxisText );

				LOGFONT lf = m_chart_params.lf_font_axis_chart;
				HDC hdc = ::CreateDC( "DISPLAY", 0, 0, 0 );
				lf.lfHeight = -MulDiv( lf.lfHeight, GetDeviceCaps( hdc, LOGPIXELSY), 72);
				::DeleteDC( hdc );

				ptr_chart_atrr->SetAxisTextFont( -1, (BYTE *)&lf, ChartAxis( cafAxisX | cafAxisY ), ChartFont( cfnDigit | cfnAxisName ) );

				if( !m_chart_params.buser_range_x )
				{
					double frange = ( f_abs_max_value_x - f_abs_min_value_x ) * 0.02;
					ptr_chart_atrr->SetChartRange( -1, f_abs_min_value_x - frange, f_abs_max_value_x + frange, crfRangeX );
				}
				else
				{
					double frange = ( m_chart_params.frange_max_x - m_chart_params.frange_min_x ) * 0.02;
					ptr_chart_atrr->SetChartRange( -1, m_chart_params.frange_min_x - frange, m_chart_params.frange_max_x + frange, crfRangeX );
				}

				ptr_chart_atrr->SetRangeState( -1, true, ChartRange( crfRangeX ) );

				if( m_chart_params.buse_loga_scale_x )
				{
					m_sptr_chart->LogarifmScale( cdfDataX );
					ptr_chart_atrr->SetChartRange( -1, 0, 1, crfRangeX );
				}

				if( !m_chart_params.buser_range_y )
				{
					double frange = ( f_abs_max_value_y - f_abs_min_value_y ) * 0.02;
					ptr_chart_atrr->SetChartRange( -1, f_abs_min_value_y - frange, f_abs_max_value_y + frange, crfRangeY );
				}
				else
				{
					double frange = ( m_chart_params.frange_max_y - m_chart_params.frange_min_y ) * 0.02;
					ptr_chart_atrr->SetChartRange( -1, m_chart_params.frange_min_y - frange, m_chart_params.frange_max_y + frange, crfRangeY );
				}

				ptr_chart_atrr->SetRangeState( -1, true, ChartRange( crfRangeY ) );

				if( m_chart_params.buse_loga_scale_y )
				{
					m_sptr_chart->LogarifmScale( cdfDataY );
					ptr_chart_atrr->SetChartRange( -1, 0, 1, crfRangeY );
				}

			}

			if( m_chart_params.buser_range_x && !m_chart_params.buse_loga_scale_x )
			{
				f_abs_min_value_x = m_chart_params.frange_min_x; 
				f_abs_max_value_x = m_chart_params.frange_max_x; 
			}

			if( m_chart_params.buser_range_y && !m_chart_params.buse_loga_scale_y )
			{
				f_abs_min_value_y = m_chart_params.frange_min_y; 
				f_abs_max_value_y = m_chart_params.frange_max_y; 
			}

			for( lid = -1; lid < m_list_attached.count(); lid++  )
			{
				if( m_chart_params.bshow_axis_labels_x || m_chart_params.bshow_axis_labels_y )
					_manage_labels( lid, f_abs_min_value_x, f_abs_max_value_x, f_abs_min_value_y, f_abs_max_value_y, 1, 1 );

				if( m_chart_params.bshow_axis_value_x || m_chart_params.bshow_axis_value_y ){
					_manage_values( lid, f_abs_min_value_x * fcoef_x, f_abs_max_value_x * fcoef_x, f_abs_min_value_y * fcoef_y, f_abs_max_value_y * fcoef_y
						, fcoef_x, fcoef_y, map_meas_params);
				}

				if( m_chart_params.bx_grid || m_chart_params.by_grid )
					_manage_grid( lid, f_abs_min_value_x, f_abs_max_value_x, f_abs_min_value_y, f_abs_max_value_y, 1, 1 );
			}
		}

		if( pbbdepend_error ) 
			delete []pbbdepend_error;

		_recalc_layout();
}
//////////////////////////////////////////////////////////////////////
void stat_dependence_view::_recalc_layout()
{
	if( m_wnd_data.block_recalc )
		return;

	m_wnd_data.block_recalc = true;

	IScrollZoomSitePtr sptrZoom = Unknown();
	POINT pt_scroll = {0};
	double fzoom_view = 1;

	if( sptrZoom )
	{
		// [vanek] : как в StatObjectView - если не в печати, то устанавливаем единичный зум, т.к.
		// при создании вьюхи ей проставляется зум предыдущей вью (см. CSplitterBase::ChangeViewType) - 23.12.2004
		if( m_bPrintMode )
			sptrZoom->GetZoom( &fzoom_view );
		else
			sptrZoom->SetZoom( 1. );

		sptrZoom->GetScrollPos( &pt_scroll );
	}

	RECT rc_client = {0};
	::GetClientRect( m_hwnd, &rc_client );
	::InflateRect( &rc_client, -SZ_BORDER, -SZ_BORDER );

	_calc_min_rect( m_wnd_data.bprint_mode );

	rc_client.right = __max( rc_client.right, m_rc_min_rect.right - SZ_BORDER );
	rc_client.bottom = __max( rc_client.bottom, m_rc_min_rect.bottom - SZ_BORDER );

	if( fzoom_view > 1 )
	{
		rc_client.right		= long( rc_client.right / fzoom_view );
		rc_client.bottom	= long( rc_client.bottom / fzoom_view );
	}

	::OffsetRect( &rc_client, -pt_scroll.x, -pt_scroll.y );

	m_rc_title.left		= rc_client.left		+ __max( m_wnd_layout.yparam_w, SZ_HOT_BTN_FULL );
	m_rc_title.top		= rc_client.top;
	m_rc_title.right	= rc_client.right - SZ_HOT_BTN_FULL;
	m_rc_title.bottom	= m_rc_title.top	+ __max( m_wnd_layout.title_h, SZ_HOT_BTN_FULL );


	m_rc_yparam.left			= rc_client.left	;
	m_rc_yparam.top			= m_rc_title.bottom;
	m_rc_yparam.right		= m_rc_title.left;
	m_rc_yparam.bottom	= rc_client.bottom - m_wnd_layout.legend_text_h - __max( m_wnd_layout.xparam_h, SZ_HOT_BTN_FULL );

	m_rc_chart.left			=  m_rc_title.left;
	m_rc_chart.top			=  m_rc_title.bottom;
	m_rc_chart.right			=  m_rc_title.right;
	m_rc_chart.bottom		=  m_rc_yparam.bottom;

	m_rc_xparam.left		=  m_rc_title.left;
	m_rc_xparam.top			=  m_rc_chart.bottom;
	m_rc_xparam.right		=  m_rc_chart.right;
	m_rc_xparam.bottom		=  m_rc_xparam.top + __max( m_wnd_layout.xparam_h, SZ_HOT_BTN_FULL );

	// [vanek] BT2000:4071 display object's name - 09.12.2004
	m_rc_name_text.left		=	m_rc_title.left + SZ_PARAM_TEXT_OFFSET;
	m_rc_name_text.top		=	m_rc_xparam.bottom;
	m_rc_name_text.right	=	m_rc_name_text.left + m_wnd_layout.legend_text_w;
	m_rc_name_text.bottom	=	m_rc_name_text.top + m_wnd_layout.legend_text_h / 4;

	m_rc_count_text.left	=	m_rc_name_text.left;
	m_rc_count_text.top		=	m_rc_name_text.bottom;
	m_rc_count_text.right	=	m_rc_name_text.right;
	m_rc_count_text.bottom	=	m_rc_count_text.top + m_rc_name_text.bottom - m_rc_name_text.top;

	m_rc_model_text.left	=	m_rc_count_text.left;
	m_rc_model_text.top		=	m_rc_count_text.bottom;
	m_rc_model_text.right	=	m_rc_count_text.right;
	m_rc_model_text.bottom	=	m_rc_model_text.top + m_rc_count_text.bottom - m_rc_count_text.top;

	m_rc_corel_text.left	=	m_rc_model_text.left;
	m_rc_corel_text.top		=	m_rc_model_text.bottom;
	m_rc_corel_text.right	=	m_rc_model_text.right;
	m_rc_corel_text.bottom	=	rc_client.bottom;

	long nlast_name_x = m_rc_name_text.right;
	long nlast_count_x = m_rc_count_text.right;
	long nlast_model_x = m_rc_model_text.right;
	long nlast_corel_x = m_rc_corel_text.right;

	for( long lpos = m_lst_legend.head(), lid = 0; lpos; lpos = m_lst_legend.next( lpos ), lid++ )
	{
		x_legend_data &legend_data = m_lst_legend.get( lpos );

		for( long lp = m_wnd_layout.m_lst_legend_layout.head(), id = 0; lp; lp = m_wnd_layout.m_lst_legend_layout.next( lp ), id++ )
		{
			if( lid == id )
			{
				x_wnd_layout::x_legend_layout &legend_layout = m_wnd_layout.m_lst_legend_layout.get( lp );

				legend_data.rc_name.left	= nlast_name_x + SZ_PARAM_TEXT_VALUE_OFFSET;
				legend_data.rc_name.top		= m_rc_name_text.top;
				legend_data.rc_name.right	= legend_data.rc_name.left + legend_layout.legend_value_w;
				legend_data.rc_name.bottom	= m_rc_name_text.bottom;
				nlast_name_x = legend_data.rc_name.right;

				legend_data.rc_count_value.left		= nlast_count_x + SZ_PARAM_TEXT_VALUE_OFFSET;
				legend_data.rc_count_value.top		= m_rc_count_text.top;
				legend_data.rc_count_value.right	= legend_data.rc_count_value.left + legend_layout.legend_value_w;
				legend_data.rc_count_value.bottom	= m_rc_count_text.bottom;
				nlast_count_x = legend_data.rc_count_value.right;

				legend_data.rc_model_value.left		= nlast_model_x + SZ_PARAM_TEXT_VALUE_OFFSET;
				legend_data.rc_model_value.top		= m_rc_model_text.top;
				legend_data.rc_model_value.right	= legend_data.rc_model_value.left + legend_layout.legend_value_w;
				legend_data.rc_model_value.bottom	= m_rc_model_text.bottom;
				nlast_model_x = legend_data.rc_model_value.right;

				legend_data.rc_corel_value.left		= nlast_corel_x + SZ_PARAM_TEXT_VALUE_OFFSET;
				legend_data.rc_corel_value.top		= m_rc_corel_text.top;
				legend_data.rc_corel_value.right	= legend_data.rc_corel_value.left + legend_layout.legend_value_w;
				legend_data.rc_corel_value.bottom	= m_rc_corel_text.bottom;
				nlast_corel_x = legend_data.rc_corel_value.right;

				break;
			}
		}
	}

	RECT rc_hot_chart = {0};
	RECT rc_hot_yparam = {0};
	RECT rc_hot_xparam = {0};
	RECT rc_hot_axis = {0};


	rc_hot_chart.left			= ( m_rc_title.right + rc_client.right ) / 2 - long( SZ_HOT_BTN / 2. + 0.5 );
	rc_hot_chart.top			= ( m_rc_title.bottom + m_rc_title.top ) / 2 - long( SZ_HOT_BTN / 2. + 0.5 );
	rc_hot_chart.right		= rc_hot_chart.left + SZ_HOT_BTN;
	rc_hot_chart.bottom		= rc_hot_chart.top +  SZ_HOT_BTN;

	rc_hot_yparam.right			= ( m_rc_yparam.right + m_rc_yparam.left ) / 2 + long( SZ_HOT_BTN / 2. + 0.5 ) ;
	rc_hot_yparam.top			= rc_hot_chart.top;
	rc_hot_yparam.left			= rc_hot_yparam.right - SZ_HOT_BTN;
	rc_hot_yparam.bottom		= rc_hot_chart.bottom;

	rc_hot_xparam.left			= rc_hot_chart.left;
	rc_hot_xparam.top			= ( m_rc_xparam.bottom + m_rc_xparam.top ) / 2 - long( SZ_HOT_BTN / 2. + 0.5 );
	rc_hot_xparam.right			= rc_hot_chart.right;
	rc_hot_xparam.bottom		= rc_hot_xparam.top + SZ_HOT_BTN;

	rc_hot_axis.left			= rc_hot_chart.left;
	rc_hot_axis.bottom		= m_rc_chart.bottom - SZ_HOT_BTN_OFFSET - m_wnd_layout.chart_border_h;
	rc_hot_axis.right			= rc_hot_chart.right;
	rc_hot_axis.top			= rc_hot_axis.bottom - SZ_HOT_BTN;

	if( m_chart_params.benable_chart_area && m_list_attached.count() )
		::ShowWindow( m_wnd_hot_btn_chart.handle(), SW_SHOW );
	else
		::ShowWindow( m_wnd_hot_btn_chart.handle(), SW_HIDE );

	if( m_chart_params.benable_xparam_area && m_list_attached.count() )
		::ShowWindow( m_wnd_hot_btn_xparam.handle(), SW_SHOW );
	else
		::ShowWindow( m_wnd_hot_btn_xparam.handle(), SW_HIDE );

	if( m_chart_params.benable_yparam_area && m_list_attached.count() )
		::ShowWindow( m_wnd_hot_btn_yparam.handle(), SW_SHOW );
	else
		::ShowWindow( m_wnd_hot_btn_yparam.handle(), SW_HIDE );

	if( m_chart_params.benable_axis_area && m_list_attached.count() )
		::ShowWindow( m_wnd_hot_btn_axis.handle(), SW_SHOW );
	else
		::ShowWindow( m_wnd_hot_btn_axis.handle(), SW_HIDE );

	if( m_chart_params.benable_chart_area )
		m_wnd_hot_btn_chart.move_window( rc_hot_chart ) ;
	if( m_chart_params.benable_xparam_area )
		m_wnd_hot_btn_xparam.move_window( rc_hot_xparam ) ;
	if( m_chart_params.benable_yparam_area )
		m_wnd_hot_btn_yparam.move_window( rc_hot_yparam ) ;
	if( m_chart_params.benable_axis_area )
		m_wnd_hot_btn_axis.move_window( rc_hot_axis ) ;

	if( m_wnd_data.berror  )
	{
		::ShowWindow( m_wnd_hot_btn_chart.handle(), SW_HIDE );
		::ShowWindow( m_wnd_hot_btn_xparam.handle(), SW_HIDE );
		::ShowWindow( m_wnd_hot_btn_yparam.handle(), SW_HIDE );
		::ShowWindow( m_wnd_hot_btn_axis.handle(), SW_HIDE );

		::InvalidateRect( m_hwnd, 0, true );
	}

	::InvalidateRect( m_hwnd, 0, false );

	m_wnd_data.block_recalc = false;

}
//////////////////////////////////////////////////////////////////////
void stat_dependence_view::_calc_min_rect( bool bprint_mode )
{
	RECT rc_client_full = {0};
	RECT rc_client = {0};
	::GetClientRect( m_hwnd, &rc_client );
	rc_client_full = rc_client; 
	::InflateRect( &rc_client, -SZ_BORDER, -SZ_BORDER );

	HDC hdc = ::GetDC( m_hwnd );
	_do_draw( hdc, rc_client, true );
	::ReleaseDC( m_hwnd, hdc );

	NULLING( m_rc_min_rect );

	const int nmin_chart_cx = 300;
	const int nmin_chart_cy = 200;

	long nfull_legend_x = 0;
	long nfull_legend_y = 0;

	for( long lp = m_wnd_layout.m_lst_legend_layout.head(), id = 0; lp; lp = m_wnd_layout.m_lst_legend_layout.next( lp ), id++ )
	{
		x_wnd_layout::x_legend_layout &legend_layout = m_wnd_layout.m_lst_legend_layout.get( lp );

		nfull_legend_x += legend_layout.legend_value_w;

		if( lp != m_wnd_layout.m_lst_legend_layout.tail() )
			nfull_legend_x += SZ_PARAM_TEXT_VALUE_OFFSET;

		nfull_legend_y = __max( nfull_legend_y, legend_layout.legend_value_h );
	}

	m_rc_min_rect.right =	SZ_BORDER + 
		__max(  SZ_HOT_BTN_FULL, m_wnd_layout.yparam_w ) +
		__max( __max( __max( m_wnd_layout.title_w, m_wnd_layout.xparam_w ), m_wnd_layout.legend_text_w + SZ_BORDER + nfull_legend_x ), __max( nmin_chart_cx, m_wnd_layout.chart_w ) ) + 
		SZ_HOT_BTN_FULL +
		SZ_BORDER;


	m_rc_min_rect.bottom =	SZ_BORDER + 
		__max(  SZ_HOT_BTN_FULL, m_wnd_layout.title_h ) + 
		__max(m_wnd_layout.yparam_h, 
		__max(nmin_chart_cy, m_wnd_layout.chart_h) ) +
		__max(  SZ_HOT_BTN_FULL, m_wnd_layout.xparam_h ) + 
		__max( m_wnd_layout.legend_text_h, nfull_legend_y ) + 
		SZ_BORDER;

	// [vanek] SBT:1200 - 06.12.2004
	SIZE size_new = {0};
	if( 1 || bprint_mode )
	{
		size_new.cx = __max( m_rc_min_rect.right, rc_client_full.right);
		size_new.cy = __max( m_rc_min_rect.bottom, rc_client_full.bottom );
	}
	else
	{
		size_new.cx = m_rc_min_rect.right;
		size_new.cy = m_rc_min_rect.bottom;
	}

	IScrollZoomSitePtr sptr_zoom = Unknown();
	sptr_zoom->SetClientSize( size_new );

}
//////////////////////////////////////////////////////////////////////
void stat_dependence_view::_do_draw( HDC hdc, RECT rc_paint, bool bcalc, double fzoom, bool bprint  )
{
	HDC hdc_disp = CreateDC( "DISPLAY", 0, 0, 0 );
	if( !bcalc && m_wnd_data.berror )
	{
		HBRUSH hbrush = ::CreateSolidBrush( ::GetSysColor( COLOR_BTNFACE ) );

		if( !bprint )
		{
			::FillRect( hdc, &scale_rect( rc_paint, fzoom ), hbrush );
			::SetBkColor( hdc, ::GetSysColor( COLOR_BTNFACE ) );
		}

		RECT rc = scale_rect( rc_paint, fzoom );
		::DrawText( hdc, m_wnd_data.m_str_error_descr, strlen( m_wnd_data.m_str_error_descr ), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE );

		HBRUSH hold_rush = (HBRUSH)::SelectObject( hdc, GetStockObject( NULL_BRUSH ) );
		::Rectangle( hdc, rc.left, rc.top, rc.right - 1, rc.bottom - 1 );
		::SelectObject( hdc, hold_rush );
		::DeleteObject( hbrush );	hbrush = 0;
		return;
	}

	LOGFONT lf = m_chart_params.lf_font_title;
	lf.lfHeight = long(-MulDiv( lf.lfHeight, GetDeviceCaps( hdc_disp, LOGPIXELSY), 72)  * fzoom );
	HFONT hfont_title = ::CreateFontIndirect( &lf );

	lf = m_chart_params.lf_font_legend;
	lf.lfHeight = long(-MulDiv( lf.lfHeight, GetDeviceCaps( hdc_disp, LOGPIXELSY), 72) * fzoom );
	HFONT hfont_legend = ::CreateFontIndirect( &lf );

	lf = m_chart_params.lf_font_axis_chart;
	lf.lfHeight = long(-MulDiv( lf.lfHeight, GetDeviceCaps( hdc_disp, LOGPIXELSY), 72) * fzoom );
	HFONT hfont_axis = ::CreateFontIndirect( &lf );

	lf = m_chart_params.lf_font_param;
	lf.lfHeight = long(-MulDiv( lf.lfHeight, GetDeviceCaps( hdc_disp, LOGPIXELSY), 72) * fzoom );
	HFONT hfont_param_x = ::CreateFontIndirect( &lf );

	lf.lfOrientation = 900;
	lf.lfEscapement = 900;
	HFONT hfont_param_y = ::CreateFontIndirect( &lf );

	HFONT hold_font = (HFONT)::SelectObject( hdc, hfont_title );
	::SetTextColor( hdc, m_chart_params.cltext_color_title );
	::SetBkMode( hdc, TRANSPARENT );

	if( !bcalc )
		::DrawText( hdc, m_chart_params.str_title, m_chart_params.str_title.GetLength(), &scale_rect( m_rc_title, fzoom ), DT_CENTER | DT_VCENTER );
	else
	{
		RECT rc_calc = {0};
		::DrawText( hdc, m_chart_params.str_title, m_chart_params.str_title.GetLength(), &rc_calc,  DT_LEFT | DT_TOP | DT_CALCRECT );
		m_wnd_layout.title_w	= rc_calc.right;
		m_wnd_layout.title_h	= rc_calc.bottom;
	}

	::SelectObject( hdc, hfont_param_x );
	::SetTextColor( hdc, m_chart_params.cltext_color_param );

	if( !bcalc )
		::DrawText( hdc, m_sz_xname_param, strlen( m_sz_xname_param ), &scale_rect( m_rc_xparam, fzoom ), DT_CENTER | DT_VCENTER | DT_SINGLELINE );
	else
	{
		RECT rc_calc = {0};
		::DrawText( hdc, m_sz_xname_param, strlen( m_sz_xname_param ), &rc_calc,  DT_LEFT | DT_TOP | DT_SINGLELINE | DT_CALCRECT );

		m_wnd_layout.xparam_w	= rc_calc.right;
		m_wnd_layout.xparam_h	= rc_calc.bottom;
	}

	::SelectObject( hdc, hfont_param_y );

	if( !bcalc )
	{
		RECT rc_draw = m_rc_yparam;
		rc_draw.top = long( ( m_rc_yparam.bottom + m_rc_yparam.top ) / 2 + long( m_wnd_layout.yparam_h ) / 2 );

		::DrawText( hdc, m_sz_yname_param, strlen( m_sz_yname_param ), &scale_rect( rc_draw, fzoom ), DT_LEFT | DT_TOP | DT_SINGLELINE | DT_NOCLIP );
	}
	else
	{
		RECT rc_calc = {0};
		::DrawText( hdc, m_sz_yname_param, strlen( m_sz_yname_param ), &rc_calc, DT_LEFT | DT_TOP | DT_SINGLELINE | DT_CALCRECT );

		m_wnd_layout.yparam_h	= rc_calc.right;
		m_wnd_layout.yparam_w	= rc_calc.bottom;
	}

	::SelectObject( hdc, hfont_legend );
	::SetTextColor( hdc, m_chart_params.cltext_color_legend );

	/* текст легенды */
	if( !bcalc )
		::DrawText( hdc, m_sz_name_text, strlen( m_sz_name_text ), &scale_rect( m_rc_name_text, fzoom ), DT_LEFT | DT_VCENTER | DT_SINGLELINE );
	else
	{
		RECT rc_calc = {0};
		::DrawText( hdc, m_sz_name_text, strlen( m_sz_name_text ), &rc_calc, DT_LEFT | DT_TOP | DT_SINGLELINE | DT_CALCRECT );

		m_wnd_layout.legend_text_w	= rc_calc.right;
		m_wnd_layout.legend_text_h	= rc_calc.bottom;
	}

	if( !bcalc )
		::DrawText( hdc, m_sz_count_text, strlen( m_sz_count_text ), &scale_rect( m_rc_count_text, fzoom ), DT_LEFT | DT_VCENTER | DT_SINGLELINE );
	else
	{
		RECT rc_calc = {0};
		::DrawText( hdc, m_sz_count_text, strlen( m_sz_count_text ), &rc_calc, DT_LEFT | DT_TOP | DT_SINGLELINE | DT_CALCRECT );

		m_wnd_layout.legend_text_w	= __max( m_wnd_layout.legend_text_w, rc_calc.right );
		m_wnd_layout.legend_text_h	+= rc_calc.bottom;
	}

	if( !bcalc )
		::DrawText( hdc, m_sz_model_text, strlen( m_sz_model_text ), &scale_rect( m_rc_model_text, fzoom ), DT_LEFT | DT_VCENTER | DT_SINGLELINE );
	else
	{
		RECT rc_calc = {0};
		::DrawText( hdc, m_sz_model_text, strlen( m_sz_model_text ), &rc_calc, DT_LEFT | DT_TOP | DT_SINGLELINE | DT_CALCRECT );

		m_wnd_layout.legend_text_w	= __max( m_wnd_layout.legend_text_w, rc_calc.right );
		m_wnd_layout.legend_text_h	+= rc_calc.bottom;
	}

	if( !bcalc )
		::DrawText( hdc, m_sz_corel_text, strlen( m_sz_corel_text ), &scale_rect( m_rc_corel_text, fzoom ), DT_LEFT | DT_VCENTER | DT_SINGLELINE );
	else
	{
		RECT rc_calc = {0};
		::DrawText( hdc, m_sz_corel_text, strlen( m_sz_corel_text ), &rc_calc, DT_LEFT | DT_TOP | DT_SINGLELINE | DT_CALCRECT );

		m_wnd_layout.legend_text_w	= __max( m_wnd_layout.legend_text_w, rc_calc.right );
		m_wnd_layout.legend_text_h	+= rc_calc.bottom;
	}

	for( long lpos = m_lst_legend.head(), lid = 0; lpos; lpos = m_lst_legend.next( lpos ), lid++ )
	{
		x_legend_data legend_data = m_lst_legend.get( lpos );

		/* значения легенды */
		if( !bcalc )
			::DrawText( hdc, legend_data.sz_name, strlen( legend_data.sz_name ), &scale_rect( legend_data.rc_name, fzoom ), DT_LEFT | DT_VCENTER | DT_SINGLELINE );
		else
		{
			RECT rc_calc = {0};
			::DrawText( hdc, legend_data.sz_name, strlen( legend_data.sz_name ), &rc_calc, DT_LEFT | DT_TOP | DT_SINGLELINE | DT_CALCRECT );

			for( long lp = m_wnd_layout.m_lst_legend_layout.head(), id = 0; lp; lp = m_wnd_layout.m_lst_legend_layout.next( lp ), id++ )
			{
				if( lid == id )
				{
					x_wnd_layout::x_legend_layout &legend_layout = m_wnd_layout.m_lst_legend_layout.get( lp );

					legend_layout.legend_value_w	= rc_calc.right;
					legend_layout.legend_value_h	= rc_calc.bottom;

					break;
				}
			}
		}

		if( !bcalc )
			::DrawText( hdc, legend_data.sz_count_value, strlen( legend_data.sz_count_value ), &scale_rect( legend_data.rc_count_value, fzoom ), DT_LEFT | DT_VCENTER | DT_SINGLELINE );
		else
		{
			RECT rc_calc = {0};
			::DrawText( hdc, legend_data.sz_count_value, strlen( legend_data.sz_count_value ), &rc_calc, DT_LEFT | DT_TOP | DT_SINGLELINE | DT_CALCRECT );

			for( long lp = m_wnd_layout.m_lst_legend_layout.head(), id = 0; lp; lp = m_wnd_layout.m_lst_legend_layout.next( lp ), id++ )
			{
				if( lid == id )
				{
					x_wnd_layout::x_legend_layout &legend_layout = m_wnd_layout.m_lst_legend_layout.get( lp );

					legend_layout.legend_value_w	= __max( legend_layout.legend_value_w, rc_calc.right );
					legend_layout.legend_value_h	+= rc_calc.bottom;

					break;
				}
			}
		}

		if( !bcalc )
			::DrawText( hdc, legend_data.sz_model_value, strlen( legend_data.sz_model_value ), &scale_rect( legend_data.rc_model_value, fzoom ), DT_LEFT | DT_VCENTER | DT_SINGLELINE );
		else
		{
			RECT rc_calc = {0};
			::DrawText( hdc, legend_data.sz_model_value, strlen( legend_data.sz_model_value ), &rc_calc, DT_LEFT | DT_TOP | DT_SINGLELINE | DT_CALCRECT );

			for( long lp = m_wnd_layout.m_lst_legend_layout.head(), id = 0; lp; lp = m_wnd_layout.m_lst_legend_layout.next( lp ), id++ )
			{
				if( lid == id )
				{
					x_wnd_layout::x_legend_layout &legend_layout = m_wnd_layout.m_lst_legend_layout.get( lp );

					legend_layout.legend_value_w	= __max( legend_layout.legend_value_w, rc_calc.right );
					legend_layout.legend_value_h	+= rc_calc.bottom;

					break;
				}
			}
		}

		if( !bcalc )
			::DrawText( hdc, legend_data.sz_corel_value, strlen( legend_data.sz_corel_value ), &scale_rect( legend_data.rc_corel_value, fzoom ), DT_LEFT | DT_VCENTER | DT_SINGLELINE );
		else
		{
			RECT rc_calc = {0};
			::DrawText( hdc, legend_data.sz_corel_value, strlen( legend_data.sz_corel_value ), &rc_calc, DT_LEFT | DT_TOP | DT_SINGLELINE | DT_CALCRECT );

			for( long lp = m_wnd_layout.m_lst_legend_layout.head(), id = 0; lp; lp = m_wnd_layout.m_lst_legend_layout.next( lp ), id++ )
			{
				if( lid == id )
				{
					x_wnd_layout::x_legend_layout &legend_layout = m_wnd_layout.m_lst_legend_layout.get( lp );

					legend_layout.legend_value_w	= __max( legend_layout.legend_value_w, rc_calc.right );
					legend_layout.legend_value_h	+= rc_calc.bottom;

					break;
				}
			}
		}
	}

	if( !bcalc )
	{
		IChartAttributesPtr sptr_chart_atrr = m_sptr_chart;

		double fz = 0;
		sptr_chart_atrr->GetTextZoom( &fz );

		long nr = 0, nl = 0, nt = 0, nb = 0, nsh = 0;

		sptr_chart_atrr->GetConstParams( &nl, ccfLBorder );
		sptr_chart_atrr->GetConstParams( &nb, ccfBBorder );
		sptr_chart_atrr->GetConstParams( &nt, ccfTBorder );
		sptr_chart_atrr->GetConstParams( &nr, ccfRBorder );
		sptr_chart_atrr->GetConstParams( &nsh, ccfScaleHalfSize );

		sptr_chart_atrr->SetConstParams( long( ( __max( m_wnd_layout.chart_border_w + 5 + nsh, nl ) ) * fzoom + 0.5 ), ccfLBorder );
		sptr_chart_atrr->SetConstParams( long( ( __max( m_wnd_layout.chart_border_h + 5 + nsh, nb ) ) * fzoom + 0.5 ), ccfBBorder );
		sptr_chart_atrr->SetConstParams( long( nt * fzoom + 0.5 ), ccfTBorder );
		sptr_chart_atrr->SetConstParams( long( nr * fzoom + 0.5 ), ccfRBorder );
		sptr_chart_atrr->SetConstParams( long( nsh * fzoom + 0.5 ), ccfScaleHalfSize );

		sptr_chart_atrr->SetTextZoom( fzoom );
		m_sptr_chart->Draw( (DWORD)hdc, scale_rect( m_rc_chart, fzoom ) );
		sptr_chart_atrr->SetTextZoom( fz );

		sptr_chart_atrr->SetConstParams( nl, ccfLBorder );
		sptr_chart_atrr->SetConstParams( nb, ccfBBorder );
		sptr_chart_atrr->SetConstParams( nt, ccfTBorder );
		sptr_chart_atrr->SetConstParams( nr, ccfRBorder );
		sptr_chart_atrr->SetConstParams( nsh, ccfScaleHalfSize );
	}
	else
	{
		::SelectObject( hdc, hfont_axis );

		RECT rc_calc_x = {0};
		::DrawText( hdc, m_str_max_val_x, m_str_max_val_x.GetLength(), &rc_calc_x, DT_SINGLELINE | DT_CALCRECT );

		RECT rc_calc_y = {0};
		::DrawText( hdc, m_str_max_val_y, m_str_max_val_y.GetLength(), &rc_calc_y, DT_SINGLELINE | DT_CALCRECT );

		m_wnd_layout.chart_w = rc_calc_x.right - rc_calc_x.left;
		m_wnd_layout.chart_h = ( rc_calc_y.bottom - rc_calc_y.top ) * m_nchart_values_y;

		m_wnd_layout.chart_border_w	= rc_calc_y.right - rc_calc_y.left;
		m_wnd_layout.chart_border_h	= rc_calc_x.bottom - rc_calc_x.top;
	}


	if( !bcalc )
	{
		//		if( m_chart_params.benable_xparam_area )
		::DrawFocusRect( hdc, &scale_rect( m_rc_xparam, fzoom ) );
		//		if( m_chart_params.benable_yparam_area )
		::DrawFocusRect( hdc, &scale_rect( m_rc_yparam, fzoom ) );
	}

	::SelectObject( hdc, hold_font );

	::DeleteObject( hfont_title );
	::DeleteObject( hfont_legend );
	::DeleteObject( hfont_param_x );
	::DeleteObject( hfont_param_y );
	::DeleteObject( hfont_axis );

	::DeleteDC( hdc_disp );
}

//////////////////////////////////////////////////////////////////////
void stat_dependence_view::_load_from_ndata(  INamedDataPtr sptr_nd )
{
	m_wnd_hot_btn_chart.set_action( (char*)::GetValueString( sptr_nd, SECT_DEPENDENCE_ROOT, FORM_NAME_CHART_AREA, "" ) );
	m_wnd_hot_btn_axis.set_action( (char*)::GetValueString( sptr_nd, SECT_DEPENDENCE_ROOT, FORM_NAME_AXIS_AREA, "" ) );
	m_wnd_hot_btn_yparam.set_action( (char*)::GetValueString( sptr_nd, SECT_DEPENDENCE_ROOT, FORM_NAME_YPARAM_AREA, "" ) );
	m_wnd_hot_btn_xparam.set_action( (char*)::GetValueString( sptr_nd, SECT_DEPENDENCE_ROOT, FORM_NAME_XPARAM_AREA, "" ) );

	m_wnd_hot_btn_chart.set_index( ::GetValueInt( sptr_nd, SECT_DEPENDENCE_ROOT, INDEX_CHART_AREA, 0 ) );
	m_wnd_hot_btn_axis.set_index( ::GetValueInt( sptr_nd, SECT_DEPENDENCE_ROOT, INDEX_AXIS_AREA, 0 ) );
	m_wnd_hot_btn_yparam.set_index( ::GetValueInt( sptr_nd, SECT_DEPENDENCE_ROOT, INDEX_YPARAM_AREA, 0 ) );
	m_wnd_hot_btn_xparam.set_index( ::GetValueInt( sptr_nd, SECT_DEPENDENCE_ROOT, INDEX_XPARAM_AREA, 0 ) );

	m_chart_params.m_lst_chart_color.clear();

	COLORREF cl_chart = ::GetValueColor( sptr_nd, SECT_DEPENDENCE_CHART_ROOT, CHART_COLOR, RGB( 255, 0, 0 ) );
	m_chart_params.m_lst_chart_color.add_tail( cl_chart );

	if( m_list_attached.count() > 1 )
	{

		for( long lpos_lst = m_list_attached.next( m_list_attached.head() ); lpos_lst; lpos_lst = m_list_attached.next( lpos_lst ) )
		{
			INamedDataPtr sptr_nd2 = m_list_attached.get( lpos_lst );
			COLORREF cl_chart = ::GetValueColor( sptr_nd2, SECT_DEPENDENCE_CHART_ROOT, CHART_COLOR, RGB( 255, 0, 0 ) );
			m_chart_params.m_lst_chart_color.add_tail( cl_chart );
		}
	}

	m_chart_params.str_title = (char*)::GetValueString( sptr_nd, SECT_DEPENDENCE_CHART_ROOT, CHART_TITLE, m_chart_params.str_title );
	m_chart_params.str_title = change_chars( m_chart_params.str_title, "\\n", "\r\n" );

	//[max]Установки для формата значений по осям
	m_chart_params.m_buse_custom_format_x = 
		m_chart_params.m_buse_custom_format_y = ::GetValueInt( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_ROOT, Y_USE_CUSTOM_FORMAT, m_chart_params.m_buse_custom_format_y ) != 0;

	m_chart_params.m_str_custom_format_x = (char*)::GetValueString( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_ROOT, X_CUSTOM_FORMAT, m_chart_params.m_str_custom_format_x );
	m_chart_params.m_str_custom_format_y = (char*)::GetValueString( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_ROOT, Y_CUSTOM_FORMAT, m_chart_params.m_str_custom_format_y );

	m_chart_params.buse_loga_scale_x = ::GetValueInt( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_ROOT, X_USE_LOGA_SCALE, m_chart_params.buse_loga_scale_x ) != 0;
	m_chart_params.buse_loga_scale_y = ::GetValueInt( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_ROOT, Y_USE_LOGA_SCALE, m_chart_params.buse_loga_scale_y ) != 0;

	m_chart_params.nx_param_key = ::GetValueInt( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_ROOT, X_PARAM_KEY, m_chart_params.nx_param_key );
	m_chart_params.ny_param_key = ::GetValueInt( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_ROOT, Y_PARAM_KEY, m_chart_params.ny_param_key );
	m_chart_params.bx_grid = ::GetValueInt( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_ROOT, X_GRID, m_chart_params.bx_grid ) != 0; 
	m_chart_params.by_grid = ::GetValueInt( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_ROOT, Y_GRID, m_chart_params.by_grid ) != 0;

	m_chart_params.fx_scale_factor = ::GetValueDouble( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_ROOT, X_SCALE_FACTOR, m_chart_params.fx_scale_factor ); 
	m_chart_params.fy_scale_factor = ::GetValueDouble( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_ROOT, Y_SCALE_FACTOR, m_chart_params.fy_scale_factor );

	m_chart_params.fx_scale_factor_values = ::GetValueDouble( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_ROOT, X_SCALE_FACTOR_VALUES, m_chart_params.fx_scale_factor_values ); 
	m_chart_params.fy_scale_factor_values = ::GetValueDouble( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_ROOT, Y_SCALE_FACTOR_VALUES, m_chart_params.fy_scale_factor_values );

	m_chart_params.fx_scale_factor_grid = ::GetValueDouble( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_ROOT, X_SCALE_FACTOR_GRID, m_chart_params.fx_scale_factor_grid ); 
	m_chart_params.fy_scale_factor_grid = ::GetValueDouble( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_ROOT, Y_SCALE_FACTOR_GRID, m_chart_params.fy_scale_factor_grid );

	m_chart_params.cl_grid_color = ::GetValueColor( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_ROOT, GRID_COLOR, m_chart_params.cl_grid_color );
	m_chart_params.bshow_axis_value_x = ::GetValueInt( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_ROOT, X_SHOW_AXIS_VALUES, m_chart_params.bshow_axis_value_x ) != 0;
	m_chart_params.bshow_axis_value_y = ::GetValueInt( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_ROOT, Y_SHOW_AXIS_VALUES, m_chart_params.bshow_axis_value_y ) != 0;

	m_chart_params.bshow_axis_labels_x = ::GetValueInt( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_ROOT, X_SHOW_AXIS_LABELS, m_chart_params.bshow_axis_labels_x ) != 0;
	m_chart_params.bshow_axis_labels_y = ::GetValueInt( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_ROOT, Y_SHOW_AXIS_LABELS, m_chart_params.bshow_axis_labels_y ) != 0;

	strcpy( m_chart_params.lf_font_title.lfFaceName, (char*)::GetValueString( sptr_nd, SECT_DEPENDENCE_CHART_TITLE_FONT_ROOT, FONT_NAME , m_chart_params.lf_font_title.lfFaceName ) );
	m_chart_params.lf_font_title.lfHeight = ::GetValueInt( sptr_nd, SECT_DEPENDENCE_CHART_TITLE_FONT_ROOT, FONT_SIZE, m_chart_params.lf_font_title.lfHeight );
	m_chart_params.lf_font_title.lfWeight = ::GetValueInt( sptr_nd, SECT_DEPENDENCE_CHART_TITLE_FONT_ROOT, FONT_BOLD, m_chart_params.lf_font_title.lfWeight > 400 ? 1 : 0 ) != 0 ? 800 : 400;
	m_chart_params.lf_font_title.lfItalic = ::GetValueInt( sptr_nd, SECT_DEPENDENCE_CHART_TITLE_FONT_ROOT, FONT_ITALIC, m_chart_params.lf_font_title.lfItalic ) != 0;
	m_chart_params.lf_font_title.lfUnderline = ::GetValueInt( sptr_nd, SECT_DEPENDENCE_CHART_TITLE_FONT_ROOT, FONT_UNDERLINE, m_chart_params.lf_font_title.lfUnderline ) != 0;
	m_chart_params.lf_font_title.lfStrikeOut = ::GetValueInt( sptr_nd, SECT_DEPENDENCE_CHART_TITLE_FONT_ROOT, FONT_STRIKEOUT, m_chart_params.lf_font_title.lfStrikeOut) != 0;
	m_chart_params.cltext_color_title = ::GetValueColor( sptr_nd, SECT_DEPENDENCE_CHART_TITLE_FONT_ROOT, TEXT_COLOR , m_chart_params.cltext_color_title );

	strcpy( m_chart_params.lf_font_legend.lfFaceName, (char*)::GetValueString( sptr_nd, SECT_DEPENDENCE_CHART_LEGEND_FONT_ROOT, FONT_NAME , m_chart_params.lf_font_legend.lfFaceName ) );
	m_chart_params.lf_font_legend.lfHeight = ::GetValueInt( sptr_nd, SECT_DEPENDENCE_CHART_LEGEND_FONT_ROOT, FONT_SIZE, m_chart_params.lf_font_legend.lfHeight );
	m_chart_params.lf_font_legend.lfWeight = ::GetValueInt( sptr_nd, SECT_DEPENDENCE_CHART_LEGEND_FONT_ROOT, FONT_BOLD, m_chart_params.lf_font_legend.lfWeight > 400 ? 1 : 0 ) != 0 ? 800 : 400;
	m_chart_params.lf_font_legend.lfItalic = ::GetValueInt( sptr_nd, SECT_DEPENDENCE_CHART_LEGEND_FONT_ROOT, FONT_ITALIC, m_chart_params.lf_font_legend.lfItalic ) != 0;
	m_chart_params.lf_font_legend.lfUnderline = ::GetValueInt( sptr_nd, SECT_DEPENDENCE_CHART_LEGEND_FONT_ROOT, FONT_UNDERLINE, m_chart_params.lf_font_legend.lfUnderline ) != 0;
	m_chart_params.lf_font_legend.lfStrikeOut = ::GetValueInt( sptr_nd, SECT_DEPENDENCE_CHART_LEGEND_FONT_ROOT, FONT_STRIKEOUT, m_chart_params.lf_font_legend.lfStrikeOut) != 0;
	m_chart_params.cltext_color_legend = ::GetValueColor( sptr_nd, SECT_DEPENDENCE_CHART_LEGEND_FONT_ROOT, TEXT_COLOR , m_chart_params.cltext_color_legend );

	strcpy( m_chart_params.lf_font_axis_chart.lfFaceName, (char*)::GetValueString( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_FONT_ROOT, FONT_NAME , m_chart_params.lf_font_axis_chart.lfFaceName ) );
	m_chart_params.lf_font_axis_chart.lfHeight = ::GetValueInt( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_FONT_ROOT, FONT_SIZE, m_chart_params.lf_font_axis_chart.lfHeight );
	m_chart_params.lf_font_axis_chart.lfWeight = ::GetValueInt( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_FONT_ROOT, FONT_BOLD, m_chart_params.lf_font_axis_chart.lfWeight > 400 ? 1 : 0 ) != 0 ? 800 : 400;
	m_chart_params.lf_font_axis_chart.lfItalic = ::GetValueInt( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_FONT_ROOT, FONT_ITALIC, m_chart_params.lf_font_axis_chart.lfItalic ) != 0;
	m_chart_params.lf_font_axis_chart.lfUnderline = ::GetValueInt( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_FONT_ROOT, FONT_UNDERLINE, m_chart_params.lf_font_axis_chart.lfUnderline ) != 0;
	m_chart_params.lf_font_axis_chart.lfStrikeOut = ::GetValueInt( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_FONT_ROOT, FONT_STRIKEOUT, m_chart_params.lf_font_axis_chart.lfStrikeOut) != 0;
	m_chart_params.cltext_color_axis = ::GetValueColor( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_FONT_ROOT, TEXT_COLOR , m_chart_params.cltext_color_axis );

	strcpy( m_chart_params.lf_font_param.lfFaceName, (char*)::GetValueString( sptr_nd, SECT_DEPENDENCE_CHART_PARAM_FONT_ROOT, FONT_NAME , m_chart_params.lf_font_param.lfFaceName ) );
	m_chart_params.lf_font_param.lfHeight = ::GetValueInt( sptr_nd, SECT_DEPENDENCE_CHART_PARAM_FONT_ROOT, FONT_SIZE, m_chart_params.lf_font_param.lfHeight );
	m_chart_params.lf_font_param.lfWeight = ::GetValueInt( sptr_nd, SECT_DEPENDENCE_CHART_PARAM_FONT_ROOT, FONT_BOLD, m_chart_params.lf_font_param.lfWeight > 400 ? 1 : 0 ) != 0 ? 800 : 400;
	m_chart_params.lf_font_param.lfItalic = ::GetValueInt( sptr_nd, SECT_DEPENDENCE_CHART_PARAM_FONT_ROOT, FONT_ITALIC, m_chart_params.lf_font_param.lfItalic ) != 0;
	m_chart_params.lf_font_param.lfUnderline = ::GetValueInt( sptr_nd, SECT_DEPENDENCE_CHART_PARAM_FONT_ROOT, FONT_UNDERLINE, m_chart_params.lf_font_param.lfUnderline ) != 0;
	m_chart_params.lf_font_param.lfStrikeOut = ::GetValueInt( sptr_nd, SECT_DEPENDENCE_CHART_PARAM_FONT_ROOT, FONT_STRIKEOUT, m_chart_params.lf_font_param.lfStrikeOut) != 0;
	m_chart_params.cltext_color_param = ::GetValueColor( sptr_nd, SECT_DEPENDENCE_CHART_PARAM_FONT_ROOT, TEXT_COLOR , m_chart_params.cltext_color_param );

	m_chart_params.benable_chart_area = ::GetValueInt( sptr_nd, SECT_DEPENDENCE_ROOT, ENABLE_CHART_AREA, m_chart_params.benable_chart_area ) != 0;
	m_chart_params.benable_axis_area = ::GetValueInt( sptr_nd, SECT_DEPENDENCE_ROOT, ENABLE_AXIS_AREA, m_chart_params.benable_axis_area ) != 0;
	m_chart_params.benable_xparam_area = ::GetValueInt( sptr_nd, SECT_DEPENDENCE_ROOT, ENABLE_X_PARAMS_AREA, m_chart_params.benable_xparam_area ) != 0;
	m_chart_params.benable_yparam_area = ::GetValueInt( sptr_nd, SECT_DEPENDENCE_ROOT, ENABLE_Y_PARAMS_AREA, m_chart_params.benable_yparam_area ) != 0;

	m_chart_params.bdraw_end_chart_grid_line_x = ::GetValueInt( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_ROOT, DRAW_END_CHART_GRID_LINE_X , m_chart_params.bdraw_end_chart_grid_line_x ) != 0;
	m_chart_params.bdraw_end_chart_grid_line_y = ::GetValueInt( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_ROOT, DRAW_END_CHART_GRID_LINE_Y , m_chart_params.bdraw_end_chart_grid_line_y ) != 0;

	m_chart_params.ndependence_type = ::GetValueInt( sptr_nd, SECT_DEPENDENCE_ROOT, DEPENDENCE_TYPE, m_chart_params.ndependence_type );
	m_chart_params.nmodel_type = ::GetValueInt( sptr_nd, SECT_DEPENDENCE_ROOT, MODEL_TYPE, m_chart_params.nmodel_type );

	m_chart_params.m_lst_marker.clear();

	{
		x_chart_params::x_marker_type marker;

		marker.nmarker_type = ::GetValueInt( sptr_nd, SECT_DEPENDENCE_ROOT, MARKER_TYPE, marker.nmarker_type );
		marker.nmarker_size = ::GetValueInt( sptr_nd, SECT_DEPENDENCE_ROOT, MARKER_SIZE_2, marker.nmarker_size );
		marker.clmarker_color = ::GetValueColor( sptr_nd, SECT_DEPENDENCE_ROOT, MARKER_COLOR, marker.clmarker_color );

		m_chart_params.m_lst_marker.add_tail( marker );
	}

	if( m_list_attached.count() > 1 )
	{
		for( long lpos_lst = m_list_attached.next( m_list_attached.head() ); lpos_lst; lpos_lst = m_list_attached.next( lpos_lst ) )
		{
			INamedDataPtr sptr_nd2 = m_list_attached.get( lpos_lst );

			x_chart_params::x_marker_type marker;
			marker.nmarker_type = ::GetValueInt( sptr_nd2, SECT_DEPENDENCE_ROOT, MARKER_TYPE, marker.nmarker_type );
			marker.nmarker_size = ::GetValueInt( sptr_nd2, SECT_DEPENDENCE_ROOT, MARKER_SIZE_2, marker.nmarker_size );
			marker.clmarker_color = ::GetValueColor( sptr_nd2, SECT_DEPENDENCE_ROOT, MARKER_COLOR, marker.clmarker_color );

			m_chart_params.m_lst_marker.add_tail( marker );
		}
	}

	m_chart_params.buser_range_x		= ::GetValueInt( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_ROOT, X_USE_VALUE_RANGE, m_chart_params.buser_range_x ) != 0;
	m_chart_params.frange_min_x		= ::GetValueDouble( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_ROOT, X_BEGIN, m_chart_params.frange_min_x );
	m_chart_params.frange_max_x		= ::GetValueDouble( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_ROOT, X_END, m_chart_params.frange_max_x );

	m_chart_params.buser_range_y		= ::GetValueInt( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_ROOT, Y_USE_VALUE_RANGE, m_chart_params.buser_range_y ) != 0;
	m_chart_params.frange_min_y		= ::GetValueDouble( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_ROOT, Y_BEGIN, m_chart_params.frange_min_y );
	m_chart_params.frange_max_y		= ::GetValueDouble( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_ROOT, Y_END, m_chart_params.frange_max_y );

	m_chart_params.bConnectingLines = (::GetValueInt( sptr_nd, SECT_DEPENDENCE_ROOT, ENABLE_CONNECTING_LINES,m_chart_params.bConnectingLines ) != 0);
	m_chart_params.clr_conn_lines = ::GetValueColor( sptr_nd, SECT_DEPENDENCE_ROOT, CONN_LINES_CLR, m_chart_params.clr_conn_lines);
}
//////////////////////////////////////////////////////////////////////
void stat_dependence_view::_store_to_ndata( INamedDataPtr sptr_nd )
{
	::SetValue( sptr_nd, SECT_DEPENDENCE_ROOT, FORM_NAME_CHART_AREA, m_wnd_hot_btn_chart.get_action() );
	::SetValue( sptr_nd, SECT_DEPENDENCE_ROOT, FORM_NAME_AXIS_AREA, m_wnd_hot_btn_axis.get_action() );
	::SetValue( sptr_nd, SECT_DEPENDENCE_ROOT, FORM_NAME_XPARAM_AREA, m_wnd_hot_btn_yparam.get_action() );
	::SetValue( sptr_nd, SECT_DEPENDENCE_ROOT, FORM_NAME_YPARAM_AREA, m_wnd_hot_btn_xparam.get_action() );

	::SetValue( sptr_nd, SECT_DEPENDENCE_ROOT, INDEX_CHART_AREA, m_wnd_hot_btn_chart.get_index() );
	::SetValue( sptr_nd, SECT_DEPENDENCE_ROOT, INDEX_AXIS_AREA, m_wnd_hot_btn_axis.get_index() );
	::SetValue( sptr_nd, SECT_DEPENDENCE_ROOT, INDEX_YPARAM_AREA, m_wnd_hot_btn_yparam.get_index() );
	::SetValue( sptr_nd, SECT_DEPENDENCE_ROOT, INDEX_XPARAM_AREA, m_wnd_hot_btn_xparam.get_index() );

	::SetValueColor( sptr_nd, SECT_DEPENDENCE_CHART_ROOT, CHART_COLOR, m_chart_params.m_lst_chart_color.get( m_chart_params.m_lst_chart_color.head() ) );

	::SetValue( sptr_nd, SECT_DEPENDENCE_CHART_ROOT, CHART_TITLE, change_chars( m_chart_params.str_title, "\r\n", "\\n" ) );

	//[max]Установки для формата значений по осям
	::SetValue( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_ROOT, X_USE_CUSTOM_FORMAT, (long)m_chart_params.m_buse_custom_format_x );
	::SetValue( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_ROOT, Y_USE_CUSTOM_FORMAT, (long)m_chart_params.m_buse_custom_format_y );

	::SetValue( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_ROOT, X_CUSTOM_FORMAT, m_chart_params.m_str_custom_format_x );
	::SetValue( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_ROOT, Y_CUSTOM_FORMAT, m_chart_params.m_str_custom_format_y );

	::SetValue( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_ROOT, X_USE_LOGA_SCALE, (long)m_chart_params.buse_loga_scale_x );
	::SetValue( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_ROOT, Y_USE_LOGA_SCALE, (long)m_chart_params.buse_loga_scale_y );

	::SetValue( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_ROOT, X_PARAM_KEY, (long)m_chart_params.nx_param_key );
	::SetValue( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_ROOT, Y_PARAM_KEY, (long)m_chart_params.ny_param_key );
	::SetValue( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_ROOT, X_GRID, (long)m_chart_params.bx_grid ); 
	::SetValue( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_ROOT, Y_GRID, (long)m_chart_params.by_grid );

	::SetValue( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_ROOT, X_SCALE_FACTOR, m_chart_params.fx_scale_factor ); 
	::SetValue( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_ROOT, Y_SCALE_FACTOR, m_chart_params.fy_scale_factor );

	::SetValue( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_ROOT, X_SCALE_FACTOR_VALUES, m_chart_params.fx_scale_factor_values ); 
	::SetValue( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_ROOT, Y_SCALE_FACTOR_VALUES, m_chart_params.fy_scale_factor_values );

	::SetValue( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_ROOT, X_SCALE_FACTOR_GRID, m_chart_params.fx_scale_factor_grid ); 
	::SetValue( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_ROOT, Y_SCALE_FACTOR_GRID, m_chart_params.fy_scale_factor_grid );

	::SetValueColor( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_ROOT, GRID_COLOR, m_chart_params.cl_grid_color );
	::SetValue( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_ROOT, X_SHOW_AXIS_VALUES, (long)m_chart_params.bshow_axis_value_x );
	::SetValue( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_ROOT, Y_SHOW_AXIS_VALUES, (long)m_chart_params.bshow_axis_value_y );
	::SetValue( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_ROOT, X_SHOW_AXIS_LABELS, (long)m_chart_params.bshow_axis_labels_x );
	::SetValue( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_ROOT, Y_SHOW_AXIS_LABELS, (long)m_chart_params.bshow_axis_labels_y );

	::SetValue( sptr_nd, SECT_DEPENDENCE_CHART_TITLE_FONT_ROOT, FONT_NAME , m_chart_params.lf_font_title.lfFaceName );
	::SetValue( sptr_nd, SECT_DEPENDENCE_CHART_TITLE_FONT_ROOT, FONT_SIZE, (long)( m_chart_params.lf_font_title.lfHeight ) );
	::SetValue( sptr_nd, SECT_DEPENDENCE_CHART_TITLE_FONT_ROOT, FONT_BOLD, (long)( m_chart_params.lf_font_title.lfWeight > 400 ? 1 : 0 ) );
	::SetValue( sptr_nd, SECT_DEPENDENCE_CHART_TITLE_FONT_ROOT, FONT_ITALIC, (long)m_chart_params.lf_font_title.lfItalic );
	::SetValue( sptr_nd, SECT_DEPENDENCE_CHART_TITLE_FONT_ROOT, FONT_UNDERLINE, (long)m_chart_params.lf_font_title.lfUnderline );
	::SetValue( sptr_nd, SECT_DEPENDENCE_CHART_TITLE_FONT_ROOT, FONT_STRIKEOUT, (long)m_chart_params.lf_font_title.lfStrikeOut);
	::SetValueColor( sptr_nd, SECT_DEPENDENCE_CHART_TITLE_FONT_ROOT, TEXT_COLOR , m_chart_params.cltext_color_title );

	::SetValue( sptr_nd, SECT_DEPENDENCE_CHART_LEGEND_FONT_ROOT, FONT_NAME , m_chart_params.lf_font_legend.lfFaceName );
	::SetValue( sptr_nd, SECT_DEPENDENCE_CHART_LEGEND_FONT_ROOT, FONT_SIZE, (long)( m_chart_params.lf_font_legend.lfHeight ) );
	::SetValue( sptr_nd, SECT_DEPENDENCE_CHART_LEGEND_FONT_ROOT, FONT_BOLD, (long)( m_chart_params.lf_font_legend.lfWeight > 400 ? 1 : 0 ) );
	::SetValue( sptr_nd, SECT_DEPENDENCE_CHART_LEGEND_FONT_ROOT, FONT_ITALIC, (long)m_chart_params.lf_font_legend.lfItalic );
	::SetValue( sptr_nd, SECT_DEPENDENCE_CHART_LEGEND_FONT_ROOT, FONT_UNDERLINE, (long)m_chart_params.lf_font_legend.lfUnderline );
	::SetValue( sptr_nd, SECT_DEPENDENCE_CHART_LEGEND_FONT_ROOT, FONT_STRIKEOUT, (long)m_chart_params.lf_font_legend.lfStrikeOut);
	::SetValueColor( sptr_nd, SECT_DEPENDENCE_CHART_LEGEND_FONT_ROOT, TEXT_COLOR , m_chart_params.cltext_color_legend );

	::SetValue( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_FONT_ROOT, FONT_NAME , m_chart_params.lf_font_axis_chart.lfFaceName );
	::SetValue( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_FONT_ROOT, FONT_SIZE, (long)( m_chart_params.lf_font_axis_chart.lfHeight ) );
	::SetValue( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_FONT_ROOT, FONT_BOLD, (long)( m_chart_params.lf_font_axis_chart.lfWeight > 400 ? 1 : 0 ) );
	::SetValue( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_FONT_ROOT, FONT_ITALIC, (long)m_chart_params.lf_font_axis_chart.lfItalic );
	::SetValue( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_FONT_ROOT, FONT_UNDERLINE, (long)m_chart_params.lf_font_axis_chart.lfUnderline );
	::SetValue( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_FONT_ROOT, FONT_STRIKEOUT, (long)m_chart_params.lf_font_axis_chart.lfStrikeOut);
	::SetValueColor( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_FONT_ROOT, TEXT_COLOR , m_chart_params.cltext_color_axis );

	::SetValue( sptr_nd, SECT_DEPENDENCE_CHART_PARAM_FONT_ROOT, FONT_NAME , m_chart_params.lf_font_param.lfFaceName );
	::SetValue( sptr_nd, SECT_DEPENDENCE_CHART_PARAM_FONT_ROOT, FONT_SIZE, (long)( m_chart_params.lf_font_param.lfHeight ) );
	::SetValue( sptr_nd, SECT_DEPENDENCE_CHART_PARAM_FONT_ROOT, FONT_BOLD, (long)( m_chart_params.lf_font_param.lfWeight > 400 ? 1 : 0 ) );
	::SetValue( sptr_nd, SECT_DEPENDENCE_CHART_PARAM_FONT_ROOT, FONT_ITALIC, (long)m_chart_params.lf_font_param.lfItalic );
	::SetValue( sptr_nd, SECT_DEPENDENCE_CHART_PARAM_FONT_ROOT, FONT_UNDERLINE, (long)m_chart_params.lf_font_param.lfUnderline );
	::SetValue( sptr_nd, SECT_DEPENDENCE_CHART_PARAM_FONT_ROOT, FONT_STRIKEOUT, (long)m_chart_params.lf_font_param.lfStrikeOut);
	::SetValueColor( sptr_nd, SECT_DEPENDENCE_CHART_PARAM_FONT_ROOT, TEXT_COLOR , m_chart_params.cltext_color_param );

	::SetValue( sptr_nd, SECT_DEPENDENCE_ROOT, ENABLE_CHART_AREA, (long)m_chart_params.benable_chart_area );
	::SetValue( sptr_nd, SECT_DEPENDENCE_ROOT, ENABLE_AXIS_AREA, (long)m_chart_params.benable_axis_area );
	::SetValue( sptr_nd, SECT_DEPENDENCE_ROOT, ENABLE_X_PARAMS_AREA, (long)m_chart_params.benable_xparam_area );
	::SetValue( sptr_nd, SECT_DEPENDENCE_ROOT, ENABLE_Y_PARAMS_AREA, (long)m_chart_params.benable_yparam_area );

	::SetValue( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_ROOT, DRAW_END_CHART_GRID_LINE_X , (long)m_chart_params.bdraw_end_chart_grid_line_x );
	::SetValue( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_ROOT, DRAW_END_CHART_GRID_LINE_Y , (long)m_chart_params.bdraw_end_chart_grid_line_y );

	::SetValue( sptr_nd, SECT_DEPENDENCE_ROOT, DEPENDENCE_TYPE, (long)m_chart_params.ndependence_type );
	::SetValue( sptr_nd, SECT_DEPENDENCE_ROOT, MODEL_TYPE, (long)m_chart_params.nmodel_type );

	::SetValue( sptr_nd, SECT_DEPENDENCE_ROOT, MARKER_TYPE, (long)m_chart_params.m_lst_marker.get( m_chart_params.m_lst_marker.head() ).nmarker_type );
	::SetValue( sptr_nd, SECT_DEPENDENCE_ROOT, MARKER_SIZE_2, (long)m_chart_params.m_lst_marker.get( m_chart_params.m_lst_marker.head() ).nmarker_size );
	::SetValueColor( sptr_nd, SECT_DEPENDENCE_ROOT, MARKER_COLOR, m_chart_params.m_lst_marker.get( m_chart_params.m_lst_marker.head() ).clmarker_color );

	::SetValue( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_ROOT, X_USE_VALUE_RANGE, (long)m_chart_params.buser_range_x );
	::SetValue( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_ROOT, X_BEGIN, (double)m_chart_params.frange_min_x );
	::SetValue( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_ROOT, X_END, (double)m_chart_params.frange_max_x );

	::SetValue( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_ROOT, Y_USE_VALUE_RANGE, (long)m_chart_params.buser_range_y );
	::SetValue( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_ROOT, Y_BEGIN, (double)m_chart_params.frange_min_y );
	::SetValue( sptr_nd, SECT_DEPENDENCE_CHART_AXIS_ROOT, Y_END, (double)m_chart_params.frange_max_y );

	::SetValue( sptr_nd, SECT_DEPENDENCE_ROOT, ENABLE_CONNECTING_LINES, (long)m_chart_params.bConnectingLines );
	::SetValueColor( sptr_nd, SECT_DEPENDENCE_ROOT, CONN_LINES_CLR, m_chart_params.clr_conn_lines);
}

//////////////////////////////////////////////////////////////////////
CString stat_dependence_view::_get_f_str( int nmodel, double fa, double fb )
{
	CString ret;

	// [vanek] BT2000:4071 set precision - 09.12.2004
	CString str_fa = "%.2g";
	CString str_fb = "%.2g";

	switch( nmodel )
	{
	case 0: 
		ret.Format( "y = " + str_fa + " + " + str_fb + " * x", fa, fb );
		break;
	case 1:
		ret.Format( "y = 1 / ( " + str_fa + " + " + str_fb + " * x )", fa, fb );
		break;
	case 2:
		ret.Format( "y = " + str_fa + " + " + str_fb + " / x", fa, fb );
		break;
		//case 3:
		//	ret.Format( "y = x / ( " + str_fa + " + " + str_fb + " * x )", fa, fb );
		//	break;
	case 3:
		ret.Format( "y = " + str_fa + " * exp( " + str_fb + " * x )", fa, fb );
		break;
	case 4:
		ret.Format( "y = " + str_fa + " * 10 ^ ( " + str_fb + " * x )", fa, fb );
		break;
	case 5:
		ret.Format( "y = " + str_fa + " * x ^ " + str_fb + "", fa, fb );
		break;
	case 6:
		ret.Format( "y = " + str_fa + " + " + str_fb + " * lg( x )", fa, fb );
		break;
	case 7:
		ret.Format( "y = " + str_fa + " + " + str_fb + " * ln( x )", fa, fb );
		break;
	case 8:
		ret.Format( "y = " + str_fa + " * exp( " + str_fb + " / x )", fa, fb );
		break;
	case 9:
		ret.Format( "y = " + str_fa + " * 10 ^ ( " + str_fb + " / x )", fa, fb );
		break;
	case 10:
		ret.Format( "y = " + str_fa + " + " + str_fb + " * x ^ 2", fa, fb );
		break;
	case 11:
		ret.Format( "y = " + str_fa + " + " + str_fb + " * x ^ 3", fa, fb );
		break;
	}

	return ret;
}

//////////////////////////////////////////////////////////////////////
double stat_dependence_view::_get_f( int nmodel, double a, double b, double x )
{
	switch( nmodel )
	{
	case 0: return ( a + b*x );
	case 1: return ( 1 / ( a + b * x ) );
	case 2: return ( a + b / x );
		//		case 3: return ( x / ( a + b * x ) );
	case 3: return ( a * exp( b * x ) );
	case 4: return ( a * pow( 10, b * x ) );
	case 5: return ( a * pow( x, b ) );
	case 6: return ( a  +  b * log10( x ) );
	case 7: return ( a  +  b * log( x ) );
	case 8: return ( a * exp( b / x ) );
	case 9: return ( a * pow( 10, b / x ) );
	case 10: return ( a + b * x * x );
	case 11: return (a + b * x * x * x );
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////
double stat_dependence_view::_get_correl( IStatTable *punk_table, long lmodel, double *pa, double *pb, bool *pberrr )
{
	double fsum_x = 0;
	double fsum_y = 0;
	double fsum_xx = 0;
	double fsum_yy = 0;
	double fsum_xy = 0;

	long lpos_row = 0; 
	punk_table->GetFirstRowPos( &lpos_row ); 

	long lcount = 0;
	punk_table->GetRowCount( &lcount );

	long lpos_param_x = 0, lpos_param_y = 0;

	punk_table->GetParamPosByKey( m_chart_params.nx_param_key, &lpos_param_x );	
	punk_table->GetParamPosByKey( m_chart_params.ny_param_key, &lpos_param_y );	

	double fb = 0.;
	double fa = 0.;
	double fcoef_correl = 0.;

	// [vanek] BT2000:4114 : use exceptions - 10.12.2004
	bool bexception = false;
	unsigned int uiold_fcs = 0;
	uiold_fcs = _control87( 0, 0 );

	__try
	{
		_clear87( );
		unsigned uiold_fcs1 = _control87(  uiold_fcs  & ~( _EM_INVALID | _EM_ZERODIVIDE | _EM_OVERFLOW ), _MCW_EM );   
		unsigned uiold_fcs2 = _control87( 0, 0 );
		while( lpos_row ) 
		{ 
			stat_value *pvalue_x = 0;

			punk_table->GetValue( lpos_row, lpos_param_x, &pvalue_x );

			stat_value *pvalue_y = 0;
			punk_table->GetValue( lpos_row, lpos_param_y, &pvalue_y );

			// [vanek] BT2000:4118 - 13.12.2004
			if( pvalue_x && pvalue_x->bwas_defined && pvalue_y && pvalue_y->bwas_defined)
			{
				double fvalue_x = pvalue_x->fvalue;
				double fvalue_y = pvalue_y->fvalue;

				switch( lmodel )
				{
				case 1:
					fvalue_y = 1 / fvalue_y;
					break;
				case 2:
					fvalue_x = 1 / fvalue_x;
					break;
					//case 3:
					//	fvalue_y = fvalue_x / fvalue_y;
					//	break;
				case 3:
					fvalue_y = log( fvalue_y );
					break;
				case 4:
					fvalue_y = log10( fvalue_y );
					break;
				case 5:
					fvalue_x = log10( fvalue_x );
					fvalue_y = log10( fvalue_y );
					break;
				case 6:
					fvalue_x = log10( fvalue_x );
					break;
				case 7:
					fvalue_x = log( fvalue_x );
					break;
				case 8:
					fvalue_x = 1. / fvalue_x;
					fvalue_y = log( fvalue_y );
					break;
				case 9:
					fvalue_x = 1 / fvalue_x;
					fvalue_y = log10( fvalue_y );
					break;
				case 10:
					fvalue_x = fvalue_x * fvalue_x;
					break;
				case 11:
					fvalue_x = fvalue_x * fvalue_x * fvalue_x;
					break;
				}

				fsum_x	+= fvalue_x;
				fsum_y	+= fvalue_y;
				fsum_xx	+= fvalue_x * fvalue_x;
				fsum_yy	+= fvalue_y * fvalue_y;
				fsum_xy	+= fvalue_x * fvalue_y;
			}
			else
				lcount --;

			stat_row *prow = 0;
			punk_table->GetNextRow( &lpos_row, &prow );
		}

		fb = ( fsum_x * fsum_y - lcount * fsum_xy ) / ( fsum_x * fsum_x - lcount * fsum_xx );
		fa = ( fsum_y - fb * fsum_x ) / lcount;

		fcoef_correl = 0;

		if ( fa == 1 && fb == 0 )
			fcoef_correl = 1;
		else
			fcoef_correl = fabs( ( fsum_xy - ( fsum_x * fsum_y ) / lcount ) /
			( sqrt( fsum_xx - fsum_x * fsum_x / lcount ) * sqrt( fsum_yy - fsum_y * fsum_y / lcount ) ) );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		_clear87();
		_fpreset();
		bexception = true;
	}

	// set old control flags
	_control87(  uiold_fcs , _MCW_EM );
	if( bexception )
	{
		if( pberrr ) *pberrr  = true;
		return 0;
	}

	if( pa )
		*pa = fa;

	if( pb )
		*pb = fb;

	return fcoef_correl;

}


//////////////////////////////////////////////////////////////////////
void stat_dependence_view::_manage_labels( long lid, double fmin_value_x, double fmax_value_x, double fmin_value_y, double fmax_value_y, double fcoef_x, double fcoef_y )
{
	IChartAttributesPtr ptr_chart_atrr = m_sptr_chart;

	if( m_chart_params.bshow_axis_labels_x )
	{
		dblArray pfvals_x;
		long lcount_x = 0;

		if( !m_chart_params.buse_loga_scale_x )	 // Разметка по оси
		{
			double fstep = m_chart_params.fx_scale_factor * fcoef_x;
			lcount_x = long( ( fmax_value_x - fmin_value_x ) / fstep ) + 1;

			if( lcount_x < MAX_SCALE_FACTOR_X ){
				pfvals_x.resize(lcount_x);

			}else{
				error_message( m_hwnd, IDS_AXIS_LABEL_ERROR_X ); 
				m_wnd_data.berror = true;
			}

			for( long z = 0; z < lcount_x; z++ )
				pfvals_x[z] = fmin_value_x + z * fstep; 
		}
		else
		{
			_get_loga_scale( pfvals_x, fmin_value_x, fmax_value_x, 1 );
			for( unsigned z = 0; z < pfvals_x.size(); z++ )
				pfvals_x[z] = ( log10( pfvals_x[z] ) - log10( fmin_value_x ) ) / ( log10( fmax_value_x ) - log10( fmin_value_x ) );
		}

		if( pfvals_x.size()>0 )
			ptr_chart_atrr->SetScaleLabels( lid, &pfvals_x[0], pfvals_x.size(), cafAxisX );

	}

	if( m_chart_params.bshow_axis_labels_y )
	{
		dblArray pfvals_y;
		long lcount_y = 0;

		if( !m_chart_params.buse_loga_scale_y )	 // Разметка по оси
		{
			double fstep = m_chart_params.fy_scale_factor * fcoef_y;
			if( ( fmax_value_y - fmin_value_y ) / fstep > 100.){
				lcount_y=100;
				fstep=( fmax_value_y - fmin_value_y )/lcount_y;
			}

			lcount_y = long( ( fmax_value_y - fmin_value_y ) / fstep ) + 1;

			if( lcount_y < MAX_SCALE_FACTOR_Y ){
				pfvals_y.resize(lcount_y);
			}else{
				error_message( m_hwnd, IDS_AXIS_LABEL_ERROR_Y ); 
				m_wnd_data.berror = true;
			}

			for( long z = 0; z < lcount_y; z++ )
				pfvals_y[z] = fmin_value_y + z * fstep; 
		}
		else
		{
			_get_loga_scale( pfvals_y, fmin_value_y, fmax_value_y, 1 );
			for( unsigned z = 0; z < pfvals_y.size(); z++ )
				pfvals_y[z] = log10(pfvals_y[z]/fmin_value_y) / log10(fmax_value_y/fmin_value_y);
		}

		if( pfvals_y.size()>0 )
			ptr_chart_atrr->SetScaleLabels( lid, &pfvals_y[0], pfvals_y.size(), cafAxisY );

	}
}

//////////////////////////////////////////////////////////////////////
void stat_dependence_view::_manage_values(long lid, double fmin_value_x, double fmax_value_x, double fmin_value_y, double fmax_value_y
																					, double fcoef_x, double fcoef_y, map_meas_params& map_meas_params)
{
	IChartAttributesPtr ptr_chart_atrr = m_sptr_chart;
	CString str_yp;
	CString str_xp;

	CString str_x = map_meas_params(m_chart_params.nx_param_key)->bstrDefFormat;
	CString str_y = map_meas_params(m_chart_params.ny_param_key)->bstrDefFormat;

	if( !m_chart_params.m_buse_custom_format_y )
	{
		str_yp = getFormatString(m_chart_params.fy_scale_factor_values * fcoef_y);
	}
	else
		str_yp = m_chart_params.m_str_custom_format_y;

	if( !m_chart_params.m_buse_custom_format_x )
	{
		str_xp = getFormatString(m_chart_params.fx_scale_factor_values * fcoef_x);
	}
	else
		str_xp = m_chart_params.m_str_custom_format_x;

	if( m_chart_params.buse_loga_scale_x )
		str_xp = str_x;
	if( m_chart_params.buse_loga_scale_y )
		str_yp = str_y;

	m_str_max_val_x.Empty();
	m_str_max_val_y.Empty();

	if( m_chart_params.bshow_axis_value_x )
	{
		dblArray pfvals_x, pfvals_x2;
		long lcount_x = 0;

		if( !m_chart_params.buse_loga_scale_x )	 // Разметка по оси
		{
			double fstep = m_chart_params.fx_scale_factor_values * fcoef_x;
			lcount_x = long( ( fmax_value_x - fmin_value_x ) / fstep ) + 1;

			if( lcount_x < MAX_SCALE_FACTOR_VALUES_X )
			{
				pfvals_x.resize(lcount_x);
				pfvals_x2.resize(lcount_x);
			}else{
				error_message( m_hwnd, IDS_AXIS_VALUES_ERROR_X ); 
				m_wnd_data.berror = true;
			}
			for( long z = 0; z < lcount_x; z++ )
			{
				pfvals_x2[z] = fmin_value_x + z * fstep; 
				pfvals_x[z] = pfvals_x2[z] / fcoef_x; 
			}
		}
		else
		{
			_get_loga_scale( pfvals_x2, fmin_value_x, fmax_value_x, 10 );
			pfvals_x.resize(pfvals_x2.size());
			pfvals_x = std::log(pfvals_x2/fmin_value_x)/log(fmax_value_x/fmin_value_x);
		}

		for( long z = 0; z < lcount_x; z++ )
		{
			CString str;
			str.Format( str_xp, pfvals_x2[z] );

			if( str.GetLength() > m_str_max_val_x.GetLength() )
				m_str_max_val_x = str;
		}

		if( pfvals_x2.size()>0 )
		{
			m_str_max_val_x += "00";

			CString str = m_str_max_val_x;
			m_str_max_val_x.Empty();

			for( unsigned i = 0; i < pfvals_x2.size(); i++ )
				m_str_max_val_x += str;
		}

		if( pfvals_x2.size()>0 )
			ptr_chart_atrr->SetScaleTextLabels( lid, &pfvals_x[0], &pfvals_x2[0], pfvals_x2.size(), cafAxisX );
	}

	if( m_chart_params.bshow_axis_value_y )
	{
		dblArray pfvals_y, pfvals_y2;

		if( !m_chart_params.buse_loga_scale_y )	 // Разметка по оси
		{
			double fstep = m_chart_params.fy_scale_factor_values * fcoef_y;
			long lcount_y;
			if( ( fmax_value_y - fmin_value_y ) / fstep > 100.){
				m_chart_params.fy_scale_factor_values=50.;
				lcount_y=100;
				fstep=( fmax_value_y - fmin_value_y )/lcount_y;
			}
			lcount_y = long( ( fmax_value_y - fmin_value_y ) / fstep ) + 1;

			if( lcount_y < MAX_SCALE_FACTOR_VALUES_Y )
			{
				pfvals_y.resize(lcount_y);
				pfvals_y2.resize(lcount_y);
			}else{
				error_message( m_hwnd, IDS_AXIS_VALUES_ERROR_X ); 
				m_wnd_data.berror = true;
			}
			for( unsigned z = 0; z < pfvals_y.size(); z++ )
			{
				pfvals_y2[z] = fmin_value_y + z * fstep; 
				pfvals_y[z] = pfvals_y2[z] / fcoef_y; 
			}
		}
		else
		{
			_get_loga_scale( pfvals_y2, fmin_value_y, fmax_value_y, 10 );
			pfvals_y.resize(pfvals_y2.size());
			pfvals_y = std::log(pfvals_y2/fmin_value_y)/log(fmax_value_y/fmin_value_y);
		}

		for( unsigned z = 0; z < pfvals_y.size(); z++ )
		{
			CString str;
			str.Format( str_yp, pfvals_y2[z] );

			if( str.GetLength() > m_str_max_val_y.GetLength() )
				m_str_max_val_y = str;
		}

		m_nchart_values_y = pfvals_y.size();
		if( pfvals_y.size()>0 )
			ptr_chart_atrr->SetScaleTextLabels( lid, &pfvals_y[0], &pfvals_y2[0], pfvals_y.size(), cafAxisY );

	}

	ptr_chart_atrr->SetTextFormat( lid, _bstr_t( str_yp ), cffDigitsY );
	ptr_chart_atrr->SetTextFormat( lid, _bstr_t( str_xp ), cffDigitsX );

}

//////////////////////////////////////////////////////////////////////
void stat_dependence_view::_manage_grid(  long lid, double fmin_value_x, double fmax_value_x, double fmin_value_y, double fmax_value_y, double fcoef_x, double fcoef_y )
{
	IChartAttributesPtr ptr_chart_atrr = m_sptr_chart;
	if( m_chart_params.bx_grid )
	{
		dblArray pfvals_x;

		if( !m_chart_params.buse_loga_scale_x )	 // Разметка по оси
		{
			double fstep = m_chart_params.fx_scale_factor_grid * fcoef_x;
			long lcount_x = long( ( fmax_value_x - fmin_value_x ) / fstep ) + 1;

			if( lcount_x < MAX_SCALE_FACTOR_GRID_X )
				pfvals_x.resize(lcount_x);
			else
			{
				error_message( m_hwnd, IDS_AXIS_GRID_ERROR_X ); 
				m_wnd_data.berror = true;
			}

			for( unsigned z = 0; z < pfvals_x.size(); z++ )
				pfvals_x[z] = fmin_value_x + z * fstep; 
		}
		else
		{
			_get_loga_scale( pfvals_x, fmin_value_x, fmax_value_x, 1 );
			pfvals_x = std::log(pfvals_x/fmin_value_x)/log(fmax_value_x/fmin_value_x);
		}

		if( pfvals_x.size() )
			ptr_chart_atrr->SetGridLines( &pfvals_x[0], pfvals_x.size(), ccgXGrid );

	}

	if( m_chart_params.by_grid )
	{
		dblArray pfvals_y;

		if( !m_chart_params.buse_loga_scale_y )	 // Разметка по оси
		{
			double fstep = m_chart_params.fy_scale_factor_grid * fcoef_y;
			long lcount_y;
			if( ( fmax_value_y - fmin_value_y ) / fstep > 100.){
				lcount_y=11;
				fstep=( fmax_value_y - fmin_value_y )/(lcount_y-1);
				m_chart_params.fy_scale_factor_grid=fstep/fcoef_y;
			}
			lcount_y = long( ( fmax_value_y - fmin_value_y ) / fstep ) + 1;

			if( lcount_y < MAX_SCALE_FACTOR_GRID_Y )
				pfvals_y.resize(lcount_y);
			else
			{
				error_message( m_hwnd, IDS_AXIS_GRID_ERROR_Y ); 
				m_wnd_data.berror = true;
			}

			for( unsigned z = 0; z < pfvals_y.size(); z++ )
				pfvals_y[z] = fmin_value_y + z * fstep; 
		}
		else
		{
			_get_loga_scale( pfvals_y, fmin_value_y, fmax_value_y, 1 );
			for( unsigned z = 0; z < pfvals_y.size(); z++ )
			{
				double fmin = fmin_value_y;

				if( fmin <= 0 )
					fmin = 0.0001;

				pfvals_y[z] = log(pfvals_y[z]/fmin)/log(fmax_value_y/fmin);
			}
		}

		if( pfvals_y.size()>0 )
			ptr_chart_atrr->SetGridLines( &pfvals_y[0], pfvals_y.size(), ccgYGrid );
	}

}

//////////////////////////////////////////////////////////////////////

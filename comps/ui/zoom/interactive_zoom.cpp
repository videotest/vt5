#include "stdafx.h"
#include "interactive_zoom.h"
#include "resource.h"

#include "zoomnavigatorpage.h"
#include "action_ui_int.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CActionInteractiveZoom, CCmdTargetEx);

// {96B9F924-14AC-4f0b-9E22-78CF0D5FC8A4}
GUARD_IMPLEMENT_OLECREATE(CActionInteractiveZoom, "zoom.InteractiveZoom",
0x96b9f924, 0x14ac, 0x4f0b, 0x9e, 0x22, 0x78, 0xcf, 0xd, 0x5f, 0xc8, 0xa4);

// {355C0B38-2DA5-4e7c-8CD4-CCFD46C4AC88}
static const GUID guidInteractiveZoom =
{ 0x355c0b38, 0x2da5, 0x4e7c, { 0x8c, 0xd4, 0xcc, 0xfd, 0x46, 0xc4, 0xac, 0x88 } };

CArray<HWND, HWND>	g_ar_ctrls;

ACTION_INFO_FULL(CActionInteractiveZoom, ID_ACTION_INTERACTIVE_ZOOM, 0, 0, guidInteractiveZoom);

ACTION_TARGET(CActionInteractiveZoom, "app");
ACTION_UI(CActionInteractiveZoom, CZoomSlider)

IUnknownPtr get_active_view()
{
	IApplicationPtr ptr_app( ::GetAppUnknown() );
	if( ptr_app == 0 )	return 0;

	IUnknown* punk_doc = 0;
	ptr_app->GetActiveDocument( &punk_doc );
	if( !punk_doc )		return 0;

	IDocumentSitePtr ptr_ds( punk_doc );
	punk_doc->Release();

	if( ptr_ds == 0 )	return 0;

	IUnknown* punk_view = 0;
	ptr_ds->GetActiveView( &punk_view );
	if( !punk_view )	return 0;

	IUnknownPtr ptr_view = punk_view;
	punk_view->Release();	punk_view = 0;

	return ptr_view;
}

bool is_action_enable()
{
	IWindowPtr ptr_wnd = get_active_view();
	if( ptr_wnd == 0 )	return false;

	DWORD dw_flags = 0;
	ptr_wnd->GetWindowFlags( &dw_flags );
	if( dw_flags & wfZoomSupport )
		return true;

	return false;
	/*
	IUnknown *punkAM = ::_GetOtherComponent( GetAppUnknown(), IID_IActionManager );
	if( punkAM == 0 )
		return false;

	IActionManagerPtr ptmAM( punkAM );
	punkAM->Release();	punkAM = 0;
	if( ptmAM == 0 )
		return false;

	DWORD dw_style = 0;
	ptmAM->GetActionFlags( L"Zoom", &dw_style );

	return ( (dw_style & afEnabled) != 0 );
	*/
}
/////////////////////////////////////////////////////////////////////////////////////
//
//	class CActionInteractiveZoom
//
/////////////////////////////////////////////////////////////////////////////////////
CActionInteractiveZoom::CActionInteractiveZoom()
{

}

/////////////////////////////////////////////////////////////////////////////////////
CActionInteractiveZoom::~CActionInteractiveZoom()
{

}

/////////////////////////////////////////////////////////////////////////////////////
bool CActionInteractiveZoom::Invoke()
{
	sptrIScrollZoomSite2	sptrSite2( m_punkTarget );
	if( sptrSite2 )	sptrSite2->SetFitDoc(FALSE);
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////
bool CActionInteractiveZoom::IsAvaible()
{
	BOOL benable = is_action_enable();

	for( int i=0;i<g_ar_ctrls.GetSize();i++ )
	{					
		if( ::IsWindowEnabled( g_ar_ctrls[i] ) != benable )
			::EnableWindow( g_ar_ctrls[i], benable );
	}
	
	return ( benable == TRUE );
}

/////////////////////////////////////////////////////////////////////////////////////
bool CActionInteractiveZoom::IsChecked()
{
	return false;
}

//////////////////////////////////////////////////////////////////////
//	Class slider_wnd
//////////////////////////////////////////////////////////////////////
slider_wnd::slider_wnd() : dlg_impl( IDD_ZOOM )
{
	m_kill_on_destroy	= true;
	m_inside_killing	= false;
}

//////////////////////////////////////////////////////////////////////
slider_wnd::~slider_wnd()
{}

//////////////////////////////////////////////////////////////////////
bool is_window_child( HWND hwnd_parent, HWND hwnd_test )
{
	if( hwnd_parent == hwnd_test )
		return true;

	HWND	hwnd_child = ::GetWindow( hwnd_parent, GW_CHILD );

	while( hwnd_child )
	{
		if( is_window_child( hwnd_child, hwnd_test ) )
			return true;
		hwnd_child = ::GetWindow( hwnd_child, GW_HWNDNEXT );
	}
	return false;
}

//////////////////////////////////////////////////////////////////////
void slider_wnd::init()
{
	HWND hwnd_slider = ::GetDlgItem( handle(), IDC_SLIDER );
	if( !hwnd_slider )	return;

	::SendMessage( hwnd_slider, TBM_SETRANGE, 1, 
			MAKELONG(CZoomNavigatorPage::ConvertFromDoubleToTick( 1/16. ), 
					CZoomNavigatorPage::ConvertFromDoubleToTick( 16. ) ) );
	::SendMessage( hwnd_slider, TBM_SETPAGESIZE, 0, ZOOM_TICK_STEP/10 );

	double fzoom = 1.;
	IScrollZoomSitePtr ptr_szs( get_active_view() );
	if( ptr_szs )
		ptr_szs->GetZoom( &fzoom );

	::SendMessage( hwnd_slider, TBM_SETPOS, 1, CZoomNavigatorPage::ConvertFromDoubleToTick( fzoom ) );	
}

//////////////////////////////////////////////////////////////////////
long slider_wnd::on_initdialog()
{
	long lres = dlg_impl::on_initdialog();
	return lres;
}


//////////////////////////////////////////////////////////////////////
LRESULT slider_wnd::on_killfocus(HWND hwndOld)
{
	long lres = dlg_impl::on_killfocus( hwndOld );
	return lres;
}

//////////////////////////////////////////////////////////////////////
LRESULT slider_wnd::handle_message( UINT m, WPARAM w, LPARAM l )
{
	if( m == WM_NCACTIVATE && w == FALSE )
	{
		// vanek - 18.09.2003
		LRESULT lres = dlg_impl::handle_message(m, w, l);
		kill_slider();       		
		return lres;
	}
	else if( m == WM_NCACTIVATE && w == TRUE )//activate main frame
	{			
		IApplicationPtr ptr_app( ::GetAppUnknown() );
		if( ptr_app == 0 )	return false;	
		
		HWND hwnd_parent = 0;
		ptr_app->GetMainWindowHandle( &hwnd_parent );
		::SendMessage( hwnd_parent, WM_NCACTIVATE, 1, 0 );

		return 0;
	}	
	
	return dlg_impl::handle_message( m, w, l );
}

//////////////////////////////////////////////////////////////////////
LRESULT slider_wnd::on_notify(uint idc, NMHDR *pnmhdr)
{
	if( idc == IDC_SLIDER && pnmhdr->code == NM_RELEASEDCAPTURE )
	{
		kill_slider();
		return 0;
	}
	return dlg_impl::on_notify( idc, pnmhdr );
}

//////////////////////////////////////////////////////////////////////
LRESULT slider_wnd::on_hscroll(unsigned code, unsigned pos, HWND hwndScroll)
{
	HWND hwnd_slider = ::GetDlgItem( handle(), IDC_SLIDER );
	if( hwnd_slider )
	{
		int npos = ::SendMessage( hwnd_slider, TBM_GETPOS, 0, 0 );
		double fzoom = CZoomNavigatorPage::ConvertFromTickToDouble( (int)npos );
		IScrollZoomSite2Ptr ptr_szs( get_active_view() );
		if( ptr_szs )
		{
			ptr_szs->SetFitDoc(FALSE);
			ptr_szs->SetZoom( fzoom );
	}
	}
	
	return dlg_impl::on_hscroll( code, pos, hwndScroll );
}

//////////////////////////////////////////////////////////////////////
void slider_wnd::kill_slider()
{
	if( m_inside_killing )	return;

	m_inside_killing = true;
	::DestroyWindow( handle() );
}


//////////////////////////////////////////////////////////////////////
//	Class editor_wnd
//////////////////////////////////////////////////////////////////////
editor_wnd::editor_wnd()
{}

//////////////////////////////////////////////////////////////////////
editor_wnd::~editor_wnd()
{}


//////////////////////////////////////////////////////////////////////
//	Class zoom_wnd
//////////////////////////////////////////////////////////////////////
zoom_wnd::zoom_wnd()
{
	m_icon_arrow	= 0;
	m_icon_zoom		= 0;
	m_btrack_leave	= false;

	m_in_notify		= false;
	m_binternal_set_wnd_text	= false;
}

//////////////////////////////////////////////////////////////////////
zoom_wnd::~zoom_wnd()
{}

//////////////////////////////////////////////////////////////////////
IUnknown* zoom_wnd::get_interface( const IID &iid )
{
	if( iid == IID_IEventListener )
		return (IEventListener*)this;

	return com_unknown::get_interface( iid );
}

//////////////////////////////////////////////////////////////////////
void zoom_wnd::handle_init()
{
	_rect no_rect;
	win_impl::handle_init();
	
	m_editor_wnd.create_ex( WS_CHILD|WS_VISIBLE, no_rect, 0, handle(), (HMENU)EDITOR_ID, "EDIT", WS_EX_CLIENTEDGE );

	::SendMessage( m_editor_wnd.handle(), WM_SETFONT, (WPARAM)(HFONT)GetStockObject(ANSI_VAR_FONT), 0 );	

	m_icon_arrow	= (HICON)::LoadImage(	AfxGetApp()->m_hInstance, 
											MAKEINTRESOURCE(IDI_ZOOM_ARROW), IMAGE_ICON, 16, 16, 0 );

	m_icon_zoom		= (HICON)::LoadImage( AfxGetApp()->m_hInstance, 
											MAKEINTRESOURCE(IDI_ZOOM), IMAGE_ICON, 16, 16, 0 );	

	_rect rc;
	::GetClientRect( handle(), &rc );
	recalc_layout( rc.width(), rc.height() );	

	INotifyControllerPtr ptr_nc( GetAppUnknown() );
	if( ptr_nc )
		ptr_nc->RegisterEventListener( 0, unknown() );

	set_zoom_to_editor();

	g_ar_ctrls.Add( handle() );
	
}

//////////////////////////////////////////////////////////////////////
LRESULT zoom_wnd::on_destroy()
{
	for( int i=0;i<g_ar_ctrls.GetSize();i++ )
	{
		if( g_ar_ctrls[i] == handle() )
		{
			g_ar_ctrls.RemoveAt( i );
			break;
		}
	}

	if( m_icon_arrow )
		::DestroyIcon( m_icon_arrow );	m_icon_arrow = 0;

	if( m_icon_zoom )
		::DestroyIcon( m_icon_zoom );	m_icon_zoom = 0;

	INotifyControllerPtr ptr_nc( GetAppUnknown() );
	if( ptr_nc )
		ptr_nc->UnRegisterEventListener( 0, unknown() );

	return win_impl::on_destroy();
}

void _FillSolidRect( HDC hdc, int x, int y, int cx, int cy, COLORREF clr )
{
	::SetBkColor(hdc, clr);
	_rect rect( x, y, x + cx, y + cy );
	::ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rect, NULL, 0, NULL);
}


void _Draw3dRect( HDC hdc, int x, int y, int cx, int cy,
	COLORREF clrTopLeft, COLORREF clrBottomRight )
{
	_FillSolidRect(hdc, x, y, cx - 1, 1, clrTopLeft);
	_FillSolidRect(hdc, x, y, 1, cy - 1, clrTopLeft);
	_FillSolidRect(hdc, x + cx, y, -1, cy, clrBottomRight);
	_FillSolidRect(hdc, x, y + cy, cx, -1, clrBottomRight);
}

#define IsLButtonDown() ( (GetKeyState(VK_LBUTTON) & (1 << (sizeof(SHORT)*8-1))) != 0   )

//////////////////////////////////////////////////////////////////////
LRESULT zoom_wnd::on_paint()
{
	PAINTSTRUCT	ps;
	HDC hdc = BeginPaint( handle(), &ps );

	bool bwnd_enable = ( TRUE == ::IsWindowEnabled( handle() ) );

	_rect	rect_paint( ps.rcPaint );

	_point pt_cur;
	GetCursorPos( &pt_cur );
	::ScreenToClient( handle(), &pt_cur );


	//fill background
	{
		HBRUSH hbrush = ::CreateSolidBrush( ::GetSysColor( COLOR_BTNFACE ) );
		::FillRect( hdc, &rect_paint, hbrush );
		::DeleteObject( hbrush );	hbrush = 0;
	}
	_rect rc_ctrl = get_ctrl_rect();

	//draw zoom icon
	if( ICON_WIDTH != 0 )
	{
		
		_rect rc_ico = get_icon_rect();
		
//		::DrawIconEx( hdc, rc_ico.left + 1, rc_ico.top + 1, m_icon_zoom, 16, 16, 0, 0, DI_NORMAL );
		//[Max] Отрисовка задизейбленной иконы
/****************************************************/
		UINT uiFlags = DST_ICON;

		if( !::IsWindowEnabled( m_editor_wnd.handle() ) )
			uiFlags |= DSS_DISABLED;

			::DrawState( hdc,	// handle to device context
				NULL,	// handle to brush
				NULL,	// pointer to callback function
				MAKELPARAM (m_icon_zoom, 0),	// image information
				NULL,	// more image information
				rc_ico.left + 1,	// horizontal location of image
				rc_ico.top + 1,	// vertical location of image
				16,	// width of image
				16,	// height of image
				uiFlags );
/**********************************************************************/

		if( rc_ico.pt_in_rect( pt_cur ) && bwnd_enable )
		{
			if( IsLButtonDown() )
				_Draw3dRect( hdc, rc_ico.left, rc_ico.top, rc_ico.width(), rc_ico.height(), 
					::GetSysColor( COLOR_3DDKSHADOW ), ::GetSysColor( COLOR_3DHILIGHT ) );
			else
				_Draw3dRect( hdc, rc_ico.left, rc_ico.top, rc_ico.width(), rc_ico.height(), 
					::GetSysColor( COLOR_3DHILIGHT ), ::GetSysColor( COLOR_3DDKSHADOW ) );
		}
	}
	
	_rect rc_btn = get_button_rect();
	//draw btn icon
	{
		int x, y;
		x = rc_btn.left + ( rc_btn.width() - 16 ) / 2;
		y = rc_btn.top + ( rc_btn.height() - 16 ) / 2;
//		::DrawIconEx( hdc, x, y, m_icon_arrow, 16, 16, 0, 0, DI_NORMAL );

		//[Max] Отрисовка задизейбленной иконы
/****************************************************/
		UINT uiFlags = DST_ICON;

		if( !::IsWindowEnabled( m_editor_wnd.handle() ) )
			uiFlags |= DSS_DISABLED;

			::DrawState( hdc,	// handle to device context
				NULL,	// handle to brush
				NULL,	// pointer to callback function
				MAKELPARAM (m_icon_arrow, 0),	// image information
				NULL,	// more image information
				x,	// horizontal location of image
				y,	// vertical location of image
				16,	// width of image
				16,	// height of image
				uiFlags );
/**********************************************************************/


		
		if( rc_btn.pt_in_rect( pt_cur ) && bwnd_enable )
		{
			if( IsLButtonDown() )
				_Draw3dRect( hdc, rc_btn.left, rc_btn.top, rc_btn.width(), rc_btn.height(), 
					::GetSysColor( COLOR_3DDKSHADOW ), ::GetSysColor( COLOR_3DHILIGHT ) );
			else
				_Draw3dRect( hdc, rc_btn.left, rc_btn.top, rc_btn.width(), rc_btn.height(), 
					::GetSysColor( COLOR_3DHILIGHT ), ::GetSysColor( COLOR_3DDKSHADOW ) );
		}
	}

	::EndPaint( handle(), &ps ); 

	return 0;

}

//////////////////////////////////////////////////////////////////////
LRESULT zoom_wnd::on_erasebkgnd(HDC)
{
	return 0;
}

//////////////////////////////////////////////////////////////////////
LRESULT zoom_wnd::handle_message(UINT m, WPARAM w, LPARAM l)
{

	if( m == WM_MOUSELEAVE )
	{
		m_btrack_leave = false;
		invalidate_btn_rect();
		m_pt_prev	= _point();		
		return 0;
	}
	else if( m == WM_COMMAND && LOWORD(w) == EDITOR_ID && HIWORD(w) == EN_CHANGE )
	{
		if( m_binternal_set_wnd_text )
			return win_impl::handle_message( m, w, l );

		char sz_buf[256];	sz_buf[0] = 0;
		::GetWindowText( m_editor_wnd.handle(), sz_buf, sizeof(sz_buf) );
		
		if( _fixx( sz_buf, 256 ) )
			::SetWindowText( m_editor_wnd.handle(), sz_buf );

		double fzoom = CZoomNavigatorPage::FormatZoomFromS2D( sz_buf ) / 100.0;
		IScrollZoomSite2Ptr ptr_szs( get_active_view() );
		bool bold_in_notify = m_in_notify;
		m_in_notify = true;
		if( ptr_szs )
		{
			ptr_szs->SetFitDoc(FALSE);
			ptr_szs->SetZoom( fzoom );
		}

		m_in_notify  = bold_in_notify;

		return 0;
	}
	else if( m == WM_ENABLE )
		::EnableWindow( m_editor_wnd.handle(), w == TRUE );
	else if( m == TTM_WINDOWFROMPOINT )
	{
		return 0;
	}
	else if( m == TTN_POP )
	{
		return 0;
	}
	else if( m == TTN_SHOW )
	{
		return 0;
	}

	return __super::handle_message( m, w, l );
}

bool zoom_wnd::_fixx( char *pStr, int len )
{
	if( pStr[0] == 0 )
	{
		strncpy( pStr, "100%\0", 5 );
		return true;
	}
	bool bCh = false;
	char *pTmp = new char[len];
	int s = 0;
	for( int i = 0; i < len; i++ )
	{
		if( pStr[i] >= '0' && pStr[i] <= '9' || pStr[i] == ',' || pStr[i] == '%' )
			pTmp[i - s] = pStr[i];
		else if( pStr[i] == 0 )
		{
			pTmp[i - s] = 0;
			break;
		}
		else
		{
			bCh = true;
			s++;
		}
	}
	if( bCh )
		strncpy( pStr, pTmp, len );
	if( pStr[0] == 0 )
	{
		strncpy( pStr, "100%\0", 5 );
		return true;
	}
	delete []pTmp;
	return bCh;
}

//////////////////////////////////////////////////////////////////////
LRESULT zoom_wnd::on_mousemove(const _point &point)
{
	_rect rc_btn = get_button_rect();
	_rect rc_ico = get_icon_rect();
	if( rc_btn.pt_in_rect( point ) && !rc_btn.pt_in_rect( m_pt_prev ) )
		invalidate_btn_rect();
	else if( rc_btn.pt_in_rect( m_pt_prev ) && !rc_btn.pt_in_rect( point ) )
		invalidate_btn_rect();	
	else if( rc_ico.pt_in_rect( point ) && !rc_ico.pt_in_rect( m_pt_prev ) )
		invalidate_icon_rect();
	else if( rc_ico.pt_in_rect( m_pt_prev ) && !rc_ico.pt_in_rect( point ) )
		invalidate_icon_rect();
	

	m_pt_prev = point;

	if( !m_btrack_leave )
	{
		TRACKMOUSEEVENT tme = {0};
        tme.cbSize		= sizeof(tme);
        tme.hwndTrack	= handle();
        tme.dwFlags		= TME_LEAVE;
        _TrackMouseEvent( &tme );
        m_btrack_leave = true;
	}

	return win_impl::on_mousemove( point );
}

//////////////////////////////////////////////////////////////////////
LRESULT zoom_wnd::on_lbuttondown(const _point &point)
{
	_rect rc_btn = get_button_rect();
	_rect rc_ico = get_icon_rect();
	if( rc_btn.pt_in_rect( point ) )
	{
		invalidate_btn_rect();		
	}
	else if( rc_ico.pt_in_rect( point ) )
	{
		invalidate_icon_rect();
	}

	m_pt_click = point;		

	return win_impl::on_lbuttondown( point );
}

//////////////////////////////////////////////////////////////////////
LRESULT zoom_wnd::on_lbuttonup(const _point &point)
{
	_rect rc_btn = get_button_rect();
	_rect rc_ico = get_icon_rect();
	if( rc_btn.pt_in_rect( point ) )
	{
		invalidate_btn_rect();
		if( rc_btn.pt_in_rect( m_pt_click ) )
		{
			show_slider();
		}			
	}
	else if( rc_ico.pt_in_rect( point ) )
	{
		invalidate_icon_rect();
		if( rc_ico.pt_in_rect( m_pt_click ) )
		{
			::ExecuteAction( "ZoomNormal" );	
		}			
	}

	m_pt_click = _point();
	
	return win_impl::on_lbuttonup( point );
}

//////////////////////////////////////////////////////////////////////
LRESULT zoom_wnd::on_size(short cx, short cy, ulong fSizeType)
{
	long lres = win_impl::on_size( cx, cy, fSizeType );
	recalc_layout( cx, cy );

	return lres;
}

//////////////////////////////////////////////////////////////////////
bool zoom_wnd::show_slider()
{
	IApplicationPtr ptr_app( ::GetAppUnknown() );
	if( ptr_app == 0 )	return false;	
	
	HWND hwnd_parent = 0;
	ptr_app->GetMainWindowHandle( &hwnd_parent );

	slider_wnd* pslider_wnd = new slider_wnd;
	if( !pslider_wnd )	return false;
	
	pslider_wnd->do_modeless( ::GetParent( handle() )/*hwnd_parent*/ );
	//pslider_wnd->create_ex( WS_BORDER|WS_POPUP|WS_VISIBLE, _rect(), 0, hwnd_parent, 0, 0, WS_EX_DLGMODALFRAME );
	if( !pslider_wnd->handle() )
	{
		delete pslider_wnd;	pslider_wnd = 0;
		return false;
	}

	HWND hwnd_desk = ::GetDesktopWindow();

	_rect rc;
	::GetWindowRect( handle(), &rc );
	::MapWindowPoints( hwnd_desk, hwnd_parent, (POINT*)&rc, 2 );

	
	_rect rc_dlg;
	::GetWindowRect( pslider_wnd->handle(), &rc_dlg );
	_rect rc_dlg_new;

	rc_dlg_new.right	= rc.right;
	rc_dlg_new.left		= rc.right - rc_dlg.width();
	rc_dlg_new.top		= rc.bottom + 1;	
	rc_dlg_new.bottom	= rc_dlg_new.top + rc_dlg.height();
	

	/*_rect rc_dlg_new = rc;
	rc_dlg_new.top		=  rc.bottom + 1;
	rc_dlg_new.bottom	=  rc_dlg_new.top +  rc.height();
	*/
	

	::MapWindowPoints( hwnd_parent, hwnd_desk, (POINT*)&rc_dlg_new, 2 );

	::MoveWindow( pslider_wnd->handle(), rc_dlg_new.left, rc_dlg_new.top, rc_dlg_new.width(), rc_dlg_new.height(), TRUE );
	::ShowWindow( pslider_wnd->handle(), SW_SHOW );
	pslider_wnd->init();
	::SetFocus( pslider_wnd->handle() );
	
	
	return true;
}
//////////////////////////////////////////////////////////////////////
_rect zoom_wnd::get_icon_rect()
{
	//center
	_rect rc	= get_ctrl_rect();	
	rc.top		= rc.top + ( rc.height() - 18 ) / 2;
	rc.bottom	= rc.top + 18;
	rc.left		= rc.left + 1;
	rc.right	= rc.left + 18;

	return rc;
}

//////////////////////////////////////////////////////////////////////
_rect zoom_wnd::get_editor_rect()
{
	_rect rc = get_ctrl_rect();
	rc.left		= ICON_WIDTH + 2;
	rc.right	= rc.left + EDITOR_WIDTH - 4;

	return rc;
}

//////////////////////////////////////////////////////////////////////
_rect zoom_wnd::get_button_rect()
{
	_rect rc	= get_ctrl_rect();
	rc.left		= rc.right - DROP_BTN_WIDTH;

	return rc;
}

//////////////////////////////////////////////////////////////////////
_rect zoom_wnd::get_ctrl_rect()
{
	_rect rc;
	::GetClientRect( handle(), &rc );

	rc.top		= ( rc.height() - ZOOM_CTRL_HEIGHT ) / 2;
	rc.bottom	= rc.top + ZOOM_CTRL_HEIGHT;

	return rc;
}


//////////////////////////////////////////////////////////////////////
void zoom_wnd::invalidate_icon_rect()
{
	_rect rc_btn = get_icon_rect();
	rc_btn.left		-= 1;
	rc_btn.top		-= 1;
	rc_btn.right	+= 1;
	rc_btn.bottom	+= 1;

	::InvalidateRect( handle(), &rc_btn, TRUE );
}


//////////////////////////////////////////////////////////////////////
void zoom_wnd::invalidate_btn_rect()
{
	_rect rc_btn = get_button_rect();
	rc_btn.left		-= 1;
	rc_btn.top		-= 1;
	rc_btn.right	+= 1;
	rc_btn.bottom	+= 1;

	::InvalidateRect( handle(), &rc_btn, TRUE );
}


//////////////////////////////////////////////////////////////////////
void zoom_wnd::recalc_layout( int cx, int cy )
{
	if( m_editor_wnd.handle() )
	{
		_rect rc = get_editor_rect();
		::MoveWindow( m_editor_wnd.handle(), rc.left, rc.top, rc.width(), rc.height(), TRUE );
	}
}

//////////////////////////////////////////////////////////////////////
void zoom_wnd::set_zoom_to_editor()
{	
	if( !m_editor_wnd.handle() )	return;

	char sz_buf[256];	sz_buf[0] = 0;
	//::GetWindowText( m_editor_wnd.handle(), sz_buf, sizeof(sz_buf) );

	double fzoom = 1.;
	IScrollZoomSitePtr ptr_szs( get_active_view() );
	if( ptr_szs )
		ptr_szs->GetZoom( &fzoom );

	CString str = CZoomNavigatorPage::FormatZoomFromD2S( fzoom * 100 );

	m_binternal_set_wnd_text = true;
	::SetWindowText( m_editor_wnd.handle(), (LPCSTR)str );
	m_binternal_set_wnd_text = false;
}

//////////////////////////////////////////////////////////////////////
HRESULT zoom_wnd::Notify( BSTR szEventDesc, IUnknown *pHint, IUnknown *pFrom, void *pdata, long cbSize )
{
	if( szEventDesc == 0 )
		return S_FALSE;

	if( m_in_notify )
		return S_OK;

	m_in_notify = true;

	_bstr_t bstr_event = szEventDesc;
	char* psz_event = (char*)bstr_event;

	if( !strcmp( psz_event, szAppActivateView ) )
	{
		set_zoom_to_editor();
	}
	else if( !strcmp( psz_event, szEventScrollPosition ) )
	{
		set_zoom_to_editor();
	}

	m_in_notify = false;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT zoom_wnd::IsInNotify( long *pbFlag )
{
	if( !pbFlag )	return E_POINTER;

	*pbFlag = (long)m_in_notify;
	return S_OK;

}
LRESULT win_impl_with_tooltip::handle_message( UINT m, WPARAM w, LPARAM l )
{
	switch( m )
	{
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MOUSEMOVE:
		{
			MSG Msg = {0};
			Msg.hwnd = handle();
			Msg.message = m;
			Msg.lParam = l;
			Msg.wParam = w;

			if( !m_ToolTipCtrl.GetSafeHwnd() )
				_enable_tooltip();

			m_ToolTipCtrl.RelayEvent( &Msg );
		}
	}
	return __super::handle_message( m, w, l );
}

#include "\vt5\awin\misc_string.h"
void win_impl_with_tooltip::_enable_tooltip()
{
	CWnd *pWnd = CWnd::FromHandle( handle() );

	m_ToolTipCtrl.Create( pWnd, TTS_ALWAYSTIP );

	RECT rcClient = {0};
	GetClientRect( handle(), &rcClient );
	
	rcClient.right  *= 16;
	rcClient.bottom *= 16;


	_string _str;
	_str.load( ID_ACTION_INTERACTIVE_ZOOM );
	CString str = _str;
	
	str = str.Right( str.GetLength() - str.ReverseFind( '\n' ) - 1 );

	m_ToolTipCtrl.AddTool( pWnd, str, &rcClient, 123 );
	
	m_ToolTipCtrl.SendMessage( TTM_SETMAXTIPWIDTH, 0,			 SHRT_MAX);
	m_ToolTipCtrl.SendMessage( TTM_SETDELAYTIME,   TTDT_AUTOPOP, SHRT_MAX);
	m_ToolTipCtrl.SendMessage( TTM_SETDELAYTIME,   TTDT_INITIAL, 200);
	m_ToolTipCtrl.SendMessage( TTM_SETDELAYTIME,   TTDT_RESHOW,  200);

	m_ToolTipCtrl.Activate(true);
}



//////////////////////////////////////////////////////////////////////
//
//	Class CZoomSlider
//
//////////////////////////////////////////////////////////////////////
CZoomSlider::CZoomSlider()
{
	m_hwnd_parent = 0;
}

//////////////////////////////////////////////////////////////////////
CZoomSlider::~CZoomSlider()
{
	
}

IMPLEMENT_DYNCREATE(CZoomSlider, CCmdTargetEx)
IMPLEMENT_UNKNOWN(CZoomSlider, UIControl)

BEGIN_INTERFACE_MAP(CZoomSlider, CCmdTargetEx)
	INTERFACE_PART(CZoomSlider, IID_IUIControl, UIControl)
END_INTERFACE_MAP()



//////////////////////////////////////////////////////////////////////
HRESULT CZoomSlider::XUIControl::Create( HWND hwnd, RECT rc )
{
	METHOD_PROLOGUE_EX(CZoomSlider, UIControl)
	
	if( !hwnd )	return E_INVALIDARG;

	pThis->m_hwnd_parent = hwnd;

	_rect rc_wnd = rc; 	
	pThis->m_zoom_wnd.create(	WS_CHILD|WS_VISIBLE,//|WS_CLIPSIBLINGS|WS_CLIPCHILDREN, 
								rc_wnd, 0, hwnd, 0, 0 );

	return S_OK;
}
//////////////////////////////////////////////////////////////////////
HRESULT CZoomSlider::XUIControl::GetSize( SIZE* psize, BOOL bhorz )
{
	METHOD_PROLOGUE_EX(CZoomSlider, UIControl)
	if( !psize )	return E_POINTER;
	psize->cx = ZOOM_CTRL_WIDTH;
	
	if( psize->cy < ZOOM_CTRL_HEIGHT )
		psize->cy = ZOOM_CTRL_HEIGHT;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CZoomSlider::XUIControl::GetHwnd( HWND* phwnd )
{
	METHOD_PROLOGUE_EX(CZoomSlider, UIControl)

	if( !phwnd )	return E_POINTER;

	*phwnd = pThis->m_zoom_wnd.handle();

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CZoomSlider::XUIControl::Draw( HDC hdc, RECT rc, DWORD dw_flags )
{
	METHOD_PROLOGUE_EX(CZoomSlider, UIControl)
	
	//char* psz_caption = "InteractiveZoom";
	//::TextOut( hdc, rc.left, rc.top, psz_caption, strlen(psz_caption) );

	return S_OK;
}


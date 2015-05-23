#include "stdafx.h"
#include "resource.h"
#include "src_wnd.h"
#include "_hook.h"
#include "utils.h"
#include <shellapi.h>

//////////////////////////////////////////////////////////////////////
//	class src_wnd
//////////////////////////////////////////////////////////////////////
src_wnd::src_wnd()
{
	m_hfont = 0;

	m_nline_x = m_nline_y = 0;
	m_npage_x = m_npage_y = 0;

	m_lactive_line	= 0;
	m_str_file		= "";
	m_str_real_file	= "";

	m_hicon_small	= 0;
	m_hicon_big		= 0;
}

//////////////////////////////////////////////////////////////////////
src_wnd::~src_wnd()
{

}

//////////////////////////////////////////////////////////////////////
bool src_wnd::create_window( HWND hwnd_parent )
{
	lock l( &g_block_mem_hook, true );

	if( m_hwnd )	return false;

	DWORD style		=	WS_OVERLAPPED|WS_POPUP|WS_BORDER|WS_SYSMENU|
						WS_CAPTION|WS_MINIMIZEBOX|WS_MAXIMIZEBOX|
						WS_VSCROLL|WS_HSCROLL;
	DWORD ex_style	=	WS_EX_CLIENTEDGE;
	_rect rc( 0, 0, 600, 600 );
	bool bres = create_ex( style, rc, "Browse", hwnd_parent, 0, 0, ex_style );

	//create font
	LOGFONT lf = {0};	
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfHeight = -12;
	strcpy( lf.lfFaceName, "Courier New" );
	m_hfont = CreateFontIndirect( &lf );

	GetPrivateProfilePlacement( SECTION_WND_PLACE, WND_SRC_BROWSE, m_hwnd, get_ini_file(), 0 );

	//set icon
	m_hicon_small	= (HICON)::LoadImage(	module::hrc(), MAKEINTRESOURCE( IDI_CPP ), 
												IMAGE_ICON, 16, 16, 0 );
	m_hicon_big		= (HICON)::LoadImage(	module::hrc(), MAKEINTRESOURCE( IDI_CPP ), 
												IMAGE_ICON, 32, 32, 0 );
	if( m_hicon_small )
		::SendMessage( m_hwnd, WM_SETICON, ICON_SMALL, (LPARAM)m_hicon_small );
	
	if( m_hicon_big )
		::SendMessage( m_hwnd, WM_SETICON, ICON_BIG, (LPARAM)m_hicon_big );

	if( !::IsWindowVisible( m_hwnd ) )
		::ShowWindow( m_hwnd, SW_SHOW );

	return bres;	
}

bool is_file_exist( const char* psz_file_name )
{
	if( !psz_file_name )
		return  false;

	WIN32_FIND_DATA wd;
	::ZeroMemory( &wd, sizeof(WIN32_FIND_DATA) );

	strcpy( wd.cFileName, psz_file_name );

	HANDLE ff = ::FindFirstFile( psz_file_name, &wd ); 
	if( ff != INVALID_HANDLE_VALUE )
	{
		FindClose( ff );
		return true;
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////
_string	src_wnd::get_real_src_file( const char* psz_file )
{
	if( !psz_file )	return "";

	if( is_file_exist( psz_file ) )	return psz_file;

	const char* psz_short_name = strrchr( psz_file, '\\' );
	if( psz_short_name )
		psz_short_name++;
	else
		psz_short_name = psz_file;

	char sz_buf[32768];	sz_buf[0] = 0;
	get_src_path( sz_buf, sizeof(sz_buf) );

	char sz_full_path[MAX_PATH];

	char* psz = strtok( sz_buf, ";" );
	while( psz )
	{
		strcpy( sz_full_path, psz );
		strcat( sz_full_path, "\\" );
		strcat( sz_full_path, psz_short_name );

		if( is_file_exist( sz_full_path ) )	return sz_full_path;

		psz = strtok( 0, ";" );
	}
	return "";
}

/////////////////////////////////////////////////////////////////////////////////////////
long src_wnd::on_size( short cx, short cy, ulong fSizeType )
{
	long lres = win_impl::on_size( cx, cy, fSizeType );
	set_client_size();
	return lres;
}

/////////////////////////////////////////////////////////////////////////////////////////
long src_wnd::on_hscroll( unsigned code, unsigned pos, HWND hwndScroll )
{
	long lres = win_impl::on_hscroll( code, pos, hwndScroll );

	int x = ::GetScrollPos( m_hwnd, SB_HORZ );
	switch( code )
	{
	case SB_TOP:
		x = 0;
		break;
	case SB_BOTTOM:
		x = 0xffffffff;
		break;
	case SB_LINEUP:
		x -= m_nline_x;
		break;
	case SB_LINEDOWN:
		x += m_nline_x;
		break;
	case SB_PAGEUP:
		x -= m_npage_x;
		break;
	case SB_PAGEDOWN:
		x += m_npage_x;
		break;
	case SB_THUMBTRACK:
		x = pos;
		break;
	}

	::SetScrollPos( m_hwnd, SB_HORZ, x, true );	
	m_pt_scroll.x = ::GetScrollPos( m_hwnd, SB_HORZ );
	
	::InvalidateRect( m_hwnd, 0, true );
	return lres;
}

/////////////////////////////////////////////////////////////////////////////////////////
long src_wnd::on_vscroll( unsigned code, unsigned pos, HWND hwndScroll )
{
	long lres = win_impl::on_vscroll( code, pos, hwndScroll );
	int y = ::GetScrollPos( m_hwnd, SB_VERT );
	switch( code )
	{
	case SB_TOP:
		y = 0;
		break;
	case SB_BOTTOM:
		y = 0xffffffff;
		break;
	case SB_LINEUP:
		y -= m_nline_y;
		break;
	case SB_LINEDOWN:
		y += m_nline_y;
		break;
	case SB_PAGEUP:
		y -= m_npage_y;
		break;
	case SB_PAGEDOWN:
		y += m_npage_y;
		break;
	case SB_THUMBTRACK:
		y = pos;
		break;
	}
	::SetScrollPos( m_hwnd, SB_VERT, y, true );
	m_pt_scroll.y = ::GetScrollPos( m_hwnd, SB_VERT );	

	::InvalidateRect( m_hwnd, 0, true );
	return lres;
}

/////////////////////////////////////////////////////////////////////////////////////////
long src_wnd::handle_message( UINT m, WPARAM w, LPARAM l )
{
	if( m == 0x020A )//WM_MOUSEWHEEL
	{
		short zDelta = HIWORD(w);
		_rect client;
		GetClientRect( m_hwnd, &client );

		long y = GetScrollPos( m_hwnd, SB_VERT );
		long x = GetScrollPos( m_hwnd, SB_HORZ );
		y-=zDelta/5;

		POINT _pt;
		_pt.x = x;
		_pt.y = y;
		
		SetScrollPos( m_hwnd, SB_HORZ, _pt.x, true ); 
		SetScrollPos( m_hwnd, SB_VERT, _pt.y, true ); 

		m_pt_scroll.x = ::GetScrollPos( m_hwnd, SB_HORZ );	
		m_pt_scroll.y = ::GetScrollPos( m_hwnd, SB_VERT );	

		::InvalidateRect( m_hwnd, 0, true );
	}
	return win_impl::handle_message( m, w, l );
	
}

//////////////////////////////////////////////////////////////////////
void src_wnd::do_paint( HDC hdc )
{
	if( !hdc )	return;
	
	_size size_line = get_line_size();

	_rect rc_client;
	::GetClientRect( m_hwnd, &rc_client );

	HFONT hfont_old = (HFONT)::SelectObject( hdc, m_hfont );
	::SetBkMode( hdc, TRANSPARENT );

	int nstart	= m_pt_scroll.y / size_line.cy;
	int nend	= nstart + rc_client.height() / size_line.cy + 1;

	char sz_buf[10];

	for( int nline = nstart; nline <= nend; nline++ )
	{
		long lpos = m_list_strings.find( nline );
		if( !lpos )
			break;

		_string str = m_list_strings.get( lpos );

		_rect rc_line;
		rc_line.left	= int( double(size_line.cx) / double(MAX_LINE_LEN) * 5. ) + 10;
		rc_line.right	= rc_line.left + size_line.cx;
		rc_line.top		= size_line.cy * nline;
		rc_line.bottom	= rc_line.top + size_line.cy;
	
		::SetTextColor( hdc, ::GetSysColor( COLOR_WINDOWTEXT ) );

		//draw active line
		if( nline + 1 == m_lactive_line )
		{
			HBRUSH hbrush_active = ::CreateSolidBrush( ::GetSysColor( COLOR_HIGHLIGHT ) );
			if( hbrush_active )
			{
				_rect rc_highlight = rc_line;
				rc_highlight.left = 0;
				::FillRect( hdc, &rc_highlight, hbrush_active );
				::DeleteObject( hbrush_active );	hbrush_active = 0;
			}
			::SetTextColor( hdc, ::GetSysColor( COLOR_HIGHLIGHTTEXT ) );
		}

		//draw line number
		sprintf( sz_buf, "%d:", nline + 1 );
		::TextOut( hdc, 0, rc_line.top, sz_buf, strlen(sz_buf) );

		//draw line content
		::DrawText( hdc, (char*)str, -1, &rc_line, DT_LEFT );
	}

	::SelectObject( hdc, hfont_old );
	
}

//////////////////////////////////////////////////////////////////////
long src_wnd::on_paint()
{
	lock l( &g_block_mem_hook, true );

	_rect rc_update;
	::GetUpdateRect( m_hwnd, &rc_update, true );

	PAINTSTRUCT	ps = {0};
	HDC	hdc = ::BeginPaint( m_hwnd, &ps );	

	//fill background
	{
		HBRUSH hbrush = ::CreateSolidBrush( ::GetSysColor( COLOR_WINDOW ) );
		if( hbrush )
		{
			::FillRect( hdc, &rc_update, hbrush );
			::DeleteObject( hbrush );	hbrush = 0;
		}
	}

	::SetViewportOrgEx( hdc, -m_pt_scroll.x, -m_pt_scroll.y, 0 );
	do_paint( hdc );
	::EndPaint( m_hwnd, &ps );

	return 0L;
}


//////////////////////////////////////////////////////////////////////
long src_wnd::on_erasebkgnd( HDC )
{
	return 1L;
}
//////////////////////////////////////////////////////////////////////
long src_wnd::on_destroy()
{
	if( m_hfont )
		::DeleteObject( m_hfont );	m_hfont = 0;

	WritePrivateProfilePlacement( SECTION_WND_PLACE, WND_SRC_BROWSE, m_hwnd, get_ini_file() );

	if( m_hicon_small )
		::DestroyIcon( m_hicon_small );	m_hicon_small = 0;

	if( m_hicon_big )
		::DestroyIcon( m_hicon_big );	m_hicon_big = 0;

	return win_impl::on_destroy();
}

//////////////////////////////////////////////////////////////////////
long src_wnd::on_keydown( long nVirtKey )
{
	long lres = win_impl::on_keydown( nVirtKey );
	if( nVirtKey == VK_ESCAPE )
		::DestroyWindow( m_hwnd );
	else if( nVirtKey == VK_RETURN && m_str_file.length() )
	{
		SHELLEXECUTEINFO sei = {0};
		sei.cbSize = sizeof(SHELLEXECUTEINFO);
		sei.lpFile = (char*)m_str_real_file;
		sei.nShow = SW_SHOWNORMAL;
		sei.fMask = SEE_MASK_NOCLOSEPROCESS;

		::ShellExecuteEx( &sei );	

	}
	return lres;
}

//////////////////////////////////////////////////////////////////////
bool src_wnd::open_file( const char* psz_file, long nline )
{
	lock l( &g_block_mem_hook, true );
	
	if( !m_hwnd )	return false;
	if( !psz_file )	return false;

	m_str_real_file = get_real_src_file( psz_file );

	m_lactive_line = nline;
	m_str_file = psz_file;

	m_list_strings.clear();

	if( m_str_real_file.length() )
	{
		char sz_buf[MAX_LINE_LEN];	sz_buf[0] = 0;
		char sz_buf_out[4*MAX_LINE_LEN];	sz_buf_out[0] = 0;

		FILE* pfile = ::fopen( (char*)m_str_real_file, "rt" );
		if( pfile )
		{
			long nline = 0;
			while( ::fgets( sz_buf, sizeof(sz_buf), pfile ) )
			{
				char* psz = sz_buf;
				int idx = 0;
				while( *psz )
				{
					if( *psz == '\t')
					{
						sz_buf_out[idx++] = ' ';
						sz_buf_out[idx++] = ' ';
						sz_buf_out[idx++] = ' ';
						sz_buf_out[idx++] = ' ';
					}
					else
						sz_buf_out[idx++] = *psz;
					psz++;
				}
				sz_buf_out[idx] = 0;
				m_list_strings.set( sz_buf_out, nline++ );
			}
			::fclose( pfile );	pfile = 0;	
		}

		set_client_size();

		//scroll to active line
		_rect rc_client;
		::GetClientRect( m_hwnd, &rc_client );

		_size size_line = get_line_size();
		int ny = size_line.cy * m_lactive_line;
		ny -= rc_client.height() / 2;

		m_pt_scroll.y = ny;
		::SetScrollPos( m_hwnd, SB_VERT, ny, true );
		char sz_text[2 * MAX_PATH + 10 ];
		sprintf( sz_text, "%s/(pdb:%s)", (char*)m_str_real_file, psz_file );
		::SetWindowText( m_hwnd, sz_text );	
	}
	else
	{
		set_client_size();
		::SetWindowText( m_hwnd, "No source available" );	
	}

	
	ActivateProcessWindow( m_hwnd );
	InvalidateRect( m_hwnd, 0, true );
	UpdateWindow( m_hwnd );

	return true;
}

//////////////////////////////////////////////////////////////////////
_size src_wnd::get_line_size()
{
	lock l( &g_block_mem_hook, true );

	_size size;
	if( m_hfont )
	{
		HDC hdc = ::GetDC( 0 );
		HFONT hfont_old = (HFONT)::SelectObject( hdc, m_hfont );
		_rect calc;
		::DrawText( hdc, "W", -1, &calc, DT_CALCRECT );		
		::SelectObject( hdc, hfont_old );
		::ReleaseDC( 0, hdc );

		size.cx = calc.width() * MAX_LINE_LEN;
		size.cy = calc.height();
	}

	return size;
}

//////////////////////////////////////////////////////////////////////
void src_wnd::set_client_size()
{
	lock l( &g_block_mem_hook, true );

	_rect rc_client;
	::GetClientRect( m_hwnd, &rc_client );
	int cx = rc_client.width();
	int cy = rc_client.height();

	//set scroll size
	{
		_size size_full = get_line_size();
		size_full.cy *= m_list_strings.count();

		m_nline_x = m_nline_y = 10;
	
		m_npage_x = min( size_full.cx, cx );
		m_npage_y = min( size_full.cy, cy );		

		SCROLLINFO si = {0};		

		si.cbSize	= sizeof(SCROLLINFO);
		si.fMask	= SIF_RANGE|SIF_PAGE;		
		si.nPage	= m_npage_x;
		si.nMax		= size_full.cx;
		::SetScrollInfo( m_hwnd, SB_HORZ, &si, FALSE );

		si.nPage	= m_npage_y;
		si.nMax		= size_full.cy;
		::SetScrollInfo( m_hwnd, SB_VERT, &si, FALSE );

		::ShowScrollBar( m_hwnd, SB_HORZ, size_full.cx > cx ); 
		::ShowScrollBar( m_hwnd, SB_VERT, size_full.cy > cy ); 		
	}
	
	m_pt_scroll.x = ::GetScrollPos( m_hwnd, SB_HORZ );
	m_pt_scroll.y = ::GetScrollPos( m_hwnd, SB_VERT );

	::InvalidateRect( m_hwnd, 0, true );
}
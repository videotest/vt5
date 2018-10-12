#pragma once

#include "win_base.h"

#define GBCTRL_DS_ROUND_RECT	0
#define GBCTRL_DS_SIMPLE_RECT	1

class group_box_ctrl : public win_impl
{
public:
	group_box_ctrl();
	virtual ~group_box_ctrl();
public:
	virtual void handle_init();
	virtual LRESULT on_paint();
	virtual LRESULT on_destroy();
	virtual bool subclass( HWND hwnd, bool f_nccreate = false );

	void set_text( HINSTANCE hrc, UINT nID );
	void set_text( char *psz );
	void set_offset( int n )	{m_cx_offset = n;}

	void set_icon( HINSTANCE hrc, uint id );
	void set_colors( COLORREF color_back, COLORREF color_text );
	void set_draw_style( int nstyle );
	void set_text_align( int nalign );
	void set_text_valign( int nalign );
protected:
	HICON		m_hicon;
	HFONT		m_font;
	COLORREF	m_color, m_color_text;
	int			m_ndraw_style;
	int			m_nalign;
	int			m_nvalign;
	char		*m_psz_text;
	int			m_cx_offset;
};

inline
group_box_ctrl::group_box_ctrl()
{
	m_hicon = 0;
	m_font = 0;
	m_ndraw_style = GBCTRL_DS_ROUND_RECT;
	m_nalign = DT_LEFT;
	m_nvalign = DT_VCENTER;

	m_color_text = RGB( 255, 255, 255 );
	m_color = RGB( 192, 192, 192 );
	m_psz_text = 0;
	m_cx_offset = 24;
}

inline
group_box_ctrl::~group_box_ctrl()
{
	if( m_hicon )::DestroyIcon( m_hicon );
	_assert( m_font == 0 );

	if( m_psz_text )
		::free( m_psz_text );
}



inline
void group_box_ctrl::handle_init()
{
	_assert( m_font == 0 );
	LOGFONT	lf;
	::GetObject( ::GetStockObject( DEFAULT_GUI_FONT ), sizeof( lf ), &lf );

	lf.lfWeight = FW_BOLD;
	lf.lfHeight = 14;

	m_font = ::CreateFontIndirect( &lf );
}

inline
LRESULT group_box_ctrl::on_destroy()
{
	if( m_font )
		::DeleteObject( m_font );
	m_font = 0;

	return win_impl::on_destroy();
}

inline
bool group_box_ctrl::subclass( HWND hwnd, bool f_nccreate )
{
	char buf[1024];
	::GetWindowText( hwnd, buf, sizeof(buf)-1 );
	::SetWindowText( hwnd, "" );
	if( m_psz_text )::free( m_psz_text );
	m_psz_text = _tcsdup( buf );
	return win_impl::subclass( hwnd, f_nccreate );
};

inline
LRESULT group_box_ctrl::on_paint()
{
	PAINTSTRUCT	ps;
	HDC	hdc = ::BeginPaint( handle(), &ps );
	HFONT hold = (HFONT)::SelectObject( hdc, m_font );

	char *psz_text = m_psz_text;


	char	sz[1024];
	
	if( !psz_text )
	{
		::GetWindowText( handle(), sz, sizeof( sz ) );
		psz_text = sz;
	}

	_rect	rect_client;
	::GetClientRect( handle(), &rect_client );
	{
		HBRUSH	hbrush = ::CreateSolidBrush( m_color );
		HBRUSH hold_brush = (HBRUSH)::SelectObject( hdc, hbrush );
		HPEN hpen = ::CreatePen( PS_SOLID, 1, m_color );
		HPEN hold_pen = (HPEN)::SelectObject( hdc, hpen );

		if( m_ndraw_style == GBCTRL_DS_ROUND_RECT )
			::RoundRect( hdc, rect_client.left, rect_client.top, rect_client.right, rect_client.bottom, 5, 5 );
		else if( m_ndraw_style == GBCTRL_DS_SIMPLE_RECT )
			::Rectangle( hdc, rect_client.left, rect_client.top, rect_client.right, rect_client.bottom );
		
		::SelectObject( hdc, hold_brush );
		::SelectObject( hdc, hold_pen );

		//???
		::DeleteObject( hbrush );
		::DeleteObject( hpen );
	}
	//draw icon

	if( m_hicon )
		::DrawIconEx( hdc, rect_client.left+rect_client.height()/2-8, rect_client.vcenter()-8, m_hicon, 16, 16, 0, 0, DI_NORMAL );

	//draw text

	_rect	rc_text( rect_client );

	if( m_nalign == DT_LEFT )
		rc_text.left += m_cx_offset;
	else if( m_nalign == DT_RIGHT )
		rc_text.right -= 12;


	::SetBkMode( hdc, TRANSPARENT );
	::SetTextColor( hdc, m_color_text );

	if( _tcslen( psz_text ) )
	{
		_rect rc_calc( 0, 0, rc_text.width(), 0 );
		::DrawText( hdc, psz_text, -1, &rc_calc, DT_LEFT|DT_CALCRECT );

		rc_text.top		= rc_text.top + ( rc_text.height() - rc_calc.height() ) / 2;
		rc_text.bottom	= rc_text.top + rc_calc.height();
		::DrawText( hdc, psz_text, -1, &rc_text, m_nalign | m_nvalign );
	}

	::SelectObject( hdc, hold );
	::EndPaint( handle(), &ps );

	return 0;
}

inline
void group_box_ctrl::set_colors( COLORREF color_back, COLORREF color_text )
{
	m_color_text	= color_text;
	m_color			= color_back;
}

inline
void group_box_ctrl::set_draw_style( int nstyle )
{
	m_ndraw_style = nstyle;
}

inline
void group_box_ctrl::set_text_align( int nalign )
{
	m_nalign = nalign;
}

inline
void group_box_ctrl::set_text_valign( int nvalign )
{
	m_nvalign = nvalign;
}

inline
void group_box_ctrl::set_text( HINSTANCE hrc, UINT nID )
{
	if( m_psz_text )
		::free( m_psz_text ); m_psz_text = 0;

	char sz[250] = {0}; 

	LoadString( hrc, nID, sz, sizeof( sz ) );
	m_psz_text = _tcsdup( sz );
}

inline
void group_box_ctrl::set_text( char *psz )
{
	if( m_psz_text )
		::free( m_psz_text ); m_psz_text = 0;
	m_psz_text = _tcsdup( psz );
}

inline
void group_box_ctrl::set_icon( HINSTANCE hrc, uint id )
{
	m_hicon = ::LoadIcon( hrc, MAKEINTRESOURCE( id ) );
}

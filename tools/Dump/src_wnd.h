#pragma once

#include "_hook.h"

#define MAX_LINE_LEN	1000
//////////////////////////////////////////////////////////////////////
//	class src_wnd
//////////////////////////////////////////////////////////////////////
class src_wnd : public win_impl
{
public:
	src_wnd();
	virtual ~src_wnd();	
	bool	create_window( HWND hwnd_parent );
	_string	get_real_src_file( const char* psz_file );	
protected:
	//message handlers
	virtual long	on_paint();
	virtual long	on_erasebkgnd( HDC );
	virtual long	on_destroy();
	virtual long	on_keydown( long nVirtKey );
	virtual long	on_size( short cx, short cy, ulong fSizeType );
	virtual long	on_hscroll( unsigned code, unsigned pos, HWND hwndScroll );
	virtual long	on_vscroll( unsigned code, unsigned pos, HWND hwndScroll );
	virtual long	handle_message( UINT m, WPARAM w, LPARAM l );

	void			do_paint( HDC hdc );
public:	
	bool			open_file( const char* psz, long nline );
protected:
	_size			get_line_size();
	void			set_client_size();

	_point			m_pt_scroll;
	int				m_nline_x, m_nline_y;
	int				m_npage_x, m_npage_y;

	//file content
	_list_map_t<_string, long, cmp_long, 0>	m_list_strings;	
	//font for drawing
	HFONT			m_hfont;

	//active line num
	long			m_lactive_line;
	//active file name
	_string			m_str_file;
	_string			m_str_real_file;
	
	//ICO
	HICON			m_hicon_small, m_hicon_big;
};

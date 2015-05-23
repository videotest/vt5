#pragma once
///////////////////////////////////////////////////////////////////////////////
// This file is part of AWIN class library
// Copyright (c) 2001-2002 Andy Yamov
//
// Permission to use, copy, modify, distribute, and sell this software and
// its documentation for any purpose is hereby granted without fee
// 
// THE SOFTWARE IS PROVIDED_T("AS-IS") AND WITHOUT WARRANTY OF ANY KIND,
// EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
// WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
//

#ifndef __render_link_h__
#define __render_link_h__

#include "render.h"
#include "misc_draw_list.h"

#define WM_HLINKCLICK	(WM_USER+1001)


class render_link : public render,
					public draw_string_list
{
public:
	render_link()
	{
	}

	virtual void add_text( const char *psz )
	{
		set_text( psz );
	}

	virtual void layout( const RECT &rect )
	{
		if( m_rect_def.bottom )
		{
			render::layout( rect );
			return;
		}

		_rect	rect_new = rect;
		
		calc_layout( rect_new.width() );
		rect_new.bottom = rect_new.top+m_size.cy;

		render::layout( rect_new );
	}
	
	virtual void add_resource( HINSTANCE hrc, unsigned ids )
	{
		char	sz[300];
		::LoadString( hrc, ids, sz, sizeof( sz ) );
		set_text( sz );
	}

	virtual void paint( HDC hdc )
	{
		draw( hdc, m_rect, 0 );
	}

	virtual long on_erase_rect( HDC hdc, const _rect &rect )
	{return 1;}

	virtual bool invoke_item_command( draw_string_item *p )
	{
		keyword	*pw = lookup( p->word );
		if( !pw )return false;
		SendMessage( get_window(), WM_COMMAND, pw->command, 0 );
		return true;
	}

	virtual bool mouse_move( const _point &point ){return false;};
	virtual bool mouse_click( const _point &point )
	{
		draw_string_item	*p = hit_test( point );
		if( !p )return false;

		return invoke_item_command( p );
	}

	virtual render_type	type()	{return type_render_text;};
	virtual bool set_cursor( const _point &point )
	{
 		if( !hit_test( point ) )return false;
		SetCursor( draw_string_list::get_cursor() );
		return true;
	}

};

class render_hlink : public render_text
{
public:
	render_hlink()
	{
		lfUnderline = true;
	}

	virtual void get_link( char *psz, size_t cb )
	{
		if( ::_tcschr( m_str, '@' ) )
			_tcsncpy( psz, _T("mailto:"), cb );
		else 
			_tcsncpy( psz, _T("http://"), cb );

		_tcsncat( psz, m_str, cb );

	}
	virtual void add_text( const char *psz )
	{
		render_text::add_text( psz );
	}
	virtual bool set_cursor( const _point &point )
	{
		SetCursor( draw_string_list::get_cursor() );
		return true;
	}
	virtual bool mouse_click( const _point &point )
	{
		SendMessage( get_window(), WM_HLINKCLICK, 0, (LPARAM)this );

		return true;
	}
	virtual void layout( const RECT &rect )
	{
		HFONT	hfont = create_font();

		HDC hdc = ::GetDC( 0 );
		::SelectObject( hdc, hfont );

		_rect	rect_new = rect;


		_rect	calc;

		::DrawText( hdc, m_str, -1, &calc, DT_LEFT|DT_TOP|DT_CALCRECT );
		rect_new.right = rect_new.left+calc.width();
		rect_new.bottom = rect_new.top+calc.height();
		::ReleaseDC( 0, hdc );

		::DeleteObject( hfont );

		render_text::layout( rect_new );
	}
};


#endif //__render_link_h__
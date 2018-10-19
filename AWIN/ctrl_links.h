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

#ifndef __link_ctrls_h__
#define __link_ctrls_h__

#include "win_base.h"
#include "misc_draw_list.h"

///////////////////////////////////////////////////////////////////////////////
//оконные классы для линков
class hlink_static : public win_impl,
					public draw_string_list
{
public:
	hlink_static();
public:
	virtual long on_erase_rect( HDC hdc, const _rect &rect );
	virtual long on_paint();
	virtual long on_setfocus( HWND hwndOld );
	virtual long on_killfocus( HWND hwndOld );
	virtual long on_setcursor( unsigned code, unsigned hit );
	virtual long on_lbuttonup( const _point &point );
	virtual long on_keyup( long nVirtKey );
	virtual void handle_init();
	virtual long handle_message( UINT m, WPARAM w, LPARAM l );
	
	WNDPROC	m_proc_stored;
};

const int cx_ofs = 3;
class hlink_list : public win_impl,
		public _list_t<draw_string_list*,draw_string_list::free>
{
public:
	hlink_list();

	void set_keywords( keyword *p );
protected:
	virtual long on_setcursor( unsigned code, unsigned hit );
	virtual long on_lbuttonup( const _point &point );
	virtual long on_keyup( long nVirtKey );
	virtual long handle_reflect_message( MSG *pmsg, long *plProcessed );
	virtual void handle_init();
	virtual long on_erasebkgnd( HDC hdc );
	virtual long on_setfocus( HWND hwndOld );
	virtual long on_killfocus( HWND hwndOld );

protected:
	draw_string_list *_get_from_point( const _point &point );
	draw_string_list *_get_from_item( int index );
protected:
	keyword	*m_pkeywords;
	bool	m_focused;
};


///////////////////////////////////////////////////////////////////////////////
//классы для линков - реализация. статик
inline hlink_static::hlink_static()
{
	m_focused = false;
	m_proc_stored = 0;
}

inline long hlink_static::on_erase_rect( HDC hdc, const _rect &rect )
{
	::FillRect( hdc, &rect, ::GetSysColorBrush( COLOR_3DFACE ) );
	return true;
}
inline long hlink_static::on_paint()
{
	RECT	rect;
	::GetClientRect( handle(), &rect );

	PAINTSTRUCT	ps;
	HDC	hdc = ::BeginPaint( handle(), &ps );

	draw( hdc, rect, m_focused );

	::EndPaint( handle(), &ps );

	return 1;
}

inline long hlink_static::on_setfocus( HWND hwndOld )
{
	m_focused = true;
	::InvalidateRect( handle(), 0, true );
	return win_impl::on_setfocus( hwndOld );
}

inline long hlink_static::on_killfocus( HWND hwndOld )
{
	m_focused = false;
	::InvalidateRect( handle(), 0, true );
	return win_impl::on_killfocus( hwndOld );
}

inline long hlink_static::on_setcursor( unsigned code, unsigned hit )
{
	_point	pt;
	::GetCursorPos( &pt );
	::ScreenToClient( handle(), &pt );
	
	if( !set_cursor( pt ) )
		return win_impl::on_setcursor( code, hit );
	return 1;
}

inline long hlink_static::on_lbuttonup( const _point &point )
{
	draw_string_item	*p = hit_test( point );
	if( p )
	{
		keyword	*pw = lookup( p->word );
		if( pw )
			SendMessage( GetParent( handle() ), WM_COMMAND, pw->command, 0 );
	}

	return win_impl::on_lbuttonup( point );
}

inline long hlink_static::on_keyup( long nVirtKey )
{
	if( nVirtKey == VK_SPACE )
	{
		keyword	*p = first_keyword();
		if( p )SendMessage( GetParent( handle() ), WM_COMMAND, p->command, 0 );		
	}
	return win_impl::on_keyup( nVirtKey );
}

inline void hlink_static::handle_init()
{
	//text
	_char	sz[1000];
	GetWindowText( handle(), sz, 1000 );
	set_text( sz );

	//font
	HFONT	h = (HFONT)::SendMessage( handle(), WM_GETFONT, 0, 0 );
	if( h )set_font( h );

	m_proc_stored = m_proc_old;
	m_proc_old = DefWindowProc;
}
inline long	hlink_static::handle_message( UINT m, WPARAM w, LPARAM l )
{
	if( m == WM_SETTEXT )
	{
		const _char *psz = (const _char *)l;
		set_text( psz );
	}
	else if( m == WM_SETFONT )
	{
		HFONT	h = (HFONT)w;
		set_font( h );
	}

	return win_impl::handle_message( m, w, l );
}

///////////////////////////////////////////////////////////////////////////////
//классы для линков - реализация. список
inline hlink_list::hlink_list()
{
	m_pkeywords = 0;
	m_focused = false;
}

inline long hlink_list::on_setfocus( HWND hwndOld )
{
	m_focused = true;
	::InvalidateRect( handle(), 0, false );
	return true;
}
inline long hlink_list::on_killfocus( HWND hwndOld )
{
	m_focused = false;
	::InvalidateRect( handle(), 0, false );
	return true;
}

inline void hlink_list::set_keywords( keyword *p )		
{
	m_pkeywords = p;
	for( LPOS lpos = head(); lpos; lpos = next( lpos ) )
		get( lpos )->set_keywords( p );
}

inline long hlink_list::on_erasebkgnd( HDC hdc )
{
	_rect	rect;
	GetClientRect( handle(), &rect );
	::FillRect( hdc, &rect, GetSysColorBrush( COLOR_3DFACE ) );
	return true;
}


inline  long hlink_list::on_setcursor( unsigned code, unsigned hit )
{
	_point	pt;
	::GetCursorPos( &pt );
	::ScreenToClient( handle(), &pt );

	draw_string_list *pdl = _get_from_point( pt );
	if( !pdl || !pdl->set_cursor( pt ) )
		return win_impl::on_setcursor( code, hit );
	return 1;
}

inline  long hlink_list::on_lbuttonup( const _point &point )
{
	draw_string_list *pdl = _get_from_point( point );
	if( !pdl )return 1;

	draw_string_item	*p = pdl->hit_test( point );
	if( p )
	{
		keyword	*pw = pdl->lookup( p->word );
		if( pw )
			SendMessage( GetParent( handle() ), WM_COMMAND, pw->command, 0 );
	}

	return win_impl::on_lbuttonup( point );
}

inline  long hlink_list::on_keyup( long nVirtKey )
{
	if( nVirtKey == VK_SPACE )
	{
		int	index = SendMessage( handle(), LB_GETCURSEL, 0, 0 );
		if( index != -1 )
		{
			draw_string_list *pdl = _get_from_item( index );
			if( pdl )
			{
				keyword	*p = pdl->first_keyword();
				if( p )SendMessage( GetParent( handle() ), WM_COMMAND, p->command, 0 );		
			}
		}
	}
	return win_impl::on_keyup( nVirtKey );
}

inline  long hlink_list::handle_reflect_message( MSG *pmsg, long *plProcessed )
{
	if( pmsg->message == WM_MEASUREITEM )
	{
		MEASUREITEMSTRUCT *ps = (MEASUREITEMSTRUCT *)pmsg->lParam;
		draw_string_list *pdl = _get_from_item( ps->itemID );

		_rect	rect;
		::GetClientRect( handle(), &rect );
		pdl->calc_layout( rect.width()-2*cx_ofs );
		_size	size;
		pdl->get_size( &size );
		ps->itemHeight = size.cy;
		ps->itemWidth = size.cx+2*cx_ofs;
		*plProcessed = true;
	}
	if( pmsg->message == WM_DRAWITEM )
	{
		DRAWITEMSTRUCT *ps = (DRAWITEMSTRUCT *)pmsg->lParam;
		draw_string_list *pdl = _get_from_item( ps->itemID );
		_rect	rect = ps->rcItem;
		rect.offset( cx_ofs, 0 );
		pdl->draw( ps->hDC, rect, (ps->itemState & ODS_FOCUS)&&(ps->itemState & ODS_SELECTED )  );
		ps->itemState = 0;

		*plProcessed = true;
	}
	return win_impl::handle_reflect_message( pmsg, plProcessed );
}

inline  void hlink_list::handle_init()
{
	unsigned style = ::GetWindowLong( handle(), GWL_STYLE );
	_assert( style & LBS_HASSTRINGS );
	_assert( style & LBS_OWNERDRAWVARIABLE );
}

inline draw_string_list *hlink_list::_get_from_point( const _point &point )
{
	int	index = SendMessage( handle(), LB_ITEMFROMPOINT, 0, MAKELONG( point.x, point.y ) )&0xffff;
	if( index == -1 )return 0;
	return _get_from_item( index );
}
inline draw_string_list *hlink_list::_get_from_item( int index )
{
	unsigned	data = SendMessage( handle(), LB_GETITEMDATA, index, 0 );

	_assert( data != (unsigned)-1 );
	if( data == 0 )
	{
		draw_string_list	*pdl = new draw_string_list;
		pdl->set_keywords( m_pkeywords );
		pdl->set_font( (HFONT)SendMessage( handle(), WM_GETFONT, 0, 0 ) );
		_char	sz[1000];
		SendMessage( handle(), LB_GETTEXT, index, (LPARAM)sz );
		pdl->set_text( sz );
		insert_before( pdl );
		data = (unsigned)pdl;
		SendMessage( handle(), LB_SETITEMDATA, index, data );
	}

	return (draw_string_list *)data;
}

#endif //__link_ctrls_h__
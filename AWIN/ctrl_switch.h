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

#ifndef __ctrl_switch_h__
#define __ctrl_switch_h__

#include "..\awin\win_base.h"
#include "..\awin\misc_list.h"
//////////////////////////////////////////////////////////////////////////////////////////////////

#define FLAG_FIRST_VIEWINFO		0x0001
#define FLAG_LAST_VIEWINFO		0x0002
#define FLAG_ACTIVE_VIEWINFO	0x0004
#define FLAG_PRESSED_VIEWINFO	0x0008
#define FLAG_HILIGHT_VIEWINFO	0x0010



struct view_info
{
	HICON		m_icon;
	unsigned	m_command;
	_char		*m_psz;

	_rect		rect;
	unsigned	flags;		//1 - first is rect, 2 - last is rect, 0
	

	view_info()
	{
		m_icon = 0;
		m_psz = 0;
		m_command = 0;
		flags = 0;
	}
	static void free( view_info *p );
};

inline void view_info::free( view_info *p )
{
	::DestroyIcon( p->m_icon );
	::free( p->m_psz );
	delete p;
}


class view_switcher : public win_impl,
	public _list_t<view_info*, view_info::free>
{
public:
	view_switcher()
	{
		m_lpos_active = 0;
		m_focused = false;
	}


	void deinit()
	{
		clear();
		m_lpos_active = 0;
	}
	long find_by_command( unsigned cmd )
	{
		for( long lpos = head(); lpos; lpos = next( lpos ) )
			if( get( lpos )->m_command == cmd )
				return lpos;
		return 0;
	}

	void set_active_view( long lpos )
	{
		_activate_vi( lpos );
	}
protected:
	virtual long on_setfocus( HWND hwndOld )			
	{
		m_focused = true;
		_redraw_vi( m_lpos_active );
		return call_default();
	}
	virtual long on_killfocus( HWND hwndOld )			
	{
		m_focused = false;
		_redraw_vi( m_lpos_active );
		return call_default();
	}
	virtual long on_lbuttondown( const _point &pt )
	{
		SetFocus( handle() );

		long	lpos = _from_point( pt );
		if( lpos )_activate_vi( lpos );
		return win_impl::on_lbuttondown( pt );
	}

	virtual void handle_init()
	{
		_layout();
	}
	virtual long on_paint()
	{
		PAINTSTRUCT	ps;
		HDC hdc = ::BeginPaint( handle(), &ps );

		::SetBkMode( hdc, TRANSPARENT );
		::SelectObject( hdc, GetStockObject( DEFAULT_GUI_FONT ) );

		for( long	lpos = head(); lpos; lpos = next( lpos ) )
		{
			view_info	*p = get( lpos );
			_draw_view_info( hdc, p );
		}

		::EndPaint( handle(), &ps );

		return 0;
	};

	virtual long on_erasebkgnd( HDC hdc )
	{
		_rect	rect;
		GetClientRect( handle(), &rect );

		long	lpos = tail();

		if( lpos )
		{
			view_info	*pv = get( lpos );
			rect.left = pv->rect.right;
		}

		::FillRect( hdc, &rect, ::GetSysColorBrush( COLOR_3DFACE ) );

		return 1;
	}

	virtual long on_size( short cx, short cy, ulong fSizeType )
	{
		_layout();
		return win_impl::on_size( cx, cy, fSizeType );
	}

	virtual long on_getdlgcode( MSG* p )				
	{
		return DLGC_WANTARROWS;
	}

	virtual long on_keydown( long nVirtKey )			
	{
		long	lpos = m_lpos_active;

		if( nVirtKey == VK_LEFT )lpos = prev( lpos )?prev( lpos ):tail();
		else if( nVirtKey == VK_RIGHT )lpos = next( lpos )?next( lpos ):head();
		else if( nVirtKey == VK_HOME )lpos = head();
		else if( nVirtKey == VK_END )lpos = tail();

		if( lpos != m_lpos_active )
			_activate_vi( lpos );

		return true;
	}

protected:
	long _from_point( const _point &pt )
	{
		for( long lpos = head(); lpos; lpos = next( lpos ) )
		{
			view_info	*p = get( lpos );
			if( p->rect.left < pt.x &&
				p->rect.right > pt.x )
				return lpos;
		}
		return 0;
	}

	void _activate_vi( long lpos )
	{
		if( m_lpos_active == lpos )return;

		if( m_lpos_active )
		{
			view_info	*pvi = get( m_lpos_active );
			pvi->flags &= ~FLAG_ACTIVE_VIEWINFO;
			_redraw_vi( m_lpos_active );
		}

		m_lpos_active = lpos;

		if( m_lpos_active )
		{
			view_info	*pvi = get( m_lpos_active );
			pvi->flags |= FLAG_ACTIVE_VIEWINFO;
			_redraw_vi( m_lpos_active );

			if( pvi->m_command )
				::SendMessage( ::GetParent( handle() ), WM_COMMAND, pvi->m_command, 0 );
		}
	}

	void _redraw_vi( long lpos )
	{
		if( !lpos )return;

		view_info	*pvi = get( lpos );

		_rect	rect = pvi->rect;
		rect.left-=rect.height();
		rect.right+=rect.height();

		::InvalidateRect( handle(), &rect, false );
	}

	void _layout()
	{
		_rect	rect_client;
		::GetClientRect( handle(), &rect_client );

		int	current_x = 0, text_width = 80;
		for( long	lpos = head(); lpos; lpos = next( lpos ) )
		{
			view_info	*p = get( lpos );

			p->flags &= ~(FLAG_FIRST_VIEWINFO|FLAG_LAST_VIEWINFO);
			if( lpos == head() )p->flags |= FLAG_FIRST_VIEWINFO;
			if( lpos == tail() )p->flags |= FLAG_LAST_VIEWINFO;

			p->rect = rect_client;
			p->rect.left = current_x;
			p->rect.right = current_x+text_width;

			current_x=p->rect.right;
		}
	}
	void _draw_view_info( HDC hdc, view_info *p )
	{
		HPEN	hpen1 = ::CreatePen( PS_SOLID, 0, GetSysColor( COLOR_3DSHADOW ) ),
			hpen2 = ::CreatePen( PS_SOLID, 0, RGB(255, 255, 255) ),//GetSysColor( COLOR_3DHILIGHT )
			hpen_old;
		POINT	poly[5];

		int	h2 = p->rect.height()/2;
		if( p->flags & FLAG_FIRST_VIEWINFO )
		{
			poly[0].x = p->rect.left;	
			poly[0].y = p->rect.bottom-1;
			poly[1].x = p->rect.left;	
			poly[1].y = p->rect.top;
		}
		else
		{
			poly[0].x = p->rect.left+h2;	
			poly[0].y = p->rect.bottom-1;
			poly[1].x = p->rect.left+h2-p->rect.height()+1;	
			poly[1].y = p->rect.top;
		}

		if( p->flags & FLAG_LAST_VIEWINFO )
		{
			poly[2].x = p->rect.right-1;	
			poly[2].y = p->rect.top;
			poly[3].x = p->rect.right-1;	
			poly[3].y = p->rect.bottom-1;
		}
		else
		{
			poly[2].x = p->rect.right+h2-p->rect.height();	
			poly[2].y = p->rect.top;
			poly[3].x = p->rect.right+h2-1;	
			poly[3].y = p->rect.bottom-1;
		}
		poly[4].x = poly[0].x;
		poly[4].y = poly[0].y;

		HBRUSH	hbrush = ::GetSysColorBrush( COLOR_3DFACE );
		if( p->flags & FLAG_ACTIVE_VIEWINFO )
			hbrush = ::GetSysColorBrush( COLOR_3DHILIGHT );
			

		::SelectObject( hdc, hbrush );
		hpen_old = (HPEN)::SelectObject( hdc, GetStockObject( NULL_PEN ) );

		::Polygon( hdc, poly, 5 );
		//::FillRect( hdc, &p->rect, hbrush );

		int	x_icon = p->rect.left+p->rect.height()/2;
		int	y_icon = p->rect.vcenter()-8;

		if(  p->m_icon )
			::DrawIconEx( hdc, x_icon, y_icon, p->m_icon, 16, 16, 0, 0, DI_NORMAL );

		_rect	rect_text = p->rect;
		rect_text.left = x_icon+20;

		::DrawText( hdc, p->m_psz, -1, &rect_text, DT_SINGLELINE|DT_VCENTER );

		if( p->flags & FLAG_ACTIVE_VIEWINFO )
			::SelectObject( hdc, hpen1 );
		else
			::SelectObject( hdc, hpen2 );
		::MoveToEx( hdc, poly[0].x, poly[0].y, 0 );
		::LineTo( hdc, poly[1].x, poly[1].y );
		::LineTo( hdc, poly[2].x, poly[2].y );

		::SelectObject( hdc, hpen2 );
		::MoveToEx( hdc, poly[2].x, poly[2].y, 0 );
		::LineTo( hdc, poly[3].x, poly[3].y );
		::LineTo( hdc, poly[4].x, poly[4].y );

		::SelectObject( hdc, hpen_old );
		::DeleteObject( hpen1 );
		::DeleteObject( hpen2 );

		if( p->flags & FLAG_ACTIVE_VIEWINFO )
		{
			if( m_focused )
			{
				_rect	rect_focus, rect_calc;

				::DrawText( hdc, p->m_psz, -1, &rect_calc, DT_SINGLELINE|DT_CALCRECT );

				rect_focus.left = x_icon+18;
				rect_focus.right = rect_focus.left+rect_calc.width()+4;
				rect_focus.top = rect_text.vcenter()-rect_calc.height()/2-2;
				rect_focus.bottom = rect_focus.top+rect_calc.height()+4;

				::DrawFocusRect( hdc, &rect_focus );
			}
		}
	}
protected:
	long	m_lpos_active;
	bool	m_focused;
};


#endif //__ctrl_switch_h__
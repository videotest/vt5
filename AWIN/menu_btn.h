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

#ifndef __menubtn_h__
#define __menubtn_h__

#include "win_base.h"
#include "misc_xpdraw.h"
#include "comdef.h"

#define MENU_GET_MENU		(WM_USER+0x111)


class menu_btn : public win_impl
{
public:
	menu_btn();
	~menu_btn();
protected:
	virtual long handle_message( UINT m, WPARAM w, LPARAM l );
	virtual void handle_init();
	virtual long handle_reflect_message( MSG *pmsg, long *plProcessed );
	virtual long on_mouseleave( );
	virtual long on_mousemove( const _point &point );
	virtual long on_lbuttondown( const _point &point );
	virtual long on_keydown( long nVirtKey );

	bool _popup_menu();

	bool		m_hilight, m_pressed;
public:
	HMENU		m_hmenu;
	DWORD		menu_exit;
};

inline menu_btn::menu_btn()
{
	menu_exit = 0;
	m_pressed = false;
	m_hilight = false;
	m_hmenu = 0;
}

inline menu_btn::~menu_btn()
{
	if( m_hmenu )
		::DestroyMenu( m_hmenu );
}

inline void menu_btn::handle_init()
{
	::SetWindowLong( handle(), GWL_STYLE, GetWindowLong( handle(), GWL_STYLE )|BS_OWNERDRAW );
}

inline long menu_btn::on_mouseleave( )
{
	m_hilight = false;
	InvalidateRect( handle(), 0, 0 );
	return 0;
}
inline long menu_btn::on_mousemove( const _point &point )
{
	if( !m_hilight )
	{
		TRACKMOUSEEVENT	e;
		e.cbSize = sizeof( e );
		e.dwFlags = TME_LEAVE;
		e.dwHoverTime = 0;
		e.hwndTrack = handle();
		_TrackMouseEvent( &e );
		
		m_hilight = true;
		InvalidateRect( handle(), 0, 0 );
	}
	return win_impl::on_mousemove( point );
}

inline long menu_btn::on_keydown( long nVirtKey )
{
	if( nVirtKey == 32 )
		_popup_menu();
	return 1;
}
inline long menu_btn::on_lbuttondown( const _point &point )
{
//	if( !m_hmenu )
//		return win_impl::on_lbuttondown( point );

	if( !_popup_menu() )
		return win_impl::on_lbuttondown( point );

	return true;
}

inline bool menu_btn::_popup_menu()
{
	::SetFocus( handle() );
	if( GetTickCount()-menu_exit < 50 )
		return true;

	_rect	rect;
	::GetClientRect( handle(), &rect );
	::ClientToScreen( handle(), (POINT*)&rect );
	::ClientToScreen( handle(), (POINT*)&rect+1 );

	HWND	hwnd_parent = ::GetParent( handle() );
	HMENU	hmenu = (HMENU)::SendMessage( hwnd_parent, MENU_GET_MENU, GetDlgCtrlID(handle()), 0 );
	if( hmenu )
	{
		if( m_hmenu )::DestroyMenu( m_hmenu );
		m_hmenu = hmenu;
	}
	
	if( !hmenu )hmenu = m_hmenu;
	if( !hmenu )return false;

	m_pressed = true;
	InvalidateRect( handle(), 0, 0 );

	int cmd = TrackPopupMenu( m_hmenu, TPM_RETURNCMD, rect.left, rect.bottom, 0, handle(), 0 );
	
	menu_exit = GetTickCount();
	if( cmd != -1 )
		SendMessage( hwnd_parent, WM_COMMAND, cmd, 0 );
	m_pressed = false;
	InvalidateRect( handle(), 0, 0 );
	return true;
}

inline long	menu_btn::handle_message( UINT m, WPARAM w, LPARAM l )
{
	switch( m )
	{
	case WM_MOUSELEAVE:
		return on_mouseleave();
	default:
		return win_impl::handle_message( m, w, l );
	}
}



inline long menu_btn::handle_reflect_message( MSG *pmsg, long *plProcessed )
{
	if( pmsg->message == WM_DRAWITEM )
	{
		DRAWITEMSTRUCT	*pdc = (DRAWITEMSTRUCT*)pmsg->lParam;
		HDC	hdc = pdc->hDC;
		_rect	rect = pdc->rcItem;

		_char	sz[100];
		GetWindowText( handle(), sz, sizeof( sz ) );

		bool	draw_noXP = true;
		bool	disabled = false;
		
		if( xpdraw::ptr()->is_xp )
		{
			HTHEME	ht = xpdraw::ptr()->OpenThemeData( handle(), L"button" );
			if( ht )
			{
				int button_state = PBS_NORMAL;
				

				if( pdc->itemState & ODS_DEFAULT )
					button_state = PBS_DEFAULTED;
				if( pdc->itemState & ODS_DISABLED )
				{
					disabled = true;
					button_state = PBS_DISABLED;
				}
				if( pdc->itemState & 0x0040 || m_hilight )
					button_state = PBS_HOT;
				if( pdc->itemState & ODS_SELECTED || m_pressed )
					button_state = PBS_PRESSED;

				_char	sz[100];

				GetWindowText( handle(), sz, sizeof( sz ) );

				xpdraw::ptr()->DrawThemeBackground( ht, hdc, BP_PUSHBUTTON, button_state, &rect, 0 );
  				xpdraw::ptr()->DrawThemeText( ht, hdc, BP_PUSHBUTTON, button_state, _bstr_t(sz), -1, DT_SINGLELINE|DT_CENTER|DT_VCENTER, 0, &rect ); 
				xpdraw::ptr()->CloseThemeData( ht );
				draw_noXP = false;
			}
		}

		if( draw_noXP )
		{
			unsigned	flags = DFCS_BUTTONPUSH;

			if( pdc->itemState & ODS_DISABLED )
			{
				flags |= DFCS_INACTIVE;
				disabled = true;
			}
			if( pdc->itemState & 0x0040 )
				flags |= 0x1000;
			if( pdc->itemState & ODS_SELECTED || m_pressed )
				flags |= DFCS_PUSHED;


			::DrawFrameControl( hdc, &rect, DFC_BUTTON, flags );

			_rect	rect_text;
			::DrawText( hdc, sz, -1, &rect_text, DT_CALCRECT );

			unsigned	ds_flags = DST_PREFIXTEXT;

			if( pdc->itemState & ODS_DISABLED )
				ds_flags |= DSS_DISABLED;

			::DrawState( hdc, GetSysColorBrush( COLOR_GRAYTEXT ), 0,
				(LPARAM)sz, 0, rect.hcenter()-rect_text.width()/2-2, 
				rect.vcenter()-rect_text.height()/2, 
				rect_text.width(), rect_text.height(), 
				ds_flags );
		}

		int	cx = 8, cy = 4;
		int	x = rect.right-10-cx/2, y = rect.vcenter()-cy/2;

		if( disabled )
		{
			::SelectObject( hdc, ::GetStockObject( GRAY_BRUSH ) );
			::SelectObject( hdc, ::GetStockObject( NULL_PEN ) );
		}
		else
		{
			::SelectObject( hdc, ::GetStockObject( BLACK_BRUSH ) );
			::SelectObject( hdc, ::GetStockObject( BLACK_PEN ) );
		}
		_point	pts[3];
		pts[0].x = x-cx/2;
		pts[0].y = y - cy/3;
		pts[1].x = pts[0].x + cx;
		pts[1].y = pts[0].y;
		pts[2].x = x;
		pts[2].y = pts[0].y+cy;

		::Polygon( hdc, pts, 3 );

		if( pdc->itemState & ODS_FOCUS || m_pressed )
		{
			_rect	rect_focus = rect;
			::InflateRect( &rect_focus, -3, -3 );
			::DrawFocusRect( hdc, &rect_focus );
		}
	}
	return 0;
}

#endif //__menubtn_h__
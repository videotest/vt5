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

#ifndef __rendef_h__
#define __rendef_h__

#include "..\awin\misc_module.h"
#include "..\awin\misc_list.h"
#include "..\awin\misc_string.h"
#include "..\awin\misc_32bpp.h"
#include "commctrl.h"

#ifndef WM_SETMESSAGESTRING 
#define WM_SETMESSAGESTRING 0x0362  // wParam = nIDS (or 0),
#endif //WM_SETMESSAGESTRING 


#pragma comment(lib,"comctl32.lib")

enum	render_type
{
	type_render,
	type_render_rect,
	type_render_page,
	type_render_tip,
	type_render_image,
	type_render_text,
	type_render_menu,
	type_render_window
};


class _logfont : public LOGFONT
{
public:
	_logfont()
	{
		ZeroMemory( this, sizeof( LOGFONT ) );
		lfHeight = -11;
		lfCharSet = 1;
		_tcscpy( lfFaceName, _T("Tahoma") );
		m_clrText = RGB( 0, 0, 0 );
	}
	void set_bold( bool bSet ) 
	{		lfWeight = bSet?FW_BOLD:FW_NORMAL;	}
	void set_face( const _char *psz_face ) 
	{		_tcscpy( lfFaceName, psz_face );	}
	void set_text_color( COLORREF clr ) 
	{		m_clrText = clr; }
	void set_height( int nHeight ) 
	{		lfHeight = nHeight; }

	virtual HFONT	create_font()	
	{		return CreateFontIndirect( this );	}

	COLORREF	m_clrText;

};

typedef _list_t<_string>			string_list;

enum layout_side
{
	store_def = 0,
	store_left = 1,
	store_top = 2,
	store_right = 3,
	store_bottom = 4,
	store_percent = 5,
	store_hcenter = 6,
	store_vcenter = 7,
	store_prev_top = 8,
	store_width = 9,
	store_height = 10
};

enum layout_side_index
{
	lsi_left = 0,
	lsi_top = 1,
	lsi_right = 2,
	lsi_bottom = 3
};


class render
{
public:
	render();
	virtual ~render();

	static void free_render( render *p )
	{delete p;}
	void set_layout( layout_side left, layout_side top, layout_side right, layout_side bottom );
	long insert_child( render *p, long lpos_before=0 );

	virtual void deinit()	{m_childs.clear();}

	virtual render_type	type()	= 0;
	_rect	&get_rect()		{return m_rect;}
	_rect	get_def_rect()	{return m_rect_def;}
public:
	bool load_file( const _char *psz );
	virtual void layout( const RECT &rect );
	virtual void paint( HDC hdc );
	virtual void set_rect( const _rect &rect );
	virtual HWND get_window()
	{return m_parent?m_parent->get_window():0;}

	

	virtual bool set_cursor( const _point &point );
	virtual bool mouse_move( const _point &point );
	virtual bool mouse_enter( const _point &point )		{return false;};
	virtual bool mouse_leave( const _point &point );
	virtual bool mouse_click( const _point &point );
protected:
	void set_parent( render *p ){m_parent = p;}
	long calc_side( const RECT &rectOld, render *p, layout_side_index lsi );
public:
	_list_t<render*, free_render>	
				m_childs;
	render*		m_parent;
protected:
	_rect	m_rect_def;
	_rect	m_rect;

	layout_side	m_layouts[4];	
	render		*m_pmoused;
};


inline render::render()
{
    m_layouts[lsi_left] = store_def;
    m_layouts[lsi_top] = store_def;
    m_layouts[lsi_right] = store_def;
    m_layouts[lsi_bottom] = store_def;

	m_pmoused = 0;
}
inline render::~render()
{
}
inline bool render::load_file( const _char *psz )
{
	return true;
}

inline bool render::mouse_leave( const _point &point )
{
	if( m_pmoused )return m_pmoused->mouse_leave( point );
	return false;
}

inline bool render::mouse_move( const _point &point )
{
	long	lpos;
	render	*pmoused = 0;

	for( lpos = m_childs.head(); lpos; lpos = m_childs.next( lpos ) )
	{
		render	*pchild = m_childs.get( lpos );
		if( pchild->get_rect().pt_in_rect( point ) )
			pmoused = pchild;
	}

	if( m_pmoused != pmoused )
	{
		if( m_pmoused )
			m_pmoused->mouse_leave( point );
		m_pmoused = pmoused;
		if( m_pmoused )
			m_pmoused->mouse_enter( point );
	}

	if( !m_pmoused )return 0;

	return m_pmoused->mouse_move( point );
}

inline bool render::set_cursor( const _point &point )
{
	if( !m_pmoused )return false;
	return m_pmoused->set_cursor( point );
}

inline bool render::mouse_click( const _point &point )
{
	if( !m_pmoused )return 0;
	return m_pmoused->mouse_click( point );
}

inline long render::calc_side( const RECT &rectOld, render *p, layout_side_index lsi )
{
	_rect	rect = p->get_rect();
	long	lold = *(((long*)&rect)+(long)lsi);
	layout_side	ls =  p->m_layouts[lsi];
	long ldistance = 0;

	_rect	rect_o = rectOld;

	_rect	rect_parent = m_rect;
	_rect	rect_parent_def = m_rect_def;
	_rect	rect_child = p->get_rect();
	_rect	rect_child_def = p->get_def_rect();
	long	ldef = *(((long*)&rect_child_def)+(long)lsi);

	switch( ls )
	{
	case store_left:return rect_parent.left+ldef-rect_parent_def.left;
	case store_right:return rect_parent.right+ldef-rect_parent_def.right;
	case store_top:return rect_parent.top+ldef-rect_parent_def.top;
	case store_bottom:return rect_parent.bottom+ldef-rect_parent_def.bottom;
	case store_hcenter:return rect_parent.hcenter()-ldef+rect_parent_def.hcenter();
	case store_vcenter:return rect_parent.vcenter()-ldef+rect_parent_def.vcenter();
	case store_percent:
	{
		return (lsi == lsi_left||lsi == lsi_right)?
		   (rect_parent.left+MulDiv(ldef-rect_parent_def.left, rect_parent.width(), rect_parent_def.width() )):
		   (rect_parent.top+MulDiv(ldef-rect_parent_def.top, rect_parent.height(), rect_parent_def.height() ));
	}
	case store_prev_top:
	{
		for( long lpos = m_childs.head(); lpos; lpos = m_childs.next( lpos ) )
			if( m_childs.get( lpos ) == p )break;

		if( lpos )
		{
			lpos = m_childs.prev( lpos );
			if( lpos )
			{
				render	*prev = m_childs.get( lpos );
				_rect	r = p->get_def_rect();
				_rect	rp = prev->get_def_rect();
				return prev->m_rect.bottom+r.top;
			}
		}
	}

	case store_width:
		return calc_side( rectOld, p, lsi_left )+rect_child_def.width();

	case store_height:
		return calc_side( rectOld, p, lsi_top )+rect_child_def.height();
	case store_def:
	default:return ldef;
	}
}
inline void render::layout( const RECT &rect )
{
	_rect	rectOld = m_rect;
	m_rect = rect;

	for( long lpos = m_childs.head(); lpos; lpos = m_childs.next( lpos ) )
	{
		render	*p = m_childs.get( lpos );

		_rect	rectNew;

		_rect	rd = p->get_def_rect();
		rectNew.left = calc_side( rectOld, p, lsi_left );
		rectNew.right = calc_side( rectOld, p, lsi_right );
		rectNew.top = calc_side( rectOld, p, lsi_top );
		rectNew.bottom = calc_side( rectOld, p, lsi_bottom );

		//paul 01.09.2003 BT 
		if( rectNew.right < rectNew.left )
			rectNew.right = rectNew.left;
		if( rectNew.bottom < rectNew.top )
			rectNew.bottom = rectNew.top;

		p->layout( rectNew );
	}
}
inline void render::paint( HDC hdc )
{
	for( long lpos = m_childs.head(); lpos; lpos = m_childs.next( lpos ) )
	{
		render	*p = m_childs.get( lpos );

		if( p->m_rect.right  < p->m_rect.left ||
			p->m_rect.bottom < p->m_rect.top )
			return;

		p->paint( hdc );
	}
}

inline void render::set_rect( const _rect &rect )
{
	memcpy( &m_rect_def, &rect, sizeof(_rect) );
	memcpy( &m_rect, &rect, sizeof(_rect) );
}

inline void render::set_layout( layout_side left, layout_side top, layout_side right, layout_side bottom )
{
	m_layouts[lsi_left] = left;
	m_layouts[lsi_right] = right;
	m_layouts[lsi_top] = top;
	m_layouts[lsi_bottom] = bottom;

}
inline long render::insert_child( render *p, long lpos_before )
{
	p->set_parent( this );
	return m_childs.insert_before( p, lpos_before );
}

class render_rect : public render
{
public:
	render_rect();
	void set_color( COLORREF clr );
	void set_gradient( _point	pt1, 
						COLORREF clr1,  
						_point	pt2,
						COLORREF cr2 );
public:
	virtual render_type	type()	{return type_render_rect;}
	virtual void paint( HDC hdc );
protected:
	COLORREF	m_clr, m_clr_e;
	_point		m_pt1, m_pt2;
	bool		m_fOneColor;
};

inline render_rect::render_rect()
{
	m_clr = RGB( 255, 255, 255 );
	m_fOneColor = true;
}

inline void render_rect::set_gradient( _point	pt1, 
						COLORREF clr1,  
						_point	pt2,
						COLORREF clr2 )
{
	m_pt1 = pt1;
	m_pt2 = pt2;
	m_clr = clr1;
	m_clr_e = clr2;
	m_fOneColor = true;
}

inline void render_rect::set_color( COLORREF clr )
{
	m_clr = clr;
	m_fOneColor = true;
}

inline void render_rect::paint( HDC hdc )
{
	if( m_fOneColor )
	{
		HBRUSH	hbrush = ::CreateSolidBrush( m_clr );
		::FillRect( hdc, &m_rect, hbrush );
		::DeleteObject( hbrush );
	}
	else
	{
		int	cx = m_rect.width();
		int	cy = m_rect.height();

		BITMAPINFOHEADER	bi;
		ZeroMemory( &bi, sizeof( bi ) );
		bi.biWidth = cx;
		bi.biHeight = cy;
		bi.biBitCount = 24;
		bi.biPlanes = 1;

		int	cx4 = (cx*3+3)/4*4;
		byte	*p = new byte[cx4*cy];

		long	x, y;

		for( y = 0; y < cy; y++ )
		{
			byte	*pr = p+(cy-1-y)*cx4+2;
			byte	*pg = p+(cy-1-y)*cx4+1;
			byte	*pb = p+(cy-1-y)*cx4;

			for( x = 0; x < cx; x++, pr+=3, pg+=3, pb+=3 )
			{
				//*pr = r0+(x-x0)*(y-y0)*(r1-r0)/((x1-x0)*(y1-y0))

			}
		}

		delete p;
	}


	render::paint( hdc );
}

class render_page : public render
{
public:
	virtual render_type	type()	{return type_render_page;}
	render_page()	{m_hwnd = 0;}
public:
	virtual void paint( HDC hdc );
	virtual HWND get_window()
	{return m_hwnd;}
	void set_window( HWND hwnd )
	{m_hwnd = hwnd;}
protected:
	HWND	m_hwnd;
};

inline void render_page::paint( HDC hdc )
{
	if( m_rect.right  < m_rect.left ||
		m_rect.bottom < m_rect.top )return;

	HDC	hdcMem = ::CreateCompatibleDC( hdc );
	HBITMAP	hbmp = ::CreateCompatibleBitmap( hdc, m_rect.width(), m_rect.height() );
	::SelectObject( hdcMem, hbmp );
	render::paint( hdcMem );
	::BitBlt( hdc, 0, 0, m_rect.width(), m_rect.height(), hdcMem, 0, 0, SRCCOPY );
	::DeleteDC( hdcMem );
	::DeleteObject( hbmp );
}

class render_tip : public render_rect, public _logfont
{
public:
	virtual render_type	type()	{return type_render_tip;}

	render_tip();
	~render_tip();

	void add_text( const _char *psz ) 
	{
		m_strings.insert_before( psz, 0 );
	}
	void add_resource( HINSTANCE hrc, UINT nID )
	{
		_char	sz[1024];
		::LoadString( hrc, nID, sz, 1024 );
		add_text( sz );
	}

	void set_icon( HINSTANCE hrc, uint id );
	void set_icon( HICON hIcon );
public:
	virtual void paint( HDC hdc );
protected:
	HICON			m_hiconTip;
	string_list		m_strings;
};

inline render_tip::render_tip()
{
	m_hiconTip = 0;
}

inline render_tip::~render_tip()
{
	if( m_hiconTip )::DestroyIcon( m_hiconTip );
}

inline void render_tip::set_icon( HINSTANCE hrc, uint id )
{
	if( m_hiconTip )::DestroyIcon( m_hiconTip );
	m_hiconTip = XPLoadIcon( hrc, MAKEINTRESOURCE(id), 32, 32, 0 );
}

inline void render_tip::set_icon( HICON hIcon )
{
	if( m_hiconTip )::DestroyIcon( m_hiconTip );
	m_hiconTip = hIcon;
}

inline void render_tip::paint( HDC hdc )
{
	HBRUSH	hbrush = ::CreateSolidBrush( m_clr );
	::FillRect( hdc, &m_rect, hbrush );
	//???
	::DeleteObject( hbrush );

	::DrawIcon( hdc, m_rect.left+m_rect.height()/2-16, m_rect.vcenter()-16, m_hiconTip );

	const _char *psz = 0;
	if( m_strings.head() )
		psz = m_strings.get( m_strings.head() );

	::SelectObject( hdc, ::GetStockObject( BLACK_PEN ) );
	::SelectObject( hdc, ::GetStockObject( NULL_BRUSH ) );
	::Rectangle( hdc, m_rect.left, m_rect.top, m_rect.right, m_rect.bottom );

	HFONT	hfont = create_font(), holdfont = (HFONT)::SelectObject( hdc, hfont );

	_rect	rectText( m_rect );
	rectText.left += 36;
	rectText.top+=2;
	rectText.bottom -= 2;
	rectText.right -= 2;

	::SetBkMode( hdc, TRANSPARENT );
	::SetTextColor( hdc, m_clrText );
	if( psz )
	::DrawText( hdc, psz, -1, &rectText, DT_LEFT|DT_WORDBREAK|/*DT_END_ELLIPSIS|*/DT_EDITCONTROL );

	::SelectObject( hdc, holdfont );
	::DeleteObject( hfont );

	render::paint( hdc );
}

class render_image : public render
{
public:
	virtual render_type	type()	{return type_render_image;}

	render_image();
	~render_image();
public:
	void set_bitmap_resource( HINSTANCE hrc, UINT nID );
	void set_bitmap( HBITMAP hbmp );
	void set_icon( HICON hicon );
	void load_file( const _char *psz_filename );
	void set_transparent( COLORREF clr )	{m_clrTransparent = clr;}
	virtual void set_rect( const _rect &rect );
	virtual void paint( HDC hdc );


	COLORREF	m_clrTransparent;
	HIMAGELIST	m_himage;
	_size		m_size;
	HBITMAP		m_hbmp;
};

inline render_image::render_image()
{
	m_clrTransparent = (COLORREF)-1;
	m_himage = 0;
	m_hbmp = 0;
}

inline render_image::~render_image()
{
	if( m_hbmp )::DeleteObject( m_hbmp );
	if( m_himage )::ImageList_Destroy( m_himage );
}

inline void render_image::set_bitmap( HBITMAP hbmp )
{
	if( m_hbmp )::DeleteObject( m_hbmp );
	if( m_himage )::ImageList_Destroy( m_himage );
	m_hbmp = hbmp;
	m_himage = 0;

	if( !m_hbmp ) return;

	BITMAP	bm;
	::GetObject( m_hbmp, sizeof(bm), &bm );
	//::GetBitmapDimensionEx( hbmp, &m_size );
	m_size.cx = bm.bmWidth;
	m_size.cy = bm.bmHeight;

	if( m_clrTransparent != COLORREF(-1) ) 
	{
		m_himage = ImageList_Create( m_size.cx, m_size.cy, ILC_COLOR24|ILC_MASK, 1, 1 );
		ImageList_AddMasked( m_himage, m_hbmp, m_clrTransparent );
		::DeleteObject( m_hbmp );
		m_hbmp = 0;
	}
}

inline void render_image::load_file( const _char *psz_filename )
{
	if( m_hbmp )::DeleteObject( m_hbmp );
	if( m_himage )::ImageList_Destroy( m_himage );
	m_himage = 0;

	_char	sz_filename[MAX_PATH];
	::GetModuleFileName( 0, sz_filename, sizeof( sz_filename ) );

	_tcscpy( _tcsrchr( sz_filename, '\\' )+1, psz_filename );
	m_hbmp = (HBITMAP)::LoadImage( 0, sz_filename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE );
}

inline void render_image::set_icon( HICON hicon )
{
	if( m_hbmp )::DeleteObject( m_hbmp );
	if( m_himage )::ImageList_Destroy( m_himage );


	ICONINFO	ii;
	ZeroMemory( &ii, sizeof( ii ) );
	::GetIconInfo( hicon, &ii );

	BITMAP	bm;
	::GetObject( ii.hbmColor, sizeof(bm), &bm );
	//::GetBitmapDimensionEx( hbmp, &m_size );
	m_size.cx = bm.bmWidth;
	m_size.cy = bm.bmHeight;

	m_himage = ImageList_Create( m_size.cx, m_size.cy, ILC_COLOR24|ILC_MASK, 1, 1 );
	ImageList_AddIcon( m_himage, hicon );
}

inline void render_image::set_bitmap_resource( HINSTANCE hrc, uint id )
{
	if( m_hbmp )::DeleteObject( m_hbmp );
	if( m_himage )::ImageList_Destroy( m_himage );
	m_hbmp = 0;
	m_himage = 0;

	m_hbmp = ::LoadBitmap( hrc, MAKEINTRESOURCE(id) );
	if( !m_hbmp ) return;

	BITMAP	bm;
	::GetObject( m_hbmp, sizeof(bm), &bm );
	//::GetBitmapDimensionEx( hbmp, &m_size );
	m_size.cx = bm.bmWidth;
	m_size.cy = bm.bmHeight;

	if( m_clrTransparent != COLORREF(-1) ) 
	{
		m_himage = ImageList_Create( m_size.cx, m_size.cy, ILC_COLOR24|ILC_MASK, 1, 1 );
		ImageList_AddMasked( m_himage, m_hbmp, m_clrTransparent );
		::DeleteObject( m_hbmp );
		m_hbmp = 0;
	}
}

inline void render_image::paint( HDC hdc )
{
	if( m_himage )
		ImageList_Draw( m_himage, 0, hdc, m_rect.left, m_rect.top, ILD_TRANSPARENT );
	else if( m_hbmp )
	{
		HDC	hdcM = ::CreateCompatibleDC( hdc );
		::SelectObject( hdcM, m_hbmp );
		::BitBlt( hdc, m_rect.left, m_rect.top, m_rect.width(), m_rect.height(), hdcM, 0, 0, SRCCOPY );
		::DeleteDC( hdcM );
	}
	render::paint( hdc );
}

inline void render_image::set_rect( const _rect &rect )
{
	m_rect.left = rect.left;
	m_rect.top = rect.top;
	m_rect.right = rect.right?rect.right:(rect.left + m_size.cx);
	m_rect.bottom = rect.bottom?rect.bottom:(rect.top + m_size.cy);
	m_rect_def = m_rect;
}


class render_text : public render, public _logfont
{
public:		
	virtual render_type	type()	{return type_render_text;}

	render_text();

	virtual void add_text( const _char *psz )	{m_str+=psz;}
	virtual void add_resource( HINSTANCE hrc, UINT nID );
	virtual void paint( HDC hdc );
	virtual void layout( const RECT &rect );
protected:
	_string	m_str;
};

inline render_text::render_text()
{
}

inline void render_text::layout( const RECT &rect )
{
	if( m_rect_def.bottom )
	{
		render::layout( rect );
		return;
	}

	HDC	hdc = ::GetDC( 0 );
	HFONT	h = create_font();

	_rect	rect_new = rect;
	
	::SelectObject( hdc, h );
	_rect	calc = rect_new;
	::DrawText( hdc, m_str, -1, &calc, DT_CALCRECT|DT_WORDBREAK );
	rect_new.bottom = calc.bottom;
	
	::ReleaseDC( 0, hdc );
	::DeleteObject( h );

	render::layout( rect_new );
}


inline void render_text::add_resource( HINSTANCE hrc, UINT nID )
{
	_char	sz[1024];
	::LoadString( hrc, nID, sz, 1024 );
	add_text( sz );
}

inline void render_text::paint( HDC hdc )
{
	HFONT	hfont = create_font(), hfontold = (HFONT)::SelectObject( hdc, hfont );


	::SetBkMode( hdc, TRANSPARENT );
	::SetTextColor( hdc, m_clrText );
	::DrawText( hdc, m_str.ptr(), m_str.length(), &m_rect, DT_WORDBREAK|DT_EDITCONTROL/*|DT_END_ELLIPSIS*/ );

	::SelectObject( hdc, hfontold );
	::DeleteObject( hfont );
}


class render_menu : public render, public _logfont
{
public:
	struct item
	{
		uint		id;
		_rect		rect;
		_string		text;
		bool		hilighted, enabled, checked;
		bool		visible, red;

		static void free( item *pitem )
		{delete pitem;}
	};
public:		
	virtual render_type	type()	{return type_render_menu;}

	render_menu();
	~render_menu();
public:
	void set_menu_resource( HINSTANCE hrc, uint id );
	void set_menu_handle( HMENU h );
	void add_item( const _char *psz, UINT id );
	void set_target( HWND hwnd )			{m_hwnd_target = hwnd;}
	void make_item_red( unsigned cmd, bool is_red );

	virtual void paint( HDC hdc );
	virtual void layout( const RECT &rect );	

	virtual bool mouse_move( const _point &point );
	virtual bool mouse_leave( const _point &point );
	virtual bool mouse_click( const _point &point );

	void	set_yspace( int n )	{m_yspace = n;}


	void _redraw_item( item  *pitem );

	HWND		target_window();



	_list_t<item*, item::free>	m_items;
	int			m_xofs, m_yofs, m_yspace;
	HWND		m_hwnd_target;
	COLORREF	m_clrEnabled, m_clrHilight, m_clrDisabled;
	bool		m_fModifyFont;
};

inline render_menu::render_menu()
{
	m_xofs = 15;
	m_yofs = 15;
	m_yspace = 5;
	lfWeight = FW_BOLD;
	lfUnderline = 1;
	m_hwnd_target = 0;
	m_clrEnabled = RGB(0, 0, 0);
	m_clrHilight = RGB(128, 192, 255);
	m_clrDisabled = RGB(192, 192, 192);
	m_fModifyFont = false;
}

inline void render_menu::_redraw_item( item  *pitem )
{
	HWND	hwnd = get_window();
	if( !hwnd ) return ;

	int	dx = ::GetScrollPos( hwnd, SB_HORZ );
	int	dy = ::GetScrollPos( hwnd, SB_VERT );

	_rect	rect_inv( pitem->rect );
	rect_inv.offset( -dx, -dy );

	::InvalidateRect( hwnd, &rect_inv, false );
	::UpdateWindow( hwnd );
}

inline void render_menu::add_item( const _char *psz, UINT id )
{
	item	*pitem = new item;

	pitem->id = id;
	pitem->hilighted = false;
	pitem->visible = false;
	pitem->enabled = true;
	pitem->checked = false;
	pitem->red = false;
	pitem->text = psz;

	m_items.insert_before( pitem, 0 );
}

inline void render_menu::make_item_red( unsigned cmd, bool is_red )
{
	for( long lpos = m_items.head(); lpos; lpos = m_items.next( lpos ) )
	{
		item	*pi = m_items.get( lpos );
		if( pi->id == cmd )
			pi->red = is_red;
	}
}

inline render_menu::~render_menu()
{
}
inline void render_menu::set_menu_resource( HINSTANCE hrc, uint id )
{
	HMENU hmenu = ::LoadMenu( hrc, MAKEINTRESOURCE( id ) );
	HMENU	hsub = ::GetSubMenu( hmenu, 0 );
	set_menu_handle( hsub );
	::DestroyMenu( hmenu );
}

inline void render_menu::set_menu_handle( HMENU h )
{
	int	nCount = ::GetMenuItemCount( h );

	for( int i = 0; i < nCount; i++ )
	{
		item	*pitem = new item;

		pitem->id = ::GetMenuItemID( h, i );
		pitem->hilighted = false;
		pitem->enabled = true;
		pitem->checked = false;
		pitem->visible = false;
		pitem->red = false;
		_char	sz[128];
		::GetMenuString( h, i, sz, sizeof(sz), MF_BYPOSITION );
		pitem->text = sz;

		m_items.insert_before( pitem, 0 );
	}
}

inline void render_menu::layout( const RECT &rect )
{
	render::layout( rect );


	HDC	hdc = GetDC( 0 );
	HFONT	hfont = create_font();
	::SelectObject( hdc, hfont );

	long	nCount = 0;

	long	start_y = m_rect.top+m_yofs;

	for( long lpos = m_items.head(); lpos; lpos = m_items.next( lpos ) )
	{
		item	*pitem = m_items.get( lpos );
		_rect	&rect = pitem->rect;
		rect.left = m_rect.left + m_xofs;
		rect.right = m_rect.right - m_xofs;
		rect.top = start_y;
		::DrawText( hdc, pitem->text.ptr(), pitem->text.length(), &rect, DT_WORDBREAK|DT_CALCRECT );
		pitem->visible = ( rect.top < m_rect.bottom );

		start_y = rect.bottom + m_yspace; 
	}

	::ReleaseDC( 0, hdc );
	::DeleteObject( hfont );
}

inline void render_menu::paint( HDC hdc )
{
	::SetBkMode( hdc, TRANSPARENT );

	HFONT	hfont = create_font(), 
		hfontold = (HFONT)::SelectObject( hdc, hfont );

	for( long lpos = m_items.head(); lpos; lpos = m_items.next( lpos ) )
	{
		item	*pitem = m_items.get( lpos );		
		
		if( !pitem->visible )continue;


		if( m_fModifyFont )
		{
			lfUnderline = pitem->enabled && pitem->hilighted?1:0;
			lfWeight = pitem->checked ? FW_BOLD:FW_NORMAL;

			HFONT	hfontnew = create_font();
			::SelectObject( hdc, hfontnew );
			::DeleteObject( hfont );
			hfont = hfontnew;
		}

		COLORREF	cr = m_clrDisabled;

		if( pitem->enabled )
		{
			if( pitem->hilighted )
				cr = m_clrHilight;
			else if( pitem->red )
				cr = RGB( 255, 0, 0 );
			else 
				cr = m_clrEnabled;
		}
		
		::SetTextColor( hdc, cr );
		::DrawText( hdc, pitem->text.ptr(), pitem->text.length(), &pitem->rect, DT_WORDBREAK|DT_END_ELLIPSIS );
	}

	::SelectObject( hdc, hfontold );
	::DeleteObject( hfont );
}	

inline bool render_menu::mouse_leave( const _point &point )
{
	for( long lpos = m_items.head(); lpos; lpos = m_items.next( lpos ) )
	{
		item	*pitem = m_items.get( lpos );
		if( pitem->hilighted )
		{
			pitem->hilighted = false;
			_redraw_item( pitem );
			::SendMessage( target_window(), WM_SETMESSAGESTRING, 0xE001, 0 );
		}
	}
	return true;
}

inline bool render_menu::mouse_move( const _point &point )
{
	bool	need_send_message = false;
	for( long lpos = m_items.head(); lpos; lpos = m_items.next( lpos ) )
	{
		item	*pitem = m_items.get( lpos );
		bool	hilight = 	pitem->rect.pt_in_rect( point );

		if( hilight != pitem->hilighted )
		{
			pitem->hilighted = hilight;
			_redraw_item( pitem );
			if( hilight )
			{
				need_send_message = false;
				::SendMessage( target_window(), WM_SETMESSAGESTRING, pitem->id, 0 );
			}
			else
				need_send_message = true;
		}
	}

	if( need_send_message )
		::SendMessage( target_window(), WM_SETMESSAGESTRING, 0xE001, 0 );

	return true;
}

inline bool render_menu::mouse_click( const _point &point )
{
	for( long lpos = m_items.head(); lpos; lpos = m_items.next( lpos ) )
	{
		item	*pitem = m_items.get( lpos );

		if( pitem->rect.pt_in_rect( point ) )
		{	
			pitem->checked = true;
			_redraw_item( pitem );

			HWND	hwnd = target_window();
			if( hwnd )::SendMessage( hwnd, WM_COMMAND, pitem->id, 0 );

			pitem->checked = false;
			pitem->red = false;
			pitem->hilighted = false;
			_redraw_item( pitem );

			return true;
		}
	}
	return false;
}

inline HWND render_menu::target_window()
{
	if( m_hwnd_target )
		return m_hwnd_target;

	HWND	hwnd = get_window();

	while( HWND hwndP = ::GetParent( hwnd ) )
		hwnd = hwndP;
	return hwnd;
}

inline _logfont *logfont_from_render( render *p )
{
	if( p->type() == type_render_tip )
		return (_logfont*)(render_tip*)p;
	else if( p->type() == type_render_text )
		return (_logfont*)(render_text*)p;
	else if( p->type() == type_render_menu )
		return (_logfont*)(render_menu*)p;
	else 
		return 0;
}

inline render *render_from_pos( long lpos )
{
	return _list_t<render*>::get( lpos );
};

class render_bmp_gradient : public render_rect
{
public:
	render_bmp_gradient();
	virtual ~render_bmp_gradient();
	void	set_bitmap_resource( HINSTANCE hrc, unsigned idb, bool use_right = false );
	virtual void paint( HDC hdc );
protected:

	COLORREF	*m_pcr;
	int			m_height;
};

inline render_bmp_gradient::render_bmp_gradient()
{
	m_pcr = 0;
}

inline render_bmp_gradient::~render_bmp_gradient()
{
	if( m_pcr )delete 	m_pcr;

}

inline void	render_bmp_gradient::set_bitmap_resource( HINSTANCE hrc, unsigned idb, bool use_right )
{
	HBITMAP	hbmp = ::LoadBitmap( hrc, MAKEINTRESOURCE( idb ) );
	if( !hbmp )return;
	BITMAP	bm;
	::GetObject( hbmp, sizeof( bm ), &bm );

	m_height = bm.bmHeight;
	m_pcr = new COLORREF[m_height];

	BITMAPINFOHEADER	bi;
	ZeroMemory( &bi, sizeof( bi ) );
	bi.biSize = sizeof( bi );
	bi.biHeight = m_height;
	bi.biWidth = 1;
	bi.biBitCount = 24;
	bi.biPlanes = 1;

	COLORREF	*pdata = 0;

	HDC		hdc_screen = ::GetDC( 0 );
	HBITMAP hs = ::CreateDIBSection( hdc_screen, (BITMAPINFO*)&bi, DIB_RGB_COLORS, (void**)&pdata, 0, 0 );
	HDC		hdc_mem = ::CreateCompatibleDC( hdc_screen );
	HDC		hdc_src = ::CreateCompatibleDC( hdc_screen );
	::ReleaseDC( 0, hdc_screen );

	::SelectObject( hdc_mem, hs );
	::SelectObject( hdc_src, hbmp );

	int	x = 0;
	if( use_right )
		x = bm.bmWidth-1;
	::BitBlt( hdc_mem, 0, 0, 1, m_height, hdc_src, x, 0, SRCCOPY );
	::DeleteDC( hdc_screen );
	::DeleteObject( hbmp );

	::memcpy( m_pcr, pdata, sizeof( COLORREF )*m_height );

	::DeleteDC( hdc_mem );
	::DeleteObject( hs );
}

inline void	render_bmp_gradient::paint( HDC hdc  )
{
	int	cx = m_rect.width();
	int	cy = m_rect.height();

	if( cx < 1 || cy < 1 || m_height < 1 || !m_pcr )
		return;
	//m_rect
	BITMAPINFOHEADER	bi;
	ZeroMemory( &bi, sizeof( bi ) );
	bi.biSize = sizeof( bi );
	bi.biHeight = m_rect.height();
	bi.biWidth = m_rect.width();
	bi.biBitCount = 24;
	bi.biPlanes = 1;

	int	cx4 = (cx*3+3)/4*4;

	byte	*pdib = 0;
	HBITMAP hs = ::CreateDIBSection( hdc, (BITMAPINFO*)&bi, DIB_RGB_COLORS, (void**)&pdib, 0, 0 );

	for( int y = 0; y < cy; y++ )
	{
		byte	*pdest = pdib+y*cx4;
		byte	*psrc = (byte*)&m_pcr[m_height-((cy-y-1)%m_height)-1];

		for( int x = 0; x < cx; x++ )
		{
			*pdest = psrc[0];pdest++;
			*pdest = psrc[1];pdest++;
			*pdest = psrc[2];pdest++;
		}
	}

	HDC	hdc_mem = ::CreateCompatibleDC( hdc );
	::SelectObject( hdc_mem, hs );
	::BitBlt( hdc, m_rect.left, m_rect.top, cx, cy, hdc_mem, 0, 0, SRCCOPY );
	::DeleteDC( hdc_mem );
	::DeleteObject( hs );
}

#endif //__rendef_h__
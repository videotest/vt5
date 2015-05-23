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

#ifndef __drawlist_h__
#define __drawlist_h__

#include "misc_module.h"
#include "misc_list.h"
#include "misc_string.h"
#include "misc_dbg.h"

#ifndef COLOR_HOTLIGHT
#define	COLOR_HOTLIGHT	26
#endif //COLOR_HOTLIGHT

///////////////////////////////////////////////////////////////////////////////
//базовые классы для линков
#define STRING_ITEM_NEWLINE	0x0001
#define STRING_ITEM_LINK	0x0002

struct keyword
{
	unsigned command;
	_char	*psz_keyword;
	_char	*psz_display;
};

struct draw_string_item
{
	_string		word;
	unsigned	flags;
	_rect		rect;

	static void free( draw_string_item *p )
	{delete p;}

};

class draw_string_list : 
		public _list_t <draw_string_item*, draw_string_item::free>
{
public:
	static void free( draw_string_list *p )
	{delete p;}
public:
	draw_string_list();
	virtual ~draw_string_list();

	virtual bool set_text( const _char *psz );
	virtual bool draw( HDC hdc, const RECT &rect, unsigned flags );
	virtual bool calc_layout( int width );
	virtual bool set_cursor( const _point &pt );

	void	set_warpable()	{m_warpable = true;}

	bool set_font( HFONT h );
		
	draw_string_item	*hit_test( const _point _pt );
	void get_size( SIZE *psize );

	void set_keywords( keyword *p );
	keyword	*lookup( const _char *psz_key );
	keyword	*first_keyword();

	virtual long on_erase_rect( HDC hdc, const _rect &rect );


	bool	m_focused;
protected:
	keyword	*m_pkeywords;
	bool	m_fLayout;
	_size	m_size;
	HFONT	m_hfont, m_hfont_link;
	_point	m_pt_offset;
	bool	m_warpable;
public:
	static HCURSOR &get_cursor(){static HCURSOR s_cursor;return s_cursor;}
};


///////////////////////////////////////////////////////////////////////////////
//классы для линков - реализация. список
inline draw_string_list::draw_string_list()
{
	m_warpable = false;
	m_pkeywords = 0;
	m_fLayout = false;
	m_hfont = m_hfont_link = 0;
	set_font( (HFONT)::GetStockObject( DEFAULT_GUI_FONT ) );
		::DeleteObject( m_hfont_link );
}

inline draw_string_list::~draw_string_list()
{
	::DeleteObject( m_hfont_link );
}

inline long draw_string_list::on_erase_rect( HDC hdc, const _rect &rect )
{
	::FillRect( hdc, &rect, ::GetSysColorBrush( COLOR_3DFACE ) );
	return true;
}

inline draw_string_item	*draw_string_list::hit_test( const _point pt )
{
	for( long lpos = head(); lpos; lpos = next( lpos ) )
	{
		draw_string_item	*pe = get( lpos );

		if( pe->flags & STRING_ITEM_LINK )
		{
			_rect	rect_test = pe->rect;
			rect_test.offset( m_pt_offset.x, m_pt_offset.y );

			if( rect_test.pt_in_rect( pt ) )
				return pe;
		}
	}

	return 0;
}

inline bool draw_string_list::set_cursor( const _point &pt )
{
	if( !hit_test( pt ) )
		return false;

	::SetCursor( get_cursor() );
	return true;
}

inline bool draw_string_list::calc_layout( int width )
{
	m_size.cx = 0;
	m_size.cy = 0;

	HDC	hdc = ::GetDC( 0 );

	::SelectObject( hdc, m_hfont );

	int	x = 0, y = 0;
	_rect	rect;
	::DrawText( hdc, _T(" "), -1, &rect, DT_CALCRECT|DT_TOP|DT_LEFT );
	int	cy = rect.height();
	int	max_width = 0, max_height = 0;

	for( long lpos = head(); lpos; lpos = next( lpos ) )
	{
		draw_string_item	*pe = get( lpos );

		::SetRectEmpty( &rect );

		const _char *psz_draw_text = pe->word;
		if( pe->flags & STRING_ITEM_LINK )
		{
			keyword	*p = lookup( pe->word );
			if( p )psz_draw_text = p->psz_display;
		}


		::DrawText( hdc, psz_draw_text, -1, &rect, DT_CALCRECT|DT_TOP|DT_LEFT );

		if( ( width!=0 && x+rect.width() > width ) || 
				pe->flags & STRING_ITEM_NEWLINE )
		{
			x = 0;
			y += cy;
		}

		pe->rect.left = x;
		pe->rect.top = y;
		pe->rect.right = x+rect.width();
		pe->rect.bottom = y+cy;

		max_width = max( max_width, pe->rect.right );
		max_height = max( max_height, pe->rect.bottom );

		x += rect.width();
	}

	::ReleaseDC( 0, hdc );

	m_size.cx = width?width:max_width;
	m_size.cy = max_height;

	m_fLayout = true;

	return true;
}

inline void draw_string_list::get_size( SIZE *psize )
{
	if( !m_fLayout )
		calc_layout( psize->cx );
	
	psize->cx = m_size.cx;
	psize->cy = m_size.cy;
}

inline bool draw_string_list::set_font( HFONT h )
{
	m_hfont = h;

	::DeleteObject( m_hfont_link );
	LOGFONT	lf;
	::GetObject( m_hfont, sizeof( LOGFONT ), &lf );
	lf.lfUnderline = 1;
	m_hfont_link = ::CreateFontIndirect( &lf );
	
	return true;
}

inline bool draw_string_list::set_text( const _char *psz_text )
{
	clear();
	m_fLayout = 0;

	//draw_string_item	*p = new draw_string_item;
	long	state = 0;	//0-word, 1 - space
	bool	new_line = false, link = false, first_char= true, one_char_token = false;

	for( const _char *psz = psz_text;; psz++ )
	{
		if( (*psz != ' ' && state == 1 && link == false) ||
			(*psz == '#' && link == true ) || //добрались до следующего слова 
			(*psz == '\n') ||				//добрались до конца строки
			one_char_token ||				//добрались до конца строки
			(*psz == 0 ) )					//добрались до конца
		{
			one_char_token = false;
			draw_string_item	*p = new draw_string_item;
			p->flags = (new_line?STRING_ITEM_NEWLINE:0)|(link?STRING_ITEM_LINK:0);
			if( link )p->word.copy( psz_text+1, (long)psz-(long)psz_text );
			else p->word.copy( psz_text, (long)psz-(long)psz_text+1 );
			
			insert_before( p );

			new_line = (*psz == '\n');
			if( new_line||link )psz++;

			while(*psz == '\n')//возможно несколько строк
			{
				draw_string_item	*p = new draw_string_item;
				p->flags = STRING_ITEM_NEWLINE;
				insert_before( p );
				psz++;
			}

			link = false;
			state = 0;
			first_char = true;

			if( !*psz )break;

			psz_text = psz;
		}

		if( *psz == ' ' && state == 0 )
		{
			state = 1;
			continue;
		}

		if( state == 0 && first_char )
		{
			link = *psz == '#';
		}

		first_char = false;

		if( *psz == '('|| *psz == ')'  )
			one_char_token = true;
	}
	return true;
}

inline bool draw_string_list::draw( HDC hdc, const RECT &rect, unsigned flags )
{
	if( !m_fLayout || m_warpable )calc_layout( rect.right-rect.left );
	m_pt_offset.x = rect.left;
	m_pt_offset.y = rect.top;

	::SetBkMode( hdc, TRANSPARENT );
	::SetBkColor( hdc, ::GetSysColor( COLOR_WINDOW ) );
	::SetROP2( hdc, R2_COPYPEN );

	for( long lpos = head(); lpos; lpos = next( lpos ) )
	{
		draw_string_item	*p = get( lpos );
		_rect	rect_item = p->rect;

		rect_item.offset( rect.left, rect.top );

		const _char *psz_draw = p->word;

		if( p->flags & STRING_ITEM_LINK )
		{
			on_erase_rect( hdc, rect_item );

			::SetTextColor( hdc, ::GetSysColor( COLOR_HOTLIGHT ) );
			::SelectObject( hdc, m_hfont_link );

			keyword	*pkw = lookup( p->word );
			if( pkw )psz_draw = pkw->psz_display;
		}
		else
		{
			::SetTextColor( hdc, ::GetSysColor( COLOR_WINDOWTEXT ) );
			::SelectObject( hdc, m_hfont );
		}
		::DrawText( hdc, psz_draw, -1, &rect_item, DT_LEFT|DT_TOP );

		if( p->flags & STRING_ITEM_LINK && flags )//focus
		{
			::DrawFocusRect( hdc, &rect_item );
		}
	}

	return true;
}

inline void draw_string_list::set_keywords( keyword *p )
{
	m_pkeywords = p;
	if( !m_pkeywords )return;
	for( int n = 0; m_pkeywords[n].psz_keyword; n++ )
		if( !m_pkeywords[n].psz_display )
			m_pkeywords[n].psz_display = m_pkeywords[n].psz_keyword;
}

inline keyword	*draw_string_list::lookup( const _char *psz_key )
{
	if( !m_pkeywords )return 0;
	for( int n = 0; m_pkeywords[n].psz_keyword; n++ )
	{
		if( !_tcsicmp( m_pkeywords[n].psz_keyword, psz_key ) )
			return &m_pkeywords[n];
	}
	return 0;
}

inline keyword	*draw_string_list::first_keyword()
{
	if( !m_pkeywords )return 0;
	for( long lpos = head(); lpos; lpos = next( lpos ) )
	{
		draw_string_item	*p = get( lpos );
		if( p->flags & STRING_ITEM_LINK )
			return lookup( p->word );
	}
	return 0;
}




#endif //__drawlist_h__
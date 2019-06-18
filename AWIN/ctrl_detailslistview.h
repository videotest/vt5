#pragma once
///////////////////////////////////////////////////////////////////////////////
// This file is part of AWIN class library
// Copyright (c) 2001-2002 Andy Yamov
//
// Permission to use, copy, modify, distribute, and sell this software and
// its documentation for any purpose is hereby granted without fee
// 
// THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,
// EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
// WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
// 


#include "stdafx.h"

#include "misc_module.h"
#include "win_base.h"
#include "misc_list.h"
#include "misc_map.h"
#include "win_controls.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//column description

#define COLUMN_STATE_HIDDEN		0
#define COLUMN_STATE_VISIBLE	1
#define COLUMN_STATE_CHILD		2

struct column_info
{
	char		*psz_name;		//default name
	char		*psz_caption;	//caption of column
	int			width;	
	unsigned	format;
	unsigned	state;

	int			def_width;
	unsigned	def_format;
	unsigned	def_state;
	int			def_order;

	unsigned	cookie;
};

class outpost_columns
{
public:
	virtual long first_column() = 0;
	virtual long next_column( long lpos ) = 0;
	virtual column_info	*get_column( long lpos ) = 0;
	virtual void clear_columns() = 0;					//do not free column_info pointers
	virtual long insert_column( column_info *p ) = 0;
};

class columns_description : 
	public outpost_columns,
	public _list_t<column_info*>
{
public:
	virtual ~columns_description()					{clear();}
public:
	virtual long first_column()						{return head();}
	virtual long next_column( long lpos )			{return next( lpos );}
	virtual column_info	*get_column( long lpos )	{return get( lpos );}
	virtual void clear_columns()					{clear();}
	virtual long insert_column( column_info *p )
	{		return insert_before( p );	}

	long add_column( unsigned cookie, char *psz_name, char *psz_caption, int width, unsigned format, unsigned state )
	{
		column_info	*pcol = new column_info;
		pcol->cookie = cookie;
		pcol->def_format = pcol->format = format;
		pcol->def_width = pcol->width = width;
		pcol->def_state = pcol->state = state;
		pcol->psz_caption = _strdup( psz_caption );
		pcol->psz_name = _strdup( psz_name );

		return insert_before( pcol );
	}

	void free( column_info	*pcol )
	{
		::free( pcol->psz_caption );
		::free( pcol->psz_name );
		delete pcol;
	}
};

#define ITEM_STATE_COLLAPSED	0
#define ITEM_STATE_EXPANDED		1

#define LISTDN_GETDISPINFO		-1500

#define LISTD_DISPINFO_NEEDTEXT		0x01
#define LISTD_DISPINFO_NEEDIMAGE	0x02

struct LISTD_DISPINFO
{
	NMHDR	hdr;

	unsigned	mask;
	char		*psz_text;
	size_t		cb_text;
	int			image;

	unsigned	column_cookie;
	unsigned	param;
};

//этот класс - оконный класс для контрола списка (listcontrol) которого могут раскрываться.
class list_details_ctrl : public list_ctrl
{
protected:
	//базовая структура - хранится в param для каждой строки списка
	struct row_data
	{
	public:
		virtual ~row_data()		{};
		static void free( row_data *p )
		{delete p;}
	public:
		bool	is_item;
		long	lpos_list;
	};

	//хранится только для основной строки (со столбцами)
	struct item_data : public row_data
	{
	public:
		item_data()	
		{
			is_item = true;
			state = ITEM_STATE_COLLAPSED;
			param = 0;
		}
	public:
		unsigned	param;
		unsigned	state;
	};

	//хранится только для строки ITEM
	struct	subitem_data  : public row_data
	{
	public:
		subitem_data( column_info *pcol, item_data *pitem )	
		{
			is_item = false;
			m_pcolumn = pcol;
			m_pitem = pitem;
			is_last = false;
		}
	public:
		column_info	*m_pcolumn;
		item_data	*m_pitem;
		bool		is_last;
	};



public:
	list_details_ctrl()
	{
		m_childs_column_count = 0;
		m_pcols = 0;
		m_images = ImageList_Create( 16, 16, ILC_COLOR|ILC_MASK, 2, 2 );
		HICON h = LoadIcon( module::hrc(), MAKEINTRESOURCE(IDI_COLLAPSED));
		ImageList_AddIcon( m_images, h );
		::DestroyIcon( h );
		h = LoadIcon( module::hrc(), MAKEINTRESOURCE(IDI_EXPANDED));
		ImageList_AddIcon( m_images, h );
		::DestroyIcon( h );
	}

	virtual ~list_details_ctrl()
	{
		ImageList_Destroy( m_images );
	}

	virtual void handle_init()
	{
		list_ctrl::set_imagelist( m_images, LVSIL_SMALL );
		list_ctrl::set_extended_style( LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT );
	}

	virtual void get_text( char *psz, size_t cb, unsigned param, unsigned cookie );
	virtual long on_keydown( long nVirtKey );
	virtual long on_paint();
	virtual long on_notify_reflect( NMHDR *pnmhdr, long *plProcessed );
	virtual long on_notify( uint idc, NMHDR *pnmhdr );
//это функции высокого уровня, доступны через интерфейс сообщений
//
public:
	virtual long set_columns_info( outpost_columns *pcols );
	virtual long insert_item( unsigned item_data );
	virtual long delete_item( unsigned item_data );
	virtual long delete_all_items();
//	virtual long expand( unsigned item_data, unsigned state );
	virtual long update_item( unsigned item_data );

	


//а это - для приватного использования
protected:
	void _load_item( item_data	*pdata );
	void _reload_columns();
	void _reload_items();
	void _free_subitems();
	void _delete_childs( int idx );
	int _insert_childs( int idx );
	row_data	*_data_from_index( int idx );
	item_data*	_data_from_param( unsigned param );
	long _index_from_data( row_data *pdata );
	void _expand( long index, int state );
protected:
	outpost_columns					*m_pcols;
	_list_map_t<item_data*,unsigned, cmp_unsigned,item_data::free>	m_items;
	_list_t<row_data*, row_data::free>								m_childs;

	HIMAGELIST						m_images;
//cache hor paint
	long							paint_width;
	HPEN							paint_pen;

	long							m_childs_column_count;
};

inline long list_details_ctrl::on_paint()
{
	_rect	rect;
	int	last_col = list_ctrl::get_columns_count()-1;
	list_ctrl::get_subitem_rect( 0, last_col, 0, &rect );

	paint_width = rect.right;
	paint_pen = ::CreatePen( PS_SOLID, 0, ::GetSysColor(COLOR_3DSHADOW)  );

	long	lret = list_ctrl::on_paint();

	::DeleteObject( paint_pen );

	return lret;
}

inline long list_details_ctrl::on_keydown( long nVirtKey )
{
	if( nVirtKey == VK_LEFT || nVirtKey == '-' )
	{
		long	index = list_ctrl::get_selected_item();
		if( index != -1 )
		{
			row_data	*pdata = _data_from_index( index );
			item_data	*pi = 0;

			if( !pdata->is_item ) 
			{
				subitem_data	*p = (subitem_data*)pdata;
				index = _index_from_data( p->m_pitem );
				_assert(index != -1 );
				pi = p->m_pitem;
			}
			else
			{
				pi = (item_data*)pdata;
			}

			if( pi->state == ITEM_STATE_EXPANDED )
			{
				_expand( index, ITEM_STATE_COLLAPSED );
				list_ctrl::set_selected_item( index );
				return 1;
			}
		}
	}
	if( nVirtKey == VK_RIGHT || nVirtKey == '+' )
	{
		long	index = list_ctrl::get_selected_item();
		if( index != -1 )
		{
			row_data	*pdata = _data_from_index( index );
			item_data	*pi = 0;

			if( pdata->is_item ) 
			{
				pi = (item_data*)pdata;
				if( pi->state == ITEM_STATE_COLLAPSED )
				{
					_expand( index, ITEM_STATE_EXPANDED );
					return 1;
				}
			}
		}
	}


	return list_ctrl::on_keydown( nVirtKey );
}

inline long list_details_ctrl::on_notify( uint idc, NMHDR *pnmhdr )
{
	if( pnmhdr->code == HDN_BEGINTRACK )
	{
		OutputDebugString( "HDN_BEGINTRACK\n" );
	}
	else if( pnmhdr->code == HDN_ENDTRACK )
	{
		OutputDebugString( "HDN_ENDTRACK\n" );
	}
	else if( pnmhdr->code == HDN_TRACK )
	{
		OutputDebugString( "HDN_TRACK\n" );
	}
	else if( pnmhdr->code == HDN_BEGINDRAG )
	{
		OutputDebugString( "HDN_BEGINDRAG\n" );
		return true;
	}
	else if( pnmhdr->code == HDN_ENDDRAG )
	{
		OutputDebugString( "HDN_ENDDRAG\n" );
	}
	return list_ctrl::on_notify( idc, pnmhdr );
}

inline long list_details_ctrl::on_notify_reflect( NMHDR *pnmhdr, long *plProcessed )
{
	if( pnmhdr->code == LVN_GETDISPINFO )
	{
		*plProcessed = true;
		NMLVDISPINFO	*pdisp = (NMLVDISPINFO*)pnmhdr;
		pdisp->item.iImage = -1;
		pdisp->item.mask = LVIF_IMAGE|LVIF_TEXT;

		row_data	*pdata = (row_data*)pdisp->item.lParam;

		
		if( pdata->is_item )
		{
			item_data	*p = (item_data*)pdata;

			if( pdisp->item.iSubItem == 0 )
			{
				*pdisp->item.pszText = 0;
				if( m_childs_column_count )
					pdisp->item.iImage = (p->state == ITEM_STATE_EXPANDED) ? 1: 0 ;
			}
			else
			{
				HWND	hwnd = list_ctrl::get_header();
				
				HD_ITEM	item;
				item.mask = HDI_LPARAM;
				Header_GetItem( hwnd, pdisp->item.iSubItem, &item );

				column_info	*pcolumn  = (column_info*)item.lParam;
				get_text( pdisp->item.pszText, pdisp->item.cchTextMax, p->param, pcolumn->cookie );
			}
		}
		else
		{
			if( pdisp->item.iSubItem == 1 )
			{
				char	sz[260];
				subitem_data	*p = (subitem_data*)pdata;
				get_text( sz, sizeof( sz ), p->m_pitem->param, p->m_pcolumn->cookie );
				_snprintf( pdisp->item.pszText, pdisp->item.cchTextMax, "%s: %s", p->m_pcolumn->psz_caption, sz );
			}
			else
				*pdisp->item.pszText = 0;

		}
		*plProcessed = true;
		return true;
	}
	else if( pnmhdr->code == NM_CLICK )
	{
		NMITEMACTIVATE	*pnm = (NMITEMACTIVATE*)pnmhdr;
		row_data	*p = _data_from_index( pnm->iItem );//(row_data*)pnm->lParam;
		if( pnm->iSubItem == 0 && p && p->is_item )
		{
			item_data	*pdata = (item_data	*)p;

			_expand( pnm->iItem, (pdata->state == ITEM_STATE_EXPANDED)?ITEM_STATE_COLLAPSED:ITEM_STATE_EXPANDED );
		}
	}
	else if( pnmhdr->code == NM_DBLCLK )
	{
		NMITEMACTIVATE	*pnm = (NMITEMACTIVATE*)pnmhdr;
		row_data	*p = _data_from_index( pnm->iItem );//(row_data*)pnm->lParam;
		if( p && p->is_item )
		{
			item_data	*pdata = (item_data	*)p;

			_expand( pnm->iItem, (pdata->state == ITEM_STATE_EXPANDED)?ITEM_STATE_COLLAPSED:ITEM_STATE_EXPANDED );
		}
	}
	else if( pnmhdr->code == NM_CUSTOMDRAW )
	{
		*plProcessed = true;
		NMLVCUSTOMDRAW	*plvcd = (NMLVCUSTOMDRAW *)pnmhdr;

		if( plvcd->nmcd.dwDrawStage == CDDS_PREPAINT )
			return CDRF_NOTIFYITEMDRAW;
		else if( plvcd->nmcd.dwDrawStage == (CDDS_ITEM | CDDS_ITEMPREPAINT))
		{
			row_data	*p = (row_data	*)plvcd->nmcd.lItemlParam;
			if( p->is_item )return CDRF_DODEFAULT|CDRF_NOTIFYITEMDRAW;
			subitem_data	*pdata = (subitem_data*)p;

			HDC	hdc = plvcd->nmcd.hdc;

			
			HPEN hold = (HPEN)::SelectObject( hdc, paint_pen );

			_rect	rect;
			list_ctrl::get_subitem_rect( plvcd->nmcd.dwItemSpec, 1, LVIR_BOUNDS, &rect );

			int	x = rect.left+rect.height()/2;
			int	x1 = rect.left+rect.height();
			int	y = rect.vcenter();

			::MoveToEx( hdc, x, rect.top, 0 );
			if( pdata->is_last )
				::LineTo( hdc, x, y );
			else
				::LineTo( hdc, x, rect.bottom );

			::MoveToEx( hdc, x, y, 0 );
			::LineTo( hdc, x1, y );

			::SelectObject( hdc, hold );

			rect.left = x1+3;
			rect.right = paint_width;


			HBRUSH	hbr = 0;
			if( plvcd->nmcd.uItemState & CDIS_SELECTED )
			{
				if( GetFocus() == handle() )
				{
					hbr = ::GetSysColorBrush( COLOR_HIGHLIGHT );
					::SetTextColor( hdc, ::GetSysColor( COLOR_CAPTIONTEXT ) );
				}
				else
				{
					hbr = ::GetSysColorBrush( COLOR_INACTIVECAPTION );
					::SetTextColor( hdc, ::GetSysColor( COLOR_INACTIVECAPTIONTEXT ) );
				}
			}
			else
			{
				hbr = ::GetSysColorBrush( COLOR_WINDOW );
				::SetTextColor( hdc, ::GetSysColor( COLOR_WINDOWTEXT ) );
			}
	

			::FillRect( hdc, &rect, hbr );

			if( plvcd->nmcd.uItemState & CDIS_FOCUS )
				::DrawFocusRect( hdc, &rect );

			char	sz[260];
			strcpy( sz, pdata->m_pcolumn->psz_caption );
			strcat( sz, ": " );
			long	len = strlen( sz );
			get_text( sz+len, sizeof( sz )-len, pdata->m_pitem->param, pdata->m_pcolumn->cookie );
			
			int	old_bk = ::SetBkMode( hdc, TRANSPARENT );
			::SelectObject( hdc, GetStockObject( DEFAULT_GUI_FONT ) );

			unsigned	flags = DT_SINGLELINE|DT_VCENTER;
			::DrawText( hdc, sz, -1, &rect, flags );
			::SetBkMode( hdc, old_bk );


			return CDRF_SKIPDEFAULT;
		}
	}

	return false;
}

inline void list_details_ctrl::get_text( char *psz, size_t cb, unsigned param, unsigned cookie )
{
	LISTD_DISPINFO	info;
	ZeroMemory( &info, sizeof( info ) );
	info.hdr.code = LISTDN_GETDISPINFO;
	info.hdr.hwndFrom = handle();
	info.hdr.idFrom = ::GetDlgCtrlID( handle() );
	info.mask = LISTD_DISPINFO_NEEDTEXT;
	info.psz_text = psz;
	info.cb_text = cb;
	info.column_cookie = cookie;
	info.param = param;
	
	HWND	hwnd_parent = ::GetParent( handle() );
	strcpy( info.psz_text, "??" );
	::SendMessage( hwnd_parent, WM_NOTIFY, (WPARAM)handle(), (LPARAM)&info );

}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//установить описание столбцов для контрола
inline long list_details_ctrl::set_columns_info( outpost_columns *pcols )
{
	delete_all_items();
	delete_all_columns();

	_free_subitems();

	m_pcols = pcols;

	_reload_columns();
	_reload_items();

	m_childs_column_count = 0;
	for( long lpos = m_pcols->first_column(); lpos; lpos = m_pcols->next_column( lpos ) )
		if( m_pcols->get_column( lpos )->state == COLUMN_STATE_CHILD )
			m_childs_column_count++;


	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//добавить элемент в контрол. передается только data для элемента, все остальное - через callback
inline long list_details_ctrl::insert_item( unsigned param )
{
	item_data	*pdata = new item_data;
	pdata->param = 	param;
	pdata->lpos_list = m_items.set( pdata, param );

	int	count = list_ctrl::get_items_count();

	_load_item( pdata );

	if( !count )list_ctrl::set_selected_item( 0 );

	return pdata->lpos_list;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//обновить элемент - факически переписовать его прямоугольник
inline long list_details_ctrl::update_item( unsigned param )
{
	item_data	*pdata = _data_from_param( param );
	if( !pdata )return 0;
	int	index = _index_from_data( pdata );
	if( index == -1 )return 0;

	_rect	rect;
	list_ctrl::get_item_rect( index, LVIR_BOUNDS, &rect );

	if( pdata->state == ITEM_STATE_EXPANDED )
	{
		int	count = list_ctrl::get_items_count();

		for( index++; index < count; index++ )
		{
			row_data	*pr = _data_from_index( index );
			if( pr->is_item )break;
		}

		index--;

		_rect	rect1;
		list_ctrl::get_item_rect( index, LVIR_BOUNDS, &rect1 );
		rect.bottom = rect1.bottom;
	}

	if( rect.left != rect.right &&
		rect.top != rect.bottom )
		InvalidateRect( handle(), &rect, false );


	return 1;


}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//удалить элемент
inline long list_details_ctrl::delete_item( unsigned param )
{
	item_data*	pdata = _data_from_param( param );
	if( !pdata )return 0;

	int	idx = _index_from_data( pdata );
	if( idx != -1 )
	{
		_delete_childs( idx );
		list_ctrl::delete_item( idx );
	}

	m_items.remove( pdata->lpos_list );

	int	new_selected = list_ctrl::get_selected_item();
	int	count = get_items_count();

	if( new_selected == -1 ) 
		list_ctrl::set_selected_item( min( idx, count-1 ) );

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//удалить все элементы.
inline long list_details_ctrl::delete_all_items()
{
	list_ctrl::delete_all_items();

	m_items.clear();
	m_childs.clear();

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//функции-толзы

inline void list_details_ctrl::_expand( long index, int state )
{
	row_data	*p = _data_from_index( index );
	_assert( p->is_item );

	item_data	*pdata = (item_data*)p;

	pdata->state = state;
	if( state == ITEM_STATE_EXPANDED )
	{
		int last = _insert_childs( index );
		list_ctrl::ensure_visible( last, false );

	}
	else
	{
		_delete_childs( index );
		list_ctrl::ensure_visible( index, false );
	}

	_rect	rect;
	list_ctrl::get_item_rect( index, LVIR_ICON, &rect );

	InvalidateRect( handle(), &rect, false );
}

inline list_details_ctrl::row_data	*list_details_ctrl::_data_from_index( int idx )
{
	return (row_data*)list_ctrl::get_item_data( idx );
}

//удалить все подзаписи из контрола (и из списка подзаписей)
inline void list_details_ctrl::_delete_childs( int start )
{
	start++;
	while( true )
	{
		row_data	*pdata = _data_from_index( start );
		if( !pdata )break;
		if( pdata->is_item )break;
		list_ctrl::delete_item( start );
		m_childs.remove( pdata->lpos_list );
	}
}


//добавить все подзаписи из контрола (и из списка подзаписей)
inline int list_details_ctrl::_insert_childs( int idx )
{
	if( !m_pcols )
		return idx;

	item_data	*pdata = (item_data	*)_data_from_index( idx );
	
	_assert( pdata->is_item );
	LV_ITEM	item;
	ZeroMemory( &item, sizeof( item ) );
	item.mask = LVIF_PARAM|LVIF_TEXT;
	item.pszText = LPSTR_TEXTCALLBACK;
	idx++;

	subitem_data	*p = 0;

	for( long lpos = m_pcols->first_column(); lpos; lpos = m_pcols->next_column( lpos ) )
	{
		column_info	*pcolumn = m_pcols->get_column( lpos );
		if( pcolumn->state == COLUMN_STATE_CHILD )
		{
			p = new subitem_data( pcolumn, pdata );
			item.lParam = (LPARAM)p;
			item.iItem = idx;
			p->lpos_list= m_childs.insert_before( p );

			list_ctrl::insert_item( &item );
			list_ctrl::set_item_text( idx, 1, LPSTR_TEXTCALLBACK );

			idx++;	
		}
	}

	if( p )p->is_last = true;

	return idx-1;
}


//поиск item_data по параметру
inline list_details_ctrl::item_data*	list_details_ctrl::_data_from_param( unsigned param )
{
	long	lpos = m_items.find( param );
	if( !lpos )return 0;
	return m_items.get( lpos );
}

inline long list_details_ctrl::_index_from_data( row_data *pdata )
{
	return list_ctrl::find_item( (unsigned)pdata );
}


//загрузить информацию об итеме
inline void list_details_ctrl::_load_item( item_data	*pdata )
{
	LV_ITEM	item;
	ZeroMemory( &item, sizeof( item ) );
	item.mask = LVIF_PARAM|LVIF_TEXT;
	item.lParam = (LPARAM)pdata;
	item.pszText = LPSTR_TEXTCALLBACK;
	item.iItem = list_ctrl::get_items_count();//это бред, потом надо будет думать насчет сортировки

	int idx_item = list_ctrl::insert_item( &item );
	int	idx_column = 0;

	if( m_pcols )
	{
		for( long lpos = m_pcols->first_column(); lpos; lpos = m_pcols->next_column( lpos ) )
		{
			column_info	*pcolumn = m_pcols->get_column( lpos );
			if( pcolumn->state == COLUMN_STATE_VISIBLE )
				list_ctrl::set_item_text( idx_item, idx_column++, LPSTR_TEXTCALLBACK );
		}
	}

	if( pdata->state == ITEM_STATE_EXPANDED )
		_insert_childs( idx_item );
}

inline void list_details_ctrl::_free_subitems()
{
//free all child entries
	m_childs.clear();
}

//reload all items
inline void list_details_ctrl::_reload_items()
{
	for( long lpos = m_items.head(); lpos; lpos = m_items.next( lpos ) )
	{
		item_data	*pdata = m_items.get( lpos );
		_load_item( pdata );
	}
}

//reload all columns
inline void list_details_ctrl::_reload_columns()
{
	int	idx = 0;
	if( !m_pcols )return;

	{
		LV_COLUMN	column;
		ZeroMemory( &column, sizeof( column ) );
		column.cx = 20;
		column.mask = LVCF_WIDTH;
		list_ctrl::insert_column( idx, &column );
	}

	HWND	hwnd_header = list_ctrl::get_header();
	
	idx++;
	for( long lpos = m_pcols->first_column(); lpos; lpos = m_pcols->next_column( lpos ) )
	{
		column_info	*pcolumn = m_pcols->get_column( lpos );
		if( pcolumn->state == COLUMN_STATE_VISIBLE )
		{
			LV_COLUMN	column;
			ZeroMemory( &column, sizeof( column ) );

			column.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT;
			column.cx = pcolumn->width;
			column.fmt = pcolumn->format;
			column.pszText = pcolumn->psz_caption;

			list_ctrl::insert_column( idx, &column );

			HD_ITEM	item;
			item.mask = HDI_LPARAM;
			item.lParam = (LPARAM)pcolumn;

			Header_SetItem( hwnd_header, idx, &item );

			idx++;
		}
	}
}




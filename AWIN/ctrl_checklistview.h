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

#ifndef __ctrl_checklistview_h__
#define __ctrl_checklistview_h__

#include "win_controls.h"
#include "misc_ptr.h"
#include "misc_list.h"

#define CLV_BASE			(WM_USER+100)

#define CLV_ADD_CHECK_COLUMN	(CLV_BASE+0)
#define CLV_SETCHECK			(CLV_BASE+1)
#define CLV_GETCHECK			(CLV_BASE+2)

#define CLV_ROWCOL(row,col)		((row<<16)|col)


#define CLVN_CHECKCHANGE		(LVN_LAST-1)

struct CLVNM_CHECK
{
	NMHDR	nmhdr;
	int		row, col, check;
	LPARAM	lParam;
};


#define SC_REDRAW	1
#define SC_NOTIFY	2
#define SC_SKIPDISABLED	4

#ifndef IDB_LIST_DISABLED_CHECKED
#define IDB_LIST_DISABLED_CHECKED	-1
#endif //IDB_LIST_DISABLED_CHECKED

class check_list_ctrl : public list_ctrl
{
	struct check_column
	{
		_ptr_t<byte>	m_values;
//		_ptr_t<byte>	m_enable_rows;
		long			col;
	};
	static void free_check_column( void *p )
	{delete (check_column*)p;}
public:
	check_list_ctrl()
	{
		m_images = 0;
		m_id = -1;
	}
	void add_checkcolumn( int col )					{on_add_checkcolumn( col );}
	void set_check( int row, int col, int check )	{on_setcheck( row, col, check );}
	long get_check( int row, int col)				{return on_getcheck( row, col );}

protected:
	virtual void handle_init()
	{
		m_images = ImageList_Create( 11, 11, ILC_MASK|ILC_COLOR4, 3, 3 );
		HBITMAP	
		hbmp = LoadBitmap( module::hrc(), MAKEINTRESOURCE(IDB_LIST_UNCHECKED) );
		if( !hbmp )hbmp = LoadBitmap( module::hinst(), MAKEINTRESOURCE(IDB_LIST_UNCHECKED) );
		ImageList_AddMasked(m_images, hbmp, RGB(255, 0, 255) );
		::DeleteObject( hbmp );

		hbmp = LoadBitmap( module::hrc(), MAKEINTRESOURCE(IDB_LIST_CHECKED) );
		if( !hbmp )hbmp = LoadBitmap( module::hinst(), MAKEINTRESOURCE(IDB_LIST_CHECKED) );
		ImageList_AddMasked(m_images, hbmp, RGB(255, 0, 255) );
		::DeleteObject( hbmp );

		hbmp = LoadBitmap( module::hrc(), MAKEINTRESOURCE(IDB_LIST_DISABLED) );
		if( !hbmp )hbmp = LoadBitmap( module::hinst(), MAKEINTRESOURCE(IDB_LIST_DISABLED) );
		ImageList_AddMasked(m_images, hbmp, RGB(255, 0, 255) );
		::DeleteObject( hbmp );

		hbmp = LoadBitmap( module::hrc(), MAKEINTRESOURCE(IDB_LIST_DISABLED_CHECKED) );
		if( !hbmp )hbmp = LoadBitmap( module::hinst(), MAKEINTRESOURCE(IDB_LIST_DISABLED_CHECKED) );
		ImageList_AddMasked(m_images, hbmp, RGB(255, 0, 255) );
		::DeleteObject( hbmp );

		m_id = ::GetDlgCtrlID( handle() );

		win_impl::handle_init();
	}

	virtual long on_notify_reflect( NMHDR *pnmhdr, long *plProcessed )
	{
		if( pnmhdr->idFrom == m_id )
		{
			if( pnmhdr->code == NM_CUSTOMDRAW )
			{
				*plProcessed =  true;
				NMLVCUSTOMDRAW	*plvcd = (NMLVCUSTOMDRAW*)pnmhdr;
				if( plvcd->nmcd.dwDrawStage == CDDS_PREPAINT )
					return CDRF_NOTIFYITEMDRAW;
				else if( plvcd->nmcd.dwDrawStage == CDDS_ITEMPREPAINT )
					return CDRF_NOTIFYSUBITEMDRAW;
				else if( plvcd->nmcd.dwDrawStage == (CDDS_SUBITEM|CDDS_ITEMPREPAINT) )
					return CDRF_NOTIFYPOSTPAINT;
				else if( plvcd->nmcd.dwDrawStage == (CDDS_SUBITEM|CDDS_ITEMPOSTPAINT) )
				{
					check_column *pcc = _get_check_column(  plvcd->iSubItem );
					if( pcc )
					{
						long code = _get_check( pcc, plvcd->nmcd.dwItemSpec );
						_rect	rect;
						
						int	x, y;

						if( plvcd->iSubItem )
						{
							ListView_GetSubItemRect( handle(), plvcd->nmcd.dwItemSpec, plvcd->iSubItem, plvcd->iSubItem?LVIR_BOUNDS:LVIR_ICON, &rect ); 
							x = rect.hcenter()-5;
							y = rect.vcenter()-5;
						}
						else
						{
							ListView_GetItemRect( handle(), plvcd->nmcd.dwItemSpec, &rect, LVIR_BOUNDS ); 
							int	h2 = rect.height()/2;
							x = rect.left+h2-1;
							y = rect.vcenter()-5;
						}


						::ImageList_DrawEx( m_images, code, plvcd->nmcd.hdc, x, y, 11, 11, ::GetSysColor( COLOR_WINDOW), CLR_NONE, ILD_NORMAL  );
						return CDRF_SKIPDEFAULT;
					}
					else
					{
						return CDRF_DODEFAULT;
					}
				}
				return CDRF_DODEFAULT;
			}
			else if( pnmhdr->code == NM_CLICK || pnmhdr->code == NM_DBLCLK )
			{


				*plProcessed = true;
				return 0;
			}

		}
		return win_impl::on_notify_reflect( pnmhdr, plProcessed );
	}

	virtual long on_lbuttondown( const _point &point )
	{
		LVHITTESTINFO	htinfo;
		ZeroMemory( &htinfo, sizeof( htinfo ) );

		memcpy( &htinfo.pt, &point, sizeof( POINT ) );

		if( ListView_SubItemHitTest( handle(), &htinfo ) >= 0 )
		{
			check_column	*pcol = _get_check_column( htinfo.iSubItem );
			if( pcol )
			{
				int	row = htinfo.iItem;

				long	selected = ListView_GetItemState( handle(), row, LVIS_SELECTED );
				long	check = _get_check( pcol, row );

				if( selected )
				{
					ListView_SetItemState( handle(), row, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED );

					for( int idx = ListView_GetNextItem( handle(), -1, LVNI_SELECTED );
						idx != -1; idx = ListView_GetNextItem( handle(), idx, LVNI_SELECTED ) )
							_set_check( pcol, idx, check?0:1, SC_SKIPDISABLED|SC_REDRAW|SC_NOTIFY );
					return true;
				}
				else
					_set_check( pcol, row, check?0:1, SC_SKIPDISABLED|SC_REDRAW|SC_NOTIFY );


				
			}
		}
		return win_impl::on_lbuttondown( point );
	}
	virtual long on_lbuttondblclk( const _point &point )
	{return on_lbuttondown( point );}

	virtual long handle_message( UINT m, WPARAM w, LPARAM l )
	{
		if( m == CLV_ADD_CHECK_COLUMN )return on_add_checkcolumn( w );
		else if( m == CLV_SETCHECK )return on_setcheck( HIWORD( w ), LOWORD( w ), l );
		else if( m == CLV_GETCHECK )return on_getcheck( HIWORD( w ), LOWORD( w ) );
		else return win_impl::handle_message( m, w, l );
	}

	virtual long on_char( long nKey )
	{
		if( nKey == 32 )
		{
			int	sel = ListView_GetNextItem( handle(), -1, LVNI_FOCUSED );

			if( sel == -1 )return 0;

			long	new_check = -1;

			for( int idx = ListView_GetNextItem( handle(), -1, LVNI_SELECTED );
				idx != -1; idx = ListView_GetNextItem( handle(), idx, LVNI_SELECTED ) )
			{
				for( long lpos = m_columns.head(); lpos; lpos = m_columns.next( lpos ) )
				{
					check_column *pcol = m_columns.get( lpos );

					long	curr_check = _get_check( pcol, idx );
					if( curr_check > 1 )continue;
					if( new_check == -1 )new_check = curr_check ? 0:1;
					_set_check( pcol, idx, new_check, SC_SKIPDISABLED|SC_REDRAW|SC_NOTIFY );
				}
			}

			return 1;
		}
		return win_impl::on_char( nKey );
	}

	virtual long on_add_checkcolumn( int col )
	{
		check_column *pnew = new check_column;
		pnew->col = col;

		m_columns.insert_before( pnew, 0 );
		return 1;
	}
	
	virtual long on_setcheck( int row, int col, int check )
	{
 		check_column	*pcol = _get_check_column( col );
		if( !pcol )return 0;

		_set_check( pcol, row, check, SC_REDRAW );
		return 1;
	}

	virtual long on_getcheck( int row, int col )
	{
		check_column	*pcol = _get_check_column( col );
		if( !pcol )return -1;

		return _get_check( pcol, row );
	}
protected:
	check_column	*_get_check_column( int col ) 
	{
		for( long lpos = m_columns.head(); lpos; lpos = m_columns.next( lpos ) )
		{
			check_column	*p = m_columns.get( lpos );
			if( p->col == col )return p;
		}
		return 0;
	}

	void _verify( check_column *pcolumn, long new_size )
	{
		int	old_size = pcolumn->m_values.size();		
		if( old_size < new_size )
		{
			pcolumn->m_values.alloc( new_size );
			ZeroMemory( pcolumn->m_values.ptr()+old_size, new_size-old_size );
		}
	}

	long _get_check( check_column *pcolumn, int row )
	{
		_verify( pcolumn, row+1 );
		return pcolumn->m_values.ptr()[row];
	}
	
	void _set_check( check_column *pcolumn, int row, int check, unsigned flags )
	{
		_verify( pcolumn, row+1 );

		if( flags & SC_SKIPDISABLED )
		{
			int	n = pcolumn->m_values.ptr()[row];
			if( n > 1 )return;
		}
		pcolumn->m_values.ptr()[row] = check;

		if( flags & SC_REDRAW )
		{
			RECT	rect;
			ListView_GetSubItemRect( handle(), row, pcolumn->col, LVIR_BOUNDS, &rect ); 
			InvalidateRect( handle(), &rect, true );
		}
		if( flags & SC_NOTIFY )
		{
			CLVNM_CHECK	ntf;
			ntf.check = check;
			ntf.col = pcolumn->col;
			ntf.row = row;

			LVITEM	item;
			item.mask = LVIF_PARAM;
			item.iItem = row;
			ListView_GetItem( handle(), &item );

			ntf.lParam = item.lParam;
			ntf.nmhdr.code = CLVN_CHECKCHANGE;
			ntf.nmhdr.hwndFrom = handle();
			ntf.nmhdr.idFrom = m_id;

			::SendMessage( GetParent( handle() ), WM_NOTIFY, m_id, (LPARAM)&ntf );
		}
	}
protected:

	HIMAGELIST	m_images;
	unsigned	m_id;
	_list_t<check_column*,free_check_column>	
				m_columns;
};

#endif //__ctrl_checklistview_h__
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

#ifndef __win_controls_h__
#define __win_controls_h__

#include "win_base.h"

#include "..\awin\win_dlg.h"

#ifndef 			ListView_SetCheckState
#define ListView_SetCheckState(hwndLV, i, fCheck) \
  ListView_SetItemState(hwndLV, i, INDEXTOSTATEIMAGEMASK((fCheck)?2:1), LVIS_STATEIMAGEMASK)
#endif //			ListView_SetCheckState
#ifndef 			ListView_SortItemsEx
#define LVM_SORTITEMSEX          (LVM_FIRST + 81)
#define ListView_SortItemsEx(hwndLV, _pfnCompare, _lPrm) \
  (BOOL)SNDMSG((hwndLV), LVM_SORTITEMSEX, (WPARAM)(LPARAM)(_lPrm), (LPARAM)(PFNLVCOMPARE)(_pfnCompare))
#endif //			ListView_SortItemsEx


#ifndef TreeView_SetItemState
#define TreeView_SetItemState(hwndTV, hti, data, _mask) \
{ TVITEM _ms_TVi;\
  _ms_TVi.mask = TVIF_STATE; \
  _ms_TVi.hItem = hti; \
  _ms_TVi.stateMask = _mask;\
  _ms_TVi.state = data;\
  SNDMSG((hwndTV), TVM_SETITEM, 0, (LPARAM)(TV_ITEM FAR *)&_ms_TVi);\
}
#endif// TreeView_SetItemState

#ifndef TreeView_SetCheckState
#define TreeView_SetCheckState(hwndTV, hti, fCheck) \
  TreeView_SetItemState(hwndTV, hti, INDEXTOSTATEIMAGEMASK((fCheck)?2:1), TVIS_STATEIMAGEMASK)
#endif// TreeView_SetCheckState

#ifndef TVM_GETITEMSTATE
#define TVM_GETITEMSTATE        (TV_FIRST + 39)
#endif// TVM_GETITEMSTATE


#ifndef TreeView_GetCheckState
#define TreeView_GetCheckState(hwndTV, hti) \
   ((((UINT)(SNDMSG((hwndTV), TVM_GETITEMSTATE, (WPARAM)(hti), TVIS_STATEIMAGEMASK))) >> 12) -1)
#endif// TreeView_GetCheckState


class listbox_ctrl : public win_impl
{
public:
	long insert_string( const _char *text, int idx = -1 )	{return send_message( LB_INSERTSTRING, (WPARAM)idx, (LPARAM)text );}
	long get_count()										{return send_message( LB_GETCOUNT );}
	long get_text( int idx, _char *psz )						{return send_message( LB_GETTEXT, WPARAM(idx), LPARAM(psz) );}
	long get_text_len( int idx )							{return send_message( LB_GETTEXTLEN, WPARAM(idx) );}
	long get_cur_sel()										{return send_message( LB_GETCURSEL );}
	long set_cur_sel( int idx )								{return send_message( LB_SETCURSEL, WPARAM(idx) );}
	long delete_string( int idx )							{return send_message( LB_DELETESTRING, WPARAM(idx) );}
	long delete_content()									{return send_message( LB_RESETCONTENT );}
};

class progress_ctrl : public win_impl
{
public:
	long	set_range( int lo, int hi )						{return send_message( PBM_SETRANGE, 0, MAKELONG(lo, hi ) );}
	long	set_pos( int pos )								{return send_message( PBM_SETPOS, pos );}
};

class list_ctrl : public win_impl
{
public:
	long find_item( unsigned param )							{LV_FINDINFO info;info.flags = LVFI_PARAM;info.lParam = param;return ListView_FindItem( handle(), -1, &info );}
	long insert_item( LVITEM *pitem )							{return ListView_InsertItem( handle(), pitem );	}
	long set_item( LVITEM *pitem )								{return ListView_SetItem( handle(), pitem );	}
	void set_item_text( int item, int subitem, _char *psz )		{ListView_SetItemText( handle(), item, subitem, psz );	}
	void delete_item( int item )								{ListView_DeleteItem( handle(), item );}
	void delete_all_items()										{ListView_DeleteAllItems( handle() );}
	unsigned get_item_data( int item )							{LV_ITEM	i;i.iItem = item;i.lParam = 0;i.mask = LVIF_PARAM;ListView_GetItem( handle(), &i );return i.lParam;}
	void get_item_rect( int item, int code , RECT *prect)		{ListView_GetItemRect( handle(), item, prect, code );}
	void get_subitem_rect( int item, int subitem, int code, RECT *prect)	{ListView_GetSubItemRect( handle(), item, subitem, code, prect );}
	long get_selected_item()									{return ListView_GetSelectionMark( handle() );}
	long get_item( LVITEM *pitem )								{return ListView_GetItem( handle(), pitem );	}
	void set_selected_item( int index )							
	{
		ListView_SetItemState( handle(), index, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED );	
		ListView_SetSelectionMark( handle(), index );
	}
	long get_items_count()										{return ListView_GetItemCount( handle() );}
	long get_top_index()										{return ListView_GetTopIndex( handle() );}
	long get_count_per_page()									{return ListView_GetCountPerPage( handle() );}

	long get_columns_count()									{return Header_GetItemCount( get_header() );}
	long insert_column( long ncol, LVCOLUMN *pcol )				{return ListView_InsertColumn( handle(), ncol, pcol );}
	void delete_column( long col )								{ListView_DeleteColumn( handle(), col );}
	void delete_all_columns()									{for( long col = get_columns_count(); col; col-- )delete_column( col-1 );}
	int  get_column_width( long col )							{return ListView_GetColumnWidth( handle(), col );}
	
	HWND get_header()											{return ListView_GetHeader( handle() );}
	void set_imagelist( HIMAGELIST hil, int code )				{ListView_SetImageList( handle(), hil, code );}
	void set_extended_style( unsigned style, unsigned mask )	{ListView_SetExtendedListViewStyleEx( handle(), style, mask );}
	void ensure_visible( long item, bool partial_ok )			{ListView_EnsureVisible( handle(), item, partial_ok );}
	void get_view_rect( RECT *prect )							{ListView_GetViewRect( handle(), prect );}

	long set_items_count( long c )								{return ListView_SetItemCount( handle(), c );}

public:
	static const char *window_class()							{return WC_LISTVIEW;}
};

inline
void TreeView_ExpandAll( HWND htree, int code, HTREEITEM hti_p = 0 )
{
	HTREEITEM	hti = TreeView_GetNextItem( htree, hti_p, TVGN_CHILD );
	while( hti )
	{
		TreeView_ExpandAll( htree, code, hti );
		hti = TreeView_GetNextItem( htree, hti, TVGN_NEXT );
	}
	TreeView_Expand( htree, hti_p, code ); 
}


class tree_ctrl : public win_impl
{
public:
	HTREEITEM insert_item( TVINSERTSTRUCT* pinsert )			{return TreeView_InsertItem( handle(), pinsert );}
	void delete_item( HTREEITEM hti )							{TreeView_DeleteItem( handle(), hti );}
	void set_image_list( HIMAGELIST hi, int code = TVSIL_NORMAL )	{TreeView_SetImageList( handle(), hi, code );}
	HTREEITEM get_next_item( HTREEITEM hti, int code )			{return TreeView_GetNextItem( handle(), hti, code );}
	void select_item( HTREEITEM hti )							{TreeView_SelectItem( handle(), hti );}
	HTREEITEM get_selected_item()								{TreeView_GetSelection( handle() );}
	void delete_all_items()										{TreeView_DeleteAllItems( handle() ); }

	void expand( HTREEITEM hti, int code )						{TreeView_Expand( handle(), hti, code );}
	void expand_all( int code )									{TreeView_ExpandAll( handle(), code );}

public:
	static const char *window_class()							{return WC_TREEVIEW;}
};


#endif // __win_controls_h__
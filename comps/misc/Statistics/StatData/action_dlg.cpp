#include "stdafx.h"
#include "action_dlg.h"
#include "resource.h"

#include "\vt5\awin\misc_ptr.h"
#include "\vt5\awin\misc_list.h"
#include "\vt5\awin\misc_map.h"

//////////////////////////////////////////////////////////////////////
//
//	class CDeleteObjectDlg
//
//////////////////////////////////////////////////////////////////////
CDeleteObjectDlg::CDeleteObjectDlg() : dlg_impl( IDD_DELETE_OBJECTS )
{
	m_bobject_list		= true;
	m_str_object_list	= "";
	m_str_image			= "";
}

//////////////////////////////////////////////////////////////////////
CDeleteObjectDlg::~CDeleteObjectDlg()
{

}

//////////////////////////////////////////////////////////////////////
long CDeleteObjectDlg::on_initdialog()
{
	long lres = dlg_impl::on_initdialog();

	//fill
	fill_combos();
	//select 0 items
	::SendMessage( ::GetDlgItem( handle(), IDC_OBJECT_LIST ), CB_SETCURSEL, 0, 0 );
	::SendMessage( ::GetDlgItem( handle(), IDC_IMAGE ), CB_SETCURSEL, 0, 0 );	

	::CheckDlgButton( handle(), m_bobject_list ? IDC_RADIO_OBJECT_LIST : IDC_RADIO_IMAGE, BST_CHECKED );
	on_ctrls_change();	

	return lres;
}

//////////////////////////////////////////////////////////////////////
LRESULT CDeleteObjectDlg::on_command(uint cmd)
{
	if( cmd == IDC_RADIO_OBJECT_LIST || cmd == IDC_RADIO_IMAGE )
	{
		on_ctrls_change();
	}
	return dlg_impl::on_command( cmd );	
}

//////////////////////////////////////////////////////////////////////
void CDeleteObjectDlg::on_ok()
{
	m_bobject_list = ( BST_CHECKED == ::IsDlgButtonChecked( handle(), IDC_RADIO_OBJECT_LIST ) );	
	
	char sz_buf[255];	sz_buf[0] = 0;
	::GetWindowText( ::GetDlgItem( handle(), IDC_OBJECT_LIST ), sz_buf, sizeof(sz_buf) );
	m_str_object_list = sz_buf;
	::GetWindowText( ::GetDlgItem( handle(), IDC_IMAGE ), sz_buf, sizeof(sz_buf) );
	m_str_image = sz_buf;

	if( m_bobject_list && !m_str_object_list.length() )
	{
		::SetFocus( ::GetDlgItem( handle(), IDC_OBJECT_LIST ) );
		return;
	}

	if( !m_bobject_list && !m_str_image.length() )
	{
		::SetFocus( ::GetDlgItem( handle(), IDC_IMAGE ) );
		return;
	}

	dlg_impl::on_ok();
}

inline long cmp_bstr_t( _bstr_t psz1, _bstr_t psz2 )
{	return _tcsicmp( (char*)psz1, (char*)psz2 );};

//////////////////////////////////////////////////////////////////////
void CDeleteObjectDlg::fill_combos()
{
	if( m_ptr_table	== 0 )	return;

	_list_map_t<long, bstr_t, cmp_bstr_t> list_image;
	_list_map_t<long, bstr_t, cmp_bstr_t> list_ol;
	

	TPOS lpos = 0;
	m_ptr_table->GetFirstGroupPos( &lpos );
	while( lpos )
	{
		stat_group* pgroup = 0;
		m_ptr_table->GetNextGroup( &lpos, &pgroup );

		if( pgroup->bstr_image_name )
		{
			_bstr_t bstr = pgroup->bstr_image_name;
			list_image.set( 0, bstr );
		}
		if( pgroup->bstr_object_list_name )
		{
			_bstr_t bstr = pgroup->bstr_object_list_name;
			list_ol.set( 0, bstr );
		}
	}

	//fill Object List combo
	HWND hwnd = ::GetDlgItem( handle(), IDC_OBJECT_LIST );
	if( hwnd )
	{
		for (TPOS lpos = list_ol.head(); lpos; lpos = list_ol.next(lpos))
		{
			_bstr_t bstr = list_ol.get_key( lpos );
			::SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)bstr );
		}
	}

	//fill Image combo
	hwnd = ::GetDlgItem( handle(), IDC_IMAGE );
	if( hwnd )
	{
		for (TPOS lpos = list_image.head(); lpos; lpos = list_image.next(lpos))
		{
			_bstr_t bstr = list_image.get_key( lpos );
			::SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)bstr );
		}
	}
}

//////////////////////////////////////////////////////////////////////
void CDeleteObjectDlg::on_ctrls_change()
{
	bool bobject_list = ( BST_CHECKED == ::IsDlgButtonChecked( handle(), IDC_RADIO_OBJECT_LIST ) );	
	::EnableWindow( ::GetDlgItem( handle(), IDC_OBJECT_LIST ), bobject_list );
	::EnableWindow( ::GetDlgItem( handle(), IDC_IMAGE ), !bobject_list );
}


int CALLBACK CompareListFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	HWND hWndList = (HWND)lParamSort;

	{
		LVITEM lvi1 = {0};
		lvi1.mask = LVIF_PARAM;
		lvi1.iItem = lParam1;

		LVITEM lvi2 = {0};
		lvi2.mask = LVIF_PARAM;
		lvi2.iItem = lParam2;

		ListView_GetItem( hWndList, &lvi1 );
		ListView_GetItem( hWndList, &lvi2 );

		if( lvi1.lParam && lvi2.lParam )
		{
			col_info* pci1 = (col_info*)lvi1.lParam;
			col_info* pci2 = (col_info*)lvi2.lParam;
			return pci1->norder - pci2->norder;
		}
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////
//
//	class CCustomizeDlg
//
//////////////////////////////////////////////////////////////////////
CCustomizeDlg::CCustomizeDlg() : dlg_impl( IDD_CUSTOMIZE )
{
	m_parr	= 0;
	m_size	= 0;
}

//////////////////////////////////////////////////////////////////////
CCustomizeDlg::~CCustomizeDlg()
{
}

//////////////////////////////////////////////////////////////////////
void CCustomizeDlg::set_col_info( col_info* parr, size_t size )
{
	m_parr = parr;
	m_size = size;
}

//////////////////////////////////////////////////////////////////////
long CCustomizeDlg::on_initdialog()
{
	long lres = dlg_impl::on_initdialog();

	HWND hwnd_list = ::GetDlgItem( handle(), IDC_LIST );
	ListView_SetExtendedListViewStyle( hwnd_list, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_INFOTIP | LVS_EX_CHECKBOXES );
	
	//insert columns
	{
		RECT rc;
		::ZeroMemory( &rc, sizeof(RECT) );
		::GetClientRect( hwnd_list, &rc );

		LVCOLUMN column;
		memset( &column, 0, sizeof(LVCOLUMN) );

		column.mask		= LVCF_TEXT | LVCF_WIDTH;
		column.pszText	= (LPTSTR)__T("");
		column.cx		= 20;

		ListView_InsertColumn( hwnd_list, 0, &column );	

		column.cx		= rc.right - rc.left - 40;
		column.pszText	= __T("Name");	
		ListView_InsertColumn( hwnd_list, 1, &column );		
	}


	if( m_parr && m_size )
	{
		for( size_t idx=0; idx<m_size; idx++ )
		{
			LVITEM item;
			::ZeroMemory( &item, sizeof(LVITEM) );
			//check box
			item.mask		= LVIF_TEXT|LVIF_PARAM;
			item.iSubItem	= 0;
			item.iItem		= idx;
			item.pszText	= "";
			BYTE* pbuf		= (BYTE*)m_parr + sizeof(col_info) * idx;
			item.lParam		= (LPARAM)pbuf;

			ListView_InsertItem( hwnd_list, &item );

			//caption
			item.mask		= LVIF_TEXT;
			item.iSubItem	= 1;		
			item.pszText	= m_parr[idx].sz_caption;

			ListView_SetItem( hwnd_list, &item );			

			ListView_SetCheckState( hwnd_list, idx, m_parr[idx].bvisible == true );
		}

		ListView_SortItemsEx( hwnd_list, CompareListFunc, hwnd_list );

		//normalize sort
		int ncount = ListView_GetItemCount( hwnd_list );
		for( int i=0;i<ncount;i++ )
		{
			col_info* pci = get_item_data( i );
			if( !pci )	continue;
			pci->norder = i;
		}
	}

	update_controls( ListView_GetSelectionMark( hwnd_list ) );

	return lres;
}

//////////////////////////////////////////////////////////////////////
LRESULT CCustomizeDlg::on_command(uint cmd)
{
	if( cmd == IDC_MOVE_UP )
	{
		HWND hwnd_list = ::GetDlgItem( handle(), IDC_LIST );
		int idx1 = ListView_GetSelectionMark( hwnd_list );
		int idx2 = idx1 - 1;

		col_info* pci1 = get_item_data( idx1 );
		col_info* pci2 = get_item_data( idx2 );
		if( pci1 && pci2 )
		{
			int norder = pci1->norder;
			pci1->norder = pci2->norder;
			pci2->norder = norder;
			ListView_SortItemsEx( hwnd_list, CompareListFunc, hwnd_list );
			ListView_EnsureVisible( hwnd_list, idx1 - 1, false );
		}

		update_controls( ListView_GetSelectionMark( hwnd_list ) );
		return 0L;
	}
	else if( cmd == IDC_MOVE_DOWN )
	{
		HWND hwnd_list = ::GetDlgItem( handle(), IDC_LIST );
		int idx1 = ListView_GetSelectionMark( hwnd_list );
		int idx2 = idx1 + 1;

		col_info* pci1 = get_item_data( idx1 );
		col_info* pci2 = get_item_data( idx2 );
		if( pci1 && pci2 )
		{
			int norder = pci1->norder;
			pci1->norder = pci2->norder;
			pci2->norder = norder;
			ListView_SortItemsEx( hwnd_list, CompareListFunc, hwnd_list );
			ListView_EnsureVisible( hwnd_list, idx1 + 1, false );
		}

		update_controls( ListView_GetSelectionMark( hwnd_list ) );
		return 0L;
	}
	return dlg_impl::on_command( cmd );	
}

//////////////////////////////////////////////////////////////////////
void CCustomizeDlg::on_ok()
{
	HWND hwnd_list = ::GetDlgItem( handle(), IDC_LIST );
	if( hwnd_list )
	{
		int ncount = ListView_GetItemCount( hwnd_list );
		for( int i=0;i<ncount;i++ )
		{
			col_info* pci = get_item_data( i );
			if( !pci )	continue;
			pci->bvisible = ( ListView_GetCheckState( hwnd_list, i ) == 1L );
		}
	}
	__super::on_ok();
}

//////////////////////////////////////////////////////////////////////
LRESULT CCustomizeDlg::on_notify(uint idc, NMHDR *pnmhdr)
{
	if( idc == IDC_LIST && pnmhdr->code == LVN_ITEMCHANGED )
	{
		NMLISTVIEW* p = (NMLISTVIEW*)pnmhdr;		
		if( p->uNewState & LVIS_SELECTED )
			update_controls( p->iItem );
	}
	return dlg_impl::on_notify( idc, pnmhdr );
}

//////////////////////////////////////////////////////////////////////
col_info* CCustomizeDlg::get_item_data( int idx )
{
	HWND hwnd_list = ::GetDlgItem( handle(), IDC_LIST );
	if( !hwnd_list )	return 0;

	LVITEM lvi = {0};
	lvi.mask = LVIF_PARAM;
	lvi.iItem = idx;

	if( !ListView_GetItem( hwnd_list, &lvi ) )
		return false;

	return (col_info*)lvi.lParam;
}

//////////////////////////////////////////////////////////////////////
void CCustomizeDlg::update_controls( int idx )
{
	col_info* pci1 = get_item_data( idx );
	col_info* pci2 = get_item_data( idx - 1 );
	::EnableWindow( ::GetDlgItem( handle(), IDC_MOVE_UP ), pci1 && pci2 );

	pci2 = get_item_data( idx + 1 );	
	::EnableWindow( ::GetDlgItem( handle(), IDC_MOVE_DOWN ), pci1 && pci2 );
}
#include "stdafx.h"
#include "resource.h"
#include "dlg.h"
#include "_hook.h"
#include "_pdb.h"
#include "utils.h"
//#include "perf_counters.h"

//globals
int g_nsort_column = 0;
bool g_bgroup = false;
unsigned g_nstack_group_depth = 0;

//map function
//////////////////////////////////////////////////////////////////////
long cmp_heap_stack( heap_info_ex* p1, heap_info_ex* p2 )
{
	if( g_bgroup )
	{
		//type
		if( p1->m_ltype != p2->m_ltype )
			return ( (long)p1->m_ltype - (long)p2->m_ltype );

		//heap
		if( p1->m_hheap != p2->m_hheap )
			return ( (long)p1->m_hheap - (long)p2->m_hheap );

		unsigned nsize1 = p1->m_size_stack;
		unsigned nsize2 = p2->m_size_stack;
		
		//stack
		if( g_nstack_group_depth )
		{
			nsize1 = min( nsize1, g_nstack_group_depth );
			nsize2 = min( nsize2, g_nstack_group_depth );
		}

		if( nsize1 != nsize2 )
			return nsize1 - nsize2;

		long lsum1 = 0;
		for( unsigned i=0;i<nsize1;i++ )
			lsum1 += p1->m_pstack[i];

		long lsum2 = 0;
		for( i=0;i<nsize2;i++ )
			lsum2 += p2->m_pstack[i];

		return ( lsum2 - lsum1 );

		/*if( p1->m_size_stack != p2->m_size_stack )
			return ( (long)p1->m_size_stack - (long)p2->m_size_stack );

		long lsum1 = 0;
		for( unsigned i=0;i<p1->m_size_stack;i++ )
			lsum1 += p1->m_pstack[i];

		long lsum2 = 0;
		for( i=0;i<p2->m_size_stack;i++ )
			lsum2 += p2->m_pstack[i];

		return ( lsum2 - lsum1 );*/
	}
	else
	{
		//address
		return ( ( p2->m_ltype + 1 ) * p2->m_laddress ) - ( ( p1->m_ltype + 1 ) * p1->m_laddress );
	}	
}

//////////////////////////////////////////////////////////////////////
int CALLBACK compare_list_func(LPARAM lParam1, LPARAM lParam2, 
LPARAM lParamSort)
{
	HWND hwnd_list = (HWND)lParamSort;
	LVITEM lvi = {0};
	lvi.mask	= LVIF_PARAM;
	lvi.iItem	= lParam1;
	if( !ListView_GetItem( hwnd_list, &lvi ) )	return 0;
	heap_info_ex* p1 = (heap_info_ex*)lvi.lParam;

	lvi.iItem	= lParam2;
	if( !ListView_GetItem( hwnd_list, &lvi ) )	return 0;
	heap_info_ex* p2 = (heap_info_ex*)lvi.lParam;

	if( !p1 || !p2 )	return 0;
		
	if( g_nsort_column == ID_ADDR_TYPE )
	{
		return p2->m_ltype - p1->m_ltype;
	}
	if( g_nsort_column == ID_ADDR_GROUP )
	{
		if( g_bgroup )
			return p2->m_count - p1->m_count;
		else
			return p2->m_dw_time - p1->m_dw_time;
	}
	
	if( g_nsort_column == ID_ADDR_HEAP )
	{
		return (long)p2->m_hheap - (long)p1->m_hheap;
	}

	if( g_nsort_column == ID_ADDR_SIZE )
	{
		return p2->m_dw_full_size - p1->m_dw_full_size;
	}

	if( g_nsort_column == ID_ADDR_POINTER )
	{
		return p2->m_laddress - p1->m_laddress;
	}
	

	return 0;
}



//////////////////////////////////////////////////////////////////////
//	class main_dlg
//////////////////////////////////////////////////////////////////////
main_dlg::main_dlg() : dlg_impl(IDD_MAIN)
{
	m_sz_dump_file[0]	= 0;

	m_hwnd_addr			= 0;
	m_hwnd_stack		= 0;
	m_hwnd_cmp			= 0;

	get_dump_file( MEMORY_DUMP, m_sz_dump_file, sizeof(m_sz_dump_file) ); 

	m_bmem_check		= true;
	m_bfile1_check		= false;
	m_bfile2_check		= false;

	m_clr_mem			= ::GetSysColor( COLOR_WINDOW );
	m_clr_file1			= RGB(189,228,182);
	m_clr_file2			= RGB(149,170,242);

	m_hicon_small		= 0;
	m_hicon_big			= 0;
}

//////////////////////////////////////////////////////////////////////
main_dlg::~main_dlg()
{
	deinit_data();
}

//////////////////////////////////////////////////////////////////////
long main_dlg::on_initdialog()
{
	{
		lock l( &g_block_mem_hook, true );

		//set icon
		m_hicon_small	= (HICON)::LoadImage(	module::hrc(), MAKEINTRESOURCE( IDI_DUMP ), 
													IMAGE_ICON, 16, 16, 0 );
		m_hicon_big		= (HICON)::LoadImage(	module::hrc(), MAKEINTRESOURCE( IDI_DUMP ), 
													IMAGE_ICON, 32, 32, 0 );
		if( m_hicon_small )
			::SendMessage( m_hwnd, WM_SETICON, ICON_SMALL, (LPARAM)m_hicon_small );
		
		if( m_hicon_big )
			::SendMessage( m_hwnd, WM_SETICON, ICON_BIG, (LPARAM)m_hicon_big );
		
		m_block_check	= true;

		m_hwnd_addr		= ::GetDlgItem( handle(), IDC_ADDR );
		m_hwnd_stack	= ::GetDlgItem( handle(), IDC_STACK );
		m_hwnd_cmp		= ::GetDlgItem( handle(), IDC_COMPARE );		

		//address
		if( m_hwnd_addr )
		{		
			ListView_SetExtendedListViewStyle( m_hwnd_addr, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_INFOTIP );

			//insert columns
			LVCOLUMN column = {0};

			column.mask		= LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
			column.pszText	= (LPTSTR)_T("Type");
			column.cx		= get_list_width( "ID_ADDR_TYPE", 40 );
			column.fmt		= LVCFMT_RIGHT;
			ListView_InsertColumn( m_hwnd_addr, ID_ADDR_TYPE, &column );	

			column.mask		= LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
			column.pszText	= (LPTSTR)_T("Group");
			column.cx		= get_list_width( "ID_ADDR_GROUP", 80 );
			column.fmt		= LVCFMT_RIGHT;

			ListView_InsertColumn( m_hwnd_addr, ID_ADDR_GROUP, &column );	

			column.cx		= get_list_width( "ID_ADDR_HEAP", 80 );
			column.pszText	= _T("Heap");
			column.fmt		= LVCFMT_RIGHT;
			ListView_InsertColumn( m_hwnd_addr, ID_ADDR_HEAP, &column );

			column.cx		= get_list_width( "ID_ADDR_SIZE", 80 );
			column.pszText	= _T("Size");	
			column.fmt		= LVCFMT_RIGHT;
			ListView_InsertColumn( m_hwnd_addr, ID_ADDR_SIZE, &column );
			
			column.cx		= get_list_width( "ID_ADDR_POINTER", 80 );
			column.pszText	= _T("Pointer");	
			column.fmt		= LVCFMT_RIGHT;
			ListView_InsertColumn( m_hwnd_addr, ID_ADDR_POINTER, &column );
		}

		//stack
		if( m_hwnd_stack )
		{		
			ListView_SetExtendedListViewStyle( m_hwnd_stack, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_INFOTIP );

			//insert columns
			LVCOLUMN column = {0};

			column.mask		= LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
			column.pszText	= (LPTSTR)_T("AA");
			column.cx		= get_list_width( "ID_STACK_ADDR", 30 );
			column.fmt		= LVCFMT_RIGHT;

			ListView_InsertColumn( m_hwnd_stack, ID_STACK_ADDR, &column );	

			column.pszText	= _T("RA");	
			column.cx		= get_list_width( "ID_STACK_RA", 30 );
			column.fmt		= LVCFMT_LEFT;
			ListView_InsertColumn( m_hwnd_stack, ID_STACK_RA, &column );

			column.pszText	= _T("Module");	
			column.cx		= get_list_width( "ID_STACK_MODULE", 70 );
			column.fmt		= LVCFMT_LEFT;
			ListView_InsertColumn( m_hwnd_stack, ID_STACK_MODULE, &column );
			
			column.cx		= get_list_width( "ID_STACK_FUNC", 70 );
			column.pszText	= _T("Func");	
			column.fmt		= LVCFMT_LEFT;
			ListView_InsertColumn( m_hwnd_stack, ID_STACK_FUNC, &column );		
			
			column.cx		= get_list_width( "ID_STACK_SRC", 120 );
			column.pszText	= _T("Src");
			column.fmt		= LVCFMT_LEFT;
			ListView_InsertColumn( m_hwnd_stack, ID_STACK_SRC, &column );


			column.cx		= get_list_width( "ID_STACK_LINE", 70 );
			column.pszText	= _T("Line");	
			column.fmt		= LVCFMT_RIGHT;
			ListView_InsertColumn( m_hwnd_stack, ID_STACK_LINE, &column );
		}

		//compare
		{
			ListView_SetExtendedListViewStyle( m_hwnd_cmp, LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_INFOTIP );

			//insert columns
			LVCOLUMN column = {0};

			column.mask		= LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
			column.pszText	= (LPTSTR)_T("Type");
			column.cx		= get_list_width( "ID_CMP_TYPE", 40 );
			column.fmt		= LVCFMT_CENTER;

			ListView_InsertColumn( m_hwnd_cmp, ID_CMP_TYPE, &column );	

			column.pszText	= _T("File");	
			column.cx		= get_list_width( "ID_CMP_FILE", 150 );
			column.fmt		= LVCFMT_LEFT;
			ListView_InsertColumn( m_hwnd_cmp, ID_CMP_FILE, &column );
			
			column.cx		= get_list_width( "ID_CMP_SIZE", 70 );
			column.pszText	= _T("Size");	
			column.fmt		= LVCFMT_RIGHT;
			ListView_InsertColumn( m_hwnd_cmp, ID_CMP_SIZE, &column );

			//insert last used files
			LVITEM item = {0};			

			char sz_file[MAX_PATH];	sz_file[0] = 0;
			
			//current memory
			item.mask		= LVIF_TEXT;
			item.iSubItem	= ID_CMP_TYPE;
			item.iItem		= 0;
			item.pszText	= "0";//DUMP_TYPE_MEM
			ListView_InsertItem( m_hwnd_cmp, &item );
			item.pszText	= "Memory Dump";
			item.iSubItem	= ID_CMP_FILE;
			ListView_SetItem( m_hwnd_cmp, &item );
			//set checked always
			ListView_SetCheckState( m_hwnd_cmp, ROW_TYPE_MEM, 1 );
			
			//file1
			get_dump_file( FILE1_DUMP, sz_file, sizeof(sz_file) );
			item.iSubItem	= ID_CMP_TYPE;
			item.iItem		= 1;
			item.pszText	= "1";//DUMP_TYPE_FILE1
			ListView_InsertItem( m_hwnd_cmp, &item );
			item.pszText	= sz_file;
			item.iSubItem	= ID_CMP_FILE;
			ListView_SetItem( m_hwnd_cmp, &item );

			//file2
			get_dump_file( FILE2_DUMP, sz_file, sizeof(sz_file) );
			item.iSubItem	= ID_CMP_TYPE;
			item.iItem		= 2;
			item.pszText	= "2";//DUMP_TYPE_FILE2
			ListView_InsertItem( m_hwnd_cmp, &item );
			item.pszText	= sz_file;
			item.iSubItem	= ID_CMP_FILE;
			ListView_SetItem( m_hwnd_cmp, &item );
		}
	}

	//state contros
	set_state_to_ctrls();

	{
		lock l( &g_block_mem_hook, true );

		//startup state
		::CheckDlgButton( m_hwnd, IDC_GATHER_STARTUP, get_startup_gather_state() == true ? 1L : 0 );

		//min alloc size
		::SetDlgItemInt( m_hwnd, IDC_MIN_SIZE, get_min_alloc_size(), false );

		//group by stack
		::CheckDlgButton( m_hwnd, IDC_GROUP_BY_STACK, get_group_by_stack() == true ? 1L : 0 );	

		//group stack size
		::SetDlgItemInt( m_hwnd, IDC_GROUP_STACK_DEPTH, get_group_stack_depth(), false );

		//grab maps
		grab_current_heap_map();
		fill_list( );

		m_block_check = false;
	}

	GetPrivateProfilePlacement( SECTION_WND_PLACE, WND_MEM_INFO, m_hwnd, get_ini_file(), GPP_NOZIZE );

	return dlg_impl::on_initdialog();
}

//////////////////////////////////////////////////////////////////////
long main_dlg::on_command( uint cmd )
{
	if( cmd == IDC_START )
	{
		g_block_mem_hook = false;
		return 1L;
	}
	else if( cmd == IDC_STOP )
	{
		g_block_mem_hook = true;
		return 1L;
	}
	else if( cmd == IDC_CLEAR )
	{
		enter_cc cc( &g_cs_map );
		lock l( &g_block_mem_hook, true );		
		g_map_heap.clear();
		clean_list( ROW_TYPE_MEM );
		grab_current_heap_map();
		fill_list( );
		return 1L;
	}
	else if( cmd == IDC_GRAB )
	{
		//grab map
		clean_list( ROW_TYPE_MEM );
		grab_current_heap_map();
		fill_list( );
		return 1L;
	}
	else if( cmd == IDC_STATE_RUN || cmd == IDC_STATE_STOP )
	{
		on_state_change();
		return 1L;
	}
	else if( cmd == IDC_GATHER_STARTUP )
	{
		set_startup_gather_state( ::IsDlgButtonChecked( m_hwnd, IDC_GATHER_STARTUP ) == 1L );
		return 1L;
	}
	else if( cmd == IDC_GROUP_BY_STACK )
	{
		set_group_by_stack( ::IsDlgButtonChecked( m_hwnd, IDC_GROUP_BY_STACK ) == 1L );
		return 1L;
	}
	else if( LOWORD(cmd) == IDC_MIN_SIZE && HIWORD(cmd) == EN_CHANGE  )
	{
		BOOL b = false;
		unsigned n = ::GetDlgItemInt( m_hwnd, IDC_MIN_SIZE, &b, false );
		if( b )
			set_min_alloc_size( (int)n );
		return 1L;
	}	
	else if( LOWORD(cmd) == IDC_GROUP_STACK_DEPTH && HIWORD(cmd) == EN_CHANGE  )
	{
		BOOL b = false;
		unsigned n = ::GetDlgItemInt( m_hwnd, IDC_GROUP_STACK_DEPTH, &b, false );
		if( b )
			set_group_stack_depth( (int)n );
		return 1L;
	}	
	else if( cmd == IDC_REFRESH_DLG )
	{
		fill_list( );
		return 1L;
	}
	else if( cmd == IDC_DUMP_SAVE )
	{
		OPENFILENAME ofn = {0};
		::ZeroMemory( &ofn, sizeof( ofn ) );
		ofn.lStructSize = sizeof( ofn );
		ofn.hwndOwner = m_hwnd; 
		ofn.hInstance = module::hinst();
		ofn.lpstrFilter = "Dump files\0*.dmp\0\0";
		ofn.lpstrDefExt = ".dmp";
		ofn.lpstrFile = m_sz_dump_file;
		ofn.nMaxFile = sizeof( m_sz_dump_file );
		ofn.Flags = OFN_EXPLORER|OFN_LONGNAMES|OFN_ENABLESIZING;		

		if( ::GetSaveFileName( &ofn ) != IDOK )
			return 0;

		save_dump( m_sz_dump_file );
		set_dump_file( MEMORY_DUMP, m_sz_dump_file );
		return 1L;
	}

	return dlg_impl::on_command( cmd );
}


//////////////////////////////////////////////////////////////////////
long main_dlg::on_notify( uint idc, NMHDR *pnmhdr )
{
	if( idc == IDC_ADDR && pnmhdr->code == LVN_ITEMCHANGED )
	{		
		NMLISTVIEW* pnmlv = (LPNMLISTVIEW)pnmhdr;
		if( pnmlv->uNewState & LVIS_SELECTED )
		{
			on_address_change( pnmlv->iItem );
		}		
	}
	else if( idc == IDC_ADDR && pnmhdr->code == LVN_COLUMNCLICK )
	{
		NMLISTVIEW* p = (NMLISTVIEW*)pnmhdr;
		g_nsort_column = p->iSubItem;
		ListView_SortItemsEx( p->hdr.hwndFrom, compare_list_func, p->hdr.hwndFrom );
	}

	else if( idc == IDC_STACK && pnmhdr->code == LVN_ITEMCHANGED )
	{		
		NMLISTVIEW* pnmlv = (LPNMLISTVIEW)pnmhdr;
		if( pnmlv->uNewState & LVIS_SELECTED )
		{
			on_stack_change( pnmlv->iItem );
		}		
	}
	else if( idc == IDC_ADDR && pnmhdr->code == NM_CUSTOMDRAW )
	{
		NMLVCUSTOMDRAW* pcd = (NMLVCUSTOMDRAW*)pnmhdr;
		DWORD dw_stage	= pcd->nmcd.dwDrawStage;
		if( dw_stage == CDDS_PREPAINT )
		{
			return CDRF_NOTIFYITEMDRAW;
		}		
		else if( dw_stage == CDDS_ITEMPREPAINT )
		{
			heap_info_ex* phi = (heap_info_ex*)pcd->nmcd.lItemlParam;
			if( phi )
			{
				if( phi->m_ltype == ROW_TYPE_MEM )
					pcd->clrTextBk = m_clr_mem;
				else if( phi->m_ltype == ROW_TYPE_FILE1 )
					pcd->clrTextBk = m_clr_file1;
				else if( phi->m_ltype == ROW_TYPE_FILE2 )
					pcd->clrTextBk = m_clr_file2;
				return CDRF_NEWFONT;
			}
		}
	}
	else if( idc == IDC_COMPARE && pnmhdr->code == NM_CUSTOMDRAW )
	{
		NMLVCUSTOMDRAW* pcd = (NMLVCUSTOMDRAW*)pnmhdr;
		DWORD dw_stage	= pcd->nmcd.dwDrawStage;
		if( dw_stage == CDDS_PREPAINT )
		{
			return CDRF_NOTIFYITEMDRAW;
		}		
		else if( dw_stage == CDDS_ITEMPREPAINT )
		{
			if( pcd->nmcd.dwItemSpec == ROW_TYPE_MEM )
				pcd->clrTextBk = m_clr_mem;
			else if( pcd->nmcd.dwItemSpec == ROW_TYPE_FILE1 )
				pcd->clrTextBk = m_clr_file1;
			else if( pcd->nmcd.dwItemSpec == ROW_TYPE_FILE2 )
				pcd->clrTextBk = m_clr_file2;
			return CDRF_NEWFONT;			
		}
	}
	else if( idc == IDC_COMPARE && pnmhdr->code == NM_DBLCLK )
	{
		NMITEMACTIVATE* pnmi = (NMITEMACTIVATE*)pnmhdr;
		if( pnmi->iSubItem == ID_CMP_FILE )
			on_browse_compare_file( pnmi->iItem );
	}
	else if( idc == IDC_STACK && pnmhdr->code == NM_DBLCLK )
	{
		NMITEMACTIVATE* pnmi = (NMITEMACTIVATE*)pnmhdr;		
		on_browse_stack( pnmi->iItem );
	}	
	else if( idc == IDC_COMPARE && pnmhdr->code == LVN_ITEMCHANGED && !m_block_check )
	{	  
		NMLISTVIEW* pnmlv = (LPNMLISTVIEW)pnmhdr;
		if( pnmlv->iItem == ROW_TYPE_MEM )
		{						    
			bool bnew_check = ( 1L == ListView_GetCheckState( m_hwnd_cmp, pnmlv->iItem ) );
			if( m_bmem_check != bnew_check )
			{
				m_bmem_check = bnew_check;

				if( m_bmem_check )
					grab_current_heap_map( );
				else
					clean_list( ROW_TYPE_MEM );
				fill_list( );
			}
		}
		else if( pnmlv->iItem == ROW_TYPE_FILE1 )
		{						    
			bool bnew_check = ( 1L == ListView_GetCheckState( m_hwnd_cmp, pnmlv->iItem ) );
			if( m_bfile1_check != bnew_check )
			{
				m_bfile1_check = bnew_check;

				if( m_bfile1_check )
				{
					char sz_file[MAX_PATH];	sz_file[0] = 0;
					ListView_GetItemText( m_hwnd_cmp, ROW_TYPE_FILE1, ID_CMP_FILE, sz_file, sizeof(sz_file) ); 
					if( strlen( sz_file ) )
						load_dump( sz_file, ROW_TYPE_FILE1 );
				}
				else
					clean_list( ROW_TYPE_FILE1 );
				fill_list( );
			}
		}
		else if( pnmlv->iItem == ROW_TYPE_FILE2 )
		{						    
			bool bnew_check = ( 1L == ListView_GetCheckState( m_hwnd_cmp, pnmlv->iItem ) );
			if( m_bfile2_check != bnew_check )
			{
				m_bfile2_check = bnew_check;

				if( m_bfile2_check )
				{
					char sz_file[MAX_PATH];	sz_file[0] = 0;
					ListView_GetItemText( m_hwnd_cmp, ROW_TYPE_FILE2, ID_CMP_FILE, sz_file, sizeof(sz_file) ); 
					if( strlen( sz_file ) )
						load_dump( sz_file, ROW_TYPE_FILE2 );
				}
				else
					clean_list( ROW_TYPE_FILE2 );
				fill_list( );
			}
		}
	}
	
	return dlg_impl::on_notify( idc, pnmhdr );
}

//////////////////////////////////////////////////////////////////////
long main_dlg::on_destroy( )
{
	//save columns width
	if( m_hwnd_addr )
	{
		set_list_width( "ID_ADDR_TYPE",		ListView_GetColumnWidth( m_hwnd_addr, ID_ADDR_TYPE ) );
		set_list_width( "ID_ADDR_GROUP",	ListView_GetColumnWidth( m_hwnd_addr, ID_ADDR_GROUP ) );
		set_list_width( "ID_ADDR_HEAP",		ListView_GetColumnWidth( m_hwnd_addr, ID_ADDR_HEAP ) );
		set_list_width( "ID_ADDR_SIZE",		ListView_GetColumnWidth( m_hwnd_addr, ID_ADDR_SIZE ) );
		set_list_width( "ID_ADDR_POINTER",	ListView_GetColumnWidth( m_hwnd_addr, ID_ADDR_POINTER ) );
	}
	
	if( m_hwnd_stack )
	{
		set_list_width( "ID_STACK_ADDR",	ListView_GetColumnWidth( m_hwnd_stack, ID_STACK_ADDR ) );
		set_list_width( "ID_STACK_RA",		ListView_GetColumnWidth( m_hwnd_stack, ID_STACK_RA ) );
		set_list_width( "ID_STACK_MODULE",	ListView_GetColumnWidth( m_hwnd_stack, ID_STACK_MODULE ) );
		set_list_width( "ID_STACK_FUNC",	ListView_GetColumnWidth( m_hwnd_stack, ID_STACK_FUNC ) );
		set_list_width( "ID_STACK_SRC",		ListView_GetColumnWidth( m_hwnd_stack, ID_STACK_SRC ) );
		set_list_width( "ID_STACK_LINE",	ListView_GetColumnWidth( m_hwnd_stack, ID_STACK_LINE ) );
	}

	if( m_hwnd_cmp )
	{
		set_list_width( "ID_CMP_TYPE",		ListView_GetColumnWidth( m_hwnd_cmp, ID_CMP_TYPE ) );
		set_list_width( "ID_CMP_FILE",		ListView_GetColumnWidth( m_hwnd_cmp, ID_CMP_FILE ) );
		set_list_width( "ID_CMP_SIZE",		ListView_GetColumnWidth( m_hwnd_cmp, ID_CMP_SIZE ) );

		char sz_file[MAX_PATH];	sz_file[0] = 0;
		ListView_GetItemText( m_hwnd_cmp, ROW_TYPE_FILE1, ID_CMP_FILE, sz_file, sizeof(sz_file) );
		set_dump_file( FILE1_DUMP, sz_file );
		ListView_GetItemText( m_hwnd_cmp, ROW_TYPE_FILE2, ID_CMP_FILE, sz_file, sizeof(sz_file) );
		set_dump_file( FILE2_DUMP, sz_file );
	}

	WritePrivateProfilePlacement( SECTION_WND_PLACE, WND_MEM_INFO, m_hwnd, get_ini_file() );

	if( m_src_wnd.handle() )
		::DestroyWindow( m_src_wnd.handle() );

	if( m_hicon_small )
		::DestroyIcon( m_hicon_small );	m_hicon_small = 0;

	if( m_hicon_big )
		::DestroyIcon( m_hicon_big );	m_hicon_big = 0;

	return dlg_impl::on_destroy( );
}

//////////////////////////////////////////////////////////////////////
void main_dlg::deinit_data()
{
	lock l( &g_block_mem_hook, true );
	m_map.clear();
	m_list.clear();
}

//////////////////////////////////////////////////////////////////////
void main_dlg::grab_current_heap_map()
{
	enter_cc cc( &g_cs_map );
	lock l( &g_block_mem_hook, true );
	clean_list( ROW_TYPE_MEM );

	DWORD dw_size = 0;

	for( long lpos=g_map_heap.head(); lpos; lpos=g_map_heap.next(lpos) )
	{
		heap_info_ex* phi_new = new heap_info_ex;

		phi_new->m_ltype = ROW_TYPE_MEM;
		m_list.add_tail( phi_new );
		
		heap_info* phi_src = g_map_heap.get( lpos );
		phi_new->copy_from( phi_src );

		dw_size += phi_new->m_dw_size;
	}

	char sz_buf[255];
	sprintf( sz_buf, "\nFull mem alloc size:%d", dw_size );
	OutputDebugString( sz_buf );
	//::MessageBox( m_hwnd, sz_buf, 0, 0 );
}

//////////////////////////////////////////////////////////////////////
void main_dlg::grab_external_heap_map()
{
	lock l( &g_block_mem_hook, true );
	clean_list( ROW_TYPE_MEM );
}

//////////////////////////////////////////////////////////////////////
void main_dlg::fill_list( )
{
	lock l( &g_block_mem_hook, true );
	
	if( !m_hwnd_addr || !m_hwnd_stack )	return;

	m_map.clear();

	//display options
	g_bgroup				= get_group_by_stack();
	int nsize				= get_min_alloc_size();
	g_nstack_group_depth	= get_group_stack_depth();

	//change title
	HWND hwnd_header = ListView_GetHeader( m_hwnd_addr );
	{
		HDITEM hdi	= {0};
		hdi.mask	= HDI_TEXT;
		hdi.pszText	= g_bgroup ? "Count" : "Time";
		Header_SetItem( hwnd_header, ID_ADDR_GROUP, &hdi );

		hdi.pszText	= g_bgroup ? "Stack CRC" : "Pointer";
		Header_SetItem( hwnd_header, ID_ADDR_POINTER, &hdi );
	}


	//empty lists
	ListView_DeleteAllItems( m_hwnd_addr );
	ListView_DeleteAllItems( m_hwnd_stack );


	DWORD dw_full_size	= 0;
	DWORD dw_mem_size	= 0;
	DWORD dw_file1_size	= 0;
	DWORD dw_file2_size	= 0;

	for( long lpos=m_list.head(); lpos; lpos=m_list.next( lpos ) )
	{
		heap_info_ex* phi = m_list.get( lpos );

		dw_full_size += phi->m_dw_size;

		if( phi->m_ltype == ROW_TYPE_MEM )
			dw_mem_size += phi->m_dw_size;
		else if( phi->m_ltype == ROW_TYPE_FILE1 )
			dw_file1_size += phi->m_dw_size;
		else if( phi->m_ltype == ROW_TYPE_FILE2 )
			dw_file2_size += phi->m_dw_size;

		if( g_bgroup )
		{
			long lpos_map = m_map.find( phi );
			if( lpos_map )
			{
				heap_info_ex* psrc = m_map.get( lpos_map );
				psrc->m_count++;
				psrc->m_dw_full_size += phi->m_dw_size;
			}
			else
			{
				phi->m_dw_full_size = phi->m_dw_size;
				phi->m_count = 1;
				m_map.set( phi, phi );
			}
		}
		else
		{
			phi->m_dw_full_size = phi->m_dw_size;
			phi->m_count = 1;
			m_map.set( phi, phi );
		}
	}

	//clear map if need
	if( nsize )
	{
		long lpos = m_map.head();
		while( lpos )
		{
			long lpos_save = lpos;
			lpos = m_map.next( lpos );	
			
			heap_info_ex* phi = m_map.get( lpos_save );
			if( phi->m_dw_full_size < (unsigned)nsize )
				m_map.remove( lpos_save );			
		}
	}

	char sz_buf[1000];	

	unsigned idx = 0;

	//update address list
	for( lpos = m_map.head(); lpos; lpos=m_map.next(lpos) )
	{		
		heap_info_ex* phi = m_map.get( lpos );

		LVITEM item;
		::ZeroMemory( &item, sizeof(LVITEM) );
		
		//type
		sprintf( sz_buf, "%d", phi->m_ltype );
		item.mask		= LVIF_TEXT|LVIF_PARAM;
		item.iSubItem	= ID_ADDR_TYPE;
		item.iItem		= idx;
		item.lParam		= (LPARAM)phi;
		item.pszText	= sz_buf;
		ListView_InsertItem( m_hwnd_addr, &item );


		//count
		item.mask		= LVIF_TEXT;
		item.iSubItem	= ID_ADDR_GROUP;
		item.iItem		= idx;
		item.lParam		= (LPARAM)phi;//(&parr[idx]);

		if( g_bgroup ) 
		{
			sprintf( sz_buf, "%d", (int)phi->m_count );
		}
		else
		{
			WORD msec = 0;
			time_t _time = get_time( phi->m_dw_time, &msec );			
			tm* _tm = localtime( &_time );
			sprintf( sz_buf, "%02d:%02d:%02d.%03d", (int)_tm->tm_hour, (int)_tm->tm_min, (int)_tm->tm_sec, (int)msec );
		}
		item.pszText	= sz_buf;
		ListView_SetItem( m_hwnd_addr, &item );

		//heap		
		if( phi->m_hheap == MI_BSTR )
			sprintf( sz_buf, "bstr" );
		else if( phi->m_hheap == MI_VIRTUAL )
			sprintf( sz_buf, "virtual" );
		else if( phi->m_hheap == MI_NT_VIRTUAL )
			sprintf( sz_buf, "nt_virtual" );
		else if( phi->m_hheap == MI_NEW_MFC60 )
			sprintf( sz_buf, "new_mfc60" );
		else if( phi->m_hheap == MI_NEW_MFC70 )
			sprintf( sz_buf, "new_mfc70" );
		else if( phi->m_hheap == MI_MALLOC60 )
			sprintf( sz_buf, "malloc60" );
		else if( phi->m_hheap == MI_NEW_CRT60 )
			sprintf( sz_buf, "new_crt60" );
		else if( phi->m_hheap == MI_MALLOC70 )
			sprintf( sz_buf, "malloc70" );
		else if( phi->m_hheap == MI_NEW_CRT70 )
			sprintf( sz_buf, "new_ctr70" );
		else
			sprintf( sz_buf, "0x%08X", (int)phi->m_hheap );

		item.mask		= LVIF_TEXT;
		item.iSubItem	= ID_ADDR_HEAP;		
		item.pszText	= sz_buf;
		ListView_SetItem( m_hwnd_addr, &item );

		//size
		sprintf( sz_buf, "%d", (int)phi->m_dw_full_size );
		item.mask		= LVIF_TEXT;
		item.iSubItem	= ID_ADDR_SIZE;		
		item.pszText	= sz_buf;
		ListView_SetItem( m_hwnd_addr, &item );		

		//pointer
		if( g_bgroup )
		{
			//calc stack crc
			unsigned crc = 0;
			for( unsigned i=0;i<phi->m_size_stack;i++ )
				crc += phi->m_pstack[i];

			sprintf( sz_buf, "0x%08X", (int)crc );
		}
		else
			sprintf( sz_buf, "0x%08X", (int)phi->m_laddress );//parr[idx].m_laddress );			

		item.mask		= LVIF_TEXT;
		item.iSubItem	= ID_ADDR_POINTER;		
		item.pszText	= sz_buf;
		ListView_SetItem( m_hwnd_addr, &item );
		idx++;
	}

	//update full size
	sprintf( sz_buf, "%d Kb (%d b)", long( dw_full_size / 1024 ), dw_full_size );
	::SetDlgItemText( m_hwnd, IDC_FULL_SIZE, sz_buf );

	ListView_SetItemState( m_hwnd_addr, 0, LVIS_FOCUSED | LVIS_SELECTED, 0x00F );		

	//update sizes for mem & files
	if( m_hwnd_cmp )
	{
		sprintf( sz_buf, "%d Kb", long( dw_mem_size / 1024 ) );
		ListView_SetItemText( m_hwnd_cmp, ROW_TYPE_MEM, ID_CMP_SIZE, sz_buf );

		sprintf( sz_buf, "%d Kb", long( dw_file1_size / 1024 ) );
		ListView_SetItemText( m_hwnd_cmp, ROW_TYPE_FILE1, ID_CMP_SIZE, sz_buf );

		sprintf( sz_buf, "%d Kb", long( dw_file2_size / 1024 ) );
		ListView_SetItemText( m_hwnd_cmp, ROW_TYPE_FILE2, ID_CMP_SIZE, sz_buf );
	}
}

//////////////////////////////////////////////////////////////////////
void main_dlg::clean_list( long ltype )
{
	lock l( &g_block_mem_hook, true );

	if( ltype == -1 )
	{
		m_list.clear();
		return;
	}

	long lpos = m_list.head();
	while( lpos )
	{
		long lpos_save = lpos;
		lpos = m_list.next( lpos );	
		
		heap_info_ex* phi = m_list.get( lpos_save );
		if( phi->m_ltype == ltype )
			m_list.remove( lpos_save );			
	}
}

//////////////////////////////////////////////////////////////////////
void main_dlg::on_address_change( int nitem )
{
	if( !m_hwnd_addr || !m_hwnd_stack )	return;

	//get item lparam
	LVITEM lvi = {0};
	lvi.mask	= LVIF_PARAM;
	lvi.iItem	= nitem;

	if( !ListView_GetItem( m_hwnd_addr, &lvi ) )	return;
	
	heap_info* phi = (heap_info*)lvi.lParam;
	if( !phi )	return;

	//empty list
	ListView_DeleteAllItems( m_hwnd_stack );
	
	char sz_buf[1000];

	for( int idx=0; idx<(int)phi->m_size_stack; idx++ )
	{	
		if( g_nstack_group_depth && (unsigned)idx == g_nstack_group_depth )
			break;

		LVITEM item;
		::ZeroMemory( &item, sizeof(LVITEM) );

		unsigned addr = phi->m_pstack[idx];
		
		line_info* pli = g_pdb_engine.get_line_info( addr );
		if( !pli )	continue;

		sprintf( sz_buf, "0x%08X", (int)addr );

		//address
		item.mask		= LVIF_TEXT|LVIF_PARAM;
		item.iSubItem	= ID_STACK_ADDR;
		item.iItem		= idx;
		item.lParam		= (LPARAM)addr;
		item.pszText	= sz_buf;
		ListView_InsertItem( m_hwnd_stack, &item );

		//ra		
		sprintf( sz_buf, "0x%08X", pli->m_lra );
		item.mask		= LVIF_TEXT;
		item.iSubItem	= ID_STACK_RA;		
		item.pszText	= sz_buf;
		ListView_SetItem( m_hwnd_stack, &item );

		//module		
		item.mask		= LVIF_TEXT;
		item.iSubItem	= ID_STACK_MODULE;		
		item.pszText	= pli->m_str_image_file_name.length() ? (char*)pli->m_str_image_file_name : "";
		ListView_SetItem( m_hwnd_stack, &item );

		//function
		item.mask		= LVIF_TEXT;
		item.iSubItem	= ID_STACK_FUNC;		
		item.pszText	= pli->m_str_src_function.length() ? (char*)pli->m_str_src_function : "";
		ListView_SetItem( m_hwnd_stack, &item );

		//src file
		item.mask		= LVIF_TEXT;
		item.iSubItem	= ID_STACK_SRC;		
		item.pszText	= pli->m_str_src_file_name.length() ? (char*)pli->m_str_src_file_name : "";
		ListView_SetItem( m_hwnd_stack, &item );

		//line
		sz_buf[0] = 0;
		if( pli->m_lsrc_line )
			sprintf( sz_buf, "%d", pli->m_lsrc_line );
		item.mask		= LVIF_TEXT;
		item.iSubItem	= ID_STACK_LINE;		
		item.pszText	= sz_buf;
		ListView_SetItem( m_hwnd_stack, &item );
	}

	//update info text
	sprintf( sz_buf, "%d/%d (all: %d)", nitem + 1, ListView_GetItemCount(m_hwnd_addr), m_list.count() );
	::SetDlgItemText( m_hwnd, IDC_ALLOC_INFO, sz_buf );
	
	//set active 0 stack item
	ListView_SetItemState( m_hwnd_stack, 0, LVIS_FOCUSED | LVIS_SELECTED, 0x00F );
}

//////////////////////////////////////////////////////////////////////
void main_dlg::on_stack_change( int nitem )
{
	if( !m_hwnd_stack )	return;	

	//update info text
	char sz_buf[100];
	sprintf( sz_buf, "%d/%d", nitem + 1, ListView_GetItemCount(m_hwnd_stack) );
	::SetDlgItemText( m_hwnd, IDC_STACK_INFO, sz_buf );
}

//////////////////////////////////////////////////////////////////////
void main_dlg::on_browse_compare_file( int nitem )
{
	lock l( &g_block_mem_hook, true );

	if( !m_hwnd_cmp )			return;
	if( nitem == ROW_TYPE_MEM )	return;

	char sz_file[MAX_PATH];	sz_file[0] = 0;
	ListView_GetItemText( m_hwnd_cmp, nitem, ID_CMP_FILE, sz_file, sizeof(sz_file) ); 

	OPENFILENAME ofn = {0};
	::ZeroMemory( &ofn, sizeof( ofn ) );
	ofn.lStructSize = sizeof( ofn );
	ofn.hwndOwner = m_hwnd; 
	ofn.hInstance = module::hinst();
	ofn.lpstrFilter = "Dump files\0*.dmp\0\0";
	ofn.lpstrDefExt = ".dmp";
	ofn.lpstrFile = sz_file;
	ofn.nMaxFile = sizeof( sz_file );
	ofn.Flags = OFN_EXPLORER|OFN_LONGNAMES|OFN_ENABLESIZING;		

	if( ::GetSaveFileName( &ofn ) != IDOK )	return;
	
	ListView_SetItemText( m_hwnd_cmp, nitem, ID_CMP_FILE, sz_file ); 
}

//////////////////////////////////////////////////////////////////////
void main_dlg::on_browse_stack( int nitem )
{
	lock l( &g_block_mem_hook, true );

	if( !m_hwnd_stack )	return;

	char sz_file[MAX_PATH];	sz_file[0] = 0;
	char sz_line[20];		sz_line[0] = 0;

	ListView_GetItemText( m_hwnd_stack, nitem, ID_STACK_SRC, sz_file, sizeof(sz_file) ); 
	ListView_GetItemText( m_hwnd_stack, nitem, ID_STACK_LINE, sz_line, sizeof(sz_file) ); 	

	int nline = atoi( sz_line );

	if( !m_src_wnd.handle() )
		m_src_wnd.create_window( 0 );

	m_src_wnd.open_file( sz_file, nline );
	
}

//state
//////////////////////////////////////////////////////////////////////
void main_dlg::on_state_change()
{
	if( ::IsDlgButtonChecked( m_hwnd, IDC_STATE_RUN ) )
		g_bRunMemTrace = true;
	else
		g_bRunMemTrace = false;
}

//////////////////////////////////////////////////////////////////////
void main_dlg::set_state_to_ctrls()
{
	if( g_block_mem_hook )
	{
		::CheckDlgButton( m_hwnd, IDC_STATE_STOP, 1L );
		::CheckDlgButton( m_hwnd, IDC_STATE_RUN, 0L );
	}
	else
	{
		::CheckDlgButton( m_hwnd, IDC_STATE_RUN, 1L );
		::CheckDlgButton( m_hwnd, IDC_STATE_STOP, 0L );
	}
}

//////////////////////////////////////////////////////////////////////
bool main_dlg::save_dump( const char* psz_file )
{
	lock l( &g_block_mem_hook, true );

	if( !psz_file )	return false;

	FILE* pfile = ::fopen( psz_file, "wt" );
	if( !pfile )
	{
		dump_error( "Can't write to file %s, W32E: %08X", psz_file, ::GetLastError() );
		return false;
	}

	bool berror = false;
	::fprintf( pfile, "Txt dump file v1.0\n" );
	::fprintf( pfile, "Time heap size address {stack}\n" );

	for( long lpos=m_list.head(); lpos; lpos=m_list.next( lpos ) )
	{
		heap_info_ex* phi = m_list.get( lpos );
		if( phi->m_ltype != ROW_TYPE_MEM )	continue;

		if( !::fprintf( pfile, "0x%08X 0x%08X 0x%08X 0x%08X", 
						phi->m_dw_time, 
						phi->m_hheap, 
						phi->m_dw_size, 
						phi->m_laddress ) )
		{
			berror = true;
			break;
		}

		::fprintf( pfile, " {" );

		for( int idx_stack=0; idx_stack<phi->m_size_stack; idx_stack++ )
		{
			if( !::fprintf( pfile, "0x%08X ", phi->m_pstack[idx_stack] ) )
			{
				berror = true;
				break;
			}
		}

		::fprintf( pfile, "}\n" );

		phi++;
	}	

	::fclose( pfile );	pfile = 0;

	if( berror )
		dump_error( "Can't write to file %s, W32E: %08X", psz_file, ::GetLastError() );

	return true;
}

//////////////////////////////////////////////////////////////////////
bool main_dlg::load_dump( const char* psz_file, long ltype )
{
	lock l( &g_block_mem_hook, true );

	clean_list( ltype );

	if( !psz_file )	return false;
	
	char sz_buf[5000];

	FILE* pfile = ::fopen( psz_file, "rt" );
	if( !pfile )
	{
		dump_error( "Can't read from file %s, W32E: %08X", psz_file, ::GetLastError() );
		return false;
	}

	//"Txt dump file v1.0"
	if( !::fgets( sz_buf, sizeof(sz_buf), pfile ) )
	{	
		dump_error( "Invalid format" );
		return false;
	}

	sz_buf[0] = 0;

	//"Time heap size address {stack}"
	if( !::fgets( sz_buf, sizeof(sz_buf), pfile ) )
	{	
		dump_error( "Invalid format" );
		return false;
	}

	unsigned stack_temp[MAX_STACK_SIZE];
	
	sz_buf[0] = 0;
	while( ::fgets( sz_buf, sizeof(sz_buf), pfile ) )
	{
		char* psz_br1 = strchr( sz_buf, '{' );
		if( !psz_br1 )	break;

		char* psz_br2 = strchr( sz_buf, '}' );
		if( !psz_br2 )	break;

		if( psz_br2 < psz_br1 )	break;

		char* psz_stack = psz_br1 + 1;
		*psz_br1 = *psz_br2 = 0;

		heap_info_ex* phi = new heap_info_ex;
		if( !phi )	return false;

		phi->m_ltype = ltype;

		//parse info
		if( 4 != ::sscanf( sz_buf, "0x%08X 0x%08X 0x%08X 0x%08X", 
						&phi->m_dw_time, 
						&phi->m_hheap, 
						&phi->m_dw_size, 
						&phi->m_laddress ) )
		{
			delete phi;
			return false;
		}

		m_list.add_tail( phi );

		//parse stack
		unsigned stack_count = 0;
		char* psz_next = strtok( psz_stack, " " );
		while( psz_next )
		{			
			unsigned address = 0;
			if( 1 != ::sscanf( psz_next, "0x%08X", &stack_temp[stack_count++] ) )
				break;

			if( stack_count == MAX_STACK_SIZE )
				break;
			psz_next = strtok( 0, " " );
		}

		if( stack_count )
		{
			phi->m_pstack = new unsigned[stack_count];
			memcpy( phi->m_pstack, stack_temp, sizeof(unsigned) * stack_count );
			phi->m_size_stack = stack_count;
		}		
	}

	return true;

}
#include "stdafx.h"
#include "resource.h"
#include "main_dlg.h"

#include <stdio.h>
#include "\vt5\awin\misc_ini.h"

//////////////////////////////////////////////////////////////////////
bool IsFileExist( const char* psz_file )
{
	if( !psz_file )	return false;

	if( psz_file[0] == '\\' )
		return false;

	HANDLE	hFile = ::CreateFile( psz_file, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0 );
	if( hFile == INVALID_HANDLE_VALUE )
		return false;
	
	::CloseHandle( hFile );
	return true;
}

bool RegDeleteKeyEx( HKEY	hGlobalKey, const char *pszRegKey )
{
	if( !strlen( pszRegKey ) )
		return false;
	int	idx= 0;

	HKEY	hKey;
	if( ::RegOpenKey( hGlobalKey, pszRegKey, &hKey ) == 0 )
	{
		DWORD	nSubKeysCount = 0;
		RegQueryInfoKey( hKey, 0, 0, 0, &nSubKeysCount, 0, 0, 0, 0, 0, 0, 0 );

		if( nSubKeysCount )
		{
			int	idx = nSubKeysCount;
			idx--;
			char	szKeyName[255];
			DWORD	dwSize;

			while( idx >= 0 )
			{
				dwSize = 255;
				::RegEnumKey( hKey, idx, szKeyName, dwSize );

				char	szFull[255];
				strcpy( szFull, pszRegKey );
				strcat( szFull, "\\" );
				strcat( szFull, szKeyName );
				::RegDeleteKeyEx( hGlobalKey, szFull );

				idx--;
			}
		}
		::RegCloseKey( hKey );
	}

	return ( ERROR_SUCCESS == ::RegDeleteKey( hGlobalKey, pszRegKey ) );
}

//////////////////////////////////////////////////////////////////////
char	g_sz_log_path[MAX_PATH] = "";

void WriteLogLine( const char *lpszFormat, ... )
{
	if( !strlen( g_sz_log_path ) )return;

	va_list args;
	va_start(args, lpszFormat);

	HANDLE	hFile = ::CreateFile( g_sz_log_path, GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_ALWAYS, 0, 0 );
	if( hFile != INVALID_HANDLE_VALUE )
	{
		
		char szBuffer[512];

		_vsntprintf(szBuffer, sizeof(szBuffer), lpszFormat, args);
		strcat( szBuffer, "\r\n" );

		DWORD	dwSize = strlen( szBuffer );
		::SetFilePointer( hFile, 0, 0, FILE_END );
		::WriteFile( hFile, szBuffer, dwSize, &dwSize, 0 );
		::CloseHandle( hFile );
	}
	va_end(args);
}


//////////////////////////////////////////////////////////////////////
char* GetCurrentLogTime()
{
	static char sz_time[255];

	time_t ltime;
	time( &ltime );

	tm* today = localtime( &ltime );
	
	strftime( sz_time, sizeof(sz_time), "%d-%m-%y %H:%M:%S", today );

	return sz_time;
}




//////////////////////////////////////////////////////////////////////
//
//	class main_dlg
//
//////////////////////////////////////////////////////////////////////
main_dlg::main_dlg() : dlg_impl( IDD_MAIN )
{
	m_hwnd_status	= 0;
	m_himage_list	= 0;

	m_bprogid		= false;
	m_str_progid	= "";
	m_bprogid_and	= false;

	m_bfile			= false;
	m_str_file		= "";
	m_bfile_and		= false;

	m_bna_file		= false;
	m_bna_file_and	= false;

	m_buse_filters	= false;
}

//////////////////////////////////////////////////////////////////////
main_dlg::~main_dlg()
{

}

//////////////////////////////////////////////////////////////////////
long main_dlg::on_initdialog()
{
	long lres = dlg_impl::on_initdialog();

	{
		g_sz_log_path[0] = 0;
		::GetModuleFileName( 0, g_sz_log_path, sizeof(g_sz_log_path) );
		char* psz = strrchr( g_sz_log_path, '\\' );
		if( psz )
		{
			*psz = 0;
			strcat( g_sz_log_path, "\\reg_info.log" );

			::DeleteFile( g_sz_log_path );
		}

		WriteLogLine( "//////////////////////////////////////////////////////////////////////" );
		WriteLogLine( "%s\t\tStart Session", GetCurrentLogTime() );
		WriteLogLine( "//////////////////////////////////////////////////////////////////////" );
	}

	load_profile();

	m_hwnd_status	= ::GetDlgItem( handle(), IDC_STATUS );	
	m_hwnd_tree		= ::GetDlgItem( handle(), IDC_TREE );	
	m_hwnd_progress	= ::GetDlgItem( handle(), IDC_PROGRESS );

	m_himage_list	= ImageList_LoadImage(	module::hinst(), MAKEINTRESOURCE(IDB_LIST), 
											16, 0, CLR_DEFAULT, IMAGE_BITMAP, 0 );
	TreeView_SetImageList( m_hwnd_tree, m_himage_list, TVSIL_NORMAL );

	::SetWindowText( m_hwnd_status, "" );

	return lres;
}

//////////////////////////////////////////////////////////////////////
long main_dlg::on_destroy()
{
	if( m_himage_list )
		ImageList_Destroy( m_himage_list );	m_himage_list = 0;

	save_profile();

	return dlg_impl::on_destroy();
}

//////////////////////////////////////////////////////////////////////
long main_dlg::on_command( uint cmd )
{
	if( cmd == IDC_ENUM )
	{
		fill_clsid_list();
		fill_control();
		return 0L;
	}
	else if( cmd == IDC_FILTERS )
	{
		bool bres = false;
		{
			filter_dlg dlg;
			
			dlg.m_bprogid		= m_bprogid;
			dlg.m_str_progid	= m_str_progid;
			dlg.m_bprogid_and	= m_bprogid_and;
			dlg.m_bfile			= m_bfile;
			dlg.m_str_file		= m_str_file;
			dlg.m_bfile_and		= m_bfile_and;
			dlg.m_bna_file		= m_bna_file;
			dlg.m_bna_file_and	= m_bna_file_and;
			dlg.m_buse_filters	= m_buse_filters;

			if(  dlg.do_modal( handle() ) == IDOK )
			{
				m_bprogid		= dlg.m_bprogid;
				m_str_progid	= dlg.m_str_progid;
				m_bprogid_and	= dlg.m_bprogid_and;
				m_bfile			= dlg.m_bfile;
				m_str_file		= dlg.m_str_file;
				m_bfile_and		= dlg.m_bfile_and;
				m_bna_file		= dlg.m_bna_file;
				m_bna_file_and	= dlg.m_bna_file_and;
				m_buse_filters	= dlg.m_buse_filters;
				
				bres =  true;
			}
		}

		if( bres )	fill_control();
		
		return 0L;
	}
	else if( cmd == IDC_SELECT_ENTRY || cmd == IDC_UNSELECT_ENTRY )
	{
		HTREEITEM hti = TreeView_GetSelection( m_hwnd_tree );
		recursive_select( hti, cmd == IDC_SELECT_ENTRY );
		return 0L;
	}
	else if( cmd == IDC_UNSELECT_ALL )
	{
		HTREEITEM hti = TreeView_GetRoot( m_hwnd_tree );
		while( hti )
		{
			recursive_select( hti, false );
			hti = TreeView_GetNextItem( m_hwnd_tree, hti, TVGN_NEXT );
		}
		return 0L;
	}
	else if( cmd == IDC_DELETE_SELECTED )
	{
		clsid_delete_list list;

		::SetWindowText( m_hwnd_status, "Gather entries.." );
		HTREEITEM hti = TreeView_GetRoot( m_hwnd_tree );
		while( hti )
		{
			recursive_gather_clsid( hti, list );
			hti = TreeView_GetNextItem( m_hwnd_tree, hti, TVGN_NEXT );
		}
		::SetWindowText( m_hwnd_status, "Gather entries Done" );

		confirm_dlg dlg;
		dlg.m_plist			= &list;
		dlg.m_himage_list	= m_himage_list;

		if( list.count() && dlg.do_modal( handle() ) == IDOK )
		{
			bool berror = false;
			::SetWindowText( m_hwnd_status, "Delete servers..." );			
			WriteLogLine( "%s\t\t Delete %d servers", GetCurrentLogTime(), list.count() );
			
			char sz_buf[255];
			int ncur = 0;
			for( long lpos = list.head(); lpos; lpos = list.next( lpos ) )
			{
				clsid_entry* pe = list.get( lpos );
				
				if( pe->m_str_clsid.length() )
				{
					strcpy( sz_buf, "CLSID\\" );
					strcat( sz_buf, (char*)pe->m_str_clsid );
					bool bres = RegDeleteKeyEx( HKEY_CLASSES_ROOT, sz_buf );
					if( bres )
					{
						WriteLogLine( "Delete [%s] entry.", sz_buf );
					}
					else
					{
						WriteLogLine( "Delete [%s] entry fail.", sz_buf );
						berror = true;
					}
				}

				if( pe->m_str_prog_id.length() )
				{
					bool bres = RegDeleteKeyEx( HKEY_CLASSES_ROOT, (char*)pe->m_str_prog_id );
					if( bres )
					{
						WriteLogLine( "Delete [%s] entry.", (char*)pe->m_str_prog_id );
					}
					else
					{
						WriteLogLine( "Delete [%s] entry fail.", (char*)pe->m_str_prog_id );
						berror = true;
					}
				}
				ncur++;
				if( ( ncur % 10 ) == 0 )
					::SendMessage( m_hwnd_progress, PBM_SETPOS, ncur * 100 / list.count(), 0 );
			}

			::SetWindowText( m_hwnd_status, berror ? "Delete fail, see log for details" : "Delete success" );
			WriteLogLine( "%s\t\t Delete %d servers done", GetCurrentLogTime(), list.count() );

			::SendMessage( m_hwnd_progress, PBM_SETPOS, 100, 0 );
		}		

		return 0L;
	}
	
	return dlg_impl::on_command( cmd );
}

//////////////////////////////////////////////////////////////////////
bool main_dlg::fill_clsid_list()
{
	m_map_server_dirs.clear();
	m_map_file_dirs.clear();

	m_server_list.clear();
	m_clsid_list.clear();

	HKEY	hkey_clsid = 0;
	if( ::RegOpenKey( HKEY_CLASSES_ROOT, "CLSID", &hkey_clsid ) != 0 )
		return false;

	DWORD	nsub_keys_count = 0;
	RegQueryInfoKey( hkey_clsid, 0, 0, 0, &nsub_keys_count, 0, 0, 0, 0, 0, 0, 0 );
	int	idx = 0;

	::SendMessage( m_hwnd_progress, PBM_SETRANGE, 0, MAKELPARAM(0, 100 ) );

	char	sz_key_name[255];	
	char	sz_server_key[255];
	char	sz_prog_id[255];
	char	sz_file_name[255];
	char	sz_long_file_name[255];
	char	sz_status[255];
	DWORD	dw_size;
	while( true )
	{
		sz_key_name[0]		=	0;
		sz_server_key[0]		= 0;
		sz_file_name[0]			= 0;
		sz_long_file_name[0]	= 0;
		sz_prog_id[0]			= 0;
		dw_size					= 255;
		if( ::RegEnumKey( hkey_clsid, idx++, sz_key_name, dw_size ) != 0 )
			break;

		clsid_entry* p = new clsid_entry;
		m_clsid_list.add_tail( p );

		p->m_str_clsid = sz_key_name;

		HKEY	hkey_server = 0;
		strcpy( sz_server_key, "CLSID\\" );
		strcat( sz_server_key, (char*)p->m_str_clsid );
		strcat( sz_server_key, "\\InprocServer32" );

		DWORD	dw_type = 0;
		
		if( ::RegOpenKey( HKEY_CLASSES_ROOT, sz_server_key, &hkey_server ) == 0 )
		{
			dw_size = 255;
			dw_type = REG_SZ;
			::RegQueryValueEx( hkey_server, 0, 0, &dw_type, (BYTE*)sz_file_name, &dw_size );
			::RegCloseKey( hkey_server );

			if( sz_file_name[0] == '\\' )
				strcpy( sz_long_file_name, sz_file_name  );
			else
				GetLongPathName( sz_file_name, sz_long_file_name, 255 );

			strlwr( sz_long_file_name );
			
			p->m_str_file_name = sz_long_file_name;
			char* psz_last = strrchr( sz_long_file_name, '\\' );
			{
				if( psz_last )
					*psz_last = 0;
				
				server_dir* pserver = 0;

				long lpos = m_map_server_dirs.find( sz_long_file_name );
				if( lpos )
				{
					pserver = m_map_server_dirs.get( lpos );
				}
				else
				{
					pserver = new server_dir;
					m_server_list.add_tail( pserver );
					pserver->m_str_dir = sz_long_file_name;
					m_map_server_dirs.set( pserver, (char*)pserver->m_str_dir );
				}

				file_server* pfile = 0;
				lpos = m_map_file_dirs.find( (char*)p->m_str_file_name );
				if( lpos )
				{
					pfile = m_map_file_dirs.get( lpos );
				}
				else
				{
					pfile = new file_server;
					pserver->m_file_list.add_tail( pfile );
					pfile->m_str_file_name = p->m_str_file_name;
					m_map_file_dirs.set( pfile, (char*)pfile->m_str_file_name );
				}

				pfile->m_clsid_ref_list.add_tail( p );
			}
		}

		HKEY	hkey_progid = 0;
		
		strcpy( sz_prog_id, "CLSID\\" );
		strcat( sz_prog_id, (char*)p->m_str_clsid );
		strcat( sz_prog_id, "\\ProgID" );

		if( ::RegOpenKey( HKEY_CLASSES_ROOT, sz_prog_id, &hkey_progid ) == 0 )
		{
			dw_size = 255;
			dw_type = REG_SZ;
			::RegQueryValueEx( hkey_progid, 0, 0, &dw_type, (BYTE*)sz_prog_id, &dw_size );
			::RegCloseKey( hkey_progid );

			p->m_str_prog_id = sz_prog_id;
		}

		if( ( idx % 100 ) == 0 )
		{
			::SendMessage( m_hwnd_progress, PBM_SETPOS, idx * 100 / nsub_keys_count, 0 );

			::sprintf( sz_status, "Enum CLSID: %d/%d", idx - 1, nsub_keys_count );
			::SetWindowText( m_hwnd_status, sz_status );
		}
	}

	::RegCloseKey( hkey_clsid );	hkey_clsid = 0;

	::SetWindowText( m_hwnd_status, "Enum Done" );
	::SendMessage( m_hwnd_progress, PBM_SETPOS, 100, 0 );

	sprintf( sz_status, "(Dirs=%d,CLSID=%d)", m_server_list.count(), m_clsid_list.count() );
	::SetDlgItemText( handle(), IDC_GEN_STATUS, sz_status );

	return true;
}

//////////////////////////////////////////////////////////////////////
bool main_dlg::fill_control()
{
	if( !m_hwnd_tree )	return false;

	if( !m_clsid_list.count() )	return false;

	long l1 = ::GetTickCount();
	TreeView_DeleteAllItems( m_hwnd_tree );
	UpdateWindow( m_hwnd_tree );
	long l2 = ::GetTickCount();

	//::SendMessage( m_hwnd_tree, WM_SETREDRAW, false, 0);

	char sz_buf[255];

	TVINSERTSTRUCT tvi;
	::ZeroMemory( &tvi, sizeof(TVINSERTSTRUCT) );

	tvi.item.mask			= TVIF_TEXT | TVIF_PARAM | TVIF_SELECTEDIMAGE | TVIF_IMAGE;

	int nreal_count = 0;

	::SetWindowText( m_hwnd_status, "Fill Tree..." );

	for( long lpos_server=m_server_list.head(); lpos_server; lpos_server = m_server_list.next( lpos_server ) )
	{
		server_dir* pdir = m_server_list.get( lpos_server );
		
		tvi.hParent				= 0;
		tvi.item.iImage			= SERVER_DIR_IDX;
		tvi.item.iSelectedImage = SERVER_DIR_IDX;
		tvi.item.lParam			= (LPARAM)0;
		tvi.item.pszText		= (char*)pdir->m_str_dir;

		HTREEITEM hti_dir = TreeView_InsertItem( m_hwnd_tree, &tvi );

		char sz_prog_id_lower[255];
		::strcpy( sz_prog_id_lower, (char*)m_str_progid );
		::strlwr( sz_prog_id_lower );
		char sz_file_lower[255];
		::strcpy( sz_file_lower, (char*)m_str_file );
		::strlwr( sz_file_lower );

		int nentry_count = 0;
		for( long lpos_dir = pdir->m_file_list.head(); lpos_dir; lpos_dir = pdir->m_file_list.next( lpos_dir ) )
		{	
			file_server* pfile	= pdir->m_file_list.get( lpos_dir );
			bool bexist			= IsFileExist( (char*)pfile->m_str_file_name );
			tvi.hParent				= hti_dir;
			tvi.item.iImage			= bexist?FILE_EX_IDX:FILE_NA_IDX;
			tvi.item.iSelectedImage = bexist?FILE_EX_IDX:FILE_NA_IDX;
			tvi.item.lParam			= (LPARAM)0;
			tvi.item.pszText		= (char*)pfile->m_str_file_name;

			HTREEITEM hti_file = TreeView_InsertItem( m_hwnd_tree, &tvi );
			for( long lpos = pfile->m_clsid_ref_list.head(); lpos; lpos = pfile->m_clsid_ref_list.next( lpos ) )
			{
				clsid_entry* pentry = pfile->m_clsid_ref_list.get( lpos );				

				nreal_count++;

				if( ( nreal_count % 100 ) == 0 )
					::SendMessage( m_hwnd_progress, PBM_SETPOS, nreal_count * 100 / m_clsid_list.count(), 0 );

				bool bcan_insert = true;
				if( m_buse_filters )
				{
					bcan_insert = false;
					bool b1, b2, b3;
					b1 = b2 = b3 = true;			

					if( m_bprogid )
					{
						if( m_str_progid.length() )
						{
							char sz_buf1[255];
							::strcpy( sz_buf1, (char*)pentry->m_str_prog_id );
							::strlwr( sz_buf1 );
							b1 = ( 0!= ::strstr( sz_buf1, sz_prog_id_lower ) );
						}
						else
						{
							b1 = ( !pentry->m_str_prog_id.length() );
						}

 						if( m_bprogid_and )
							bcan_insert = bcan_insert && b1;
						else
							bcan_insert = bcan_insert || b1;

					}
					if( m_bfile )
					{
						if( m_str_file.length() )
						{
							char sz_buf1[255];
							::strcpy( sz_buf1, (char*)pentry->m_str_file_name );
							::strlwr( sz_buf1 );
							b2 = ( 0!= ::strstr( sz_buf1, sz_file_lower ) );
						}
						else
						{
							b2 = ( !pentry->m_str_file_name.length() );
						}

 						if( m_bfile_and )
							bcan_insert = bcan_insert && b2;
						else
							bcan_insert = bcan_insert || b2;
					}
					if( m_bna_file )
					{
						b3 = !bexist;

 						if( m_bna_file_and )
							bcan_insert = bcan_insert && b3;
						else
							bcan_insert = bcan_insert || b3;
					}
				}
				
				if( !bcan_insert )	continue;

				sprintf( sz_buf, "%s [%s]", (char*)pentry->m_str_clsid, (char*)pentry->m_str_prog_id );
				
				tvi.hParent				= hti_file;
				tvi.item.iImage			= CLSID_IDX;
				tvi.item.iSelectedImage = CLSID_IDX;
				tvi.item.lParam			= (LPARAM)pentry;
				tvi.item.pszText		= sz_buf;

				TreeView_InsertItem( m_hwnd_tree, &tvi );
				nentry_count++;
			}
		}
		
		/*
		if( m_buse_filters )
		{
			sprintf( sz_buf, "%s(%d/%d)", (char*)pserver->m_str_dir, nentry_count, pserver->m_clsid_ref_list.count() );
			
			TVITEM tvi_server	= {0};
			tvi_server.mask		= TVIF_TEXT|TVIF_HANDLE;
			tvi_server.pszText	= sz_buf;
			tvi_server.hItem	= hti_server;

			TreeView_SetItem( m_hwnd_tree, &tvi_server );
		}
		*/
	}

	long l3 = ::GetTickCount();
	
	//kill empty & set count
	HTREEITEM hti_dir = TreeView_GetRoot( m_hwnd_tree );
	while( hti_dir )
	{	
		int ndir_clsid_count = 0;		
		HTREEITEM hti_file = TreeView_GetNextItem( m_hwnd_tree, hti_dir, TVGN_CHILD );
		while( hti_file )
		{
			int nfile_clsid_count = 0;
			HTREEITEM hti_clsid = TreeView_GetNextItem( m_hwnd_tree, hti_file, TVGN_CHILD );
			while( hti_clsid )
			{
				ndir_clsid_count++;
				nfile_clsid_count++;
				HTREEITEM hti_clsid_save = hti_clsid;	
				hti_clsid = TreeView_GetNextItem( m_hwnd_tree, hti_clsid, TVGN_NEXT );
			}
			
			HTREEITEM hti_file_save = hti_file;
			hti_file = TreeView_GetNextItem( m_hwnd_tree, hti_file, TVGN_NEXT );
			if( !TreeView_GetNextItem( m_hwnd_tree, hti_file_save, TVGN_CHILD ) )
			{
				TreeView_DeleteItem( m_hwnd_tree, hti_file_save );
			}
			else
			{
				char sz_text[255];	sz_text[0] = 0;
				sz_buf[0] = 0;
				TVITEM tvi_file		= {0};
				tvi_file.mask		= TVIF_HANDLE|TVIF_TEXT;
				tvi_file.hItem		= hti_file_save;
				tvi_file.pszText	= sz_buf;
				tvi_file.cchTextMax	= sizeof(sz_buf);
				if( TreeView_GetItem( m_hwnd_tree, &tvi_file ) )
				{
					long lpos = m_map_file_dirs.find( sz_buf );
					file_server* pfile = 0;
					if( lpos )
					{
						pfile = m_map_file_dirs.get( lpos );
					}
					if( pfile )
					{
						if( m_buse_filters )
							sprintf( sz_text, "%s(%d/%d)", sz_buf, nfile_clsid_count, pfile->m_clsid_ref_list.count() );
						else
							sprintf( sz_text, "%s(%d)", sz_buf, pfile->m_clsid_ref_list.count() );
				
						tvi_file.pszText	= sz_text;
						TreeView_SetItem( m_hwnd_tree, &tvi_file );
					}				
				}
			}
		}
		

		HTREEITEM hti_dir_save = hti_dir;
		hti_dir = TreeView_GetNextItem( m_hwnd_tree, hti_dir, TVGN_NEXT );
		if( !TreeView_GetNextItem( m_hwnd_tree, hti_dir_save, TVGN_CHILD ) )
		{
			TreeView_DeleteItem( m_hwnd_tree, hti_dir_save );
		}
		else
		{
			char sz_text[255];	sz_text[0] = 0;
			sz_buf[0] = 0;
			TVITEM tvi_dir		= {0};
			tvi_dir.mask		= TVIF_HANDLE|TVIF_TEXT;
			tvi_dir.hItem		= hti_dir_save;
			tvi_dir.pszText		= sz_buf;
			tvi_dir.cchTextMax	= sizeof(sz_buf);
			if( TreeView_GetItem( m_hwnd_tree, &tvi_dir ) )
			{
				long lpos = m_map_server_dirs.find( sz_buf );
				server_dir* pdir = 0;
				if( lpos )
				{
					pdir = m_map_server_dirs.get( lpos );
				}
				if( pdir )
				{
					int ndir_clsid_sum = 0;
					for( long lpos_file = pdir->m_file_list.head(); lpos_file; lpos_file = pdir->m_file_list.next( lpos_file ) )
					{
						file_server* pfile = pdir->m_file_list.get( lpos_file );
						ndir_clsid_sum += pfile->m_clsid_ref_list.count();
					}
					if( m_buse_filters )
						sprintf( sz_text, "%s(%d/%d)", sz_buf, ndir_clsid_count, ndir_clsid_sum );
					else
						sprintf( sz_text, "%s(%d)", sz_buf, ndir_clsid_sum );

					tvi_dir.pszText	= sz_text;
					TreeView_SetItem( m_hwnd_tree, &tvi_dir );
				}				
			}
		}
	}

	long l4 = ::GetTickCount();
	
	::SendMessage( m_hwnd_progress, PBM_SETPOS, 100, 0 );
	::SetWindowText( m_hwnd_status, "Fill Tree Done" );

	TreeView_SortChildren( m_hwnd_tree, 0, false );

	//::SendMessage( m_hwnd_tree, WM_SETREDRAW, true, 0);
	::InvalidateRect( m_hwnd_tree, 0, TRUE );

	long l5 = ::GetTickCount();

	return true;
}

//////////////////////////////////////////////////////////////////////
bool main_dlg::recursive_select( HTREEITEM hti, bool bselect )
{
	if( !hti || !m_hwnd_tree )	return false;

	TreeView_SetCheckState( m_hwnd_tree, hti, bselect );
	hti = TreeView_GetNextItem( m_hwnd_tree, hti, TVGN_CHILD );
	while( hti )
	{
		recursive_select( hti, bselect );
		hti = TreeView_GetNextItem( m_hwnd_tree, hti, TVGN_NEXT );
	}
	
	return true;
}

//////////////////////////////////////////////////////////////////////
bool main_dlg::recursive_gather_clsid( HTREEITEM hti, clsid_delete_list& list )
{
	if( !hti || !m_hwnd_tree )	return false;

	UINT ui_state = TreeView_GetCheckState( m_hwnd_tree, hti );
	if( ui_state == 1 )
	{
		TVITEM tvi	= {0};
		tvi.mask	= TVIF_PARAM|TVIF_HANDLE;
		tvi.hItem	= hti;

		if( TreeView_GetItem( m_hwnd_tree, &tvi ) && tvi.lParam )
		{
			clsid_entry* pe = (clsid_entry*)tvi.lParam;
			list.add_tail( pe );
		}
	}
	hti = TreeView_GetNextItem( m_hwnd_tree, hti, TVGN_CHILD );
	while( hti )
	{
		recursive_gather_clsid( hti, list );
		hti = TreeView_GetNextItem( m_hwnd_tree, hti, TVGN_NEXT );
	}
	
	return true;
}

#define SECTION	"Filters"

//////////////////////////////////////////////////////////////////////
void main_dlg::load_profile()
{
	char sz_ini[255];	sz_ini[0] = 0;
	char sz_buf[255];	sz_buf[0] = 0;

	::GetModuleFileName( 0, sz_ini, sizeof(sz_ini) );
	char* psz = strrchr( sz_ini, '\\' );
	if( !psz )	return;

	*psz = 0;
	strcat( sz_ini, "\\reg_info.ini" );

	m_bprogid		= GetPrivateProfileBool( SECTION, "UseProgID", false, sz_ini );
	::GetPrivateProfileString( SECTION, "ProgID", "", sz_buf, sizeof(sz_buf), sz_ini );
	m_str_progid	= sz_buf;
	m_bprogid_and	= GetPrivateProfileBool( SECTION, "ProgIDAnd", false, sz_ini );

	m_bfile			= GetPrivateProfileBool( SECTION, "UseFile", false, sz_ini );
	::GetPrivateProfileString( SECTION, "File", "", sz_buf, sizeof(sz_buf), sz_ini );
	m_str_file		= sz_buf;
	m_bfile_and		= GetPrivateProfileBool( SECTION, "FileAnd", false, sz_ini );

	m_bna_file		= GetPrivateProfileBool( SECTION, "UseNAFile", false, sz_ini );
	m_bna_file_and	= GetPrivateProfileBool( SECTION, "NAFileAnd", false, sz_ini );

	m_buse_filters	= GetPrivateProfileBool( SECTION, "UseFilters", false, sz_ini );

	m_bprogid_and = m_bfile_and = m_bna_file_and = false;

}

//////////////////////////////////////////////////////////////////////
void main_dlg::save_profile()
{
	char sz_ini[255];	sz_ini[0] = 0;

	::GetModuleFileName( 0, sz_ini, sizeof(sz_ini) );
	char* psz = strrchr( sz_ini, '\\' );
	if( !psz )	return;

	*psz = 0;
	strcat( sz_ini, "\\reg_info.ini" );

	WritePrivateProfileBool( SECTION, "UseProgID", m_bprogid, sz_ini );
	::WritePrivateProfileString( SECTION, "ProgID", (char*)m_str_progid, sz_ini );
	WritePrivateProfileBool( SECTION, "ProgIDAnd", m_bprogid_and, sz_ini );;

	WritePrivateProfileBool( SECTION, "UseFile", m_bfile, sz_ini );;
	::WritePrivateProfileString( SECTION, "File", (char*)m_str_file, sz_ini );
	WritePrivateProfileBool( SECTION, "FileAnd", m_bfile_and, sz_ini );;

	WritePrivateProfileBool( SECTION, "UseNAFile", m_bna_file, sz_ini );;
	WritePrivateProfileBool( SECTION, "NAFileAnd", m_bna_file_and, sz_ini );;
	
	WritePrivateProfileBool( SECTION, "UseFilters", m_buse_filters, sz_ini );
}

//////////////////////////////////////////////////////////////////////
//
//	class confirm_dlg
//
//////////////////////////////////////////////////////////////////////
confirm_dlg::confirm_dlg() : dlg_impl( IDD_CONFIRM )
{
	m_plist			=  0;
	m_himage_list	= 0;
}

//////////////////////////////////////////////////////////////////////
confirm_dlg::~confirm_dlg()
{

}

//////////////////////////////////////////////////////////////////////
long confirm_dlg::on_initdialog()
{
	long lres = dlg_impl::on_initdialog();

	if( !m_plist )		return lres;

	HWND hwnd_list = ::GetDlgItem( handle(), IDC_LIST );
	if( !hwnd_list )	return lres;

	ListView_SetExtendedListViewStyle( hwnd_list, LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT );

	ListView_SetImageList( hwnd_list, m_himage_list, LVSIL_SMALL );


	LVCOLUMN column;
	memset( &column, 0, sizeof(LVCOLUMN) );

	column.mask		= LVCF_TEXT | LVCF_WIDTH;

	column.pszText	= "N";
	column.cx		= 60;
	ListView_InsertColumn( hwnd_list, 0, &column );	

	column.pszText	= "CLSID";
	column.cx		= 200;

	ListView_InsertColumn( hwnd_list, 1, &column );	

	column.pszText	= "ProgID";
	column.cx		= 200;

	ListView_InsertColumn( hwnd_list, 2, &column );	

	column.pszText	= "File";
	column.cx		= 300;
	ListView_InsertColumn( hwnd_list, 3, &column );	

	column.pszText	= "Exist";
	column.cx		= 50;
	ListView_InsertColumn( hwnd_list, 4, &column );	

	LVITEM item = {0};
	item.mask		= LVIF_TEXT|LVIF_IMAGE;	
	char sz_buf[255];

	for( long lpos = m_plist->head(); lpos; lpos = m_plist->next( lpos ) )
	{
		clsid_entry* pe = m_plist->get( lpos );
		
		sprintf( sz_buf, "%d", item.iItem );
		bool bexist = IsFileExist( pe->m_str_file_name );

		item.iImage = bexist ? FILE_EX_IDX : FILE_NA_IDX;
		item.pszText = sz_buf;
		item.iSubItem = 0;
		ListView_InsertItem( hwnd_list, &item );

		item.iSubItem++;
		item.pszText = pe->m_str_clsid;
		ListView_SetItem( hwnd_list, &item );

		item.iSubItem++;
		item.pszText = pe->m_str_prog_id;
		ListView_SetItem( hwnd_list, &item );

		item.iSubItem++;
		item.pszText = pe->m_str_file_name;
		ListView_SetItem( hwnd_list, &item );

		item.iSubItem++;
		item.pszText = bexist ? "*" : "" ;
		ListView_SetItem( hwnd_list, &item );

		item.iItem++;		
	}

	return lres;
}

int g_nCurSortColumn = 0;
int CALLBACK CompareListFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	HWND hwnd_list = (HWND)lParamSort;

	static char sz_buf1[255];
	static char sz_buf2[255];

	ListView_GetItemText( hwnd_list, lParam1, g_nCurSortColumn, sz_buf1, sizeof(sz_buf1) );
	ListView_GetItemText( hwnd_list, lParam2, g_nCurSortColumn, sz_buf2, sizeof(sz_buf2) );

	return stricmp( sz_buf1, sz_buf2 );
}


//////////////////////////////////////////////////////////////////////
long confirm_dlg::on_notify( uint idc, NMHDR *pnmhdr )
{
	if( idc == IDC_LIST && pnmhdr->code == LVN_COLUMNCLICK )
	{
		NMLISTVIEW* p = (NMLISTVIEW*)pnmhdr;
		g_nCurSortColumn = p->iSubItem;
		ListView_SortItemsEx( p->hdr.hwndFrom, CompareListFunc, p->hdr.hwndFrom );
	}
	return dlg_impl::on_notify( idc, pnmhdr );
}

//////////////////////////////////////////////////////////////////////
//
//	class filter_dlg
//
//////////////////////////////////////////////////////////////////////
filter_dlg::filter_dlg() : dlg_impl( IDD_FILTER )
{
	m_bprogid		= false;
	m_str_progid	= "";
	m_bprogid_and	= false;

	m_bfile			= false;
	m_str_file		= "";
	m_bfile_and		= false;

	m_bna_file		= false;
	m_bna_file_and	= false;

	m_buse_filters	= false;

}

//////////////////////////////////////////////////////////////////////
filter_dlg::~filter_dlg()
{

}

//////////////////////////////////////////////////////////////////////
long filter_dlg::on_initdialog()
{
	long lres = dlg_impl::on_initdialog();	
	fill_controls();
	update_ui();	
	return lres;
}

//////////////////////////////////////////////////////////////////////
long filter_dlg::on_command( uint cmd )
{
	if( cmd == IDC_PROGID_CHECK ||  cmd == IDC_FILE_CHECK || 
		cmd == IDC_NA_FILE_CHECK || cmd == IDC_USE_FILTER )
	{
		fill_members();
		update_ui();		
	}

	return dlg_impl::on_command( cmd );
}

//////////////////////////////////////////////////////////////////////
void filter_dlg::on_ok()
{
	fill_members();
	dlg_impl::on_ok();
}

//////////////////////////////////////////////////////////////////////
void filter_dlg::update_ui()
{
	::EnableWindow( ::GetDlgItem( handle(), IDC_PROGID_EDIT ), m_bprogid && m_buse_filters );
	::EnableWindow( ::GetDlgItem( handle(), IDC_PROGID_CHECK ), m_buse_filters );
	::EnableWindow( ::GetDlgItem( handle(), IDC_PROGID_CLAUSE ), m_bprogid && m_buse_filters );

	::EnableWindow( ::GetDlgItem( handle(), IDC_FILE_EDIT ), m_bfile && m_buse_filters );
	::EnableWindow( ::GetDlgItem( handle(), IDC_FILE_CHECK ), m_buse_filters );
	::EnableWindow( ::GetDlgItem( handle(), IDC_FILE_CLAUSE ), m_bfile && m_buse_filters );

	::EnableWindow( ::GetDlgItem( handle(), IDC_NA_FILE_CHECK ), m_buse_filters );
	::EnableWindow( ::GetDlgItem( handle(), IDC_NA_FILE_CLAUSE ), m_bna_file && m_buse_filters );
}

//////////////////////////////////////////////////////////////////////
void filter_dlg::fill_controls()
{
	CheckDlgButton( handle(), IDC_PROGID_CHECK, m_bprogid ? BST_CHECKED : BST_UNCHECKED );
	::SetDlgItemText( handle(), IDC_PROGID_EDIT, (char*)m_str_progid );
	CheckDlgButton( handle(), IDC_PROGID_CLAUSE, m_bprogid_and ? BST_CHECKED : BST_UNCHECKED );
	
	CheckDlgButton( handle(), IDC_FILE_CHECK, m_bfile ? BST_CHECKED : BST_UNCHECKED );
	::SetDlgItemText( handle(), IDC_FILE_EDIT, (char*)m_str_file );
	CheckDlgButton( handle(), IDC_FILE_CLAUSE, m_bfile_and ? BST_CHECKED : BST_UNCHECKED );

	CheckDlgButton( handle(), IDC_NA_FILE_CHECK, m_bna_file ? BST_CHECKED : BST_UNCHECKED );
	CheckDlgButton( handle(), IDC_NA_FILE_CLAUSE, m_bna_file_and ? BST_CHECKED : BST_UNCHECKED );
	
	CheckDlgButton( handle(), IDC_USE_FILTER, m_buse_filters ? BST_CHECKED : BST_UNCHECKED );	
}
//////////////////////////////////////////////////////////////////////
void filter_dlg::fill_members()
{
	char sz_buf[255];	sz_buf[0] = 0;

	m_bprogid = ( BST_CHECKED == IsDlgButtonChecked( handle(), IDC_PROGID_CHECK ) );
	::GetDlgItemText( handle(), IDC_PROGID_EDIT, sz_buf, sizeof(sz_buf) );
	m_str_progid = sz_buf;
	m_bprogid_and = ( BST_CHECKED == IsDlgButtonChecked( handle(), IDC_PROGID_CLAUSE ) );
	
	m_bfile = ( BST_CHECKED == IsDlgButtonChecked( handle(), IDC_FILE_CHECK ) );
	::GetDlgItemText( handle(), IDC_FILE_EDIT, sz_buf, sizeof(sz_buf) );
	m_str_file = sz_buf;
	m_bfile_and = ( BST_CHECKED == IsDlgButtonChecked( handle(), IDC_FILE_CLAUSE ) );

	m_bna_file = ( BST_CHECKED == IsDlgButtonChecked( handle(), IDC_NA_FILE_CHECK ) );
	m_bna_file_and = ( BST_CHECKED == IsDlgButtonChecked( handle(), IDC_NA_FILE_CLAUSE ) );

	m_buse_filters = ( BST_CHECKED == IsDlgButtonChecked( handle(), IDC_USE_FILTER ) );
}

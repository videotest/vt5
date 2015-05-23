#include "stdafx.h"
#include "utils.h"
#include "_hook.h"

////////////////////////////////////////////////////////////////////////////
const char* get_ini_file()
{
	static char sz_buf[MAX_PATH];
	if( !strlen( sz_buf) )
	{
		::GetModuleFileName( 0, sz_buf, sizeof(sz_buf) );
		strcpy( strrchr( sz_buf, '\\' ), _T("\\dump.ini") );
	}
	return sz_buf;
}

////////////////////////////////////////////////////////////////////////////
//startup state
bool get_startup_gather_state()
{
	const char* psz_ini = get_ini_file();
	bool b = ::GetPrivateProfileBool( SECTION_GENERAL, STARTUP_GATHER, false, psz_ini );
	::WritePrivateProfileBool( SECTION_GENERAL, STARTUP_GATHER, b, psz_ini );
	return b;
}

void set_startup_gather_state( bool bset )
{
	::WritePrivateProfileBool( SECTION_GENERAL, STARTUP_GATHER, bset, get_ini_file() );
}

////////////////////////////////////////////////////////////////////////////
//min alloc size
int get_min_alloc_size()
{
	const char* psz_ini = get_ini_file();
	int n = ::GetPrivateProfileInt( SECTION_GENERAL, MIN_ALLOC_SIZE, 0, psz_ini );
	::WritePrivateProfileInt( SECTION_GENERAL, MIN_ALLOC_SIZE, n, psz_ini );
	return n;
}

void set_min_alloc_size( int nsize )
{
	::WritePrivateProfileInt( SECTION_GENERAL, MIN_ALLOC_SIZE, nsize, get_ini_file() );
}

////////////////////////////////////////////////////////////////////////////
//group by stack
bool get_group_by_stack()
{
	const char* psz_ini = get_ini_file();
	bool b = ::GetPrivateProfileBool( SECTION_GENERAL, GROUP_BY_STACK, false, psz_ini );
	::WritePrivateProfileBool( SECTION_GENERAL, GROUP_BY_STACK, b, psz_ini );
	return b;
}

void set_group_by_stack( bool bset )
{
	::WritePrivateProfileBool( SECTION_GENERAL, GROUP_BY_STACK, bset, get_ini_file() );
}

////////////////////////////////////////////////////////////////////////////
//stack size
unsigned get_stack_size()
{
	const char* psz_ini = get_ini_file();
	int n = ::GetPrivateProfileInt( SECTION_GENERAL, STACK_SIZE, 20, psz_ini );	
	return n;
}

void set_stack_size( unsigned nsize )
{
	::WritePrivateProfileInt( SECTION_GENERAL, STACK_SIZE, nsize, get_ini_file() );
}

////////////////////////////////////////////////////////////////////////////
//group stack size
unsigned get_group_stack_depth()
{
	const char* psz_ini = get_ini_file();
	int n = ::GetPrivateProfileInt( SECTION_GENERAL, GROUP_STACK_DEPTH, 0, psz_ini );	
	return n;
}

void set_group_stack_depth( unsigned nsize )
{
	::WritePrivateProfileInt( SECTION_GENERAL, GROUP_STACK_DEPTH, nsize, get_ini_file() );
}

////////////////////////////////////////////////////////////////////////////
//time
time_t get_time( DWORD dw_tick, WORD* pmsec )
{
	DWORD dw_ticks = dw_tick - g_dw_start_tick;

	if( pmsec )
		*pmsec = WORD( dw_ticks - ( dw_ticks / 1000 ) * 1000 );

	return g_time_start + long( dw_ticks / 1000.);
}

////////////////////////////////////////////////////////////////////////////
//columns width
int	get_list_width( const char* psz_key, int ndef_width )
{
	const char* psz_ini = get_ini_file();
	int n = ::GetPrivateProfileInt( SECTION_LIST_WIDTH, psz_key, ndef_width, psz_ini );	
	return n;
}

void set_list_width( const char* psz_key, int nwidth )
{
	::WritePrivateProfileInt( SECTION_LIST_WIDTH, psz_key, nwidth, get_ini_file() );
}

////////////////////////////////////////////////////////////////////////////
//pdb path
void get_pdb_path( char* psz_path, size_t size )
{
	const char* psz_def = "F:\\Test\\dump_lib\\Debug\\pdb;E:\\msdev\\VC98\\Lib\\;E:\\msdev\\VC98\\MFC\\Lib;D:\\WINXP\\system32;E:\\Studio.NET\\Vc7\\lib;E:\\Studio.NET\\Vc7\\atlmfc\\lib";
	::GetPrivateProfileString( SECTION_GENERAL, PDB_PATH, psz_def, psz_path, size, get_ini_file() );	
}

void set_pdb_path( const char* psz_path )
{
	::WritePrivateProfileString( SECTION_GENERAL, PDB_PATH, psz_path, get_ini_file() );
}

////////////////////////////////////////////////////////////////////////////
//dump file
void get_dump_file( const char* psz_key, char* psz_file, size_t size )
{
	::GetPrivateProfileString( SECTION_DUMP_FILE, psz_key, "", psz_file, size, get_ini_file() );	
}

void set_dump_file( const char* psz_key, const char* psz_file )
{
	::WritePrivateProfileString( SECTION_DUMP_FILE, psz_key, psz_file, get_ini_file() );	
}

////////////////////////////////////////////////////////////////////////////
//error message
void dump_error( const char *psz_format... )
{
	char	sz_buffer[1024];

	va_list args;
	va_start(args, psz_format);
	::_vsntprintf( sz_buffer, sizeof( sz_buffer ), psz_format, args );
	va_end(args);

	::MessageBox( 0, sz_buffer, "Error", MB_APPLMODAL | MB_ICONERROR );
}

////////////////////////////////////////////////////////////////////////////
//src file
void get_src_path( char* psz_path, size_t size )
{
	::GetPrivateProfileString( SECTION_GENERAL, SRC_PATH, "", psz_path, size, get_ini_file() );	
	::WritePrivateProfileString( SECTION_GENERAL, SRC_PATH, psz_path, get_ini_file() );	
}

bool _get_install_hook( const char* psz_key, bool bdef = true )
{
	const char* psz_ini = get_ini_file();
	bool b = ::GetPrivateProfileBool( SECTION_HOOK, psz_key, bdef, psz_ini );
	::WritePrivateProfileBool( SECTION_HOOK, psz_key, b, psz_ini );
	return b;
}
////////////////////////////////////////////////////////////////////////////
//hook filter
bool get_install_bstr()
{	
	return _get_install_hook( BSTR_HOOK );
}

bool get_install_heap_alloc()
{
	return _get_install_hook( HEAP_HOOK );
}

bool get_install_virtual_heap_alloc()
{
	return _get_install_hook( VIRTUAL_HEAP_HOOK );
}

bool get_install_nt_allocate()
{
	return _get_install_hook( NT_ALLOCATE_HOOK, false );
}

//mfc & crt
bool get_install_new_mfc60()
{
	return _get_install_hook( NEW_MFC60_HOOK );
}

bool get_install_new_mfc70()
{
	return _get_install_hook( NEW_MFC70_HOOK );
}

bool get_install_malloc60()
{
	return _get_install_hook( MALLOC60_HOOK );
}

bool get_install_new_crt60()
{
	return _get_install_hook( NEW_CRT60_HOOK );
}

bool get_install_malloc70()
{
	return _get_install_hook( MALLOC70_HOOK );
}

bool get_install_new_crt70()
{
	return _get_install_hook( NEW_CRT70_HOOK );
}


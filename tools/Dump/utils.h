#pragma once


#define SECTION_GENERAL			"General"
#define STARTUP_GATHER			"StartupGather"
#define MIN_ALLOC_SIZE			"MinAllocSize"
#define GROUP_BY_STACK			"GroupByStack"
#define STACK_SIZE				"StackSize"
#define GROUP_STACK_DEPTH		"StackDepth"
#define PDB_PATH				"PdbPath"
#define SRC_PATH				"SrcPath"

#define SECTION_LIST_WIDTH		"ListWidth"

#define SECTION_DUMP_FILE		"DumpFile"
#define MEMORY_DUMP				"MemoryDump"
#define FILE1_DUMP				"File1Dump"
#define FILE2_DUMP				"File2Dump"

#define SECTION_WND_PLACE		"WindowPlacement"
#define WND_MEM_INFO			"MemInfo"
#define WND_SRC_BROWSE			"SrcBrowse"

#define SECTION_HOOK			"Hooks"
#define BSTR_HOOK				"InstallBSTR(0xFFFFFFFF)"
#define HEAP_HOOK				"InstallHeapAlloc"
#define VIRTUAL_HEAP_HOOK		"InstallVirtualHeapAlloc(0xEEEEEEEE)"
#define NT_ALLOCATE_HOOK		"InstallNtAllocate(0xDDDDDDDD)"

#define NEW_MFC60_HOOK			"InstallNewMfc60(0x99999999)"
#define NEW_MFC70_HOOK			"InstallNewMfc70(0x88888888)"
#define MALLOC60_HOOK			"InstallMalloc60(0x77777777)"
#define NEW_CRT60_HOOK			"InstallNewCrt60(0x66666666)"
#define MALLOC70_HOOK			"InstallMalloc70(0x55555555)"
#define NEW_CRT70_HOOK			"InstallNewCrt70(0x44444444)"

////////////////////////////////////////////////////////////////////////////
const char* get_ini_file();

////////////////////////////////////////////////////////////////////////////
//startup state
bool get_startup_gather_state();
void set_startup_gather_state( bool bset );

////////////////////////////////////////////////////////////////////////////
//min alloc size
int get_min_alloc_size();
void set_min_alloc_size( int nsize );

////////////////////////////////////////////////////////////////////////////
//group by stack
bool get_group_by_stack();
void set_group_by_stack( bool bset );

////////////////////////////////////////////////////////////////////////////
//stack size
unsigned get_stack_size();
void set_stack_size( unsigned nsize );

////////////////////////////////////////////////////////////////////////////
//group stack size
unsigned get_group_stack_depth();
void set_group_stack_depth( unsigned nsize );

////////////////////////////////////////////////////////////////////////////
//time
time_t	get_time( DWORD dw_tick, WORD* pmsec = 0 );


////////////////////////////////////////////////////////////////////////////
//columns width
int	get_list_width( const char* psz_key, int ndef_width );
void set_list_width( const char* psz_key, int nwidth );

////////////////////////////////////////////////////////////////////////////
//pdb path
void get_pdb_path( char* psz_path, size_t size );
void set_pdb_path( const char* psz_path );

////////////////////////////////////////////////////////////////////////////
//dump file
void get_dump_file( const char* psz_key, char* psz_file, size_t size );
void set_dump_file( const char* psz_key, const char* psz_file );

////////////////////////////////////////////////////////////////////////////
//error message
void dump_error( const char *psz_format... );

////////////////////////////////////////////////////////////////////////////
//src file
void get_src_path( char* psz_path, size_t size );

////////////////////////////////////////////////////////////////////////////
//hook filter
bool get_install_bstr();
bool get_install_heap_alloc();
bool get_install_virtual_heap_alloc();
bool get_install_nt_allocate();
//mfc & crt
bool get_install_new_mfc60();
bool get_install_new_mfc70();
bool get_install_malloc60();
bool get_install_new_crt60();
bool get_install_malloc70();
bool get_install_new_crt70();


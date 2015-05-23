#pragma once

#include "_types.h"

#include "misc_map.h"

//Memory identificators
#define MI_BSTR			((HANDLE)0xFFFFFFFF)
#define MI_VIRTUAL		((HANDLE)0xEEEEEEEE)
#define MI_NT_VIRTUAL	((HANDLE)0xDDDDDDDD)

#define MI_NEW_MFC60	((HANDLE)0x99999999)
#define MI_NEW_MFC70	((HANDLE)0x88888888)
#define MI_MALLOC60		((HANDLE)0x77777777)
#define MI_NEW_CRT60	((HANDLE)0x66666666)
#define MI_MALLOC70		((HANDLE)0x55555555)
#define MI_NEW_CRT70	((HANDLE)0x44444444)

//start time
extern DWORD g_dw_start_tick;
extern time_t g_time_start;

////////////////////////////////////////////////////////////////////////////
//Memory map
extern long g_block_mem_hook;
extern bool g_bRunMemTrace;

extern __declspec(dllexport) CRITICAL_SECTION g_cs_map;
struct MapHeapInfo:_list_map_t<heap_info*,long, cmp_long, heap_info::free>
{
	MapHeapInfo(){::InitializeCriticalSection(&g_cs_map);}
	~MapHeapInfo(){::DeleteCriticalSection(&g_cs_map);}
};
extern MapHeapInfo g_map_heap;
bool add_mem( HANDLE hheap, void* pmem, DWORD dw_size );
bool remove_mem( void* pmem );

///////////////////////////////////////////////////////////////////////////
bool install_mem_hook();
bool dump_mem_hook();
void deinstall_mem_hook();
EXTERN_C __declspec( dllexport ) void hook_options( HWND hwnd_main );

////////////////////////////////////////////////////////////////////////////
//BSTR
bool install_string_hook();
void deinstall_string_hook();

////////////////////////////////////////////////////////////////////////////
//crt
bool install_crt_hook();
void deinstall_crt_hook();


//Stack
#define MAX_STACK_SIZE	100
extern unsigned g_nmax_stack_size;
bool get_stack( unsigned* parr_stack, size_t size_in, size_t* psize_out = 0 );

//PDB engine
class pdb_engine;
extern pdb_engine g_pdb_engine;

////////////////////////////////////////////////////////////////////////////
//Share entry
bool install_hook( DWORD dw_flags );
bool dump_hook( DWORD dw_flags );
void deinstall_hook( DWORD dw_flags );

/*Use this class to temporary lock memory gather
typedef  long (*PF_SET)( long );

class mem_lock
{
public:
	mem_lock( long llock_mem )
	{
		m_lprev = 0;

		m_hmod = GetModuleHandle("dump.dll");
		if( m_hmod )
			m_pfunc_set = (PF_SET)GetProcAddress( m_hmod, "set_lock_mem_hook"); 
		if( m_pfunc_set )
			m_lprev = m_pfunc_set( llock_mem );

	}
	virtual ~mem_lock()
	{
		if( m_pfunc_set )
			m_pfunc_set( m_lprev );

	}
	HMODULE	m_hmod;
	PF_SET	m_pfunc_set;
	long	m_lprev;	
};
*/
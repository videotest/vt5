#include "stdafx.h"
#include "_hook.h"
#include "_pdb.h"
#include "dlg.h"
#include "dump.h"
//#include "\vt5\awin\profiler.h"


unsigned g_nmax_stack_size = 20;
unsigned g_stack[MAX_STACK_SIZE];
////////////////////////////////////////////////////////////////////////////
//Memory map
bool add_mem( HANDLE hheap, void* pmem, DWORD dw_size )
{
//	PROFILE_TEST( "add_mem" );
	
	enter_cc cc( &g_cs_map );
	lock l( &g_block_mem_hook, true );

	size_t size_stack = 0;
	
	{
//		PROFILE_TEST( "add_mem::get_stack" );
		get_stack( g_stack, g_nmax_stack_size, &size_stack );
	}

	heap_info* phi	= new heap_info;
	phi->m_hheap	= hheap;
	phi->m_laddress = (long)pmem;
	phi->m_dw_size	= dw_size;
	{
//		PROFILE_TEST( "add_mem::set" );
		g_map_heap.set( phi, phi->m_laddress );
	}

	if( size_stack )
	{
		phi->m_pstack = new unsigned[size_stack];
		memcpy( phi->m_pstack, g_stack, size_stack*sizeof(unsigned) );
		phi->m_size_stack = size_stack;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////
bool remove_mem( void* pmem )
{
//	PROFILE_TEST( "remove_mem" );

	enter_cc cc( &g_cs_map );
	lock l( &g_block_mem_hook, true );

	long lpos = 0;
	{
//		PROFILE_TEST( "remove_mem.find" );
		lpos = g_map_heap.find( (long)pmem );
	}
	if( lpos )
	{
//		PROFILE_TEST( "remove_mem.remove" );
		g_map_heap.remove( lpos );
	}

	return true;
}


//global PDB engine
pdb_engine g_pdb_engine;

////////////////////////////////////////////////////////////////////////////
//Share entry
bool install_hook( DWORD dw_flags )
{
	ProcessAttach(GetModuleHandle(NULL));
	g_pdb_engine.init();
	install_mem_hook();
	//install_crt_hook();
	//install_string_hook();	
	return true;
}

////////////////////////////////////////////////////////////////////////////
bool dump_hook( DWORD dw_flags )
{
	dump_mem_hook();
	return true;
}

////////////////////////////////////////////////////////////////////////////
void deinstall_hook( DWORD dw_flags )
{
	//deinstall_string_hook();
	//deinstall_crt_hook();
	deinstall_mem_hook();
	g_pdb_engine.deinit();
	ProcessDetach();
}

////////////////////////////////////////////////////////////////////////////
//extern poins
EXTERN_C __declspec( dllexport ) void hook_options( HWND hwnd_main )
{
	main_dlg dlg;	
	dlg.do_modal( hwnd_main );
}

////////////////////////////////////////////////////////////////////////////
EXTERN_C __declspec( dllexport ) long get_lock_mem_hook( )
{
	return g_block_mem_hook;
}

////////////////////////////////////////////////////////////////////////////
EXTERN_C __declspec( dllexport ) long set_lock_mem_hook( long block )
{
	long bold = g_block_mem_hook;
	InterlockedExchange( &g_block_mem_hook, block );
	
	return bold;
}


#include "stdafx.h"
#include "_pdb.h"
#include "_hook.h"
#include "utils.h"

#include <imagehlp.h>

HMODULE	__module_by_address( unsigned addr_ask );
const char *__module_fname( HMODULE h, char *psz, size_t cb );

////////////////////////////////////////////////////////////////////////////
pdb_engine::pdb_engine()
{
	m_hprocess = 0;	
}

////////////////////////////////////////////////////////////////////////////
pdb_engine::~pdb_engine()
{
	deinit();	
}


////////////////////////////////////////////////////////////////////////////
bool pdb_engine::init()
{
	lock l( &g_block_mem_hook, true );

	char sz_buf[32000];	sz_buf[0] = 0;
	get_pdb_path( sz_buf, sizeof(sz_buf) );
	set_pdb_path( sz_buf );
	m_hprocess = ::GetCurrentProcess();
	return ( TRUE == ::SymInitialize( m_hprocess, strlen(sz_buf) ? sz_buf : 0, true ) );

}

////////////////////////////////////////////////////////////////////////////
void pdb_engine::deinit()
{
	lock l( &g_block_mem_hook, true );

	if( m_hprocess )
		::SymCleanup ( m_hprocess );
	m_hprocess = 0;

	m_map.clear();
}


////////////////////////////////////////////////////////////////////////////
line_info* pdb_engine::get_line_info( DWORD dw_address )
{
	lock l( &g_block_mem_hook, true );

	long lpos = m_map.find( dw_address );
	if( lpos )
		return m_map.get( lpos );


	line_info* pli	= new line_info;
	pli->m_laddress	= dw_address;
	m_map.set( pli, dw_address );

	::SetLastError( 0 );
	//get module
	IMAGEHLP_MODULE module = {0};
	BOOL bres = ::SymGetModuleInfo( m_hprocess, dw_address, &module );
	if( bres && module.ImageName[0] )
		pli->m_str_image_file_name = module.ModuleName;//ImageName;
	
	//get ra & module name
	char sz_module[MAX_PATH];	sz_module[0] = 0;
	HMODULE hmod = __module_by_address( dw_address );
	if( hmod )
		__module_fname( hmod, sz_module, sizeof(sz_module) );

	if( !pli->m_str_image_file_name.length() )
	{			
		char* psz = strrchr( sz_module, '\\' );
		pli->m_str_image_file_name = psz ? psz + 1: sz_module;

		::SymLoadModule( m_hprocess, 0, (char*)pli->m_str_image_file_name, 0, (DWORD)hmod, 0 );
	}

	pli->m_lra = dw_address - (DWORD)hmod;


	//get function
	IMAGEHLP_SYMBOL* psymbol = (IMAGEHLP_SYMBOL*) new BYTE[sizeof(IMAGEHLP_SYMBOL) + MAX_PATH];
	::ZeroMemory( psymbol, sizeof(IMAGEHLP_SYMBOL) + MAX_PATH );
	psymbol->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL);
	psymbol->MaxNameLength = MAX_PATH;	

	DWORD dw_out = 0;
	bres = ::SymGetSymFromAddr( m_hprocess, dw_address, &dw_out, psymbol );
	HRESULT hr = ::GetLastError( );
	if( bres && psymbol->Name[0] )
		pli->m_str_src_function = psymbol->Name;
	delete psymbol;	psymbol = 0;

	
	//get line information	
	dw_out = 0;
	IMAGEHLP_LINE il = {0};
	il.SizeOfStruct = sizeof(il);
	DWORD dwTempDis = 0;

	/*while( FALSE == ( bres = ::SymGetLineFromAddr( m_hprocess, dw_address - dwTempDis, &dw_out, &il ) ) )
	{
		dwTempDis += 1;
		if ( 100 == dwTempDis )
			break;
	}*/
	bres = ::SymGetLineFromAddr( m_hprocess, dw_address, &dw_out, &il );
	if( bres )
	{
		pli->m_lsrc_line = il.LineNumber;
		if( il.FileName[0] )
			pli->m_str_src_file_name	= il.FileName;
	}	

	return pli;
}

////////////////////////////////////////////////////////////////////////////
bool pdb_engine::get_stack( unsigned* parr_stack, size_t size_in, size_t* psize_out )
{
	lock l( &g_block_mem_hook, true );

	if( !parr_stack || !size_in)	return false;

	// force undecorated names to get params
	DWORD dw = SymGetOptions();
	dw &= ~SYMOPT_UNDNAME;
	SymSetOptions(dw);

	HANDLE hThread = ::GetCurrentThread();
	CONTEXT threadContext;

	threadContext.ContextFlags = CONTEXT_FULL;

	if (::GetThreadContext(hThread, &threadContext))
	{
		STACKFRAME stackFrame;
		memset(&stackFrame, 0, sizeof(stackFrame));
		stackFrame.AddrPC.Mode = AddrModeFlat;

		DWORD dwMachType = IMAGE_FILE_MACHINE_I386;

		// program counter, stack pointer, and frame pointer
		stackFrame.AddrPC.Offset    = threadContext.Eip;
		stackFrame.AddrStack.Offset = threadContext.Esp;
		stackFrame.AddrStack.Mode   = AddrModeFlat;
		stackFrame.AddrFrame.Offset = threadContext.Ebp;
		stackFrame.AddrFrame.Mode   = AddrModeFlat;

		int nFrame;
		for (nFrame = 0; nFrame < 1024; nFrame++)
		{
			if (!StackWalk(dwMachType, m_hprocess, hThread,
				&stackFrame, &threadContext, NULL,
				/*FunctionTableAccess*/0, /*GetModuleBase*/0, NULL))
			{
				break;
			}

			if( (unsigned)nFrame >= size_in )	break;
			parr_stack[nFrame] = stackFrame.AddrPC.Offset;
		}
		if( psize_out )
			*psize_out = nFrame;
	}

	return true;
}

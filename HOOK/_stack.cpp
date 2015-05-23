#include "stdafx.h"
#include "_hook.h"

struct	__stack_frame
{
	__stack_frame	*prev;
	unsigned		ret_address;

	unsigned		arg1;
	unsigned		arg2;
	unsigned		arg3;
	unsigned		arg4;
};

_map_t<bool,unsigned,cmp_unsigned>	g_map_addr;

bool is_address_available( unsigned addr, long* plpos = 0 )
{
	MEMORY_BASIC_INFORMATION	mem_info;
	ZeroMemory( &mem_info, sizeof( mem_info ) );	

	long lbase = ( addr & ~0x0FFF );

	long lpos = g_map_addr.find( addr );
	if( lpos )
	{
		bool b = g_map_addr.get( lpos );
		return b;
	}
	else
	{
		VirtualQuery( (void*)(lbase), &mem_info, sizeof( mem_info ) );

		bool b = ( mem_info.Protect == PAGE_READWRITE ||
			mem_info.Protect == PAGE_EXECUTE_READWRITE || 
			mem_info.Protect == PAGE_EXECUTE_READ ||
			mem_info.Protect == PAGE_EXECUTE || 
			mem_info.Protect == PAGE_EXECUTE_WRITECOPY );

		g_map_addr.set( b, addr );
		return b;
	}
} 

long __zero_if_frame_valid2( __stack_frame *pframe )
{
	if( g_map_addr.count() >= 10000 )
		g_map_addr.clear();

	if( !is_address_available( (unsigned)pframe ) )
		return 4;

	if( !pframe )return 1;
	if( !pframe->ret_address )return 2;

	if( !is_address_available( (unsigned)(pframe->ret_address-6) ) )
		return 4;

	if( *((BYTE*)(pframe->ret_address-5))!=0xe8 &&		//call_
		*((BYTE*)(pframe->ret_address-2))!=0xe8 &&		//call [ebx]
		*((BYTE*)(pframe->ret_address-2))!=0xFF &&		//call esp//paul
		*((BYTE*)(pframe->ret_address-3))!=0xFF &&	//call [ebp]
		*((BYTE*)(pframe->ret_address-6))!=0xFF 	//call dword ptr []
		)
	{
		g_map_addr.set( false, pframe->ret_address );
		return 3;
	}
	return 0;
}

long __zero_if_frame_valid( __stack_frame *pframe )
{
	MEMORY_BASIC_INFORMATION	mem_info;
	ZeroMemory( &mem_info, sizeof( mem_info ) );	

	BOOL bres = VirtualQuery( (void*)((unsigned)pframe&~0x0FFF), &mem_info, sizeof( mem_info ) );
	//if( !bres )	return -1;

	if( !( mem_info.Protect == PAGE_READWRITE ||
		mem_info.Protect == PAGE_EXECUTE_READWRITE || 
		mem_info.Protect == PAGE_EXECUTE_READ ||
		mem_info.Protect == PAGE_EXECUTE || 
		mem_info.Protect == PAGE_EXECUTE_WRITECOPY ) )
		return 4;

//	if( IsBadReadPtr( pframe, 16 ) )
//		return 4;

	if( !pframe )return 1;
	if( !pframe->ret_address )return 2;

	ZeroMemory( &mem_info, sizeof( mem_info ) );
	bres = VirtualQuery( (void*)((unsigned)(pframe->ret_address-6)&~0x0FFF), &mem_info, sizeof( mem_info ) );
	//if( !bres )	return -1;

	if( !( mem_info.Protect == PAGE_READWRITE ||
		mem_info.Protect == PAGE_EXECUTE_READWRITE || 
		mem_info.Protect == PAGE_EXECUTE_READ ||
		mem_info.Protect == PAGE_EXECUTE ||
		mem_info.Protect == PAGE_EXECUTE_WRITECOPY ) )
		return 4;

//	if( IsBadReadPtr( (const void*)(pframe->ret_address-6), 16 ) )
//		return 4;


	if( *((BYTE*)(pframe->ret_address-5))!=0xe8 &&		//call_
		*((BYTE*)(pframe->ret_address-2))!=0xe8 &&		//call [ebx]
		*((BYTE*)(pframe->ret_address-2))!=0xFF &&		//call esp//paul
		*((BYTE*)(pframe->ret_address-3))!=0xFF &&	//call [ebp]
		*((BYTE*)(pframe->ret_address-6))!=0xFF 	//call dword ptr []
		)
		return 3;
	return 0;
}


HMODULE	__module_by_address( unsigned addr_ask )
{
	if( !addr_ask )	return 0;
	unsigned	error_code = 1, addr = addr_ask;
	MEMORY_BASIC_INFORMATION	mem_info;
	ZeroMemory( &mem_info, sizeof( mem_info ) );

	//модули выравнены по 4 кб.
	addr &= ~0x0FFF;//0x7ff;

	while( addr )
	{

		VirtualQuery( (void*)((unsigned)addr&~0x0FFF), &mem_info, sizeof( mem_info ) );

		if( mem_info.Protect != PAGE_READONLY &&
			mem_info.Protect != PAGE_READWRITE &&
			mem_info.Protect != PAGE_EXECUTE_READ &&
			mem_info.Protect != PAGE_EXECUTE_READWRITE )
		{
			addr -= 0x800;
			continue;
		}

		if( *(WORD*)addr == MAKEWORD('M','Z') )
		{
			error_code = 0;
			break;
		}
		addr -= 0x800;
	}

	//if( error_code )
		//_trace_file( 0, "[module_by_addr] handle not found/%08X", addr_ask );

	return (HMODULE)addr;
}

const char *__module_fname( HMODULE h, char *psz, size_t cb )
{
	unsigned	error_code = 0;

	if( !cb )return 0;
	if( !psz )return 0;

	*psz = 0;

	if( ::GetModuleFileName( h, psz, cb ) )
	{
		_tcsupr( psz );

		char	*psz_e = _tcsrchr( psz, '\\' );
		if( psz_e )return psz_e+1;
		return psz;
	}

	return psz;
}

const char *get_module_fname( unsigned addr_ask, char *psz, size_t cb )
{
	HMODULE	h = __module_by_address( addr_ask );
	if( !h )	return 0;

	return __module_fname( h, psz, cb );
}


////////////////////////////////////////////////////////////////////////////
bool get_stack( unsigned* parr_stack, size_t size_in, size_t* psize_out )
{
	//lock l( &g_block_mem_hook, true );

	if( !parr_stack || !size_in)	return false;

	unsigned stack_addr;
	_asm mov stack_addr, ebp;

	__stack_frame	*pstack = (__stack_frame*)stack_addr;
	long lret = 0;
	size_t idx = 0;
	
	while( !(lret = __zero_if_frame_valid2( pstack )) )
	{
		//HMODULE hmod = _module_by_address( pstack->ret_address );
		if( idx >= size_in )	break;
		parr_stack[idx] = pstack->ret_address;
		idx++;
		pstack = pstack->prev;

		/*if( hmod )
			_trace_file( psz_log, "%s+%08x\t(%08x)\t %08X %08X %08X %08X", _module_fname( hmod, sz, sizeof(sz)), pstack->ret_address-(unsigned)hmod, pstack->ret_address, pstack->arg1, pstack->arg2, pstack->arg3, pstack->arg4 );
		else
			_trace_file( psz_log, "%08x\t %08X %08X %08X %08X", pstack->ret_address, pstack->arg1, pstack->arg2, pstack->arg3, pstack->arg4 );
			*/		
	}

	if( psize_out )
		*psize_out = idx;

	return true;	
}
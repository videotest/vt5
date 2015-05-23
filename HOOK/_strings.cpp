#include "stdafx.h"
#include "_hook.h"
#include "_pdb.h"
#include "utils.h"

DETOUR_TRAMPOLINE(BSTR WINAPI Real_SysAllocString( const OLECHAR * ), SysAllocString );
DETOUR_TRAMPOLINE(BSTR WINAPI Real_SysAllocStringLen(const OLECHAR * c, UINT n ), SysAllocStringLen );
DETOUR_TRAMPOLINE(BSTR WINAPI Real_SysAllocStringByteLen(LPCSTR psz, UINT len ), SysAllocStringByteLen );
DETOUR_TRAMPOLINE(void WINAPI Real_SysFreeString( BSTR bstr ), SysFreeString );

////////////////////////////////////////////////////////////////////////////
BSTR __stdcall _SysAllocString(const OLECHAR *c )
{
	BSTR bstr = 0;
	{
		lock l( &g_block_mem_hook, true );
		bstr = Real_SysAllocString(c);
	}
	
	if( !g_block_mem_hook )
	{		
		add_mem( MI_BSTR, bstr, 2 * ( ::SysStringLen( bstr ) + 1 ) );
	}

	return bstr;
}

////////////////////////////////////////////////////////////////////////////
BSTR __stdcall _SysAllocStringLen(const OLECHAR * c, UINT n )
{
	BSTR bstr = 0;
	{
		lock l( &g_block_mem_hook, true );
		bstr = Real_SysAllocStringLen(c, n );	
	}

	if( !g_block_mem_hook )
		add_mem( MI_BSTR, bstr, n + 2 );

	return bstr;	
}

////////////////////////////////////////////////////////////////////////////
BSTR __stdcall _SysAllocStringByteLen(LPCSTR psz, UINT len )
{
	BSTR bstr = 0;
	{
		lock l( &g_block_mem_hook, true );
		bstr = Real_SysAllocStringByteLen(psz, len );	
	}

	if( !g_block_mem_hook )
		add_mem( MI_BSTR, bstr, len + 2 );

	return bstr;	
}

////////////////////////////////////////////////////////////////////////////
void __stdcall _SysFreeString( BSTR bstr )
{	
	//if( !g_block_mem_hook )
	remove_mem( bstr );

	{
		lock l( &g_block_mem_hook, true );
		Real_SysFreeString( bstr );
	}
}

////////////////////////////////////////////////////////////////////////////
bool install_string_hook()
{
	if( get_install_bstr() )
	{
		DetourFunctionWithTrampoline( (PBYTE)Real_SysAllocString, (PBYTE)_SysAllocString );
		DetourFunctionWithTrampoline( (PBYTE)Real_SysAllocStringLen, (PBYTE)_SysAllocStringLen );
		DetourFunctionWithTrampoline( (PBYTE)Real_SysAllocStringByteLen, (PBYTE)_SysAllocStringByteLen );
		DetourFunctionWithTrampoline( (PBYTE)Real_SysFreeString, (PBYTE)_SysFreeString );
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////
void deinstall_string_hook()
{
	DetourRemove( (PBYTE)Real_SysAllocString, (PBYTE)_SysAllocString );
	DetourRemove( (PBYTE)Real_SysAllocStringLen, (PBYTE)_SysAllocStringLen );
	DetourRemove( (PBYTE)Real_SysAllocStringByteLen, (PBYTE)_SysAllocStringByteLen );
	DetourRemove( (PBYTE)Real_SysFreeString, (PBYTE)_SysFreeString );
}
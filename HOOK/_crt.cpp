#include "stdafx.h"
#include "_mem.h"
#include "_hook.h"
#include "_pdb.h"
#include "utils.h"

//MFC
typedef void* (__cdecl *PFN_NEW_MFC)( size_t nSize );
PFN_NEW_MFC pfn_New60_mfc = 0;
PFN_NEW_MFC pfn_RealNew60_mfc = 0;

PFN_NEW_MFC pfn_New70_mfc = 0;
PFN_NEW_MFC pfn_RealNew70_mfc = 0;

//crt new
typedef void* (__cdecl* PFN_NEW_CRT) (size_t);
PFN_NEW_CRT pfn_New60_crt = 0;
PFN_NEW_CRT pfn_RealNew60_crt = 0;

PFN_NEW_CRT pfn_New70_crt = 0;
PFN_NEW_CRT pfn_RealNew70_crt = 0;

//crt malloc
typedef void* (__cdecl* PFN_MALLOC) (size_t);
PFN_MALLOC pfn_Malloc60 = 0;
PFN_MALLOC pfn_RealMalloc60 = 0;

PFN_MALLOC pfn_Malloc70 = 0;
PFN_MALLOC pfn_RealMalloc70 = 0;

////////////////////////////////////////////////////////////////////////////
void* __cdecl _new60_mfc( size_t nSize )
{
	void* pres = 0;
	{
		lock l( &g_block_mem_hook, true );
		pres = pfn_RealNew60_mfc( nSize );
	}
	if( !g_block_mem_hook )
	{
		add_mem( MI_NEW_MFC60, pres, nSize );
	}

	return pres;
}

////////////////////////////////////////////////////////////////////////////
void* __cdecl _new70_mfc( size_t nSize )
{
	void* pres = 0;
	{
		lock l( &g_block_mem_hook, true );
		pres = pfn_RealNew70_mfc( nSize );
	}
	if( !g_block_mem_hook )
	{
		add_mem( MI_NEW_MFC70, pres, nSize );
	}

	return pres;
}

////////////////////////////////////////////////////////////////////////////
void* __cdecl _malloc60( size_t nSize )
{
	void* pres = 0;
	{
		lock l( &g_block_mem_hook, true );
		pres = pfn_RealMalloc60( nSize );
	}
	if( !g_block_mem_hook )
	{
		add_mem( MI_MALLOC60, pres, nSize );
	}

	return pres;
}

////////////////////////////////////////////////////////////////////////////
void* __cdecl _new60_crt( size_t nSize )
{
	void* pres = 0;
	{
		lock l( &g_block_mem_hook, true );
		pres = pfn_RealNew60_crt( nSize );
	}
	if( !g_block_mem_hook )
	{
		add_mem( MI_NEW_CRT60, pres, nSize );
	}

	return pres;
}

////////////////////////////////////////////////////////////////////////////
void* __cdecl _malloc70( size_t nSize )
{
	void* pres = 0;
	{
		lock l( &g_block_mem_hook, true );
		pres = pfn_RealMalloc70( nSize );
	}
	if( !g_block_mem_hook )
	{
		add_mem( MI_MALLOC70, pres, nSize );
	}

	return pres;
}

////////////////////////////////////////////////////////////////////////////
void* __cdecl _new70_crt( size_t nSize )
{
	void* pres = 0;
	{
		lock l( &g_block_mem_hook, true );
		pres = pfn_RealNew70_crt( nSize );
	}
	if( !g_block_mem_hook )
	{
		add_mem( MI_NEW_CRT70, pres, nSize );
	}

	return pres;
}

////////////////////////////////////////////////////////////////////////////
//crt
bool install_crt_hook()
{
	HMODULE hmod_mfc60 = ::GetModuleHandle( "mfc42.dll" );
	if( hmod_mfc60 )
	{
		if( get_install_new_mfc60() )
		{
			pfn_New60_mfc = (PFN_NEW_MFC)::GetProcAddress( hmod_mfc60, (LPCSTR)823 );
			if( pfn_New60_mfc )
				pfn_RealNew60_mfc = (PFN_NEW_MFC)DetourFunction( (PBYTE)pfn_New60_mfc, (PBYTE)_new60_mfc );
		}
	}

	HMODULE hmod_mfc70 = ::GetModuleHandle( "mfc70.dll" );
	if( hmod_mfc70 )
	{
		if( get_install_new_mfc70() )
		{
			pfn_New70_mfc = (PFN_NEW_MFC)::GetProcAddress( hmod_mfc70, (LPCSTR)703 );
			if( pfn_New70_mfc )
				pfn_RealNew70_mfc = (PFN_NEW_MFC)DetourFunction( (PBYTE)pfn_New70_mfc, (PBYTE)_new70_mfc );		
		}
	}

	HMODULE hmod_crt60 = ::GetModuleHandle( "msvcrt.dll" );
	if( hmod_crt60 )
	{
		if( get_install_new_crt60() )
		{
			pfn_New60_crt = (PFN_MALLOC)::GetProcAddress( hmod_crt60, (LPCSTR)17 );
			if( pfn_New60_crt )
				pfn_RealNew60_crt = (PFN_MALLOC)DetourFunction( (PBYTE)pfn_New60_crt, (PBYTE)_new60_crt );		
		}

		if( get_install_malloc60() )
		{
			pfn_Malloc60 = (PFN_MALLOC)::GetProcAddress( hmod_crt60, (LPCSTR)"malloc" );
			if( pfn_Malloc60 )
				pfn_RealMalloc60 = (PFN_MALLOC)DetourFunction( (PBYTE)pfn_Malloc60, (PBYTE)_malloc60 );		
		}
	}

	HMODULE hmod_crt70 = ::GetModuleHandle( "msvcr70.dll" );
	if( hmod_crt70 )
	{
		if( get_install_new_crt70() )
		{
			pfn_New70_crt = (PFN_MALLOC)::GetProcAddress( hmod_crt70, (LPCSTR)17 );
			if( pfn_New70_crt )
				pfn_RealNew70_crt = (PFN_MALLOC)DetourFunction( (PBYTE)pfn_New70_crt, (PBYTE)_new70_crt );		
		}

		if( get_install_malloc70() )
		{
			pfn_Malloc70 = (PFN_MALLOC)::GetProcAddress( hmod_crt70, (LPCSTR)"malloc" );
			if( pfn_Malloc70 )
				pfn_RealMalloc70 = (PFN_MALLOC)DetourFunction( (PBYTE)pfn_Malloc70, (PBYTE)_malloc70 );		
		}
	}

	return true;	
}

////////////////////////////////////////////////////////////////////////////
void deinstall_crt_hook()
{
	if( pfn_RealNew60_mfc )
		DetourRemove( (PBYTE)pfn_RealNew60_mfc, (PBYTE)_new60_mfc );

	if( pfn_RealNew70_mfc )
		DetourRemove( (PBYTE)pfn_RealNew70_mfc, (PBYTE)_new70_mfc );

	if( pfn_RealNew60_crt )
		DetourRemove( (PBYTE)pfn_RealNew60_crt, (PBYTE)_new60_crt );

	if( pfn_RealMalloc60 )
		DetourRemove( (PBYTE)pfn_RealMalloc60, (PBYTE)_malloc60 );

	if( pfn_RealNew70_crt )
		DetourRemove( (PBYTE)pfn_RealNew70_crt, (PBYTE)_new70_crt );

	if( pfn_RealMalloc70 )
		DetourRemove( (PBYTE)pfn_RealMalloc70, (PBYTE)_malloc70 );
}

#include "stdafx.h"
#include "_mem.h"
#include "_hook.h"
#include "_pdb.h"
#include "utils.h"
#include "APIHook.h"

namespace{
	DWORD Index_HeapAllocRecursion=TlsAlloc();
	BOOL bGetRecursion(){return (BOOL)TlsGetValue(Index_HeapAllocRecursion);}
	void SetRecursion(BOOL b){
		TlsSetValue(Index_HeapAllocRecursion,(LPVOID)b);
	}
}
////////////////////////////////////////////////////////////////////////////
#define HOOKMACRO(DLL, FUNC_RET_TYPE, FUNC_NAME, FUNC_PARAMS) \
	typedef FUNC_RET_TYPE FN_##FUNC_NAME FUNC_PARAMS; \
	FN_##FUNC_NAME _##FUNC_NAME; \
	typedef FN_##FUNC_NAME* PFN_##FUNC_NAME; \
	CAPIHook g_##FUNC_NAME(DLL, #FUNC_NAME, (PROC)_##FUNC_NAME, TRUE); \
	FUNC_RET_TYPE _##FUNC_NAME FUNC_PARAMS
////////////////////////////////////////////////////////////////////////////
////heap alloc
#if 1
////////////////////////////////////////////////////////////////////////////
typedef LPVOID(__stdcall *PFN_HEAPALLOC)(IN HANDLE hHeap, IN DWORD dwFlags, IN SIZE_T dwBytes);
extern CAPIHook g_HeapAlloc;

LPVOID __stdcall _HeapAlloc( IN HANDLE hHeap, IN DWORD dwFlags, IN SIZE_T dwBytes )
{
	void* p = 0;
	{
		p = ((PFN_HEAPALLOC)(PROC)g_HeapAlloc)( hHeap, dwFlags, dwBytes );
	}
	BOOL b_HeapAllocRecursion=bGetRecursion();
	if(g_bRunMemTrace && !b_HeapAllocRecursion)
	{
		SetRecursion(TRUE);
		add_mem( hHeap, p, dwBytes );
		SetRecursion(FALSE);
	}

	return p;
}
CAPIHook g_HeapAlloc("Kernel32.dll", "HeapAlloc", (PROC)_HeapAlloc, TRUE);
CAPIHook g_RtlAllocateHeap("NTDLL.DLL", "RtlAllocateHeap", (PROC)_HeapAlloc, TRUE);

////////////////////////////////////////////////////////////////////////////
typedef LPVOID(__stdcall *PFN_HEAPREALLOC)(IN HANDLE hHeap, IN DWORD dwFlags, IN LPVOID lpMem, IN SIZE_T dwBytes);
extern CAPIHook g_HeapReAlloc;

LPVOID __stdcall _HeapReAlloc( IN HANDLE hHeap, IN DWORD dwFlags, IN LPVOID lpMem, IN SIZE_T dwBytes )
{
	BOOL b_HeapAllocRecursion=bGetRecursion();
	if(!b_HeapAllocRecursion){
		SetRecursion(TRUE);
	remove_mem( lpMem );
		SetRecursion(FALSE);
	}
	void* p = ((PFN_HEAPREALLOC)(PROC)g_HeapReAlloc)( hHeap, dwFlags, lpMem, dwBytes );	
	if(g_bRunMemTrace && !b_HeapAllocRecursion)
	{		
		SetRecursion(TRUE);
		add_mem( hHeap, p, dwBytes );
		SetRecursion(FALSE);
	}

	return p;
}
CAPIHook g_HeapReAlloc("Kernel32.dll", "HeapReAlloc", (PROC)_HeapReAlloc, TRUE);
CAPIHook g_RtlReAllocateHeap("NTDLL.DLL", "RtlReAllocateHeap", (PROC)_HeapReAlloc, TRUE);
////////////////////////////////////////////////////////////////////////////
typedef BOOL(__stdcall *PFN_HeapFree)(IN HANDLE hHeap, IN DWORD dwFlags, IN LPVOID lpMem);
extern CAPIHook g_HeapFree;
BOOL __stdcall _HeapFree( IN HANDLE hHeap, IN DWORD dwFlags, IN LPVOID lpMem )
{
	BOOL b_HeapAllocRecursion=bGetRecursion();
	if(!b_HeapAllocRecursion){
		SetRecursion(TRUE);
	remove_mem( lpMem );
		SetRecursion(FALSE);
	}
	BOOL b = ((PFN_HeapFree)(PROC)g_HeapFree)( hHeap, dwFlags, lpMem );
	return b;
}
CAPIHook g_HeapFree("Kernel32.dll", "HeapFree", (PROC)_HeapFree, TRUE);
CAPIHook g_RtlFreeHeap("NTDLL.DLL", "RtlFreeHeap", (PROC)_HeapFree, TRUE);
////////////////////////////////////////////////////////////////////////////
typedef void __stdcall FN_SysFreeString (BSTR bstr);
FN_SysFreeString _SysFreeString;
typedef FN_SysFreeString* PFN_SysFreeString;
CAPIHook g_SysFreeString("OLEAUT32.DLL", "SysFreeString", (PROC)_SysFreeString, TRUE);
void __stdcall _SysFreeString(BSTR bstr)
{
	BOOL b_HeapAllocRecursion=bGetRecursion();
	if(!b_HeapAllocRecursion){
		SetRecursion(TRUE);
		remove_mem( bstr );
		((PFN_SysFreeString)(PROC)g_SysFreeString)(bstr);
		SetRecursion(FALSE);
	}else{
		((PFN_SysFreeString)(PROC)g_SysFreeString)(bstr);
	}
}
////////////////////////////////////////////////////////////////////////////
HOOKMACRO("OLEAUT32.DLL", BSTR __stdcall, SysAllocString, (const OLECHAR * oleChar))
{
	BSTR bstr;
	BOOL b_HeapAllocRecursion=bGetRecursion();
	if(!b_HeapAllocRecursion){
		SetRecursion(TRUE);
		bstr=((PFN_SysAllocString)(PROC)g_SysAllocString)(oleChar);
		if(bstr)
			add_mem(MI_BSTR, bstr, wcslen(oleChar)+2);
		SetRecursion(FALSE);
	}else{
		bstr=((PFN_SysAllocString)(PROC)g_SysAllocString)(oleChar);
	}
	return bstr;
}
////////////////////////////////////////////////////////////////////////////
HOOKMACRO("OLEAUT32.DLL", BSTR __stdcall, SysAllocStringLen, (const OLECHAR* c, UINT n))
{
	BSTR bstr = 0;
	BOOL b_HeapAllocRecursion=bGetRecursion();
	if(!b_HeapAllocRecursion){
		SetRecursion(TRUE);
		bstr = ((PFN_SysAllocStringLen)(PROC)g_SysAllocStringLen)(c, n );	
		add_mem( MI_BSTR, bstr, n + 2 );
		SetRecursion(FALSE);
	}else{
		bstr = ((PFN_SysAllocStringLen)(PROC)g_SysAllocStringLen)(c, n );	
	}
	return bstr;	
}

////////////////////////////////////////////////////////////////////////////
HOOKMACRO("OLEAUT32.DLL", BSTR __stdcall, SysAllocStringByteLen, (LPCSTR psz, UINT len))
{
	BSTR bstr = 0;
	BOOL b_HeapAllocRecursion=bGetRecursion();
	if(!b_HeapAllocRecursion){
		SetRecursion(TRUE);
		bstr = ((PFN_SysAllocStringByteLen)(PROC)g_SysAllocStringByteLen)(psz, len );	
		add_mem( MI_BSTR, bstr, len + 2 );
		SetRecursion(FALSE);
	}else{
		bstr = ((PFN_SysAllocStringByteLen)(PROC)g_SysAllocStringByteLen)(psz, len );	
	}
	return bstr;	
}
////////////////////////////////////////////////////////////////////////////
#endif
#if 1
////////////////////////////////////////////////////////////////////////////
//virtual memory 
////////////////////////////////////////////////////////////////////////////
typedef LPVOID(__stdcall *PFN_VirtualAlloc)(IN LPVOID lpAddress, IN SIZE_T dwSize, IN DWORD flAllocationType, IN DWORD flProtect);
extern CAPIHook g_VirtualAlloc;
LPVOID __stdcall _VirtualAlloc(IN LPVOID lpAddress, IN SIZE_T dwSize, IN DWORD flAllocationType, IN DWORD flProtect)
{
	void* p = 0;
	{
		p = ((PFN_VirtualAlloc)(PROC)g_VirtualAlloc)(lpAddress, dwSize, flAllocationType, flProtect );
	}
	if(g_bRunMemTrace ){
		add_mem( MI_VIRTUAL, p, dwSize );
	}

	return p;
}
CAPIHook g_VirtualAlloc("Kernel32.dll", "VirtualAlloc", (PROC)_VirtualAlloc, TRUE);
////////////////////////////////////////////////////////////////////////////
typedef LPVOID(__stdcall *PFN_VirtualAllocEx)(IN HANDLE hProcess, IN LPVOID lpAddress,
						IN SIZE_T dwSize, IN DWORD flAllocationType, IN DWORD flProtect);
extern CAPIHook g_VirtualAllocEx;
LPVOID __stdcall _VirtualAllocEx( IN HANDLE hProcess, IN LPVOID lpAddress,
						IN SIZE_T dwSize, IN DWORD flAllocationType, IN DWORD flProtect )
{
	void* p = 0;
	{
		p = ((PFN_VirtualAllocEx)(PROC)g_VirtualAllocEx)( hProcess, lpAddress, dwSize, flAllocationType, flProtect );
	}
	if(g_bRunMemTrace){
		add_mem( MI_VIRTUAL, p, dwSize );
	}

	return p;
}
CAPIHook g_VirtualAllocEx("Kernel32.dll", "VirtualAllocEx", (PROC)_VirtualAllocEx, TRUE);
////////////////////////////////////////////////////////////////////////////
typedef BOOL(__stdcall *PFN_VirtualFree)( IN LPVOID lpAddress, IN SIZE_T dwSize, IN DWORD dwFreeType);
extern CAPIHook g_VirtualFree;
BOOL __stdcall _VirtualFree( IN LPVOID lpAddress, IN SIZE_T dwSize, IN DWORD dwFreeType )
{
	//if( !g_block_mem_hook )
	remove_mem( lpAddress );
	
	BOOL b = FALSE;
	{
//		lock l( &g_block_mem_hook, true );
		b = ((PFN_VirtualFree)(PROC)g_VirtualFree)( lpAddress, dwSize, dwFreeType );
	}
	
	return b;
}
CAPIHook g_VirtualFree("Kernel32.dll", "VirtualFree", (PROC)_VirtualFree, TRUE);
////////////////////////////////////////////////////////////////////////////
typedef BOOL(__stdcall *PFN_VirtualFreeEx)(IN HANDLE hProcess, IN LPVOID lpAddress, IN SIZE_T dwSize, IN DWORD dwFreeType);
extern CAPIHook g_VirtualFreeEx;
BOOL __stdcall _VirtualFreeEx( IN HANDLE hProcess, IN LPVOID lpAddress, IN SIZE_T dwSize, IN DWORD dwFreeType )
{
	//if( !g_block_mem_hook )
	remove_mem( lpAddress );

	BOOL b = FALSE;
	{
//		lock l( &g_block_mem_hook, true );
		b = ((PFN_VirtualFreeEx)(PROC)g_VirtualFreeEx)(hProcess, lpAddress, dwSize, dwFreeType);
	}
	
	return b;
}
CAPIHook g_VirtualFreeEx("Kernel32.dll", "VirtualFreeEx", (PROC)_VirtualFreeEx, TRUE);
////////////////////////////////////////////////////////////////////////////
#endif
//NT
////////////////////////////////////////////////////////////////////////////
HOOKMACRO("NTDLL.DLL", HRESULT __stdcall, NtAllocateVirtualMemory,
	(IN HANDLE ProcessHandle, IN OUT PVOID *BaseAddress, IN ULONG ZeroBits, IN OUT PULONG RegionSize, IN ULONG AllocationType, IN ULONG Protect ))
{
	HRESULT hr = 0;
	{
		hr = ((PFN_NtAllocateVirtualMemory)(PROC)g_NtAllocateVirtualMemory)( ProcessHandle, BaseAddress, ZeroBits, RegionSize, AllocationType, Protect );
	}
	if(g_bRunMemTrace)
	{
		add_mem( MI_NT_VIRTUAL, *BaseAddress, *RegionSize );
	}

	return hr;
}


////////////////////////////////////////////////////////////////////////////
HOOKMACRO("NTDLL.DLL", HRESULT __stdcall, NtFreeVirtualMemory,
	( IN HANDLE               ProcessHandle,
									IN PVOID                *BaseAddress,
									IN OUT PULONG           RegionSize,
		IN ULONG                FreeType ))
{
	remove_mem( *BaseAddress );

	HRESULT hr = 0;
	{
		hr = ((PFN_NtFreeVirtualMemory)(PROC)g_NtFreeVirtualMemory)( ProcessHandle, BaseAddress, RegionSize, FreeType );
	}

	return hr;
}


////////////////////////////////////////////////////////////////////////////
bool install_mem_hook()
{
	return true;
}

bool dump_mem_hook()
{
	lock l( &g_block_mem_hook, true );

	_trace_file_clear( "heap.log" );
	char sz_buffer[1024];
	char sz_stack[1024];

	DWORD dw_full_size = 0;

	for( long lpos=g_map_heap.head(); lpos; lpos=g_map_heap.next(lpos) )
	{
		heap_info* phi = g_map_heap.get( lpos );
		dw_full_size += phi->m_dw_size;
		sprintf( sz_buffer, "0x%08X[%d]: 0x%08X", (long)phi->m_hheap, phi->m_dw_size, phi->m_laddress );
		strcat( sz_buffer, "{" );
		for( unsigned idx=0;idx<phi->m_size_stack;idx++ )
		{
			if( phi->m_pstack[idx] == 0xcdcdcdcd )
			{
				int i=0;
				i++;
			}
			sprintf( sz_stack, idx ? " 0x%08X" : "0x%08X", phi->m_pstack[idx] );
			strcat( sz_buffer, sz_stack );			
		}
		
		strcat( sz_buffer, "}" );

		_trace_file_text( "heap.log", sz_buffer );
		_trace_file_text( "heap.log", "\n" );
	}

	_trace_file_text( "heap.log", "-------------------------\n" );
	sprintf( sz_buffer, "Full size = [%d]", (long)dw_full_size );
	_trace_file_text( "heap.log", sz_buffer );
	_trace_file_text( "heap.log", "\n" );

	return true;
}

void deinstall_mem_hook()
{
	enter_cc cc( &g_cs_map );
	g_map_heap.clear();
}
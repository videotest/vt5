// keeper.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "\vt5\awin\misc_dbg.h"
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// Special entry points required for inproc servers

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	return E_NOTIMPL;
}

STDAPI DllCanUnloadNow(void)
{
	return S_FALSE;
}

static int g_seconds=0;
static char g_sz_path[MAX_PATH]={0};

DWORD WINAPI ThreadFunc( LPVOID lpParam ) 
{
	while(1)
	{
		//_trace_file("keeper.log","...");
		Sleep(1000);
		g_seconds++;
		HWND hwnd = FindWindow(NULL, g_sz_path); // "D:\\VT5\\VT5\\debug\\shell.exe"
		if(hwnd)
		{
			//_trace_file("keeper.log","Dialog found");
			HWND hwndRetry = ::GetDlgItem(hwnd, IDRETRY);
			
			if(hwndRetry)
			{
				//_trace_file("keeper.log","Retry button found");
				g_seconds++;
				if( g_seconds >= 10 )
				{ // если видим нужный диалог - каждые 10 секунд жмем кнопку Retry
					//_trace_file("keeper.log","Retrying...");
					PostMessage(hwnd, WM_COMMAND, MAKEWORD( IDRETRY, BN_CLICKED ), LPARAM( 0 ) );
					g_seconds=0;
				}
			}
		}
		else
		{
			g_seconds = 0;
		}

	}

    return 0; 
} 


// by exporting DllRegisterServer, you can use regsvr.exe
STDAPI DllRegisterServer(void)
{
	char sz_path[MAX_PATH];	sz_path[0] = 0;
	::GetModuleFileName( 0, sz_path, sizeof(sz_path) );
	char* psz_last = strrchr( sz_path, '\\' );
	if( psz_last )
	{
		psz_last++;
		strcpy( psz_last, "comps\\keeper.dll" );
	}
	HMODULE	h = ::LoadLibrary( sz_path ); // еще раз загрузимся - чтобы враг не выгрузил
	//HMODULE	h = ::LoadLibrary( "comps\\keeper.dll" ); // еще раз загрузимся - чтобы враг не выгрузил

	::GetModuleFileName( 0, g_sz_path, sizeof(g_sz_path) );

	DWORD dwThreadId, dwThrdParam = 1;
	HANDLE hThread;

	hThread = CreateThread( 
		NULL,                        // no security attributes 
		0,                           // use default stack size  
		ThreadFunc,                  // thread function 
		&dwThrdParam,                // argument to thread function 
		0,                           // use default creation flags 
		&dwThreadId);                // returns the thread identifier 

	// Check the return value for success. 
	if (hThread == NULL) 
	{
		char szMsg[80];
		wsprintf( szMsg, "CreateThread failed." ); 
		MessageBox( NULL, szMsg, "keeper", MB_OK );
	}

	return S_OK;
}

// by exporting DllRegisterServer, you can use regsvr.exe
STDAPI DllUnregisterServer(void)
{
	return S_OK;
}


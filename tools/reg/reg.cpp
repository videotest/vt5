// reg.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
//#include <winnt.h>

bool RegisterFile(LPCTSTR szFile)
{
	// load it 
	HMODULE hModule = LoadLibrary(szFile);

	if( !hModule )
	{
		HRESULT hr = ::GetLastError();
		TCHAR str_error[1024];

		LPVOID lpMsgBuf = 0;
		if( FormatMessage( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			hr,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR) &lpMsgBuf,
			0,
			NULL ))
		{
			fprintf(stderr, "Error loading dll '%s'.Error:%s (%x)", szFile, (LPCSTR)lpMsgBuf, hr );
			LocalFree( lpMsgBuf );
		}
	}

	if (!hModule)
		return false;

	typedef HRESULT (STDAPICALLTYPE *PFNRS)();
	// and call DllRegisterServer
	PFNRS fn = (PFNRS)GetProcAddress(hModule, _T("DllRegisterServer"));
	if (!fn)
	{
		::FreeLibrary( hModule );
		return false;
	}

	bool bres = SUCCEEDED(fn());
	::FreeLibrary( hModule );

	return bres;
}

bool UnregisterFile(LPCTSTR szFile)
{
	// load it 
	HMODULE hModule = LoadLibrary(szFile);
	if (!hModule)
		return false;

	typedef HRESULT (STDAPICALLTYPE *PFNRS)();
	// and call DllRegisterServer
	PFNRS fn = (PFNRS)GetProcAddress(hModule, _T("DllUnregisterServer"));
	if (!fn)
	{
		::FreeLibrary( hModule );
		return false;
	}

	bool bres = SUCCEEDED(fn());
	::FreeLibrary( hModule );

	return bres;
}


int _tmain(int argc, _TCHAR* argv[])
{
	if(argc<=0)
		return 1;
	RegisterFile(argv[1]);
	return 0;
}


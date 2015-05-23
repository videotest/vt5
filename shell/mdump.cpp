
#include "stdafx.h"

#include "mdump.h"

LPCSTR MiniDumper::m_szAppName;

MiniDumper::MiniDumper( LPCSTR szAppName )
{
	// if this assert fires then you have two instances of MiniDumper
	// which is not allowed
	ASSERT( m_szAppName==NULL );

	m_szAppName = szAppName ? _strdup(szAppName) : "Application";

	::SetUnhandledExceptionFilter( TopLevelFilter );
}

LONG MiniDumper::TopLevelFilter( struct _EXCEPTION_POINTERS *pExceptionInfo )
{
	LONG retval = EXCEPTION_CONTINUE_SEARCH;
	HWND hParent = NULL;						// find a better value for your app

	// firstly see if dbghelp.dll is around and has the function we need
	// look next to the EXE first, as the one in System32 might be old 
	// (e.g. Windows 2000)
	HMODULE hDll = NULL;
	char szDbgHelpPath[_MAX_PATH];

	if (GetModuleFileName( NULL, szDbgHelpPath, _MAX_PATH ))
	{
		char *pSlash = _tcsrchr( szDbgHelpPath, '\\' );
		if (pSlash)
		{
			_tcscpy( pSlash+1, "DBGHELP.DLL" );
			hDll = ::LoadLibrary( szDbgHelpPath );
		}
	}

	if (hDll==NULL)
	{
		// load any version we can
		hDll = ::LoadLibrary( "DBGHELP.DLL" );
	}

	LPCTSTR szResult = NULL;

	if (hDll)
	{
		MINIDUMPWRITEDUMP pDump = (MINIDUMPWRITEDUMP)::GetProcAddress( hDll, "MiniDumpWriteDump" );
		if (pDump)
		{
			char szDumpPath[_MAX_PATH];
			char szScratch [_MAX_PATH];

			if( !get_dump_path( szDumpPath, sizeof(szDumpPath) ) )
				return retval;

			// ask the user if they want to save a dump file
			if (::MessageBox( NULL, "Something error happened in your program, would you like to save a diagnostic file?", m_szAppName, MB_YESNO|MB_ICONERROR )==IDYES)
			{
				// create the file
				HANDLE hFile = ::CreateFile( szDumpPath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,
											FILE_ATTRIBUTE_NORMAL, NULL );

				if (hFile!=INVALID_HANDLE_VALUE)
				{
					_MINIDUMP_EXCEPTION_INFORMATION ExInfo;

					ExInfo.ThreadId = ::GetCurrentThreadId();
					ExInfo.ExceptionPointers = pExceptionInfo;
					ExInfo.ClientPointers = NULL;

					// write the dump
					BOOL bOK = pDump( GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &ExInfo, NULL, NULL );
					if (bOK)
					{
						sprintf( szScratch, "Saved dump file to '%s'", szDumpPath );
						szResult = szScratch;
						retval = EXCEPTION_EXECUTE_HANDLER;
					}
					else
					{
						sprintf( szScratch, "Failed to save dump file to '%s' (error %d)", szDumpPath, GetLastError() );
						szResult = szScratch;
					}
					::CloseHandle(hFile);
				}
				else
				{
					sprintf( szScratch, "Failed to create dump file '%s' (error %d)", szDumpPath, GetLastError() );
					szResult = szScratch;
				}
			}
		}
		else
		{
			szResult = "DBGHELP.DLL too old";
		}
	}
	else
	{
		szResult = "DBGHELP.DLL not found";
	}

	if (szResult)
		::MessageBox( NULL, szResult, m_szAppName, MB_OK|MB_ICONINFORMATION );

	return EXCEPTION_EXECUTE_HANDLER;//retval;
}


bool MiniDumper::get_dump_path( char* sz_path, size_t size_buf )
{
	char sz_dir[_MAX_PATH];

	if( !GetModuleFileName( NULL, sz_dir, _MAX_PATH ) )
		return false;

	char *pSlash = _tcsrchr( sz_dir, '\\' );
	if( !pSlash )
		return false;

	pSlash++;
	*pSlash = 0;
	strcat( sz_dir, "Dumps\\" );

	char sz_file_name[_MAX_PATH];
	SYSTEMTIME st;
	::ZeroMemory( &st, sizeof(st) );
	::GetLocalTime( &st );
	::sprintf( sz_file_name, "vt_%04d%02d%02d_%02d%02d%02d.mdm", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond );

	::CreateDirectory( sz_dir, 0 );

	strcpy( sz_path, sz_dir );		
	strcat( sz_path, sz_file_name );

	return true;
}
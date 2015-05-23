#pragma once

inline bool write_minidump( DWORD pid, DWORD tid  )
{
	char	sz[MAX_PATH];
	char	sz_args[MAX_PATH+20];

	char	sz_fbase[30];
	::sprintf( sz_fbase, "dump%03d", -1 );

	GetModuleFileName( 0, sz, sizeof(sz) );
	strcpy( strrchr( sz, '\\' ), "\\write_dump.exe" );
	sprintf( sz_args, "\"%s\"/pid:%d /tid:%d /pex:%p /fb:%s", sz, ::GetCurrentProcessId(), ::GetCurrentThreadId(), 0, sz_fbase );
 
	PROCESS_INFORMATION	pi;
	ZeroMemory( &pi, sizeof( pi ) );
	STARTUPINFO	si;
	ZeroMemory( &si, sizeof( si ) );
	if( ::CreateProcess( sz, sz_args, 0, 0, 0, 0, 0, 0, &si, &pi ) )
	{
		::WaitForSingleObject( pi.hProcess, 1000000 );
		::CloseHandle( pi.hProcess );
		::CloseHandle( pi.hThread );
	}

	return 0;
}
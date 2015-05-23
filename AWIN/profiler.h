#ifndef __profiler_h__
#define __profiler_h__

#include <time.h>

//////////////////////////////////////////////////////////////////////
inline const char* __GetCurrentTime__()
{
	static char sz_time[255];	sz_time[0] = 0;

	time_t ltime;
	time( &ltime );
	tm* today = localtime( &ltime );	
	strftime( sz_time, sizeof(sz_time), "%d-%m-%y %H:%M:%S", today );	

	return sz_time;
}

//////////////////////////////////////////////////////////////////////
inline void __WriteLog__( const char *psz_format, ... )
{
	//if( !IsLoggingEnable() )		return;

	static char	sz_log_file[MAX_PATH];	

	if( !strlen( sz_log_file ) )
	{
		::GetModuleFileName( 0, sz_log_file, sizeof(sz_log_file) );
		char* psz_slash = strrchr( sz_log_file, '\\' );
		if( !psz_slash )		return;
		*psz_slash = 0;
		strcat( sz_log_file, "\\profiler.log" );
		_strlwr( sz_log_file );
	}

	va_list args;
	va_start( args, psz_format );

	char sz[2048];		::ZeroMemory( sz, sizeof(sz) );
	char sz_pid[128];	sz_pid[0]	= 0;

	_vsnprintf( sz, sizeof(sz)-20, psz_format, args );
	strcat( sz, "\r" );

	HANDLE	hFile = ::CreateFile( sz_log_file, GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_ALWAYS, 0, 0 );
	if( hFile != INVALID_HANDLE_VALUE )
	{
		DWORD dw_offset = ::SetFilePointer( hFile, 0, 0, FILE_END );
		//write time
		const char* psz_time = __GetCurrentTime__();
		DWORD dwSize = (DWORD)strlen(psz_time);
		::WriteFile( hFile, psz_time, dwSize, &dwSize, 0 );

		//write tabs
		::WriteFile( hFile, "\t", 1, &dwSize, 0 );

		//write out string
		dwSize = (DWORD)strlen(sz);
		::WriteFile( hFile, sz, dwSize, &dwSize, 0 );

		::CloseHandle( hFile );	hFile = 0;		
	}

	va_end(args);
}


inline __int64 __cputick__()
{
	
	LARGE_INTEGER l;
	QueryPerformanceCounter( &l );

	return l.QuadPart;	
}

inline __int64 __cpuspeed__()
{
	static __int64 n = 0;

	if( !n )
	{	
		LARGE_INTEGER l;
		QueryPerformanceFrequency( &l );
		n = l.QuadPart / 1000;
	}

	return n;
}


class __call_counter__
{
public:
	__call_counter__( const char *psz )
	{
		m_psz = _tcsdup( psz );
		call_time = 0;
		call_count = 0;
		min_time = -1;
		max_time = 0;
	}

	~__call_counter__()
	{
		call_time	/= __cpuspeed__();
		min_time	/= __cpuspeed__();
		max_time	/= __cpuspeed__();
		char	sz[200];
		::wsprintf( sz, _T("PROFILE_REPORT <%s> calls %d times, takes %d ms [%d - %d]\n"), m_psz, call_count, (int)call_time, (int)min_time, (int)max_time );
		::free( m_psz );

		::OutputDebugString( sz );
		__WriteLog__( sz );
	}

	void enter()
	{
		call_count++;
		enter_time = __cputick__();
	}

	void leave()
	{
		__int64 stage_time = __cputick__()-enter_time;
		max_time = max(stage_time, max_time);
		if( min_time != -1 )
			min_time = min(stage_time, min_time);
		else 
			min_time = stage_time;		
		call_time += stage_time;
	}

	long	call_count;
	char	*m_psz;
	__int64	call_time, enter_time;
	__int64	min_time, max_time;
};

class __func_call__
{
public:
	__func_call__( __call_counter__ &c ) : m_c( c )
	{
		m_c.enter();
	}
	~__func_call__()
	{
		m_c.leave();
	}
protected:
	__call_counter__ &m_c;
};

class __time_test__
{
public:
	__time_test__( const char *psz )
	{
		m_psz = _tcsdup( psz );
		call_time = __cputick__();
	}

	~__time_test__()
	{
		call_time = __cputick__() - call_time;
		call_time	/= __cpuspeed__();
		char	sz[200];
		::wsprintf( sz, _T("TIME_TEST <%s> takes %d ms\n"), m_psz, (int)call_time );
		::free( m_psz );

		::OutputDebugString( sz );
	}

	__int64	call_time;
	char	*m_psz;
};

#define _PROFILE_TEST( pfunc )		\
static __call_counter__	__counter( pfunc );	\
__func_call__		__caller( __counter );	\

#define _TIME_TEST( pfunc )		\
__time_test__	__time_test___( pfunc );	\


#ifdef _DEBUG
#define PROFILE_TEST( pfunc )	_PROFILE_TEST( pfunc )
#define TIME_TEST( pfunc )		_TIME_TEST( pfunc )
#else
#define PROFILE_TEST( pfunc )	_PROFILE_TEST( pfunc )
#define TIME_TEST( pfunc )
#endif _DEBUG


#endif//__profiler_h__


#pragma once
///////////////////////////////////////////////////////////////////////////////
// This file is part of AWIN class library
// Copyright (c) 2001-2002 Andy Yamov
//
// Permission to use, copy, modify, distribute, and sell this software and
// its documentation for any purpose is hereby granted without fee
// 
// THE SOFTWARE IS PROVIDED_T("AS-IS") AND WITHOUT WARRANTY OF ANY KIND,
// EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
// WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
//

#ifndef __misc_dbg_h__
#define __misc_dbg_h__

#include "stdio.h"
#include "malloc.h"
#include <tchar.h>
#include "time.h"
#include "type_defs.h"

const char sz_profiler_log[] = "profiler.log";


inline bool is_enable_logging()
{
	static long enable_logging = -1;

	if( enable_logging == -1 )
	{
		char	sz[MAX_PATH];

		::GetModuleFileName( 0, sz, sizeof( sz ) );
		_char	*pe = _tcsrchr( sz, '.' );
		_tcscpy( pe, ".ini" );

		enable_logging = ::GetPrivateProfileInt( "General", "EnableLogging", 0, sz );
		::WritePrivateProfileString( "General", "EnableLogging", enable_logging?"1":"0", sz );
	}

	return enable_logging!=0;
}


inline long get_log_path( _char *psz, size_t cb, const _char *psz_log )
{
	static long logs_exist = -1;

	GetModuleFileName( 0, psz, (DWORD)cb );
	_char	*pe = _tcsrchr( psz, '\\' );

	if( logs_exist== -1 )
	{
		_tcscpy( pe, "\\log" );
		if( GetFileAttributes( psz ) == (DWORD)-1 )
			logs_exist = 0;
		else
			logs_exist = 1;
	}

	if( logs_exist == 0 )
	{
		_tcscpy( pe+1, psz_log );
	}
	else
	{
		_tcscpy( pe+1, "log\\" );
		_tcscat( pe, psz_log );
	}

	return true;
}

inline void _trace_file_clear( const _char *psz_filename )
{
	if( !is_enable_logging() )return;

	_char	sz_filename[MAX_PATH];
	_char	sz_filename1[MAX_PATH];
	get_log_path( sz_filename, sizeof( sz_filename ), psz_filename );

	_tcscpy( sz_filename1, sz_filename );
	_tcscpy( _tcsrchr( sz_filename1, '.' ), _T(".0") );

	::DeleteFile( sz_filename1 );
	::MoveFile( sz_filename, sz_filename1 );
}

inline void _trace_file_text( const _char *psz_filename, const _char *psz_text )
{
	if( !is_enable_logging() )return;

	_char	sz_filename[MAX_PATH];
	get_log_path( sz_filename, sizeof( sz_filename ), psz_filename );

	HANDLE	hFile = ::CreateFile( sz_filename, GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_ARCHIVE, 0 );
	if( hFile != INVALID_HANDLE_VALUE )
	{
		::SetFilePointer( hFile, 0, 0, FILE_END );

		DWORD	dwWritten = 0;

		for( ; *psz_text; psz_text++ )
		{
			char	ch = *psz_text;
			if( ch == '\n' )
			{
				ch='\r';::WriteFile( hFile, &ch, 1, &dwWritten, 0 );
				ch='\n';::WriteFile( hFile, &ch, 1, &dwWritten, 0 );
			}
			else if( ch == '\r' )
				continue;
			else
			{
				::WriteFile( hFile, &ch, 1, &dwWritten, 0 );
			}
		}
		::CloseHandle( hFile );
	}
}

inline void _trace_file( const _char *psz_filename, const _char *psz_format, ... )
{
	if( !is_enable_logging() )return;

	char	sz_buffer[1024];

	va_list args;
	va_start(args, psz_format);
	::_vsntprintf( sz_buffer, sizeof( sz_buffer ), psz_format, args );
	va_end(args);

	//date
	{
		time_t	t;  
		::time( &t );

		tm	*ptm = ::localtime( &t );
		char	sz[30];
		wsprintf( sz, "%d/%02d/%02d %02d:%02d:%02d   ", ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec );
		_trace_file_text( psz_filename, sz );
	}


	_trace_file_text( psz_filename, sz_buffer );
	_trace_file_text( psz_filename, "\n" );
}

//inline void debug_break()
//{	
//	__asm
//	{		int 3	}
//}

inline void debug_assert( BOOL lAssertCode )
{
	if( lAssertCode == false )DebugBreak();
}

class time_test
{
public:
	time_test( const _char *psz=0, bool report=true )
	{
		m_psz = _tcsdup( psz );
		m_dw = ::GetTickCount();
		m_report = report;
		m_calls = 0;
	}
	~time_test()
	{
		if( m_report )
			report();
		if( m_psz )
			::free( m_psz );
	}

	void report()
	{
		DWORD	dw = GetTickCount()-m_dw;

		if( dw < 100 )
		{
			if( m_psz )_trace_file( sz_profiler_log, _T("[%s:%d] %d"), m_psz, m_calls, dw );
			else _trace_file( sz_profiler_log, _T("[unknown:%d] %d"), m_calls, dw );
		}
		else
		{
			if( m_psz )_trace_file( sz_profiler_log, _T("!!![%s:%d] %d"), m_psz, m_calls, dw );
			else _trace_file( sz_profiler_log, _T("!!![unknown:%d] %d"), m_calls, dw );
		}
		m_dw = GetTickCount();
		m_calls++;
	}

	void report_critical()
	{
		if( GetTickCount() - m_dw >= 100 )
			report();
		else
		{
			m_calls++;
			m_dw = GetTickCount();
		}
	}
public:
	DWORD	m_dw;
	bool	m_report;
	_char	*m_psz;
	int		m_calls;
};


#ifdef _DEBUG

#define _assert( lcode )	debug_assert( (lcode)!=0 )

#ifndef _COM_ASSERT
#define _COM_ASSERT( lcode )	debug_assert( (lcode)!=0 )
#endif //_COM_ASSERT

#else

#define _assert( lcode )

#ifndef _COM_ASSERT
#define _COM_ASSERT( lcode )
#endif //_COM_ASSERT

#endif //_DEBUG

inline __int64 __cputick()
{
	__int64	n;
	::QueryPerformanceCounter( (LARGE_INTEGER*)&n );

	return n;
}

inline __int64 __cpuspeed()
{
	static __int64 n = 0;//1500000;	//1.5 ghz 

	if( !n )
	{
		::QueryPerformanceFrequency( (LARGE_INTEGER*)&n );
		n /= 1000;
	}
	return n;
}

class __call_counter
{
public:
	__call_counter( const _char *psz )
	{
		m_psz = _tcsdup( psz );
		call_time = 0;
		call_count = 0;
	}

	~__call_counter()
	{
		call_time	/= __cpuspeed();
		_trace_file( sz_profiler_log, _T("PROFILE_REPORT %s calls %d times, takes %d ms"), m_psz, call_count, (int)call_time );
		::free( m_psz );

	}

	void enter()
	{
		call_count++;
		enter_time = __cputick();
	}

	void leave()
	{
		call_time+=__cputick()-enter_time;
	}

	long	call_count;
	_char	*m_psz;
	__int64	call_time, enter_time;
};

class __func_call
{
public:
	__func_call( __call_counter &c ) : m_c( c )
	{
		m_c.enter();
	}
	~__func_call()
	{
		m_c.leave();
	}
protected:
	__call_counter &m_c;
};

#define PROFILE_FUNCTION( pfunc )		\
static __call_counter	__counter( pfunc );	\
__func_call		__caller( __counter );	\



#endif //
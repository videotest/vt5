#pragma once
///////////////////////////////////////////////////////////////////////////////
// This file is part of AWIN class library
// Copyright (c) 2001-2002 Andy Yamov
//
// Permission to use, copy, modify, distribute, and sell this software and
// its documentation for any purpose is hereby granted without fee
// 
// THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,
// EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
// WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
//

#ifndef __trace_h__
#define __trace_h__

//#include "varargs.h"

enum	message_codes
{
	mc_DebugText,
	mc_Error,
};

template <int code> void message( const char *psz_format, ... )
{
	static char	sz_file[MAX_PATH];

	if( !strlen( sz_file ) )
	{
		GetModuleFileName( 0, sz_file, sizeof( sz_file ) );
		strcpy( strrchr( sz_file, '.' ), ".log" );
	}

	va_list args;
	va_start(args, psz_format);

	char sz[512];

	vsprintf(sz, psz_format, args);
	strcat( sz, "\r\n" );

	::OutputDebugString( sz );

	HANDLE	hFile = ::CreateFile( sz_file, GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_ALWAYS, 0, 0 );
	if( hFile != INVALID_HANDLE_VALUE )
	{
		DWORD	dwSize = strlen( sz	 );
		::SetFilePointer( hFile, 0, 0, FILE_END );
		::WriteFile( hFile, sz, dwSize, &dwSize, 0 );
		::CloseHandle( hFile );

	}
	va_end(args);
};


#define trace		message<mc_DebugText>
#define trace_error	message<mc_Error>



#endif //__trace_h__
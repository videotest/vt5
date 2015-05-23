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

#ifndef __misc_string_h__
#define __misc_string_h__

#include "type_defs.h"
#include "misc_ptr.h"
#include "misc_module.h"

class _string : public _ptr_t<_char>
{
public:
	_string( const _char *psz )
	{
		copy( psz );
	}
	_string( _string &src )
	{
		copy( src.ptr() );
	}
	_string()
	{
	}

	virtual ~_string()
	{
	}

	const _char *copy( const _char *psz, int len=-1 )
	{
		free();

		if( len < 0 )len= (int)_tcslen( psz )+1;
		alloc( len, 50 );
		_tcsncpy( ptr(), psz, len-1 );
		ptr()[len-1]=0;
		m_size = len;
		return ptr();
	}
	const _char *concat( const _char *psz )
	{
		long	cur_len = length();
		long	len = (long)_tcslen( psz )+cur_len+1;
		alloc( len, 50 );
		m_size = len;
		return _tcscpy( ptr()+cur_len, psz );
	}
	long length()
	{
		if( !ptr() )return 0;
		return (long)_tcslen( ptr() );
	}


	const _char *load( unsigned ids, HINSTANCE h = 0 )
	{
		free();

		if( !h ) h = module::hrc();

		_char	sz_buffer[256];	//маленькие строки - через него
		int	cb = sizeof( sz_buffer );

		int	loaded = ::LoadString( h, ids, sz_buffer, cb );

		if( loaded < cb-2 )
			copy( sz_buffer );
		else
			while( true )
			{
				cb+=256;
				alloc( cb );
				loaded = ::LoadString( h, ids, ptr(),  cb );
				if( loaded < cb-2 )break;
			}

		return ptr();
	}
public:
	const _char *operator=(const _char *psz )
	{return copy( psz );}
	const _char *operator=( _string &s )
	{return copy( s.ptr() );}
	const _char *operator+=(const _char *psz )
	{return concat( psz );}
};

#endif //__misc_string_h__
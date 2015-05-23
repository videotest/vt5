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

#ifndef __misc_ptr_h__
#define __misc_ptr_h__

template<class _class>
class _ptr_t
{
public:
	_ptr_t( int	length = 0 )					{clear();if( length )alloc( length );}
	virtual ~_ptr_t()							{free();}	

	void clear()								{m_size = 0; m_ptr = 0; max_size = 0;}
	void free()
	{
		if( m_ptr )delete []m_ptr;
		clear(); 
	}
	_class*	alloc( int nlen, int delta = 100 )
	{
		if( nlen < max_size )
		{
			m_size = nlen;
			return m_ptr;
		}
		int	new_max_size = (nlen+delta-1)/delta*delta;
		_class	*p = new _class[new_max_size];
		if( m_ptr && m_size )
            for(int i=0; i<m_size; i++) p[i]=m_ptr[i];
			//memcpy( p, m_ptr, sizeof( _class )*m_size );
		if( m_ptr )delete []m_ptr;
		m_ptr = p;
		max_size = new_max_size;
		m_size = nlen;
		return m_ptr;
	}
	long	size()				{return m_size;}
	operator _class*()			{return m_ptr;}
	operator const _class*()	{return m_ptr;}

	_class *ptr()	{return m_ptr;}
protected:
	int		m_size;
	int		max_size;
	_class	*m_ptr;

};


#endif //__misc_ptr_h__
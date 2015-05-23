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

#ifndef __typedefs_h__
#define __typedefs_h__

#include "tchar.h"

#ifndef SIMPLE_TYPES_DEFINED

typedef unsigned ulong;
typedef unsigned uint;
typedef unsigned long dword;
typedef unsigned short word;
typedef unsigned char byte;
typedef long	lresult;

#ifdef UNICODE

#define _char wchar_t

#define _sscanf		swscanf
#define _sprintf	swprintf
#define	__snprintf	_snwprintf

inline const _char *char_to_text( _char *psz_t, const char *psz_a, int cb_t )
{	MultiByteToWideChar(CP_ACP, 0, psz_a, -1, psz_t, cb_t );return psz_t;}
inline const char *text_to_char( char *psz_a, const _char *psz_t, int cb_t )
{	WideCharToMultiByte(CP_ACP, 0, psz_t, -1, psz_a, cb_t, 0, 0 );return psz_a;}

#else
#define _char char

#define _sscanf		sscanf
#define _sprintf	sprintf
#define	__snprintf	_snprintf

inline const _char *char_to_text( _char *psz_t, const char *psz_a, int cb_t )
{	strncpy( psz_t, psz_a, cb_t );return psz_t;}
inline const char *text_to_char( char *psz_a, const _char *psz_t, int cb_t )
{	strncpy( psz_a, psz_t, cb_t );return psz_a;}


#endif //UNICODE

#ifndef splitpath

#ifdef UNICODE
#define splitpath	_wsplitpath
#define makepath	_wmakepath
#else
#define splitpath	_splitpath
#define makepath	_makepath
#endif //

#endif //splitpath



class _rect : public RECT
{
public:
	_rect()
	{left = top = right = bottom = 0;}
	_rect( long l, long t, long r, long b )
	{left = l;top = t;right = r, bottom = b;}
	_rect( const RECT &r )
	{left = r.left, right = r.right, top = r.top, bottom = r.bottom;}

	_rect& operator=( const RECT &r )
	{left = r.left, right = r.right, top = r.top, bottom = r.bottom;return *this;}

	bool	pt_in_rect( const POINT &pt )
	{	return pt.x >= left && pt.x < right && pt.y >= top && pt.y < bottom;}
	void offset( int dx, int dy )
	{	left+=dx;right+=dx;top+=dy;bottom+=dy;}

	long	width()		{return right-left;}
	long	height()	{return bottom-top;}
	long	hcenter()	{return (left+right)>>1;}
	long	vcenter()	{return (top+bottom)>>1;}
};

class _size : public SIZE
{
public:
	_size()	
	{cx = cy = 0;}
	_size( int dx, int dy )	
	{cx = dx; cy = dy;}
};

class _point : public POINT
{
public:
	_point()			{x = y = 0;}
	_point( long l )	{y = short(l>>16); x = short(l&0xFFFF);}
	_point( const POINT &p )	{x = p.x; y = p.y;}
	_point( const POINTL &p )	{x = p.x; y = p.y;}
	_point( int _x, int _y)		{x = _x; y = _y;}
	_point( const POINTS &p )	{x = p.x; y = p.y;}
};
#else
	#define _char TCHAR
	#ifdef UNICODE
		#define _sscanf		swscanf
		#define _sprintf	swprintf
		#define	__snprintf	_snwprintf
	#else
		#define _sscanf		sscanf
		#define _sprintf	sprintf
		#define	__snprintf	_snprintf
	#endif
#endif //SIMPLE_TYPES_DEFINED



template <class _class>
class _static_pointer_t
{
public:
    _static_pointer_t()
    {        static_ptr() = (_class*)this;    }
    ~_static_pointer_t()
    {        static_ptr() = 0;    }
public:
    static _class  *&static_ptr()
    {static _class  *pclass = 0;return pclass;}
};

#define CW_DEFAULT_RECT	_rect( CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT ) 
//#include "\agnitum\awin\misc_new.h"

#endif // __typedefs_h__
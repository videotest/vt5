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

#ifndef __module_h__
#define __module_h__

#include "type_defs.h"
#include "commctrl.h"
#pragma comment(lib, "comctl32.lib")

class module : public _static_pointer_t<module>
{
public:
	module( HMODULE h );
	virtual ~module();

	virtual bool init( HMODULE h );
	virtual bool deinit();

	void set_resource_handle( HINSTANCE h )	{m_hrc = h;}

	virtual bool run();

	static const _char *title()	{return static_ptr()->m_title;}
	static HINSTANCE hrc()		{return static_ptr()->m_hrc;}
	static HINSTANCE hinst()	{return static_ptr()->m_hinst;}
//	static HINSTANCE handle()	{return static_ptr()->m_hinst;}
protected:
	HINSTANCE	m_hinst, m_hrc;
	_char		m_title[100];
};

inline module::module( HMODULE h )
{
	init( h );
}

inline module::~module()
{
}

inline bool module::init( HINSTANCE h )
{
	m_hinst = h;
	m_hrc = h;

	INITCOMMONCONTROLSEX	init;
	init.dwSize = sizeof( INITCOMMONCONTROLSEX );
	init.dwICC = ICC_COOL_CLASSES|ICC_WIN95_CLASSES;

    :: InitCommonControlsEx( &init );

	_char	sz_filename[MAX_PATH];
	::GetModuleFileName( 0, sz_filename, sizeof( sz_filename ) );
	
	::_tcscpy( m_title, _tcsrchr( sz_filename, '\\' )+1 ); 


	return true;
}

inline bool module::deinit()
{
	m_hinst = 0;
	return true;
}

inline bool module::run()
{
	MSG	msg;
	while( GetMessage( &msg, 0, 0, 0 ) )
	{
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}

	return deinit();
}


#endif //__module
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
#pragma once

#ifndef __misc_utils_h__
#define __misc_utils_h__

#include <shlobj.h>
#include "misc_list.h"
#include "misc_module.h"
#include "stdio.h"

#define SPI_GETFOREGROUNDLOCKTIMEOUT        0x2000
#define SPI_SETFOREGROUNDLOCKTIMEOUT        0x2001


inline void ActivateProcessWindow( HWND hwnd )
{
	if( ::IsIconic( hwnd ) )
		::ShowWindow( hwnd, SW_RESTORE );

	HWND hCurrWnd;
	int iMyTID;
	int iCurrTID;

	hCurrWnd = ::GetForegroundWindow();
	iMyTID   = ::GetCurrentThreadId();
	iCurrTID = ::GetWindowThreadProcessId(hCurrWnd,0);

	SystemParametersInfo(SPI_SETFOREGROUNDLOCKTIMEOUT, 0, 0, 0);

	//variant 1
	/*{
		DWORD dwTimeout;

		SystemParametersInfo(SPI_GETFOREGROUNDLOCKTIMEOUT, 0, &dwTimeout, 0);
		SystemParametersInfo(SPI_SETFOREGROUNDLOCKTIMEOUT, 0, 0, 0);

		::SetForegroundWindow( hwnd );

		SystemParametersInfo(SPI_SETFOREGROUNDLOCKTIMEOUT, 0, (LPVOID)dwTimeout, 0);
	}*/

	//variant 2
	/*{
		::AttachThreadInput(iMyTID, iCurrTID, TRUE);
		::SetForegroundWindow( hwnd );
		::AttachThreadInput(iMyTID, iCurrTID, FALSE);
	}*/
	//variant 3

/*	HWND	hwnd_p = GetParent( hwnd );
	while( GetParent( hwnd_p ) )
		hwnd_p = GetParent( hwnd_p );


	SendMessage(hwnd_p,WM_SYSCOMMAND,SC_MINIMIZE,0);
	SendMessage(hwnd_p,WM_SYSCOMMAND,SC_RESTORE,0);
	{
		HMODULE hLib = GetModuleHandle("user32.dll");
		void (__stdcall *SwitchToThisWindow)(HWND, BOOL);
		(FARPROC &)SwitchToThisWindow = GetProcAddress(hlib, "SwitchToThisWindow");
		SwitchToThisWindow( hwnd_p, TRUE );
	}*/
	

/**	RECT	rect;
	GetWindowRect( hwnd, &rect );
	mouse_event( MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_LEFTDOWN, rect.left+10, rect.top+10, 0, 0 );
	mouse_event( MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_LEFTUP, rect.left+10, rect.top+10, 0, 0 );*/
	::SetForegroundWindow( hwnd );


	
}

inline long calc_text_width( HDC hdc, const _char *psz )
{
	RECT	rect;
	::SetRectEmpty( &rect );
	::DrawText( hdc, psz, -1, &rect, DT_CALCRECT );
	return rect.right - rect.left;
}

static _char g_pathName[MAX_PATH];

inline int CALLBACK _browsecallbackproc(HWND hwnd,UINT uMsg,LPARAM lp, LPARAM pData)
{
	if( uMsg == BFFM_INITIALIZED )
	{
		SendMessage( hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)g_pathName );
	}

	return 0;
}

inline _char* browse_dir( BROWSEINFO* pinfo, const _char* pszDefPath )
{
	_tcscpy( g_pathName, pszDefPath );
	pinfo->lpfn = _browsecallbackproc;
	ITEMIDLIST	*pidl = ::SHBrowseForFolder( pinfo );
	if( pidl )
	{
		::SHGetPathFromIDList( pidl, g_pathName );
		return g_pathName;
	}

	return 0;	
}

#define MAX_STRIG_LEN	256

inline void char_list_free_func( void* pdata )
{
	_char* p = (_char*)pdata;
	delete p;
}

typedef _list_t<_char*, char_list_free_func> string_list2;

#define SEC_TO_DATE( _sec ) ( double( double(_sec) / 24.0 / 60.0 / 60.0 ) )

inline DATE get_current_time()
{
	SYSTEMTIME	sysTime;
	DATE	date;
	
	::GetLocalTime( &sysTime );
	::SystemTimeToVariantTime( &sysTime, &date );
	date += SEC_TO_DATE(sysTime.wMilliseconds) / 1000.0;

	return date;
}
/////////////////////////////////////////////////////////////////////////////////////////
//string API
inline int  strcmpz(const _char *psz1, const _char *psz2 )
{
	if( !psz1 && !psz2 )return 0;
	else if( !psz1 )return 1;
	else if( !psz2 )return -1;
	else return _tcscmp( psz1, psz2 );
}
inline int  _tcsncmpz(const _char *psz1, const _char *psz2, int cb )
{
	if( !psz1 && !psz2 )return 0;
	else if( !psz1 )return 1;
	else if( !psz2 )return -1;
	else return _tcsncmp( psz1, psz2, cb );
}
inline int  _tcsicmpz(const _char *psz1, const _char *psz2 )
{
	if( !psz1 && !psz2 )return 0;
	else if( !psz1 )return 1;
	else if( !psz2 )return -1;
	else return _tcsicmp( psz1, psz2 );
}
inline int  _tcsnicmpz(const _char *psz1, const _char *psz2, int cb )
{
	if( !psz1 && !psz2 )return 0;
	else if( !psz1 )return 1;
	else if( !psz2 )return -1;
	else return _tcsnicmp( psz1, psz2, cb );
}

inline _char *  _tcsncpyz(_char *d, const _char *s, size_t cb )
{
	if( !cb )return d;
	if( !s )
	{
		*d = 0;
		return d;
	}
	_char	*pr = _tcsncpy( d, s, cb );d[cb-1]=0;return pr;
}

inline _char *  _tcsncatz(_char *d, const _char *s, size_t cb )
{
	if( !cb )return d;
	if( !s )
	{
		*d = 0;
		return d;
	}
	_char	*pr = _tcsncat( d, s, cb );d[cb-1]=0;return pr;
}

inline void strreplace( _char *psz, const _char *psz_find, _char *psz_replace )
{
	_char	*pfound = _tcsstr( psz, psz_find );
	if( !pfound )return;

	long	len_replace = _tcslen( psz_replace );

	_char	*ps = pfound+_tcslen( psz_find );
	_char	*pd = pfound + len_replace;

	if( (long)ps > (long)pd )
	{
		while( *ps )
		{*pd = *ps; ps++; pd++;}
		*pd = *ps;
	}
	else
	{
		long	len_s = _tcslen( ps );

		_char	*pse = ps+len_s;
		_char	*pde = pd+len_s;
		while( pse != ps )
		{*pde = *pse;pde--;pse--;}
		*pde = *pse;
	}
	memcpy( pfound, psz_replace, len_replace );
}

inline _char *strdecode( _char *psz, const _char *psrc, size_t cb )
{
	_char	*pret = psz;
	for( unsigned c = 0; c < cb-1 && *psrc; c++, psz++, psrc++ )
	{
		if( *psrc == '%' )
		{	
			psrc++;
			if( *psrc != '%' )
			{
				int	ch;
				::_sscanf( psrc, _T("%02x"), &ch );
				*psz = ch;
				psrc++;
				continue;
			}
		}
		*psz = *psrc;
	}
	*psz = 0;
	return pret;
}

inline _char *strencode( _char *psz, const _char *psrc, const _char *psz_repl, size_t cb )
{
	_char	*pret = psz;
	for( unsigned c = 0; c < cb-1 && *psrc; psz++, psrc++ )
	{
		bool	fReplaced = false;
		for( const _char *ptest = psz_repl; *ptest; ptest++ )
		{
			if( *psrc == *ptest )
			{
				*psz = '%';
				psz++;
				_sprintf( psz, _T("%02x"), *psrc );
				psz++;
				fReplaced = true;
				break;
			}
		}
		if( fReplaced )
			continue;

		if( *psrc == '%' )
		{
			*psz = '%';
			psz++;
			*psz = '%';
			continue;
		}
		*psz = *psrc;
	}
	*psz = 0;
	return pret;
}

inline bool __cmpguid( const GUID &g1, const GUID &g2 )
{
	return *((long*)&g1+0) == *((long*)&g2+0)&&
		   *((long*)&g1+1) == *((long*)&g2+1)&&
		   *((long*)&g1+2) == *((long*)&g2+2)&&
		   *((long*)&g1+3) == *((long*)&g2+3);

}

class _enable_ok_timer_class
{
public:
	inline static void CALLBACK enable_proc(HWND hwnd, UINT , UINT_PTR, DWORD)
	{
		HWND	hwnd_ok = ::GetDlgItem( hwnd, IDOK );
		::EnableWindow( hwnd_ok, true );
		::KillTimer( hwnd, 1000 );
	}
};

inline void enable_ok_timer( HWND hwnd )
{
	HWND	hwnd_ok = ::GetDlgItem( hwnd, IDOK );

	::EnableWindow( hwnd_ok, false );
	::SetTimer( hwnd, 1000, 1000, _enable_ok_timer_class::enable_proc );
}

inline bool get_long_path_name( const char *psz_from, char *psz, size_t cb )
{
	static DWORD (WINAPI *pfnGetLongPathName)(LPCTSTR,LPTSTR,DWORD) = 0;
	if( !pfnGetLongPathName )(FARPROC&)pfnGetLongPathName = 
		::GetProcAddress( ::GetModuleHandle(_T("kernel32.dll") ), _T("GetLongPathNameA") );
	if( pfnGetLongPathName )
		pfnGetLongPathName( psz_from, psz, cb );

	return true;
}


#endif//__misc_utils_h__
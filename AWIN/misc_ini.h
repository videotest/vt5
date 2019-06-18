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

#ifndef __misc_ini_h__
#define __misc_ini_h__

#include "stdio.h"

#define MAX_INI_SIZE	32767
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

inline const _char *__profile( const _char *psz )
{
	if( psz )return psz;
	static _char sz[MAX_PATH] =_T("");
	
	if( !_tcslen( sz ) )
	{
		GetModuleFileName( 0, sz, sizeof( sz ) );
		_tcscpy( _tcsrchr( sz, '.' ), _T(".ini") );
	}
	return sz;
}

inline const _char *__sect_window( const _char *psz )
{
	if( psz )return psz;
	static const _char *psz_w =_T("Windows");
	return psz_w;
}

inline const _char *__def_string( const _char *psz )
{
	if( psz )return psz;
	static const _char *psz_d =_T("");
	return psz_d;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
//записать в profile int
inline
BOOL WritePrivateProfileInt(
  LPCTSTR lpAppName,  // section name
  LPCTSTR lpKeyName,  // key name
  long lValue,   // string to add
  LPCTSTR lpFileName  // initialization file
)
{
	_char	sz[10];
	::_sprintf( sz, _T("%d"), lValue );
	return ::WritePrivateProfileString( lpAppName, lpKeyName, sz, lpFileName );
}

//очистить секцию в profile
inline
BOOL DeletePrivateProfileSection( 
  LPCTSTR lpAppName,  // section name
  LPCTSTR lpFileName  // initialization file
)
{
	return ::WritePrivateProfileString( lpAppName, 0, 0, lpFileName ); 
}

//удалить значение из profile
inline
BOOL DeletePrivateProfileValue( 
  LPCTSTR lpAppName,  // section name
  LPCTSTR lpKeyName,  // key name
  LPCTSTR lpFileName  // initialization file
)
{
	return ::WritePrivateProfileString( lpAppName, lpKeyName, 0, lpFileName ); 
}

inline void CreatePrivateProfileSection( const _char *psz_section, const _char *psz_parent, const _char *psz_ini )
{
	_char	buffer[MAX_INI_SIZE];
	::GetPrivateProfileSection( psz_section, buffer, sizeof(buffer), psz_ini );

	if( _tcslen( buffer ) != 0 )
		return;

	::DeletePrivateProfileSection( psz_section, psz_ini );

	::GetPrivateProfileSection( psz_parent, buffer, sizeof(buffer), psz_ini );
	for( _char	*pend = buffer; *pend; pend = pend+_tcslen( pend )+1 );

	::_sprintf( pend, _T("[%s]"), psz_section );
	*(pend+_tcslen( pend )+1)=0;
	
	::WritePrivateProfileSection( psz_parent, buffer, psz_ini );
	::WritePrivateProfileSection( psz_section, _T("\0"), psz_ini );
}

inline bool GetPrivateProfileBool( const _char *psz_section, const _char *psz_entry, bool bDef, const _char *psz_ini )
{
	_char	sz[20];
	GetPrivateProfileString( psz_section, psz_entry, _T(""), sz, sizeof(sz), psz_ini );
	if( !_tcslen( sz ) )return bDef;

	if( !_tcsicmp( sz, _T("yes") )||
		!_tcscmp( sz, _T("1") )||
		!_tcsicmp( sz, _T("true") )||
		!_tcsicmp( sz, _T("y") ) )return true;

	if( !_tcsicmp( sz, _T("no") )||
		!_tcscmp( sz, _T("0") )||
		!_tcsicmp( sz, _T("false") )||
		!_tcsicmp( sz, _T("n") ) )return false;

	return bDef;
}

inline void WritePrivateProfileBool( const _char *psz_section, const _char *psz_entry, bool bVal, const _char *psz_ini )
{
	_char	sz[20] =_T("yes");
	::GetPrivateProfileString( psz_section, psz_entry, _T(""), sz, sizeof(sz), psz_ini );

	if( !_tcscmp( sz, _T("1") ) || !_tcscmp( sz, _T("0") ) )
		_tcscpy( sz, bVal ?_T("1"):_T("0") );
	else if( !_tcsicmp( sz, _T("true") ) || !_tcsicmp( sz, _T("false") ) )
		_tcscpy( sz, bVal ?_T("true") :_T("false") );
	else if( !_tcsicmp( sz, _T("y") ) || !_tcsicmp( sz, _T("n") ) )
		_tcscpy( sz, bVal ?_T("y") :_T("n") );
	else
		_tcscpy( sz, bVal ?_T("yes"):_T("no") );

	::WritePrivateProfileString( psz_section, psz_entry, sz, psz_ini );
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline DWORD _GetPrivateProfileString(
    const _char *lpAppName, const _char *lpKeyName, const _char *lpDefault,
    _char *lpReturnedString, DWORD nSize, const _char *lpFileName )
{	return ::GetPrivateProfileString( lpAppName, lpKeyName, __def_string(lpDefault), 
		lpReturnedString, nSize, __profile( lpFileName ) );}

inline DWORD _WritePrivateProfileString(
    const _char *lpAppName, const _char *lpKeyName, const _char *lpString, const _char *lpFileName )
{	return WritePrivateProfileString( lpAppName, lpKeyName, lpString, __profile( lpFileName ) );}

inline UINT _GetPrivateProfileInt(
    const _char *lpAppName, const _char *lpKeyName, UINT def, const _char *lpFileName )
{	return ::GetPrivateProfileInt( lpAppName, lpKeyName, def, __profile( lpFileName ) );}

inline DWORD _WritePrivateProfileInt(
    const _char *lpAppName, const _char *lpKeyName, UINT lval, const _char *lpFileName )
{	return WritePrivateProfileInt( lpAppName, lpKeyName, lval, __profile( lpFileName ) );}

inline bool _GetPrivateProfileBool( const _char *psz_section, const _char *psz_entry, bool bDef, const _char *psz_ini )
{	return ::GetPrivateProfileBool( psz_section, psz_entry, bDef, __profile( psz_ini ) ); }

inline void _WritePrivateProfileBool( const _char *psz_section, const _char *psz_entry, bool bVal, const _char *psz_ini )
{	::WritePrivateProfileBool( psz_section, psz_entry, bVal, __profile( psz_ini ) ); }


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//

#define GPP_NOZIZE	0x01

inline void WritePrivateProfilePlacement( const _char *psz_section, const _char *psz_entry, HWND hwnd, const _char *psz_ini )
{
	WINDOWPLACEMENT	wp;
	ZeroMemory( &wp, sizeof( wp ) );
	wp.length = sizeof( wp );
	wp.flags = WPF_SETMINPOSITION|WPF_RESTORETOMAXIMIZED;

	::GetWindowPlacement( hwnd, &wp );

	_char	sz[100];

	_sprintf( sz, _T("%d (%d, %d) (%d, %d), (%d, %d, %d,%d)"), wp.showCmd, wp.ptMinPosition.x, wp.ptMinPosition.y,
		wp.ptMaxPosition.x, wp.ptMaxPosition.y, wp.rcNormalPosition.left, wp.rcNormalPosition.top,
		wp.rcNormalPosition.right, wp.rcNormalPosition.bottom );

	::WritePrivateProfileString( __sect_window(psz_section), psz_entry, sz, __profile(psz_ini) );
}


inline long GetPrivateProfilePlacement( const _char *psz_section, const _char *psz_entry, HWND hwnd, const _char *psz_ini, unsigned flags )
{
	RECT	rect, rect_desk;
	::GetWindowRect( hwnd, &rect );
	::GetClientRect( ::GetDesktopWindow(), &rect_desk );

	_char	sz[100];
	GetPrivateProfileString( __sect_window(psz_section), psz_entry, _T(""), sz, sizeof( sz ), __profile(psz_ini) );

	if( !_tcslen( sz ) )return false;


	WINDOWPLACEMENT	wp;
	ZeroMemory( &wp, sizeof( wp ) );
	wp.length = sizeof( wp );
	wp.flags = WPF_SETMINPOSITION|WPF_RESTORETOMAXIMIZED;

	if( _sscanf( sz, _T("%d (%d, %d) (%d, %d), (%d, %d, %d,%d)"), &wp.showCmd, &wp.ptMinPosition.x, &wp.ptMinPosition.y,
		&wp.ptMaxPosition.x, &wp.ptMaxPosition.y, &wp.rcNormalPosition.left, &wp.rcNormalPosition.top,
		&wp.rcNormalPosition.right, &wp.rcNormalPosition.bottom ) != 9 )
		return false;

	
	if( flags & GPP_NOZIZE )
	{
		wp.rcNormalPosition.right = wp.rcNormalPosition.left+rect.right-rect.left;
		wp.rcNormalPosition.bottom = wp.rcNormalPosition.top+rect.bottom-rect.top;
	}



	if( wp.rcNormalPosition.right >= rect_desk.right )
	{
		wp.rcNormalPosition.left -= wp.rcNormalPosition.right-rect_desk.right+1;
		wp.rcNormalPosition.right -= wp.rcNormalPosition.right-rect_desk.right+1;
	}

	if( wp.rcNormalPosition.bottom >= rect_desk.bottom )
	{
		wp.rcNormalPosition.top -= wp.rcNormalPosition.bottom-rect_desk.bottom+1;
		wp.rcNormalPosition.bottom -= wp.rcNormalPosition.bottom-rect_desk.bottom+1;
	}

	if( wp.rcNormalPosition.left <= rect_desk.left )
	{
		wp.rcNormalPosition.right += rect_desk.left-wp.rcNormalPosition.left+1;
		wp.rcNormalPosition.left += rect_desk.left-wp.rcNormalPosition.left+1;
	}

	if( wp.rcNormalPosition.top <= rect_desk.top )
	{
		wp.rcNormalPosition.bottom += rect_desk.top-wp.rcNormalPosition.top+1;
		wp.rcNormalPosition.top += rect_desk.top-wp.rcNormalPosition.top+1;
	}

	return ::SetWindowPlacement( hwnd, &wp );
}

inline bool WritePrivateProfileColumns( HWND hwnd_list, const _char *psz_section, const _char *psz_entry, const _char *psz_file )
{
	_char	sz[200] = "";

	HWND	hwnd_header = ListView_GetHeader( hwnd_list );
	int	count = Header_GetItemCount( hwnd_header );

	for( int i = 0; i < count; i++ )
	{
		if( i )_tcscat( sz, ", " );
		::sprintf( sz+_tcslen( sz ), "%d", ListView_GetColumnWidth( hwnd_list, i ) );
	}

	_WritePrivateProfileString( __sect_window(psz_section), psz_entry,  sz, __profile( psz_file ) );

	return true;
}

inline bool GetPrivateProfileColumns( HWND hwnd_list, const _char *psz_section, const _char *psz_entry, const _char *psz_file )
{
	_char	sz[200] = "";
	_GetPrivateProfileString( __sect_window(psz_section), psz_entry,  sz, sz, sizeof( sz ), __profile( psz_file ) );

	HWND	hwnd_header = ListView_GetHeader( hwnd_list );
	int	count = Header_GetItemCount( hwnd_header );

	_char	*p = _tcstok( sz, ", " );

	for( int i = 0; i < count; i++ )
	{
		int	width = 0;
		if( p && sscanf( p, "%d", &width ) == 1 )
			ListView_SetColumnWidth( hwnd_list, i, width );
		p = _tcstok( 0, ", " );
	}

	return true;	
}





#endif //__misc_ini_h__
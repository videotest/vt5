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

#ifndef __dlg_impl__
#define __dlg_impl__

#include "win_base.h"
#include "misc_module.h"

#define RT_DLGINIT  MAKEINTRESOURCE(240)


#ifndef _NO_DLG_DEBUG_SYMBOLS

#include "\vt5\awin\misc_ini.h"
inline void add_debug_symbols( HWND hwnd )
{
	bool	add_symbols =  _GetPrivateProfileBool( "Debug", "AddDebugSymbolToDlgControls", false, 0 );
	_WritePrivateProfileBool( "Debug", "AddDebugSymbolToDlgControls", add_symbols, 0 );

	if( add_symbols )
	{
		char	sz[10000];

		hwnd = ::GetWindow( hwnd, GW_CHILD );
		while( hwnd )
		{
			::GetWindowText( hwnd, sz, sizeof( sz ) );
			if( _tcslen( sz ) )
			{
				_tcsncat( sz, "*", sizeof( sz ) );
				sz[sizeof( sz )-1] = 0;
				::SetWindowText( hwnd, sz );
			}
			hwnd = ::GetWindow( hwnd, GW_HWNDNEXT );
		}
	}
}

#endif //_NO_DLG_DEBUG_SYMBOLS


inline 
int __stdcall subclass_dlg_proc(HWND hwnd, UINT m, WPARAM w, LPARAM l )
{
	if( m == WM_INITDIALOG )
	{
		win_impl	*pwi = (win_impl*)l;
		pwi->subclass( hwnd, true );
		return pwi->handle_message( m, w, l );
	}
	return 0;//DefDlgProc( hwnd, m, w, l );
}


class dlg_impl : public win_impl
{
public:
	dlg_impl( uint id );
public:
	INT_PTR do_modal( HWND hwndParent );
	int do_modeless( HWND hwndParent );
public:
	virtual long on_initdialog();
	virtual void on_ok()				{end_dialog( IDOK );}
	virtual void on_cancel()			{end_dialog( IDCANCEL );}
	virtual void end_dialog( int idc )	{m_result = idc; EndDialog( handle(), idc );}
	virtual long translate( MSG *pmsg )	{return ::IsDialogMessage( handle(), pmsg );}
protected:
	virtual LRESULT on_paint()		{ return call_default(); };
	virtual LRESULT on_command(uint cmd);
	virtual long handle_translate( MSG *pmsg )	{return IsDialogMessage( handle(), pmsg );};
protected:
	uint	m_idTemplate;
	long	m_result;
};

inline 	dlg_impl::dlg_impl( uint id )
{		m_result = -2; m_idTemplate = id;		m_proc_def = (WNDPROC)DefDlgProc;	}
inline 	INT_PTR dlg_impl::do_modal( HWND hwndParent )
{
	return ::DialogBoxParam( module::hrc(), MAKEINTRESOURCE(m_idTemplate), hwndParent, (DLGPROC)&subclass_dlg_proc, (long)this );
}
inline 	int dlg_impl::do_modeless( HWND hwndParent )
{
	HINSTANCE hInstance = module::hrc();
	return CreateDialogParam( hInstance, MAKEINTRESOURCE(m_idTemplate), hwndParent, (DLGPROC)&subclass_dlg_proc, (long)this )!=0;
}

inline 	LRESULT dlg_impl::on_command( uint cmd )
{
	switch( cmd )
	{
	case IDOK:on_ok();return true;
	case IDCANCEL:on_cancel();return true;
	};
	return false;
}

inline long dlg_impl::on_initdialog()
{
	long	lres = win_impl::on_initdialog();

	//load from RT_INITDLG resource
	HRSRC hrc = ::FindResource( module::hrc(), MAKEINTRESOURCE(m_idTemplate), RT_DLGINIT );
	if( !hrc )return lres;

	HGLOBAL	hres = ::LoadResource( module::hrc(), hrc );
	if( !hres )return lres;

	WORD *pres = (WORD*)::LockResource( hres );

	while( *pres )
	{
		unsigned	idc = *pres++;
		unsigned	m = *pres++;

		unsigned	size = *(unsigned*)pres;
		pres+=2;
//translate old messages
#define WIN16_LB_ADDSTRING  0x0401
#define WIN16_CB_ADDSTRING  0x0403
#define AFX_CB_ADDSTRING    0x1234

		if (m == AFX_CB_ADDSTRING)
			m = CBEM_INSERTITEM;
		else if (m == WIN16_LB_ADDSTRING)
			m = LB_ADDSTRING;
		else if (m == WIN16_CB_ADDSTRING)
			m = CB_ADDSTRING;
//combo_ex
		if (m == CBEM_INSERTITEM)
		{
			COMBOBOXEXITEM item;
			item.mask = CBEIF_TEXT;
			item.iItem = -1;
			item.pszText = (_char*)pres;

			::SendDlgItemMessage( handle(), idc, m, 0, (LPARAM)&item );
		}
		else if (m == LB_ADDSTRING || m == CB_ADDSTRING)
		{
			::SendDlgItemMessageA( handle(), idc, m, 0, (LPARAM)pres );
		}
		
		pres = (WORD*)((byte*)pres+size);
	}


//	::UnlockResource( hres );
	::FreeResource( hres );

#ifdef _NO_DLG_DEBUG_SYMBOLS
	add_debug_symbols( handle() );
#endif //_NO_DLG_DEBUG_SYMBOLS

	return lres;
}




#endif // __dlg_impl__
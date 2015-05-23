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

#ifndef __page_impl_h__
#define __page_impl_h__

#include "win_dlg.h"

class page_impl : public dlg_impl
{
public:
	page_impl( unsigned id ) : dlg_impl( id )
	{
	}

	void set_modified( bool is_modified = true )
	{
		if( is_modified )
			PropSheet_Changed( GetParent( handle() ), handle() );
		else
			PropSheet_UnChanged( GetParent( handle() ), handle() );
	}

	virtual long on_notify( uint idc, NMHDR *pnmhdr )
	{
		if( idc == 0 && pnmhdr->code == PSN_APPLY )
		{
			on_ok();
			return false;
		}
		return false;
	}

	virtual void on_ok()		
	{
		set_modified( false );
	}

	virtual bool subclass( HWND hwnd, bool f_nccreate = false )
	{
		if( !win_impl::subclass( hwnd, f_nccreate ) )return false;
//		SetWindowLong( hwnd, DWL_DLGPROC, (LONG)&m_thunk );
		return true;
	}


	static UINT CALLBACK __create_proc(
		HWND hwnd,
		UINT m,
		PROPSHEETPAGE *ppsp
	)
	{
		if( m == PSPCB_CREATE )
		{
			page_impl	*p = (page_impl*)ppsp->lParam;
			p->subclass( hwnd, false );
		}
		return 1;
	}

	static 
	int __stdcall subclass_page_proc(HWND hwnd, UINT m, WPARAM w, LPARAM l )
	{
		if( m == WM_INITDIALOG )
		{
			PROPSHEETPAGE	*p = (PROPSHEETPAGE*)l;
			win_impl	*pwi = (win_impl*)p->lParam;
			pwi->subclass( hwnd, true );
			return pwi->handle_message( m, w, l );
		}
		return 0;//DefDlgProc( hwnd, m, w, l );
	}


	HPROPSHEETPAGE create_page()
	{
		PROPSHEETPAGE	page;
		ZeroMemory( &page, sizeof( page ) );
		page.dwSize = sizeof( page );
		page.dwFlags = PSP_DEFAULT|PSP_USECALLBACK;
		page.hInstance = module::hrc();
		page.pszTemplate = MAKEINTRESOURCE(m_idTemplate);
		page.pszTitle = 0;
		page.pfnDlgProc = subclass_page_proc;
		page.lParam = (LPARAM)this;
		page.pfnCallback = 0;
		return CreatePropertySheetPage( &page );
	}
};


#endif //__page_impl_h__
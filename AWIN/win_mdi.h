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

#ifndef __win_mdi_h__
#define __win_mdi_h__

#include "win_base.h"

class mdi_client : public win_impl
{
public:
	mdi_client()
	{
	}
	virtual bool create( DWORD style, HWND parent )
	{
		CLIENTCREATESTRUCT	ccs;
		ccs.hWindowMenu = 0;
		ccs.idFirstChild = 1001;
		m_hwnd = ::CreateWindowEx( 0, "mdiclient", 0, style, 0, 0, 0, 0, parent, (HMENU)1000, module::hinst(), (void*)&ccs );
		if( m_hwnd == 0 )
		{
			//error( "can't create mdi client" );
			return false;
		}
		subclass( m_hwnd );

		return true;
	}
};

class win_mdi_frame : public win_impl
{
public:
	win_mdi_frame()
	{
		m_pclient = 0;
	}
	~win_mdi_frame()
	{
		delete m_pclient;
	}

	virtual mdi_client	*create_client_object()
	{		return new mdi_client;	}

	virtual long on_create( CREATESTRUCT *pcs )			
	{
		m_pclient = create_client_object();
		m_pclient->create( WS_CHILD|WS_VISIBLE, handle() );
		return call_default();
	}

	virtual long on_size( short cx, short cy, ulong fSizeType )	
	{
		if( m_pclient )
		{
			::SetWindowPos( mdi_handle(), 0, 0, 0, cx, cy, SWP_NOOWNERZORDER );
		}
		return call_default();
	}
	virtual long on_getinterface()							
	{
		return (long)this;
	}
	virtual long on_destroy()
	{
		DestroyWindow( mdi_handle() );

		PostQuitMessage( 0 );
		return win_impl::on_destroy();
	}
	virtual long call_default()
	{
		return DefFrameProc( handle(), m_pclient?mdi_handle():0, m_current_message.message, m_current_message.wParam, m_current_message.lParam );
	}
public:
	HWND	mdi_handle()	{return  m_pclient->handle();}
protected:
	mdi_client	*m_pclient;
};


class mdi_child : public win_impl
{
public:
	mdi_child()
	{
		m_proc_def = DefMDIChildProc;
		m_kill_on_destroy = true;
	}
	virtual ~mdi_child()
	{
	}
	virtual bool create( DWORD style, const RECT rect, const char *pszTitle = 0, HWND parent = 0, HMENU hmenu = 0, const char *pszClass = 0 )
	{
		MDICREATESTRUCT		mcs;
		mcs.szClass = win_class::get_class_mdi();
		mcs.szTitle = pszTitle;;
		mcs.hOwner = module::hinst();
		mcs.x = rect.left;
		mcs.y = rect.top;
		mcs.cx = rect.right-rect.left;
		mcs.cy = rect.bottom-rect.top;
		mcs.style = style;
		mcs.lParam = (LPARAM)this;

		win_mdi_frame	*pframe = (win_mdi_frame*)::SendMessage( parent, WM_GETINTERFACE, 0, 0 );
		SendMessage( pframe->mdi_handle(), WM_MDICREATE, 0, (LPARAM)&mcs );
		return m_hwnd != 0;
	}

	virtual long on_close()
	{
		return SendMessage( ::GetParent( handle() ), WM_MDIDESTROY, (WPARAM)handle(), 0 );
	}

};

#endif //__win_mdi_h__
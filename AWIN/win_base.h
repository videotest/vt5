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

#ifndef __win_impl_h__
#define __win_impl_h__

#include "message_filter.h"
#include "misc_module.h"
#include <atlstdthunk.h>

inline HWND GetTopLevelParent( HWND hwnd )
{
	HWND	hwnd_p = GetParent( hwnd );
	while( hwnd_p )
	{
		hwnd = hwnd_p;
		hwnd_p = GetParent( hwnd );
	}
	return hwnd;
}


#define WM_GETINTERFACE		(WM_USER+779)
#define WM_NOTYFYREFLECT	(WM_USER+1002)
#define WM_WINDOWFOCUSED	(WM_USER+1003)


LONG_PTR __stdcall subclass_proc(HWND hwnd, UINT m, WPARAM w, LPARAM l );
LONG_PTR __stdcall subclassed_proc(HWND hwnd, UINT m, WPARAM w, LPARAM l );
LONG_PTR __stdcall subclass_mdi_proc(HWND hwnd, UINT m, WPARAM w, LPARAM l );

class win_class
{
public:
	static const _char	*get_class_default()
	{
		static bool registred = 0;
		static _char szDefClass[] = _T("defwindowclass");

		if( !registred )
		{
			WNDCLASS	wndclass;
			ZeroMemory( &wndclass, sizeof( wndclass ) );
			wndclass.lpszClassName = szDefClass;
			wndclass.hInstance = module::hinst();
			wndclass.lpfnWndProc = subclass_proc;
			wndclass.hbrBackground = (HBRUSH)::GetStockObject( WHITE_BRUSH );
			wndclass.hCursor = ::LoadCursor(0, MAKEINTRESOURCE(IDC_ARROW));

			::RegisterClass( &wndclass );		
			registred = true;
		}
		return szDefClass;
	}
	static const _char	*get_class_mdi()
	{
		static bool registred = 0;
		static _char szDefClass[] = _T("defmdiclass");

		if( !registred )
		{
			WNDCLASS	wndclass;
			ZeroMemory( &wndclass, sizeof( wndclass ) );
			wndclass.lpszClassName = szDefClass;
			wndclass.hInstance = module::hinst();
			wndclass.lpfnWndProc = subclass_mdi_proc;
			wndclass.hbrBackground = (HBRUSH)::GetStockObject( WHITE_BRUSH );
			wndclass.hCursor = ::LoadCursor(0, MAKEINTRESOURCE(IDC_ARROW));

			::RegisterClass( &wndclass );		
			registred = true;
		}
		return szDefClass;
	}
};

#pragma pack(push)
#pragma pack(1)
struct _wndproc_thunk
{
	DWORD   m_mov;          // mov dword ptr [esp+0x4], pThis (esp+0x4 is hWnd)
	DWORD   m_this;         //
	BYTE    m_jmp;          // jmp WndProc
	DWORD   m_relproc;      // relative jmp
};
#pragma pack(pop)

//__declspec(novtable)
class win_impl : public message_filter
{
protected:
	ATL::CStdCallThunk	m_thunk;
public:
	win_impl();
	virtual ~win_impl(){};
public:
	virtual bool subclass( HWND hwnd, bool f_nccreate = false );
	virtual bool detach();
	virtual bool create( DWORD style, const RECT &rect, const _char *pszTitle = 0, HWND parent = 0, HMENU hmenu = 0, const _char *pszClass = 0 );
	virtual bool create_ex( DWORD style, const RECT &rect, const _char *pszTitle = 0, HWND parent = 0, HMENU hmenu = 0, const _char *pszClass = 0, DWORD ex_style = 0 );
public:
	HWND	handle()	{return m_hwnd;}
public:
	virtual void	handle_init(){};
	virtual LRESULT	handle_message( UINT m, WPARAM w, LPARAM l );
protected:
	virtual LRESULT on_create(CREATESTRUCT *pcs)			{ handle_init(); return call_default(); };
	virtual LRESULT on_close()								{ return call_default(); };
	virtual LRESULT on_destroy()							{ return call_default(); };
	virtual LRESULT on_paint();
	virtual LRESULT on_ncdestroy();
	virtual LRESULT on_size(short cx, short cy, UINT_PTR fSizeType)	{ return call_default(); }
	virtual LRESULT on_show(bool bShow, LPARAM status)		{ return call_default(); }
	virtual long on_initdialog()						{return 0;};
	virtual LRESULT on_timer(UINT_PTR lEvent)				{ return call_default(); };
	virtual LRESULT on_command(UINT_PTR cmd)					{ return call_default(); };
	virtual LRESULT on_syscommand(uint cmd)				{ return call_default(); };
	virtual LRESULT on_mousemove(const _point &point)	{ return call_default(); };
	virtual LRESULT on_lbuttondown(const _point &point)	{ return call_default(); };
	virtual LRESULT on_lbuttonup(const _point &point)	{ return call_default(); };
	virtual LRESULT on_rbuttondown(const _point &point)	{ return call_default(); };
	virtual LRESULT on_rbuttonup(const _point &point)	{ return call_default(); };
	virtual LRESULT on_getminmaxinfo(MINMAXINFO *pmminfo){ return call_default(); };
	virtual LRESULT on_keydown(long nVirtKey)			{ return call_default(); }
	virtual LRESULT on_keyup(long nVirtKey)				{ return call_default(); }
	virtual LRESULT on_char(long nVirtKey)				{ return call_default(); }
	virtual LRESULT on_getdlgcode(MSG* p)				{ return call_default(); }
	virtual LRESULT on_notify(uint idc, NMHDR *pnmhdr)						{ return notify_reflect(); }
	virtual LRESULT on_measureitem(int idc, MEASUREITEMSTRUCT *pmsr)			{ return notify_reflect(); }
	virtual LRESULT on_drawitem(int idc, DRAWITEMSTRUCT *pdrw)				{ return notify_reflect(); }
	virtual LRESULT on_compareitem( int idc, COMPAREITEMSTRUCT *pdrw )			{return notify_reflect();}
	virtual LRESULT on_dropfiles(HANDLE hdrop)			{ return call_default(); };
	virtual LRESULT on_setfocus(HWND hwndOld)			{ return call_default(); };
	virtual LRESULT on_killfocus(HWND hwndOld)			{ return call_default(); };
	virtual LRESULT on_erasebkgnd(HDC)					{ return call_default(); }
	virtual LRESULT on_hscroll(unsigned code, unsigned pos, HWND hwndScroll)	{ return call_default(); }
	virtual LRESULT on_vscroll(unsigned code, unsigned pos, HWND hwndScroll)	{ return call_default(); }
	virtual LRESULT on_setcursor(unsigned code, unsigned hit)				{ return call_default(); }
	virtual LRESULT on_activate(unsigned code, HWND hwnd)					{ return call_default(); }
	virtual LRESULT on_mouseactivate(unsigned hit, unsigned message)			{ return call_default(); }

	virtual LRESULT call_default();
	virtual LRESULT notify_reflect();
	virtual long handle_reflect_message( MSG *pmsg, long *plProcessed );
	virtual long on_notify_reflect( NMHDR *pnmhdr, long *plProcessed )		{return 0;}
	virtual long on_getinterface()							{return 0;}
	virtual LRESULT on_helpinfo(HELPINFO *phi)				{ return call_default(); }
	virtual long on_windowfocused( HWND hwnd )				{return 0;}
	virtual LRESULT on_lbuttondblclk(const _point &point)	{ return call_default(); };
	virtual LRESULT on_rbuttondblclk(const _point &point)	{ return call_default(); };
	virtual LRESULT on_contextmenu(const _point &point)		{ return call_default(); }
	virtual LRESULT on_cancelmode()							{ return call_default(); }
public:	//wrappers
	HWND get_dlg_item( UINT idc )								{return ::GetDlgItem( handle(), idc );}
	BOOL get_window_rect( RECT *pr )							{return ::GetWindowRect( handle(), pr ); }
	BOOL get_client_rect( RECT *pr )							{return ::GetClientRect( handle(), pr ); }
	BOOL move_window( const RECT &rect, long lRepaint = true )	{return ::MoveWindow( handle(), rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, lRepaint );}
	LRESULT send_message( UINT m, WPARAM w = 0, LPARAM l = 0)		{return ::SendMessage( handle(), m, w, l );}
	UINT get_dlg_item_text( UINT idc, _char *psz, int cb )		{return ::GetDlgItemText( handle(), idc, psz, cb );}
	UINT is_dlg_button_checked( UINT idc )						{return ::IsDlgButtonChecked( handle(), idc );}
	BOOL check_dlg_button( UINT idc, int check )				{return ::CheckDlgButton( handle(), idc, check );}
	BOOL set_dlg_item_text( UINT idc, const _char *psz )			{return ::SetDlgItemText( handle(), idc, psz );}

protected:
	HWND			m_hwnd;
	MSG				m_current_message;
public:
	long			m_lock_delete, 
					m_kill_on_destroy, 
					m_call_old;
protected:
	WNDPROC			m_proc_old, m_proc_def;

};


inline LRESULT win_impl::notify_reflect()
{
	HWND	hwnd = 0;

	if( m_current_message.message == WM_NOTIFY )
		hwnd = ((NMHDR*)m_current_message.lParam)->hwndFrom;
	else
		hwnd = ::GetDlgItem( handle(), (int)m_current_message.wParam );
	
	long	lProcessed = false;
	LRESULT	lret_reflect = 0;
	
	if( hwnd )
		lret_reflect = SendMessage( hwnd, WM_NOTYFYREFLECT, (WPARAM)&lProcessed, (LPARAM)&m_current_message );
	
	return lProcessed?lret_reflect:call_default();
};


inline LRESULT	win_impl::on_paint()
{
	if( m_proc_old )return call_default();;

	PAINTSTRUCT	paint;
	HDC	hdc = ::BeginPaint( m_hwnd, &paint );
	::EndPaint( m_hwnd, &paint );
	return 1;
}

inline LRESULT	win_impl::on_ncdestroy()
{
	LRESULT	lret = call_default();
	detach();
	
	return lret;
}

inline win_impl::win_impl()
{
	m_call_old = false;
	m_proc_old = 0;
	m_proc_def = 0;

	m_kill_on_destroy = 0;
	m_lock_delete = 0;
	m_hwnd = 0;
	memset( &m_current_message, 0, sizeof( m_current_message ) );
	m_proc_def = DefWindowProc;
}

inline bool win_impl::subclass( HWND hwnd, bool f_nccreate )
{
	//m_call_old = !f_nccreate;

	//m_proc_old = (WNDPROC)::GetWindowLong( hwnd, GWL_WNDPROC );
	//m_hwnd = hwnd;

	//LONG	lproc = (LONG)&m_thunk.m_mov;
	//long	lthunk = sizeof(m_thunk);

	//m_thunk.m_mov = 0x042444C7;  //C7 44 24 0C
	//m_thunk.m_this = (DWORD)/*(message_filter*)*/this;
	//m_thunk.m_jmp = 0xe9;
	//m_thunk.m_relproc = (int)subclassed_proc - (lproc+lthunk);

	//::FlushInstructionCache(GetCurrentProcess(), &m_thunk, sizeof(m_thunk));
	//::SetWindowLong( m_hwnd, GWL_WNDPROC, lproc );

	//if( !f_nccreate )
	//	handle_init();

	//return true;
	
	m_call_old = !f_nccreate;

	m_proc_old = (WNDPROC)::GetWindowLongPtr( hwnd, GWLP_WNDPROC );
	m_hwnd = hwnd;

	m_thunk.Init((DWORD_PTR)subclassed_proc,this);

	::SetWindowLongPtr( m_hwnd, GWLP_WNDPROC, (LONG_PTR)m_thunk.GetCodeAddress() );

	if( !f_nccreate )
		handle_init();

	return true;
}

inline bool win_impl::detach()
{
	if( !m_hwnd || !m_proc_old )return false;
	::SetWindowLongPtr( m_hwnd, GWLP_WNDPROC, (LONG_PTR)m_proc_old );

	CallWindowProc( m_proc_old, m_hwnd, m_current_message.message,
		m_current_message.wParam, m_current_message.lParam );

	m_hwnd  = 0;
	m_proc_old = 0;

	return true;
}

inline 
bool win_impl::create( DWORD style, const RECT &rect, const _char *pszTitle, HWND parent, HMENU hmenu, const _char *pszClass )
{
	m_hwnd = 0;

	int	cx = (rect.right == CW_USEDEFAULT)?CW_USEDEFAULT:rect.right-rect.left;
	int	cy = (rect.bottom == CW_USEDEFAULT)?CW_USEDEFAULT:rect.bottom-rect.top;
	HWND hwnd = CreateWindow( pszClass?pszClass:win_class::get_class_default(), pszTitle, style, 
							rect.left, rect.top, cx, cy,
							parent, hmenu, module::hinst(), this );
	if( !hwnd )
		return false;
	if( !m_hwnd )
		return subclass( hwnd );
	return true;
}

inline
bool win_impl::create_ex( DWORD style, const RECT &rect, const _char *pszTitle, HWND parent, HMENU hmenu, const _char *pszClass, DWORD ex_style )
{
	m_hwnd = 0;

	int	cx = (rect.right == CW_USEDEFAULT)?CW_USEDEFAULT:rect.right-rect.left;
	int	cy = (rect.bottom == CW_USEDEFAULT)?CW_USEDEFAULT:rect.bottom-rect.top;
	HWND hwnd = CreateWindowEx( ex_style, pszClass?pszClass:win_class::get_class_default(), pszTitle, style, 
							rect.left, rect.top, cx, cy,
							parent, hmenu, module::hinst(), this );
	if( !hwnd )
		return false;
	if( !m_hwnd )
		return subclass( hwnd );
	return true;
}

inline 
LRESULT	win_impl::handle_message(UINT m, WPARAM w, LPARAM l)
{
	LRESULT	lres = 0;
//store old message
	m_lock_delete++;

	MSG	last_current;
	memcpy( &last_current, &m_current_message, sizeof(m_current_message) );
	m_current_message.message = m;
	m_current_message.lParam = l;
	m_current_message.wParam = w;

	if( m == WM_SETFOCUS )
	{
		HWND	hwnd_top = ::GetTopLevelParent( handle() );
		if( hwnd_top != handle() )
			::SendMessage( hwnd_top, WM_WINDOWFOCUSED, 0, (LPARAM)handle() );
	}

//process message map
	switch( m )
	{
	case WM_CREATE:			lres = on_create( (CREATESTRUCT*)l );break;
	case WM_PAINT:			lres = on_paint();break;
	case WM_DESTROY:		lres = on_destroy();break;
	case WM_CLOSE:			lres = on_close();break;
	case WM_NCDESTROY:		lres = on_ncdestroy();break;
	case WM_SIZE:			lres = on_size( LOWORD(l), HIWORD(l), w );break;
	case WM_SHOWWINDOW:		lres = on_show( w != 0, l );break;
	case WM_INITDIALOG:		lres = on_initdialog();break;
	case WM_COMMAND:		lres = on_command( w );break;
	case WM_SYSCOMMAND:		lres = on_syscommand( LOWORD(w) );break;
	case WM_TIMER:			lres = on_timer( w );break;
	case WM_LBUTTONDOWN:	lres = on_lbuttondown( _point( (long)l ) );break;
	case WM_LBUTTONUP:		lres = on_lbuttonup(_point((long)l)); break;
	case WM_RBUTTONDOWN:	lres = on_rbuttondown(_point((long)l)); break;
	case WM_RBUTTONUP:		lres = on_rbuttonup(_point((long)l)); break;
	case WM_MOUSEMOVE:		lres = on_mousemove(_point((long)l)); break;
	case WM_GETMINMAXINFO:	lres = on_getminmaxinfo( (MINMAXINFO *)l );break;
	case WM_GETINTERFACE:	lres = on_getinterface();break;
	case WM_NOTIFY:			lres = on_notify( (uint)w, (NMHDR*)l );break;
	case WM_DROPFILES:		lres = on_dropfiles( (HANDLE)w );break;
	case WM_NOTYFYREFLECT:	lres = handle_reflect_message( (MSG*)l, (long*)w );break;
	case WM_SETFOCUS:		lres = on_setfocus( (HWND)w );break;
	case WM_KILLFOCUS:		lres = on_killfocus( (HWND)w );break;
	case WM_CHAR:			lres = on_char( w );break;
	case WM_KEYDOWN:		lres = on_keydown( w );break;
	case WM_KEYUP:			lres = on_keyup( w );break;
	case WM_GETDLGCODE:		lres = on_getdlgcode( (MSG *)l );break;
	case WM_ERASEBKGND:		lres = on_erasebkgnd( (HDC)w );break; 
	case WM_HSCROLL:		lres = on_hscroll( LOWORD( w ), HIWORD( w ), (HWND)l );break;
	case WM_VSCROLL:		lres = on_vscroll( LOWORD( w ), HIWORD( w ), (HWND)l );break;
	case WM_DRAWITEM:		lres = on_drawitem( w, (DRAWITEMSTRUCT*)l );break;
	case WM_MEASUREITEM:	lres = on_measureitem( w, (MEASUREITEMSTRUCT*)l );break;
	case WM_COMPAREITEM:	lres = on_compareitem( w, (COMPAREITEMSTRUCT*)l );break;
	case WM_SETCURSOR:		lres = on_setcursor( HIWORD(l), LOWORD(l) );break;
	case WM_HELP:			lres = on_helpinfo( (HELPINFO*)l );break;
	case WM_WINDOWFOCUSED:	lres = on_windowfocused( (HWND)l );break;
	case WM_ACTIVATE:		lres = on_activate( LOWORD(w), (HWND)l );break;
	case WM_MOUSEACTIVATE:	lres = on_mouseactivate( LOWORD(l), HIWORD(l) );break;
	case WM_LBUTTONDBLCLK:	lres = on_lbuttondblclk( _point((long)l) );break;
	case WM_RBUTTONDBLCLK:	lres = on_rbuttondblclk( _point((long)l) );break;
	case WM_CONTEXTMENU:	lres = on_contextmenu( _point((long)l) );break;
	case WM_CANCELMODE:		lres = on_cancelmode();break;

	default:lres = call_default();
	}
//restore original message
	memcpy( &m_current_message, &last_current, sizeof(m_current_message) );
	m_lock_delete--;

	if( !m_hwnd && m_lock_delete == 0 && m_kill_on_destroy )
		delete this;

	return lres ;
}

inline long win_impl::handle_reflect_message( MSG *pmsg, long *plProcessed )
{
	if( pmsg->message == WM_NOTIFY )
		return on_notify_reflect( (NMHDR*)pmsg->lParam, plProcessed );
	return 0;
}

inline LRESULT win_impl::call_default()
{
	if( m_proc_old && m_call_old )
		return CallWindowProc( m_proc_old, handle(), m_current_message.message, m_current_message.wParam, m_current_message.lParam );
	if( m_proc_def )
		return CallWindowProc( m_proc_def, handle(), m_current_message.message, m_current_message.wParam, m_current_message.lParam );
	return 0;
}


inline 
LONG_PTR __stdcall subclass_proc(HWND hwnd, UINT m, WPARAM w, LPARAM l )
{
	if( m == WM_NCCREATE )
	{
		CREATESTRUCT	*pcs = (CREATESTRUCT*) l;
		win_impl	*pwi = (win_impl*)pcs->lpCreateParams;
		pwi->subclass( hwnd, true );
		return pwi->handle_message( m, w, l );
	}
	return 0;
}

inline 
LONG_PTR __stdcall subclassed_proc(HWND hwnd, UINT m, WPARAM w, LPARAM l )
{
	message_filter	*pm = (message_filter*)(LONG_PTR)hwnd;
	return pm->handle_message( m, w, l );
}

inline 
LONG_PTR __stdcall subclass_mdi_proc(HWND hwnd, UINT m, WPARAM w, LPARAM l )
{
	if( m == WM_NCCREATE )
	{
		
		CREATESTRUCT	*pcs = (CREATESTRUCT*) l;
		MDICREATESTRUCT	*pm = (MDICREATESTRUCT*)pcs->lpCreateParams;
		win_impl	*pwi = (win_impl*)pm->lParam;
		pwi->subclass( hwnd, true );
		return pwi->handle_message( m, w, l );
	}
	return DefMDIChildProc( hwnd, m, w, l );
}




#endif // __win_impl_h__
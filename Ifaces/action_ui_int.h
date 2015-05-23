#ifndef __action_ui_int_h__
#define __action_ui_int_h__

#include "defs.h"


//UI Control
interface IUIControl : public IUnknown
{
	com_call Create( HWND hwnd, RECT rc ) = 0;		
	com_call GetSize( SIZE* psize, BOOL bhorz ) = 0;
	com_call GetHwnd( HWND* phwnd ) = 0;
	com_call Draw( HDC hdc, RECT rc, DWORD dw_flags ) = 0;
};

declare_interface( IUIControl, "7B4B45FF-E81D-4c85-998A-0BECD2B51A76" )

#endif //__action_ui_int_h__
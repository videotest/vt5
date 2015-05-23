#pragma once

#define _SET_COLOR "SetColorRGB"
#define _GET_COLOR "GetColorRGB"

#include "Fish.h"
#include "CustomStatic.h"
#include "Settings.h"
#define TIMER_ID 102030
// CChildWnd dialog
class CChildWnd
{
	bool m_bTimer;
public:
	CCustomStatic m_wBorder;
	CBrush m_brBack;
	CChildWnd() : m_brBack((COLORREF)g_BackgroundColor)
	{
		m_bTimer = false;
	}
protected:
	HRESULT _invoke_script_func( IDispatch *pDisp, BSTR bstrFuncName, VARIANT* pargs, int nArgsCount, VARIANT* pvarResult );
	void _mouse_move( HWND hwnd  );
	void _timer( HWND hwnd );
	void _lbutton_down( HWND hwnd );
private:
	void _mouse_leave(HWND hwnd);
	void _mouse_hover(HWND hwnd);
};


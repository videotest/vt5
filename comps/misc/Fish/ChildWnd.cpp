// ChildWnd.cpp : implementation file
//

#include "stdafx.h"
#include "Fish.h"
#include "ChildWnd.h"


HRESULT CChildWnd::_invoke_script_func( IDispatch *pDisp, BSTR bstrFuncName, VARIANT* pargs, int nArgsCount, VARIANT* pvarResult )
{
	CString szFuncName( bstrFuncName );

	if( szFuncName.IsEmpty() )
		return S_FALSE;

	CString str = szFuncName;
	BSTR bstr = str.AllocSysString( );
	DISPID id = -1;
	HRESULT hr = pDisp->GetIDsOfNames( IID_NULL, &bstr, 1, LOCALE_USER_DEFAULT, &id );
	if( hr != S_OK )
	{
		::SysFreeString( bstr );	bstr = 0;
		return S_FALSE;
	}
	
	::SysFreeString( bstr );	bstr = 0;

	DISPPARAMS dispparams = {0};

	dispparams.rgvarg			= pargs;
	dispparams.cArgs			= nArgsCount;
	dispparams.cNamedArgs		= 0;
		
	VARIANT vt = {0};

	hr = pDisp->Invoke( 
					id, 
					IID_NULL, 
					LOCALE_SYSTEM_DEFAULT,
					DISPATCH_METHOD,
					&dispparams,
					&vt,
					NULL,
					NULL
					);

	if( pvarResult )
		*pvarResult = _variant_t( vt );

	return S_OK;
}

void CChildWnd::_mouse_leave(HWND hwnd)
{
	if( m_wBorder.SetState( false, 1 ) )
	{
		RECT rc = {0};
		::GetClientRect( hwnd, &rc );
		::InvalidateRect( hwnd, &rc, 1 );
	}
}
void CChildWnd::_mouse_hover(HWND hwnd)
{
	if( m_wBorder.SetState( true, 1 ) )
	{
		RECT rc = {0};
		::GetClientRect( hwnd, &rc );
		::InvalidateRect( hwnd, &rc, 1 );
	}
}

void CChildWnd::_mouse_move( HWND hwnd )
{
	if( !m_bTimer )
	{
		SetTimer( GetParent( hwnd ), TIMER_ID, 1, 0 );
		_mouse_hover( hwnd );
		m_bTimer = true;
	}
}

void CChildWnd::_timer( HWND hwnd )
{
	m_bTimer = false;
	_mouse_leave( hwnd );
}

void CChildWnd::_lbutton_down( HWND hwnd )
{
	if( m_wBorder.SetState( true, 0 ) )
	{
		RECT rc = {0};
		::GetClientRect( hwnd, &rc );
		::InvalidateRect( hwnd, &rc, 1 );
	}
}



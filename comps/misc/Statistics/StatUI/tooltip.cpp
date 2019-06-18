#include "StdAfx.h"
#include "tooltip.h"

CToolTip32::CToolTip32()
{
	m_bIsActive = true;

	INITCOMMONCONTROLSEX iccex; 

	iccex.dwICC = ICC_WIN95_CLASSES; 
    iccex.dwSize = sizeof(INITCOMMONCONTROLSEX); 
    InitCommonControlsEx(&iccex); 
}

CToolTip32::~CToolTip32()
{
}

bool CToolTip32::CreateTooltip( HWND hParent, DWORD style )
{
	RECT rc = { CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT };
	return create_ex(  WS_POPUP | style, rc, 0, hParent, 0, TOOLTIPS_CLASS, WS_EX_TOPMOST );
}

LRESULT CToolTip32::OnDisableModal(WPARAM, LPARAM)
{
	::SendMessage( handle(), TTM_ACTIVATE, FALSE, 0 );
	return FALSE;
}

void CToolTip32::OnEnable(BOOL bEnable)
{
	::SendMessage( handle(), TTM_ACTIVATE, bEnable, 0 );
}

LRESULT CToolTip32::OnWindowFromPoint(WPARAM, LPARAM lParam)
{
	HWND hWnd = ::WindowFromPoint( *(POINT*)lParam );
	return (LRESULT)hWnd;
}

BOOL CToolTip32::AddTool(HWND hwnd, LPCTSTR lpszText, LPCRECT lpRectTool, UINT_PTR nIDTool )
{
	TOOLINFO ti = {0};
	FillInToolInfo( ti, hwnd, nIDTool );
	
	if( lpRectTool != 0 )
		memcpy( &ti.rect, lpRectTool, sizeof( RECT ) );
	ti.lpszText = ( LPTSTR )lpszText;

	BOOL b = (BOOL)::SendMessage( handle(), TTM_ADDTOOL, 0, (LPARAM)&ti );
	
	if( b )
		m_listTips.add_tail( nIDTool );

	return b;
}

void CToolTip32::DeleteAll( HWND hwnd )
{
	for( long lPos = m_listTips.head(); lPos; lPos = m_listTips.next( lPos ) )
	{
		DelTool( hwnd, m_listTips.get( lPos ) );
	}

	m_listTips.clear();

}

void CToolTip32::DelTool( HWND hwnd, UINT_PTR nIDTool)
{
	TOOLINFO ti = {0};
	FillInToolInfo( ti, hwnd, nIDTool );
	::SendMessage( handle(), TTM_DELTOOL, 0, (LPARAM)&ti );
}

void CToolTip32::GetText( LPSTR strText, HWND hwnd, UINT_PTR nIDTool ) 
{
	TOOLINFO ti = {0};
	FillInToolInfo( ti, hwnd, nIDTool );
	ti.lpszText = strText;
	::SendMessage( handle(), TTM_GETTEXT, 0, (LPARAM)&ti );
}


BOOL CToolTip32::GetToolInfo( ::CToolInfo& ToolInfo,  HWND hwnd,
	UINT_PTR nIDTool)
{
	FillInToolInfo( ToolInfo,  hwnd, nIDTool );
	ToolInfo.lpszText = ToolInfo.szText;

	return (BOOL)::SendMessage( handle(), TTM_GETTOOLINFO, 0, (LPARAM)&ToolInfo );
}

BOOL CToolTip32::HitTest( HWND hwnd, POINT pt, LPTOOLINFO lpToolInfo )
{
	TTHITTESTINFO hti = {0};
	memset( &hti, 0, sizeof( hti ) );
	hti.ti.cbSize = sizeof(AFX_OLDTOOLINFO);
	hti.hwnd = hwnd;
	hti.pt.x = pt.x;
	hti.pt.y = pt.y;
	if ((BOOL)::SendMessage( handle(), TTM_HITTEST, 0, (LPARAM)&hti))
	{
		memcpy(lpToolInfo, &hti.ti, sizeof(AFX_OLDTOOLINFO));
		return TRUE;
	}
	return FALSE;
}

void CToolTip32::SetToolRect( HWND hwnd, UINT_PTR nIDTool, LPCRECT lpRect)
{
	TOOLINFO ti = {0};
	FillInToolInfo( ti, hwnd, nIDTool );
	memcpy(&ti.rect, lpRect, sizeof(RECT));
	::SendMessage( handle(), TTM_NEWTOOLRECT, 0, (LPARAM)&ti );
}

void CToolTip32::UpdateTipText( LPCTSTR lpszText,  HWND hwnd, UINT_PTR nIDTool )
{
	TOOLINFO ti = {0};
	FillInToolInfo( ti, hwnd, nIDTool );
	ti.lpszText = (LPTSTR)lpszText;
	::SendMessage( handle(), TTM_UPDATETIPTEXT, 0, (LPARAM)&ti );
}


void CToolTip32::FillInToolInfo(TOOLINFO& ti, HWND hwnd, UINT_PTR nIDTool)
{
	memset(&ti, 0, sizeof(AFX_OLDTOOLINFO));
	ti.cbSize = sizeof(AFX_OLDTOOLINFO);
	if (nIDTool == 0)
	{
		ti.hwnd = ::GetParent(hwnd);
		ti.uFlags = TTF_IDISHWND;
		ti.uId = (UINT_PTR)hwnd;
	}
	else
	{
		ti.hwnd = hwnd;
		ti.uFlags = 0;
		ti.uId = nIDTool;
	}
}

// ShellStatusBar.cpp : implementation file
//

#include "stdafx.h"
#include "shell.h"
#include "ShellStatusBar.h"
#include "\vt5\controls\xpbar_ctrl\xp_bar.h"

/////////////////////////////////////////////////////////////////////////////
// CShellStatusBar

CShellStatusBar::CShellStatusBar() :lposStatus_(0)
{
	m_hwndXPBar = 0;
}

CShellStatusBar::~CShellStatusBar()
{
	for( int i = 0; i < m_panes.GetSize(); i++ )
	{
		delete (PaneInfo*)m_panes[i];
	}
	m_panes.RemoveAll();
}


BEGIN_MESSAGE_MAP(CShellStatusBar, CStatusBar)
	//{{AFX_MSG_MAP(CShellStatusBar)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(SB_SETTEXT, OnSetPaneText)
	ON_MESSAGE(WM_SETTEXT, OnSetText)
END_MESSAGE_MAP()

BEGIN_INTERFACE_MAP(CShellStatusBar, CStatusBar)
	INTERFACE_PART(CShellStatusBar, IID_IWindow, Wnd)
	INTERFACE_PART(CShellStatusBar, IID_IWindow2, Wnd)
	INTERFACE_PART(CShellStatusBar, IID_INewStatusBar, Status)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CShellStatusBar, Status);

/////////////////////////////////////////////////////////////////////////////
// CShellStatusBar message handlers
HRESULT CShellStatusBar::XStatus::AddPane( const GUID &guidPane, int cxWidth, HWND hwnd, DWORD dwStyle )
{
	METHOD_PROLOGUE_EX(CShellStatusBar, Status)

	CShellStatusBar::PaneInfo	*pPaneInfo = pThis->_Find( guidPane );
	if( pPaneInfo )
	{
		pPaneInfo->nRefCounter++;
		return S_OK;
	}
	pPaneInfo = new CShellStatusBar::PaneInfo;
	pPaneInfo->cx = cxWidth;
	pPaneInfo->dwFlags = dwStyle;
	pPaneInfo->guidPane = guidPane;
	pPaneInfo->hwnd = hwnd;
	pPaneInfo->nID = -1;
	pPaneInfo->nRefCounter = 1;

	if( pPaneInfo->hwnd )
		::SendMessage( pPaneInfo->hwnd, WM_SETFONT, ::SendMessage( *pThis, WM_GETFONT, 0, 0 ), 0 );

	pThis->m_panes.Add( pPaneInfo );

	pThis->RecalcLayout();

	if( pThis->m_hwndXPBar )
	{
		XP_STATUS	status;
		ZeroMemory( &status, sizeof( status ) );
		status.mask = XPSM_HWND|XPSM_IMAGE|XPSM_GUID;
		status.hwnd = hwnd;
		status.guid = guidPane;

		::SendMessage(pThis->m_hwndXPBar, XPB_ADDSTATUSPANE, (WPARAM)pThis->lposStatus_, (LPARAM)&status);
	}

	return S_OK;
}

HRESULT CShellStatusBar::XStatus::RemovePane( const GUID &guidPane )
{
	METHOD_PROLOGUE_EX(CShellStatusBar, Status)

	CShellStatusBar::PaneInfo	*pPaneInfo = pThis->_Find( guidPane );
	if( !pPaneInfo )return E_INVALIDARG;
	pPaneInfo->nRefCounter--;
	if( pPaneInfo->nRefCounter )return S_OK;

	pThis->m_panes.RemoveAt( pPaneInfo->nID );
	delete pPaneInfo;
	pThis->RecalcLayout();

	if( pThis->m_hwndXPBar )
		::SendMessage(pThis->m_hwndXPBar, XBP_REMOVESTATUSPANE, (WPARAM)pThis->lposStatus_, (LPARAM)&guidPane);


	return S_OK;
}
HRESULT CShellStatusBar::XStatus::GetPaneID( const GUID &guidPane, long *pnID )
{
	METHOD_PROLOGUE_EX(CShellStatusBar, Status)

	*pnID = -1;
	CShellStatusBar::PaneInfo	*pPaneInfo = pThis->_Find( guidPane );
	if( pPaneInfo )*pnID = pPaneInfo->nID;
	
	return S_OK;
}

HRESULT CShellStatusBar::XStatus::SetPaneIcon( const GUID &guidPane, HICON hIcon )
{
	METHOD_PROLOGUE_EX(CShellStatusBar, Status)

	CShellStatusBar::PaneInfo	*pPaneInfo = pThis->_Find( guidPane );
	if( pPaneInfo )
	{
		if( pPaneInfo->hIcon )::DestroyIcon( pPaneInfo->hIcon );
		pPaneInfo->hIcon = hIcon;

		CStatusBarCtrl	&ctrl = pThis->GetStatusBarCtrl();
		ctrl.SetIcon( pPaneInfo->nID, pPaneInfo->hIcon );

		if( pThis->m_hwndXPBar )
		{
			XP_STATUS	status;
			status.mask = XPSM_IMAGE|XPSM_GUID;
			status.guid = guidPane;
			status.hIcon = hIcon;

			::SendMessage(pThis->m_hwndXPBar, XPB_SETSTATUSPANE, (WPARAM)pThis->lposStatus_, (LPARAM)&status);
		}
	}
	
	return S_OK;
}

CShellStatusBar::PaneInfo	*
	CShellStatusBar::_Find( const GUID &guid )
{
	for( int n = 0; n < m_panes.GetSize(); n++ )
	{
		PaneInfo	*pPaneInfo = (PaneInfo*)m_panes[n];
		pPaneInfo->nID = n;
		if( pPaneInfo->guidPane == guid )
			return pPaneInfo;
	}
	return 0;
}

void CShellStatusBar::RecalcLayout()
{
	if( m_panes.GetSize() == 0 )
		return;

	CStatusBarCtrl	&ctrl = GetStatusBarCtrl();

	CRect	rect;
	GetClientRect( &rect );

	int	nWidth = rect.Width();

	int	*pnWidths = new int[m_panes.GetSize()];

	for( int n = m_panes.GetSize()-1; n >= 0; n-- )
	{
		PaneInfo	*pPaneInfo = (PaneInfo*)m_panes[n];
		if( pPaneInfo->cx != -1 )
		{
			pnWidths[n] = nWidth;
			nWidth -= pPaneInfo->cx;
		}
		else
			pnWidths[n] = nWidth;
	}

	ctrl.SetParts( m_panes.GetSize(), pnWidths );

	for( n = 0; n < m_panes.GetSize(); n++ )
	{
		PaneInfo	*pPaneInfo = (PaneInfo*)m_panes[n];
		if( pPaneInfo->hwnd && m_hwndXPBar == 0 )
		{
			CRect	rect;
			ctrl.GetRect( n, &rect );	
			::MoveWindow( pPaneInfo->hwnd, 
				rect.left, rect.top, rect.Width(), rect.Height(), true );
		}
		else
			if( pPaneInfo->hIcon )
				ctrl.SetIcon( n, pPaneInfo->hIcon );
	}

	delete pnWidths;
}

void CShellStatusBar::OnSize(UINT nType, int cx, int cy) 
{
	CStatusBar::OnSize(nType, cx, cy);
	RecalcLayout();
}

LRESULT CShellStatusBar::OnSetPaneText(WPARAM wParam, LPARAM lParam)
{
	char	*pszNewText = (char*)lParam;
	int		nPane = (wParam&0xFF );

	if( nPane >= 0 && nPane < m_panes.GetSize() )
	{
		PaneInfo	*pPaneInfo = (PaneInfo*)m_panes[nPane];

		if( m_hwndXPBar )
		{
			XP_STATUS	status;
			status.mask = XPSM_GUID|XPSM_TEXT;
			status.pszText = pszNewText;
			status.guid = pPaneInfo->guidPane;

			::SendMessage(m_hwndXPBar, XPB_SETSTATUSPANE, (WPARAM)lposStatus_, (LPARAM)&status);
		}
		else
			if( pPaneInfo->hwnd )
				::SetWindowText( pPaneInfo->hwnd, pszNewText );
	}
	
	return Default();
}	

CString _get_active_view_text()
{
	IApplicationPtr ptr_app = ::GetAppUnknown();
	if( ptr_app == 0 )	return "";

	IUnknownPtr ptr_doc;
	ptr_app->GetActiveDocument( &ptr_doc );
	if( ptr_doc == 0 )	return "";

	IDocumentSitePtr ptr_ds = ptr_doc;
	if( ptr_ds == 0 )	return "";

	IUnknownPtr ptr_view;
	ptr_ds->GetActiveView( &ptr_view );
	if( ptr_view == 0 )	return "";

	CString str_view_name = ::GetObjectName( ptr_view );
	if( !str_view_name.GetLength() )	return "";

	CString str_status = ::GetValueString( ptr_app, "ViewStatusText", str_view_name, "" );

	return str_status;
}

LRESULT CShellStatusBar::OnSetText(WPARAM wParam, LPARAM lParam)
{
	char	*pszNewText = (char*)lParam;

	CString	strText = pszNewText;
	if( !strText.GetLength() )
	{
		strText = ::LanguageLoadCString( AFX_IDS_IDLEMESSAGE );
		//strText.LoadString( AFX_IDS_IDLEMESSAGE );
	}

	//Заменяем дефалт "Ready" текст, спрашиваем у вьюхи
	if( strText == ::LanguageLoadCString( AFX_IDS_IDLEMESSAGE ) )
	{
		CString str_view_text = _get_active_view_text();
		if( str_view_text.GetLength() )
			strText = str_view_text;
	}

	if( m_panes.GetSize() > 0 )
	{
		PaneInfo	*pPaneInfo = (PaneInfo*)m_panes[0];
		if( m_hwndXPBar )
		{
			XP_STATUS	status;
			status.mask = XPSM_GUID|XPSM_TEXT;
			status.pszText = (char*)(const char*)strText;
			status.guid = pPaneInfo->guidPane;

			::SendMessage(m_hwndXPBar, XPB_SETSTATUSPANE, (WPARAM)lposStatus_, (LPARAM)&status);
		}
		else
			if( pPaneInfo->hwnd )
				::SetWindowText( pPaneInfo->hwnd, strText );

	}
	
	return Default();
}	

TPOS CShellStatusBar::SetXPBar( HWND hwnd )
{
	m_hwndXPBar = hwnd;

	if( m_hwndXPBar )
	{
			//add panes
		XPINSERTITEM	insert;
		ZeroMemory( &insert, sizeof( insert ) );

		CString str_details = ::LanguageLoadCString( IDS_XP_DETAILS );
		//str_details.LoadString( IDS_XP_DETAILS );

		insert.insert_pos = 0;
		insert.item.pszText = (char*)(LPCSTR)str_details;
		insert.item.style = XPPS_STATUS;
		insert.item.mask |= XPF_TEXT|XPF_STYLE;

		lposStatus_ = (TPOS)::SendMessage(m_hwndXPBar, XPB_INSERTITEM, 0, (LPARAM)&insert);

		//load all panes
		long	nCount = GetPanesCount();
		
		for( long n = 0; n < nCount; n++ )
		{
			CShellStatusBar::PaneInfo	*ppane = 
				GetPaneInfo( n );

			XP_STATUS	status;
			ZeroMemory( &status, sizeof( status ) );

			char	sz[100];
			SendMessage( SB_GETTEXT, n, (LPARAM)sz );


			status.mask = XPSM_HWND|XPSM_GUID|XPSM_IMAGE|XPSM_TEXT|XPSM_STYLE|XPSM_ROWCOUNT|XPSM_ID;
			status.hwnd = ppane->hwnd;
			status.guid = ppane->guidPane;
			status.hIcon = ppane->hIcon;
			status.nRows = 1;
			status.pszText = sz;
			status.dwStyle = 0;

			::SendMessage(m_hwndXPBar, XPB_ADDSTATUSPANE, (WPARAM)lposStatus_, (LPARAM)&status);
		}

	}
	else
	{
		//load all panes
		long	nCount = GetPanesCount();
		
		for( long n = 0; n < nCount; n++ )
		{
			CShellStatusBar::PaneInfo	*ppane = 
				GetPaneInfo( n );

			::SetParent( ppane->hwnd, GetSafeHwnd() );
			RecalcLayout();
		}
		lposStatus_ = 0;
	}
	return lposStatus_;
}


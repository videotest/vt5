#include "stdafx.h"
#include "shell.h"
#include "WindowActions.h"
#include "dialogs.h"
#include "shellframe.h"
#include "MainFrm.h"



class CWindowMenu : public CCmdTargetEx
{
	DECLARE_DYNCREATE(CWindowMenu);
protected:
	CWindowMenu()
	{		m_nCurrentBase = -1;	}
	void Init();

	DECLARE_INTERFACE_MAP();
	BEGIN_INTERFACE_PART(Menu,IUIDropDownMenu)
		com_call AppendMenuItems( HMENU hMenu, UINT nCommandBase );
	 	com_call ExecuteCommand( UINT nCommand );
	END_INTERFACE_PART(Menu)
protected:
	int	m_nCurrentBase;
	int	m_nSelected;
	CStringArray	m_strsWindows;
};

IMPLEMENT_DYNCREATE(CWindowMenu, CCmdTargetEx)
IMPLEMENT_UNKNOWN(CWindowMenu, Menu)

BEGIN_INTERFACE_MAP(CWindowMenu, CCmdTargetEx)
	INTERFACE_PART(CWindowMenu, IID_IUIDropDownMenu, Menu )
END_INTERFACE_MAP()

void CWindowMenu::Init()
{
	m_strsWindows.RemoveAll();

	CMainFrame	*pmain = (CMainFrame *)AfxGetMainWnd();

	POSITION	pos = pmain->GetFirstMDIChildPosition();	

	CWnd	*pwndMDIActive = pmain->_GetActiveFrame();

	while( pos )
	{
		CWnd	*pwnd = pmain->GetNextMDIChild( pos );
		CString	strTitle;

		pwnd->GetWindowText( strTitle );

		if( pwnd == pwndMDIActive )
			m_nSelected = m_strsWindows.GetSize();
		m_strsWindows.Add( strTitle );
	}
}

HRESULT CWindowMenu::XMenu::AppendMenuItems( HMENU hMenu, UINT nCommandBase )
{
	METHOD_PROLOGUE_EX(CWindowMenu, Menu)

	pThis->Init();

	pThis->m_nCurrentBase = nCommandBase;

	int	n = 0;

	if( pThis->m_strsWindows.GetSize() == 0 )
	{
		CString	strNoRecent = ::LanguageLoadCString( IDS_NO_WINDOWS );
		//strNoRecent.LoadString( IDS_NO_WINDOWS );

		::AppendMenu( hMenu, MF_STRING|MF_GRAYED|MF_DISABLED, nCommandBase, strNoRecent );
	}
	else
	{
		for( n = 0; n < pThis->m_strsWindows.GetSize(); n++ )
		{
			DWORD	dwFlags = MF_STRING;
			if( n == pThis->m_nSelected )
				dwFlags |= MF_CHECKED;
			::AppendMenu( hMenu, dwFlags, nCommandBase+n, pThis->m_strsWindows[n] );
		}
	}

	CString	strOpen = ::LanguageLoadCString( IDS_ACTIVATE_WINDOW );
	//strOpen.LoadString( IDS_ACTIVATE_WINDOW );

	::AppendMenu( hMenu, MF_SEPARATOR, 0, 0 );
	::AppendMenu( hMenu, MF_STRING, nCommandBase+n, strOpen );

	return S_OK;
}


HRESULT CWindowMenu::XMenu::ExecuteCommand( UINT nCommand )
{
	METHOD_PROLOGUE_EX(CWindowMenu, Menu)

	nCommand -= pThis->m_nCurrentBase;

	if( nCommand >= 0 && nCommand < (UINT)pThis->m_strsWindows.GetSize() )
	{
		CString	strWindow = pThis->m_strsWindows[nCommand];

		ExecuteAction( "WindowActivate", CString( "\"" )+strWindow+CString( "\"" ) );
	}
	else if( nCommand == pThis->m_strsWindows.GetSize() )
		ExecuteAction( "WindowActivate" );
	else
		return S_FALSE;
	
	return S_OK;
}



//[ag]1. dyncreate

IMPLEMENT_DYNCREATE(CActionWindowActivate, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionWindowNew, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionWindowTile, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionWindowCascade, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionWindowArrange, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionWindowClose, CCmdTargetEx);

//[ag]4. olecreate release

// {2CB8C363-1768-11d3-A5C9-0000B493FF1B}
GUARD_IMPLEMENT_OLECREATE(CActionWindowActivate, "Shell.WindowActivate",
0x2cb8c363, 0x1768, 0x11d3, 0xa5, 0xc9, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b);
// {42B12D53-1759-11d3-A5C9-0000B493FF1B}
GUARD_IMPLEMENT_OLECREATE(CActionWindowNew, "Shell.WindowNew",
0x42b12d53, 0x1759, 0x11d3, 0xa5, 0xc9, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b);
// {42B12D4F-1759-11d3-A5C9-0000B493FF1B}
GUARD_IMPLEMENT_OLECREATE(CActionWindowTile, "Shell.WindowTile",
0x42b12d4f, 0x1759, 0x11d3, 0xa5, 0xc9, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b);
// {42B12D4B-1759-11d3-A5C9-0000B493FF1B}
GUARD_IMPLEMENT_OLECREATE(CActionWindowCascade, "Shell.WindowCascade",
0x42b12d4b, 0x1759, 0x11d3, 0xa5, 0xc9, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b);
// {42B12D47-1759-11d3-A5C9-0000B493FF1B}
GUARD_IMPLEMENT_OLECREATE(CActionWindowArrange, "Shell.WindowArrange",
0x42b12d47, 0x1759, 0x11d3, 0xa5, 0xc9, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b);
// {42B12D43-1759-11d3-A5C9-0000B493FF1B}
GUARD_IMPLEMENT_OLECREATE(CActionWindowClose, "Shell.WindowClose",
0x42b12d43, 0x1759, 0x11d3, 0xa5, 0xc9, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b);

//[ag]5. guidinfo release

// {2CB8C361-1768-11d3-A5C9-0000B493FF1B}
static const GUID guidWindowActivate =
{ 0x2cb8c361, 0x1768, 0x11d3, { 0xa5, 0xc9, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b } };
// {42B12D51-1759-11d3-A5C9-0000B493FF1B}
static const GUID guidWindowNew =
{ 0x42b12d51, 0x1759, 0x11d3, { 0xa5, 0xc9, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b } };
// {42B12D4D-1759-11d3-A5C9-0000B493FF1B}
static const GUID guidWindowTile =
{ 0x42b12d4d, 0x1759, 0x11d3, { 0xa5, 0xc9, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b } };
// {42B12D49-1759-11d3-A5C9-0000B493FF1B}
static const GUID guidWindowCascade =
{ 0x42b12d49, 0x1759, 0x11d3, { 0xa5, 0xc9, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b } };
// {42B12D45-1759-11d3-A5C9-0000B493FF1B}
static const GUID guidWindowArrange =
{ 0x42b12d45, 0x1759, 0x11d3, { 0xa5, 0xc9, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b } };
// {42B12D41-1759-11d3-A5C9-0000B493FF1B}
static const GUID guidWindowClose =
{ 0x42b12d41, 0x1759, 0x11d3, { 0xa5, 0xc9, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b } };


//[ag]6. action info

ACTION_INFO_FULL(CActionWindowActivate, IDS_ACTION_WINDOW_ACTIVATE, IDS_MENU_WINDOW, -1, guidWindowActivate);
ACTION_INFO_FULL(CActionWindowNew, IDS_ACTION_WINDOW_NEW, IDS_MENU_WINDOW, -1, guidWindowNew);
ACTION_INFO_FULL(CActionWindowTile, IDS_ACTION_WINDOW_TILE, IDS_MENU_WINDOW, -1, guidWindowTile);
ACTION_INFO_FULL(CActionWindowCascade, IDS_ACTION_WINDOW_TILE2, IDS_MENU_WINDOW, -1, guidWindowCascade);
ACTION_INFO_FULL(CActionWindowArrange, IDS_ACTION_WINDOW_ARRANGE, IDS_MENU_WINDOW, -1, guidWindowArrange);
ACTION_INFO_FULL(CActionWindowClose, IDS_ACTION_WINDOW_TILE3, IDS_MENU_WINDOW, -1, guidWindowClose);

//[ag]7. targets

ACTION_TARGET(CActionWindowActivate, "anywindow");
ACTION_TARGET(CActionWindowNew, "anywindow");
ACTION_TARGET(CActionWindowTile, "anywindow");
ACTION_TARGET(CActionWindowCascade, "anywindow");
ACTION_TARGET(CActionWindowArrange, "anywindow");
ACTION_TARGET(CActionWindowClose, "anywindow");

//[ag]8. arguments

ACTION_ARG_LIST(CActionWindowActivate)
	ARG_STRING("WindowTitle", 0)
END_ACTION_ARG_LIST()
ACTION_ARG_LIST(CActionWindowClose)
	ARG_STRING("WindowTitle", 0)
END_ACTION_ARG_LIST()



ACTION_UI(CActionWindowActivate, CWindowMenu)

//[ag]9. implementation

//////////////////////////////////////////////////////////////////////
//CActionWindowActivate implementation
CActionWindowActivate::CActionWindowActivate()
{
	m_pwndToActivate = 0;
}

CActionWindowActivate::~CActionWindowActivate()
{
}

bool CActionWindowActivate::ExecuteSettings( CWnd *pwndParent )
{
	CString	strWindowTitle = GetArgumentString( "WindowTitle" );

	if( strWindowTitle.IsEmpty() )
	{
		CChooseWindowDlg	dlg;

		if( dlg.DoModal() == IDOK )
			strWindowTitle = dlg.m_strWindowName;
		else
			return false;
	}

	m_pwndToActivate = GetMainFrame()->GetWindowByTitle( strWindowTitle );

	if( !m_pwndToActivate )
	//window not found	
		return false;

	SetArgument( "WindowTitle", COleVariant( strWindowTitle ) );
	return true;
}

bool CActionWindowActivate::Invoke()
{
	if( !m_pwndToActivate )
		return false;


	//************************************************
	// Old code here
	//if( m_pwndToActivate == GetMainFrame()->MDIGetActive() )
	//	return true;

	//	m_pwndToActivate->MDIActivate();
		// End old code
	//************************************************

//*****************************************************************************
//							MDI/PDI/DDI		support. START point			  |
	CWnd	*pMainWnd = AfxGetMainWnd( );

	ASSERT_KINDOF(CMainFrame, pMainWnd);

	CMainFrame	*pMDIFrm = (CMainFrame*)pMainWnd;

	CWnd	*pChildWnd = pMDIFrm->_GetActiveFrame( );
	
	if( m_pwndToActivate == pChildWnd )
		return true;

	m_pwndToActivate->MDIActivate();
//							MDI/PDI/DDI		support. END point				  |
//*****************************************************************************

	CMainFrame* pMainFrame = pMainFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CWnd* pActiveWnd = NULL;
	if( pMainFrame )
		pActiveWnd = pMainFrame->_GetActiveFrame();
	if (pActiveWnd != m_pwndToActivate)
	{
		ASSERT_KINDOF(CShellFrame,m_pwndToActivate);
		((CShellFrame*)m_pwndToActivate)->SetActive(true);
	}

	return true;
}


CMainFrame	*CActionWndBase::GetMainFrame()
{
	CWnd *pwnd = AfxGetMainWnd();

	ASSERT_KINDOF(CMainFrame, pwnd);

	return (CMainFrame*)pwnd;
}
//////////////////////////////////////////////////////////////////////
//CActionWindowNew implementation
CActionWindowNew::CActionWindowNew()
{
}

CActionWindowNew::~CActionWindowNew()
{
}

bool CActionWindowNew::Invoke()
{
	GetMainFrame()->CreateNewWindow();

	return true;
}

//////////////////////////////////////////////////////////////////////
//CActionWindowTile implementation
CActionWindowTile::CActionWindowTile()
{
}

CActionWindowTile::~CActionWindowTile()
{
}

bool CActionWindowTile::Invoke()
{
	return GetMainFrame()->ExecuteMDICommand( ID_WINDOW_TILE_VERT );
}

//////////////////////////////////////////////////////////////////////
//CActionWindowCascade implementation
CActionWindowCascade::CActionWindowCascade()
{
}

CActionWindowCascade::~CActionWindowCascade()
{
}

bool CActionWindowCascade::Invoke()
{
	return GetMainFrame()->ExecuteMDICommand( ID_WINDOW_CASCADE );
}

//////////////////////////////////////////////////////////////////////
//CActionWindowArrange implementation
CActionWindowArrange::CActionWindowArrange()
{
}

CActionWindowArrange::~CActionWindowArrange()
{
}

bool CActionWindowArrange::Invoke()
{
	return GetMainFrame()->ExecuteMDICommand( ID_WINDOW_ARRANGE );
}

//////////////////////////////////////////////////////////////////////
//CActionWindowClose implementation
CActionWindowClose::CActionWindowClose()
{
	m_pwndToActivate = 0;
}

CActionWindowClose::~CActionWindowClose()
{
}

bool CActionWindowClose::Invoke()
{
	ASSERT(m_punkTarget);
	//freee target
//	m_punkTarget->Release();
//	m_punkTarget = 0;
	//IFrameWindowPtr	ptrFrame( 

	m_pwndToActivate->DestroyWindow(); // [Max] bt:2801
	//if( m_pwndToActivate )
	//	m_pwndToActivate->SendMessage( WM_CLOSE, 0, 0 );

	AfxGetMainWnd()->SetFocus();
	m_punkTarget = 0;	
	//таргет - это само окно, оно само и убъется
	return true;
}

bool CActionWindowClose::ExecuteSettings( CWnd *pwndParent )
{
	CString	strWindowTitle = GetArgumentString( "WindowTitle" );

	if( strWindowTitle.IsEmpty() )
	{

	//************************************************
	// Old code here
		/*
		CWnd *pwnd = GetMainFrame()->MDIGetActive();
		if( !pwnd )
			return false;

		pwnd->GetWindowText( strWindowTitle );
		*/
		// End old code
	//************************************************

//*****************************************************************************
//							MDI/PDI/DDI		support. START point			  |

		CWnd	*pMainWnd = AfxGetMainWnd( );

		ASSERT_KINDOF(CMainFrame, pMainWnd);

		CMainFrame	*pMDIFrm = (CMainFrame*)pMainWnd;

		CWnd	*pChildWnd = pMDIFrm->_GetActiveFrame( );
		pChildWnd->GetWindowText( strWindowTitle );

//							MDI/PDI/DDI		support. END point				  |
//*****************************************************************************


	}

	if( ForceShowDialog() )
	{
		CChooseWindowDlg	dlg;

		dlg.m_strWindowName = strWindowTitle;
		if( dlg.DoModal() == IDOK )
			strWindowTitle = dlg.m_strWindowName;
		else
			return false;
	}

	if( strWindowTitle.IsEmpty() )
		return false;

	m_pwndToActivate = GetMainFrame()->GetWindowByTitle( strWindowTitle );

	if( !m_pwndToActivate )
	//window not found	
		return false;

	SetArgument( "WindowTitle", COleVariant( strWindowTitle ) );
	return true;
}
// HelpDlgImpl.cpp: implementation of the CHelpDlgImpl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HelpDlgImpl.h"

#ifdef _COMMON_LIB
#include "HelpBase.h"
#else

#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHelpDlgImpl::CHelpDlgImpl()
{		
	m_hWndDlg = NULL;
	::ZeroMemory( m_szHelpComponentName, sizeof(char) * STR_LEN );
	::ZeroMemory( m_szHelpPageName, sizeof(char) * STR_LEN );
	::ZeroMemory( m_szPrefix, sizeof(char) * STR_LEN );
	
}

//////////////////////////////////////////////////////////////////////
CHelpDlgImpl::~CHelpDlgImpl()
{

}

//////////////////////////////////////////////////////////////////////
bool CHelpDlgImpl::InitHelpImlp( HWND hWnd, const char* pszComponentName, const char* pszPageName, const char* pszPrefix )
{
	m_hWndDlg = hWnd;

	if( pszComponentName )
		strcpy( m_szHelpComponentName, pszComponentName );

	if( pszPageName )
		strcpy( m_szHelpPageName, pszPageName );

	if( pszPageName )
		strcpy( m_szPrefix, pszPageName );


	if( !::IsWindow( m_hWndDlg ) )
	{
		//ups!!!
		stop_assert;
		return false;
	}

	DWORD dwStyle = GetWindowLong( m_hWndDlg, GWL_EXSTYLE );

	if( !(dwStyle & WS_EX_CONTEXTHELP) )
	{
		//set dlg ex style : WS_EX_CONTEXTHELP
		stop_assert;
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
BOOL CHelpDlgImpl::IsHelpMsg( UINT message )
{
	if( message == WM_HELP )
		return TRUE;

	return FALSE;
}

//////////////////////////////////////////////////////////////////////
BOOL CHelpDlgImpl::FilterHelpMsg(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( !IsHelpMsg( message ) )
		return FALSE;

	if( !m_hWndDlg || !m_szHelpComponentName || !m_szHelpPageName )
		return FALSE;

	HELPINFO* phi = (HELPINFO*)lParam; 	
	if( !phi )
		return FALSE;
	
	HWND hWndFind = 0;

	HWND hwnd = ::GetWindow( m_hWndDlg, GW_CHILD );

	POINT ptMouse;
	::GetCursorPos( &ptMouse );

	//enum childs
	while( hwnd ) 
	{
		RECT rcWnd;
		SetRect( &rcWnd, 0, 0, 0, 0 );
		::GetWindowRect( hwnd, &rcWnd );

		HWND hWndSave = hwnd;

		hwnd = ::GetWindow( hwnd, GW_HWNDNEXT );

		if( ::PtInRect( &rcWnd, ptMouse ) )
		{
			hWndFind = hWndSave;
			break;
		}
	}


	#ifdef _COMMON_LIB
		CString	strHtmlFileName;
		CString	strPageName = m_szHelpPageName;

		CString strCaption = m_szHelpPageName;

		::HelpRegisterWindow( "VideoHelP", strCaption );
		//::HelpGenerateFileName( strHtmlFileName );
		//::HelpDisplayTopic( strHtmlFileName, "", strPageName, strCaption );
		//strHtmlFileName = 

		if( hWndFind )
		{
			::HelpLoadCollection( 0 );
			::HelpDisplayPopup( strHtmlFileName, "", strPageName, hWndFind );
		}
		else
			::HelpDisplayTopic( strHtmlFileName, "", strPageName, strCaption );
	#else

		//Not implementyet
		stop_assert;
	
	#endif

	
	return TRUE;
}

//WM_HELPHITTEST
//WM_HELP


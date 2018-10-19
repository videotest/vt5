#include "stdafx.h"
#include "ActionControlWrp.h"

#ifdef _DEBUG
const char szSectionPath[] = "software\\VideoTest\\vt5\\add-ins";
#else
const char szSectionPath[] = "software\\VideoTest\\vt5\\add-ins(debug)";
#endif //_DEBUG

CActionControlsRegistrator::CActionControlsRegistrator()
{
	HRESULT hr = ::RegOpenKey( HKEY_LOCAL_MACHINE, szSectionPath, &m_hKey );
	if( hr )::RegCreateKey( HKEY_LOCAL_MACHINE, szSectionPath, &m_hKey );

	int	nCount = 0;

	while( true )
	{
		char	szValue[255];
		DWORD	dwValueLen = 255;
		DWORD	dwType = REG_SZ;
		char	szData[255];
		DWORD	dwDataSize = 255;
		if( RegEnumValue( m_hKey, nCount++, szValue, &dwValueLen, 0, &dwType, (byte*)szData, &dwDataSize ) ) break;
		m_strControls.Add( szData );
		m_strActions.Add( szValue );
	}
}

CActionControlsRegistrator::~CActionControlsRegistrator()
{
	if( m_hKey )::RegCloseKey( m_hKey );
}

void CActionControlsRegistrator::RegisterActionControl( const char *pszProgID, const char *pszActionName, bool bRegister )
{
	if( bRegister )
	{
		::RegSetValueEx( m_hKey, pszActionName, 0, REG_SZ, (byte*)pszProgID, (DWORD)strlen( pszProgID ) );
	}
	else
	{
		::RegDeleteValue( m_hKey, pszActionName );
	}
}

long CActionControlsRegistrator::GetRegistredCount()
{
	ASSERT( m_strControls.GetSize() == m_strActions.GetSize() );
	return (long)m_strActions.GetSize();
}

void CActionControlsRegistrator::GetActionControl( int nPos, CString &strProgID, CString &strActionName )
{
	strProgID = m_strControls[nPos];
	strActionName = m_strActions[nPos];
}



CActionControlWrp::CActionControlWrp()
{
	m_hwnd = 0;
	m_ptr = 0;
}

CActionControlWrp::~CActionControlWrp()
{
}

void CActionControlWrp::Init( const char *pszProgID )
{
	m_ptr.CreateInstance( pszProgID );
}

void CActionControlWrp::Create( CWnd	*pwnd )
{
	IWindowPtr	ptrWindow( m_ptr );
	if( ptrWindow == 0 )return;

	ptrWindow->CreateWnd( pwnd->GetSafeHwnd(),NORECT, WS_CHILD|WS_VISIBLE, 1001 );
	ptrWindow->GetHandle( (HANDLE*)&m_hwnd );
}

void CActionControlWrp::DestroyWindow()
{
	IWindowPtr	ptrWindow( m_ptr );
	if( ptrWindow == 0 )return;
	ptrWindow->DestroyWindow();
}

HWND	CActionControlWrp::GetSafeHwnd()
{
	if( !this )return 0;
	return m_hwnd;
}

CWnd	*CActionControlWrp::GetParent()
{
	if( !GetSafeHwnd() )return 0;
	CWnd	*pwnd = CWnd::FromHandle( m_hwnd );
	return pwnd->GetParent();
}

void CActionControlWrp::ShowWindow( int nCmdShow )
{
	if( !m_hwnd )return;
	::ShowWindow( m_hwnd, nCmdShow );
}

void CActionControlWrp::MoveWindow( CRect rect )
{
	if( !m_hwnd )return;
	::MoveWindow( m_hwnd, rect.left, rect.top, rect.Width(), rect.Height(), false );
}

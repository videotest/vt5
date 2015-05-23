#include "StdAfx.h"
#include "Library.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CLibraryException,CException);
IMPLEMENT_DYNAMIC(CFuncNotFoundException,CException);

int CLibraryException::ReportError( UINT nType, UINT nMessageID)
{
	char szBuff[50];
	CString sMess;
	sMess = m_sName + " not found! Error ";
	wsprintf(szBuff, "%d.", GetLastError());
	sMess += szBuff;
	return AfxMessageBox(sMess,nType);
}

int CFuncNotFoundException::ReportError( UINT nType, UINT nMessageID)
{
	char szBuff[50];
	CString sMess;
	sMess = m_sFuncName + " in " + m_sLibName;
	sMess += " not found! Error ";
	wsprintf(szBuff, "%d.", GetLastError());
	sMess += szBuff;
	return AfxMessageBox(sMess,nType);
}

CLibrary::CLibrary()
{
	m_hMod = NULL;
}

CLibrary::CLibrary( const char *pszLibName )
{
	LoadLibrary(pszLibName);
}

CLibrary::~CLibrary()
{
	FreeLibrary();
}

BOOL CLibrary::LoadLibrary( const char *pszLibName )
{
	ASSERT(pszLibName);
	m_hMod = ::LoadLibrary(pszLibName);
	if (!m_hMod)
	{
		throw new CLibraryException(pszLibName);
	}
	m_sLibName = pszLibName;
	return TRUE;
}

BOOL CLibrary::FreeLibrary()
{
	if (m_hMod)
	{
		::FreeLibrary(m_hMod);
		m_hMod = NULL;
		m_sLibName.Empty();
	}
	return TRUE;
}

BOOL CLibrary::IsLoaded()
{
	return m_hMod!=0;
}

CLibrary::operator HMODULE()
{
	return m_hMod;
}

FARPROC CLibrary::GetProcAddress( const char *pszFuncName )
{
	ASSERT(m_hMod);
	FARPROC fp = ::GetProcAddress(m_hMod,pszFuncName);
	if (!fp)
		throw new CFuncNotFoundException(m_sLibName, pszFuncName);
	return fp;
}

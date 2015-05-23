#include "StdAfx.h"
#include "StdProfile.h"
#include "Debug.h"

const char *g_pszDebugName = "c:\\driver.log";

int g_nDebug = -1;

static void InitDebug()
{
	g_nDebug = CStdProfileManager::m_pMgr->GetProfileInt("Settings", "Debug", 0, true, true);
	CStdProfileManager::m_pMgr->WriteProfileInt("Settings", "Debug", g_nDebug);
	static char szPath[_MAX_PATH];
	char szPath1[_MAX_PATH], szDrive[_MAX_DRIVE],szDir[_MAX_DIR],szFName[_MAX_FNAME];
	GetModuleFileName(AfxGetInstanceHandle(), szPath1, _MAX_PATH);
	GetLongPathName(szPath1, szPath, _MAX_PATH);
	_splitpath(szPath, szDrive, szDir, szFName, NULL);
	_makepath(szPath, szDrive, szDir, szFName, ".log");
	g_pszDebugName = szPath;
}

void dprintf(char * szFormat, ...)
{
	bool bFirstly = false;
	if (g_nDebug == -1)
	{
		InitDebug();
		bFirstly = true;
	}
	if (g_nDebug == 0)
		return;
    char buf[256];
    va_list va;
    va_start(va, szFormat);
    wvsprintfA(buf, szFormat, va);
    va_end(va);
	HFILE file;
	OFSTRUCT of;
	if (bFirstly || OpenFile(g_pszDebugName,&of,OF_EXIST)==HFILE_ERROR)
		file = OpenFile(g_pszDebugName,&of,OF_CREATE|OF_WRITE);
	else
		file = OpenFile(g_pszDebugName,&of,OF_WRITE);
	_llseek(file,0,FILE_END);
	_lwrite(file,buf,lstrlen(buf));
	_lclose(file); 
	OutputDebugString(buf);
}

void eprintf(char * szFormat, ...)
{
	if (g_nDebug == -1)
		InitDebug();
    char buf[256];
    va_list va;
    va_start(va, szFormat);
    wvsprintfA(buf, szFormat, va);
    va_end(va);
	AfxMessageBox(buf, MB_OK|MB_ICONEXCLAMATION);
	if (g_nDebug == 0)
		return;
	HFILE file;
	OFSTRUCT of;
	if (OpenFile(g_pszDebugName,&of,OF_EXIST)==HFILE_ERROR)
		file = OpenFile(g_pszDebugName,&of,OF_CREATE|OF_WRITE);
	else
		file = OpenFile(g_pszDebugName,&of,OF_WRITE);
	_llseek(file,0,FILE_END);
	_lwrite(file,buf,lstrlen(buf));
	_lclose(file); 
}


void eprintf(int nFormat, ...)
{
	if (g_nDebug == -1)
		InitDebug();
	char szFormat[256];
	::LoadString(AfxGetInstanceHandle(), nFormat, szFormat, 256);
    char buf[256];
    va_list va;
    va_start(va, szFormat);
    wvsprintfA(buf, szFormat, va);
    va_end(va);
	AfxMessageBox(buf, MB_OK|MB_ICONEXCLAMATION);
	if (g_nDebug == 0)
		return;
	HFILE file;
	OFSTRUCT of;
	if (OpenFile(g_pszDebugName,&of,OF_EXIST)==HFILE_ERROR)
		file = OpenFile(g_pszDebugName,&of,OF_CREATE|OF_WRITE);
	else
		file = OpenFile(g_pszDebugName,&of,OF_WRITE);
	_llseek(file,0,FILE_END);
	_lwrite(file,buf,lstrlen(buf));
	_lclose(file); 
}

bool g_bErrorOccured = false; 

void __dbg_assert_failed(TCHAR *pszFileName, int nLine)
{
	dprintf("Assertion failed %s, %d", pszFileName, nLine);
	g_bErrorOccured = true;
	throw new CProgramException(0,TRUE);
}

BOOL CProgramException::GetErrorMessage( LPTSTR lpszError, UINT nMaxError, PUINT pnHelpContext)
{
	if (m_nError == 0)
		return FALSE;
	int n = ::LoadString(AfxGetInstanceHandle(), m_nError, lpszError, nMaxError);
	if (n == 0)
		_tcsncpy(lpszError, _T("Unknown error"), nMaxError);
	return TRUE;
}

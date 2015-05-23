// ClassTest.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "ClassTest.h"
#include "ClassTestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString _GetProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszDefault)
{
	CString s;
	::GetPrivateProfileString(lpszSection, lpszEntry, lpszDefault, s.GetBuffer(1000), 1000, _GetApp()->m_sProfile);
	s.ReleaseBuffer();
	return s;
}

void _SetProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszString)
{
	::WritePrivateProfileString(lpszSection, lpszEntry, lpszString, _GetApp()->m_sProfile);
}



/////////////////////////////////////////////////////////////////////////////
// CClassTestApp

BEGIN_MESSAGE_MAP(CClassTestApp, CWinApp)
	//{{AFX_MSG_MAP(CClassTestApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClassTestApp construction

CClassTestApp::CClassTestApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CClassTestApp object

CClassTestApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CClassTestApp initialization

BOOL CClassTestApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	SetRegistryKey(_T("Videotest"));
	m_sProfile = AfxGetApp()->GetProfileString(_T("Settings"), _T("ProjectFile"), NULL);
	if (m_sProfile.IsEmpty())
	{
		TCHAR szPath[_MAX_PATH],szDrive[_MAX_DRIVE],szDir[_MAX_DIR];
		GetModuleFileName(AfxGetInstanceHandle(), szPath, _MAX_PATH);
		_tsplitpath(szPath, szDrive, szDir, NULL, NULL);
		_tmakepath(szPath, szDrive, szDir, _T("default"), _T(".ini"));
		m_sProfile =_tcsdup(szPath);
	}
	/*m_sProfile = AfxGetApp()->GetProfileString(_T("Settings"), _T("ProjectFile"), NULL);
	TCHAR szDrive[_MAX_DRIVE],szDir[_MAX_DIR],szFName[_MAX_FNAME],szExt[_MAX_EXT];
	TCHAR szPath1[_MAX_PATH],szDrive1[_MAX_DRIVE],szDir1[_MAX_DIR];
	GetModuleFileName(AfxGetInstanceHandle(), szPath1, _MAX_PATH);
	_tsplitpath(szPath1, szDrive1, szDir1, NULL, NULL);
	_tsplitpath(m_sProfile,szDrive,szDir,szFName,szExt);
	_tmakepath(szPath1,szDrive[0]?szDrive:szDrive1,szDir[0]?szDir:szDir1,szFName[0]?szFName:_T("default"),
		szExt[0]?szExt:_T(".inf"));
	m_sProfile = szPath1;*/


	CClassTestDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

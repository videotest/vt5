// AbacusViewer.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "AbacusViewer.h"
#include "AbacusViewerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAbacusViewerApp

BEGIN_MESSAGE_MAP(CAbacusViewerApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CAbacusViewerApp construction

CAbacusViewerApp::CAbacusViewerApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CAbacusViewerApp object

CAbacusViewerApp theApp;


// CAbacusViewerApp initialization

BOOL CAbacusViewerApp::InitInstance()
{
	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	char szPath[_MAX_PATH],szDrive[_MAX_DRIVE],szDir[_MAX_DIR],szFName[_MAX_FNAME];
	GetModuleFileName(AfxGetInstanceHandle(), szPath, _MAX_PATH);
	_splitpath(szPath, szDrive, szDir, szFName, NULL);
	_makepath(szPath, szDrive, szDir, szFName, ".ini");
	m_pszProfileName = strdup(szPath);

	CAbacusViewerDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
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

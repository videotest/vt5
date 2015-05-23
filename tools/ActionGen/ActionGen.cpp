// ActionGen.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "ActionGen.h"
#include "ActionGenDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CActionGenApp

BEGIN_MESSAGE_MAP(CActionGenApp, CWinApp)
	//{{AFX_MSG_MAP(CActionGenApp)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CActionGenApp construction

CActionGenApp::CActionGenApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CActionGenApp object

CActionGenApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CActionGenApp initialization

BOOL CActionGenApp::InitInstance()
{
	// Standard initialization

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CActionGenDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
	}
	else if (nResponse == IDCANCEL)
	{
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

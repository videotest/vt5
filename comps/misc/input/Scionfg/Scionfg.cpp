// Scionfg.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "Scionfg.h"
#include <comdef.h>
#include "com_main.h"
#include "VT5Profile.h"
#include "DriverScionFG.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma comment(lib, "htmlhelp.lib")
#pragma comment (lib, "common2.lib")

#define pszDriverScionFG "IDriver.DriverScionFG"

// {1DA8F6E8-7071-47d7-8E71-97C16C2550CC}
static const GUID clsidScionFG = 
{ 0x1da8f6e8, 0x7071, 0x47d7, { 0x8e, 0x71, 0x97, 0xc1, 0x6c, 0x25, 0x50, 0xcc } };

CVT5ProfileManager s_ProfileMgr(_T("ScionFG"));
void AjustHelpFileName(CWinApp *papp);


/////////////////////////////////////////////////////////////////////////////
// CCCamApp

BEGIN_MESSAGE_MAP(CScionApp, CWinApp)
	//{{AFX_MSG_MAP(CPixeraApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScionApp construction

CScionApp::CScionApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CScionApp object

CScionApp theApp;

BOOL CScionApp::InitInstance() 
{
	new App;
	App::instance()->Init((HINSTANCE)AfxGetInstanceHandle());
	App::instance()->ObjectInfo(_dyncreate_t<CDriverScion>::CreateObject, clsidScionFG, pszDriverScionFG, szPluginInputDrv);
	ClassFactory * pfactory = (ClassFactory*)ClassFactory::CreateObject();
	pfactory->LoadObjectInfo();
	pfactory->Release();

	// Register all OLE server (factories) as running.  This enables the
	//  OLE libraries to create objects from other applications.
	InitProfile(NULL);
	m_bDiffProf = GetProfileInt(_T("General"), _T("FISH"), -1);
	if (m_bDiffProf == -1)
	{
		m_bDiffProf = FALSE;
		WriteProfileInt(_T("General"), _T("FISH"), m_bDiffProf);
	}
//	if (!((CVT5ProfileManager*)CStdProfileManager::m_pMgr)->IsVT5Profile())
//		AjustHelpFileName(this);
	
	return CWinApp::InitInstance();
}

void CScionApp::InitProfile(LPCTSTR lpName)
{
	TCHAR szPath[_MAX_PATH],szDrive[_MAX_DRIVE],szDir[_MAX_DIR];
	GetModuleFileName(m_hInstance, szPath, _MAX_PATH);
	_tsplitpath(szPath, szDrive, szDir, NULL, NULL);
	TCHAR szDriver[_MAX_FNAME+_MAX_EXT];
	_tcscpy(szDriver, _T("ScionFG"));
	if (m_bDiffProf && lpName && *lpName && _tcsicmp(lpName,_T("Default")))
	{
		_tcscat(szDriver, _T("_"));
		_tcscat(szDriver, lpName);
	}
	_tmakepath(szPath, szDrive, szDir, szDriver, _T(".ini"));
	free((void*)m_pszProfileName);
	m_pszProfileName=_tcsdup(szPath);
}

int CScionApp::ExitInstance() 
{
	OutputDebugString("ScionFG.dll released");
	App::instance()->Deinit();
	App::instance()->Release();
	return CWinApp::ExitInstance();
}

void CScionApp::WinHelp(DWORD dwData, UINT nCmd) 
{
	if (CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("DisableHelp"), 1))
		return;
//	if (!RunHelpTopic(dwData, nCmd))	
		CWinApp::WinHelp(dwData, nCmd);
}

implement_com_exports();





// Pixera.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "Pixera.h"
#include <comdef.h>
#include "com_main.h"
#include "VT5Profile.h"
#include "DriverPixera.h"
#include "PixHelp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma comment(lib, "htmlhelp.lib")
#pragma comment (lib, "common2.lib")

#define pszDriverPixera "IDriver.DriverPixera"

// {59347E7B-C765-4c1f-8E60-41E792CB918A}
static const GUID clsidPixera =
{ 0x59347e7b, 0xc765, 0x4c1f, { 0x8e, 0x60, 0x41, 0xe7, 0x92, 0xcb, 0x91, 0x8a } };

CVT5ProfileManager s_ProfileMgr(_T("Pixera"));
void AjustHelpFileName( CWinApp *papp );

//sergey 25/01/06
int g_nStringIdNewMethodic = IDS_NEW_METHODIC;
//end


//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

/////////////////////////////////////////////////////////////////////////////
// CPixeraApp

BEGIN_MESSAGE_MAP(CPixeraApp, CWinApp)
	//{{AFX_MSG_MAP(CPixeraApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPixeraApp construction

CPixeraApp::CPixeraApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CPixeraApp object

CPixeraApp theApp;

BOOL CPixeraApp::InitInstance() 
{
	new App;
	App::instance()->Init((HINSTANCE)AfxGetInstanceHandle());
	App::instance()->ObjectInfo(_dyncreate_t<CDriverPixera>::CreateObject, clsidPixera, pszDriverPixera, szPluginInputDrv);
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
	if (((CVT5ProfileManager*)CStdProfileManager::m_pMgr)->IsVT5Profile())
		SetHelpMode(afxHTMLHelp);
	else
		AjustHelpFileName(this);
	
	return CWinApp::InitInstance();
}

void CPixeraApp::InitProfile(LPCTSTR lpName)
{
	TCHAR szPath[_MAX_PATH],szDrive[_MAX_DRIVE],szDir[_MAX_DIR];
	GetModuleFileName(m_hInstance, szPath, _MAX_PATH);
	_tsplitpath(szPath, szDrive, szDir, NULL, NULL);
	TCHAR szBaumer[_MAX_FNAME+_MAX_EXT];
	_tcscpy(szBaumer, _T("Pixera"));
	if (m_bDiffProf && lpName && *lpName && _tcsicmp(lpName,_T("Default")))
	{
		_tcscat(szBaumer, _T("_"));
		_tcscat(szBaumer, lpName);
	}
	_tmakepath(szPath, szDrive, szDir, szBaumer, _T(".ini"));
	free((void*)m_pszProfileName);
	m_pszProfileName=_tcsdup(szPath);
}

int CPixeraApp::ExitInstance() 
{	
	App::instance()->Deinit();
	App::instance()->Release();
	return CWinApp::ExitInstance();
}

void CPixeraApp::WinHelpInternal(DWORD dwData, UINT nCmd) 
{
	if (CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("DisableHelp"), 1))
		return;
	if (GetHelpMode() == afxHTMLHelp)
		RunHelpTopic(dwData, nCmd);
	else
		__super::WinHelpInternal(dwData, nCmd);
}

implement_com_exports();


// Sumix.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "Sumix.h"
#include "SumixDriver.h"
#include <comdef.h>
#include "com_main.h"
#include "misc_utils.h"
#include "VT5Profile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma comment(lib, "htmlhelp.lib")
#pragma comment (lib, "common2.lib")

#define pszDriverSumix "IDriver.DriverSumix"

// {E81347EF-1F49-4464-8306-19E9F5BE52AE}
static const GUID clsidSumix = 
{ 0xe81347ef, 0x1f49, 0x4464, { 0x83, 0x6, 0x19, 0xe9, 0xf5, 0xbe, 0x52, 0xae } };


CStdProfileManager s_ProfileMgr;
int g_nStringIdNewMethodic = IDS_NEW_METHODIC;


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

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

// CSumixApp

BEGIN_MESSAGE_MAP(CSumixApp, CWinApp)
END_MESSAGE_MAP()


// CSumixApp construction

CSumixApp::CSumixApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CSumixApp object

CSumixApp theApp;


// CSumixApp initialization

BOOL CSumixApp::InitInstance()
{
	new App;
	App::instance()->Init((HINSTANCE)AfxGetInstanceHandle());
	App::instance()->ObjectInfo(_dyncreate_t<CSumixDriver>::CreateObject, clsidSumix, pszDriverSumix, szPluginInputDrv);
	ClassFactory * pfactory = (ClassFactory*)ClassFactory::CreateObject();
	pfactory->LoadObjectInfo();
	pfactory->Release();
	if (((CVT5ProfileManager*)CStdProfileManager::m_pMgr)->IsVT5Profile())
		SetHelpMode(afxHTMLHelp);
	CWinApp::InitInstance();
	return TRUE;
}

int CSumixApp::ExitInstance()
{
	App::instance()->Deinit();
	App::instance()->Release();
	return CWinApp::ExitInstance();
}

void CSumixApp::WinHelpInternal(DWORD dwData, UINT nCmd) 
{
	if (CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("DisableHelp"), 1))
		return;
	if (GetHelpMode() == afxHTMLHelp)
		HelpDisplayTopic("ProgRes",NULL,"Preview","$GLOBAL_main");
	else
		__super::WinHelpInternal(dwData, nCmd);
}

implement_com_exports();

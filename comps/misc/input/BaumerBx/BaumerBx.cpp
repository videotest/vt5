// BaumerBx.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "BaumerBx.h"
#include "BaumerBxDriver.h"
#include <comdef.h>
#include "com_main.h"
#include "misc_utils.h"
#include "VT5Profile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma comment(lib, "htmlhelp.lib")
#pragma comment (lib, "common2.lib")

#define pszDriverBaumerBx "IDriver.DriverBaumerBx"

// {A72FEC30-35A5-4bd3-8F26-DEEA7F6A005B}
static const GUID clsidBaumerBx = 
{ 0xa72fec30, 0x35a5, 0x4bd3, { 0x8f, 0x26, 0xde, 0xea, 0x7f, 0x6a, 0x0, 0x5b } };

CStdProfileManager s_ProfileMgr;
int g_nStringIdNewMethodic = IDS_NEW_METHODIC;

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

// CBaumerBxApp

BEGIN_MESSAGE_MAP(CBaumerBxApp, CWinApp)
END_MESSAGE_MAP()


// CBaumerBxApp construction

CBaumerBxApp::CBaumerBxApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CBaumerBxApp object

CBaumerBxApp theApp;


// CBaumerBxApp initialization

BOOL CBaumerBxApp::InitInstance()
{
	new App;
	App::instance()->Init((HINSTANCE)AfxGetInstanceHandle());
	App::instance()->ObjectInfo(_dyncreate_t<CBaumerBxDriver>::CreateObject, clsidBaumerBx, pszDriverBaumerBx, szPluginInputDrv);
	ClassFactory * pfactory = (ClassFactory*)ClassFactory::CreateObject();
	pfactory->LoadObjectInfo();
	pfactory->Release();
	if (((CVT5ProfileManager*)CStdProfileManager::m_pMgr)->IsVT5Profile())
		SetHelpMode(afxHTMLHelp);
	CWinApp::InitInstance();
	return TRUE;
}

int CBaumerBxApp::ExitInstance()
{
	App::instance()->Deinit();
	App::instance()->Release();
	return CWinApp::ExitInstance();
}

void CBaumerBxApp::WinHelpInternal(DWORD_PTR dwData, UINT nCmd) 
{
	if (CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("DisableHelp"), 1))
		return;
	if (GetHelpMode() == afxHTMLHelp)
		HelpDisplayTopic("ProgRes",NULL,"Preview","$GLOBAL_main");
	else
		__super::WinHelpInternal(dwData, nCmd);
}

implement_com_exports();


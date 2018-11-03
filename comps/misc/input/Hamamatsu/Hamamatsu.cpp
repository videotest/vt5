// Hamamatsu.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "Hamamatsu.h"
#include "HamamatsuDriver.h"
#include <comdef.h>
#include <../common2/com_main.h>
#include "VT5Profile.h"
#include <data5.h>
#include <../common2/misc_utils.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma comment(lib, "htmlhelp.lib")
#pragma comment (lib, "common2.lib")

#define pszDriverHamamatsu "IDriver.DriverHamamatsu"

// {0257AAB0-E05A-4425-BD4F-FD7839D20A80}
static const GUID clsidHamamatsu = 
{ 0x257aab0, 0xe05a, 0x4425, { 0xbd, 0x4f, 0xfd, 0x78, 0x39, 0xd2, 0xa, 0x80 } };

CStdProfileManager s_ProfileMgr;
int g_nStringIdNewMethodic = IDS_NEW_METHODIC;


// CHamamatsuApp

BEGIN_MESSAGE_MAP(CHamamatsuApp, CWinApp)
END_MESSAGE_MAP()


// CHamamatsuApp construction

CHamamatsuApp::CHamamatsuApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CHamamatsuApp object

CHamamatsuApp theApp;


// CHamamatsuApp initialization

BOOL CHamamatsuApp::InitInstance()
{
	new App;
	App::instance()->Init((HINSTANCE)AfxGetInstanceHandle());
	App::instance()->ObjectInfo(_dyncreate_t<CHamamatsuDriver>::CreateObject, clsidHamamatsu, pszDriverHamamatsu, szPluginInputDrv);
	ClassFactory * pfactory = (ClassFactory*)ClassFactory::CreateObject();
	pfactory->LoadObjectInfo();
	pfactory->Release();
	if (((CVT5ProfileManager*)CStdProfileManager::m_pMgr)->IsVT5Profile())
		SetHelpMode(afxHTMLHelp);
	CWinApp::InitInstance();
	return TRUE;
}

int CHamamatsuApp::ExitInstance()
{
	App::instance()->Deinit();
	App::instance()->Release();
	return CWinApp::ExitInstance();
}

void CHamamatsuApp::WinHelpInternal(DWORD_PTR dwData, UINT nCmd) 
{
	if (CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("DisableHelp"), 1))
		return;
	if (GetHelpMode() == afxHTMLHelp)
		HelpDisplayTopic("ProgRes",NULL,"Preview","$GLOBAL_main");
	else
		__super::WinHelpInternal(dwData, nCmd);
}

implement_com_exports();


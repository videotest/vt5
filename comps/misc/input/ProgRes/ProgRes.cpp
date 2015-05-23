// ProgRes.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "ProgRes.h"
#include "ProgResDriver.h"
#include <comdef.h>
#include "com_main.h"
#include "misc_utils.h"
#include "VT5Profile.h"
#include "DrvHelp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma comment(lib, "htmlhelp.lib")
#pragma comment (lib, "common2.lib")

#define pszDriverProgRes "IDriver.DriverProgRes"

// {E385B23F-4E46-4494-93C9-D4D6BF64AA0C}
static const GUID clsidProgRes = 
{ 0xe385b23f, 0x4e46, 0x4494, { 0x93, 0xc9, 0xd4, 0xd6, 0xbf, 0x64, 0xaa, 0xc } };

CStdProfileManager s_ProfileMgr;
int g_nStringIdNewMethodic = IDS_NEW_METHODIC;


// CProgResApp

BEGIN_MESSAGE_MAP(CProgResApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CProgResApp construction

CProgResApp::CProgResApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CProgResApp object

CProgResApp theApp;


// CProgResApp initialization

BOOL CProgResApp::InitInstance()
{
	new App;
	App::instance()->Init((HINSTANCE)AfxGetInstanceHandle());
	App::instance()->ObjectInfo(_dyncreate_t<CProgResDriver>::CreateObject, clsidProgRes, pszDriverProgRes, szPluginInputDrv);
	ClassFactory * pfactory = (ClassFactory*)ClassFactory::CreateObject();
	pfactory->LoadObjectInfo();
	pfactory->Release();
	if (((CVT5ProfileManager*)CStdProfileManager::m_pMgr)->IsVT5Profile())
		SetHelpMode(afxHTMLHelp);
//	else
//		AjustHelpFileName(this);
	CWinApp::InitInstance();
	return TRUE;
}

int CProgResApp::ExitInstance()
{
	App::instance()->Deinit();
	App::instance()->Release();
	return CWinApp::ExitInstance();
}

CHtmlTopic Topics[] = 
{
	{IDD_PREVIEW, _T("Preview")},
	{IDD_ENHANCEMENT, _T("Enhancement")},
	{IDD_IMAGE, _T("Image")},
	{IDD_COLOR_BALANCE, _T("ColorBalance")},
	{IDD_SETTINGS, _T("Settings")},
	{IDD_SHUTTER, _T("Shutter")},
	{IDD_HISTO, _T("Histogram")},
	{IDD_DIALOG_SETTING_NAME, _T("RenameProfileDialog")},
	{IDD_DIALOG_SETTINGS, _T("Settings")},
	{0, NULL},
};


void CProgResApp::WinHelpInternal(DWORD dwData, UINT nCmd) 
{
	if (CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("DisableHelp"), 1, true, true))
		return;
	if (GetHelpMode() == afxHTMLHelp)
		HelpDisplayTopic("ProgRes",NULL,_topic_by_data(dwData,Topics),"$GLOBAL_main");
	else
		__super::WinHelpInternal(dwData, nCmd);
}

implement_com_exports();


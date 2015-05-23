// PVCam.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "PVCamApp.h"
#include "PVCamDriver.h"
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


#define pszDriverPVCam "IDriver.DriverPVCam"

// {795D8120-08FC-461d-8F82-27B16B8AABEE}
static const GUID clsidPVCam = 
{ 0x795d8120, 0x8fc, 0x461d, { 0x8f, 0x82, 0x27, 0xb1, 0x6b, 0x8a, 0xab, 0xee } };

CStdProfileManager s_ProfileMgr;
int g_nStringIdNewMethodic = IDS_NEW_METHODIC;

// CPVCamApp

BEGIN_MESSAGE_MAP(CPVCamApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CPVCamApp construction

CPVCamApp::CPVCamApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CPVCamApp object

CPVCamApp theApp;


// CPVCamApp initialization

BOOL CPVCamApp::InitInstance()
{
	new App;
	App::instance()->Init((HINSTANCE)AfxGetInstanceHandle());
	App::instance()->ObjectInfo(_dyncreate_t<CPVCamDriver>::CreateObject, clsidPVCam, pszDriverPVCam, szPluginInputDrv);
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

int CPVCamApp::ExitInstance()
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


void CPVCamApp::WinHelpInternal(DWORD dwData, UINT nCmd) 
{
	if (CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("DisableHelp"), 1, true, true))
		return;
	if (GetHelpMode() == afxHTMLHelp)
		HelpDisplayTopic("ProgRes",NULL,_topic_by_data(dwData,Topics),"$GLOBAL_main");
	else
		__super::WinHelpInternal(dwData, nCmd);
}

implement_com_exports();



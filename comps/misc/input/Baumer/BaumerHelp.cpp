#include "StdAfx.h"
#include "Defs.h"
#include "Baumer.h"
//#include "BaumerHelp.h"
//#include "Vt5Profile.h"
//#include "htmlhelp.h"
#include "ComDef.h"
#include "misc_utils.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

_bstr_t g_strHtmlFileName;
static void InitHtmlFile()
{
	char	szPathName[_MAX_PATH], szDrv[_MAX_DRIVE], szPath[_MAX_DIR], szName[_MAX_PATH];
	const char	cszHelp[] = "Help";
	::GetModuleFileName( ::AfxGetInstanceHandle(), szPathName, _MAX_PATH );
	::_splitpath( szPathName, szDrv, szPath, szName, 0 );
	::_makepath( szPathName, szDrv, szPath, 0, 0 );
	g_strHtmlFileName  = ::GetValueString( ::GetAppUnknown(), _T("Paths"), _T("HelpPath"), szPathName );
	::_splitpath(g_strHtmlFileName, szDrv, szPath, 0, 0 );
	::_makepath(szPathName, szDrv, szPath, _T("Baumer"), _T(".chm") );
	g_strHtmlFileName = szPathName;
}

struct CHtmlTopic
{
	DWORD dwData;
	LPCTSTR lpTopicName;
};

CHtmlTopic Topics[] = 
{
	{IDD_BAUMER, _T("Preview")},
	{IDD_AMPL, _T("Gain")},
	{IDD_CAMERA, _T("Camera")},
	{IDD_COLOR_BALANCE, _T("color_balance")},
	{IDD_FRAME, _T("Frame")},
	{IDD_WHITE_BALANCE, _T("white_balance")},
	{IDD_MANUAL, _T("Manual")},
	{IDD_TRIGGER, _T("Trigger")},
};

#define TOPICS (sizeof(Topics)/sizeof(CHtmlTopic))

LPCTSTR __topic_by_data(DWORD dwData)
{
	for (int i = 0; i < TOPICS; i++)
	{
		if (Topics[i].dwData == dwData-0x20000)
			return Topics[i].lpTopicName;
	}
	return NULL;
}


int __topic_by_contextID(DWORD dwContextID)
{
	return dwContextID;
}

BOOL IsUseHtmlHelp()
{
	return ((CVT5ProfileManager *)CStdProfileManager::m_pMgr)->IsVT5Profile();
}

BOOL RunHelpTopic(DWORD dwData, UINT uCmd)
{
	if (IsUseHtmlHelp())
	{
#if 0
		// Load library
		if (g_strHtmlFileName.IsEmpty()) InitHtmlFile();
		HWND hwnd = AfxGetMainWnd()->m_hWnd;
		// Init window type
		HH_WINTYPE	hhwin;
		ZeroMemory(&hhwin, sizeof(hhwin));
		hhwin.cbStruct = sizeof(hhwin);
		hhwin.pszType = "mainwnd";        
		hhwin.pszCaption = "VideoHelP";
		hhwin.fsWinProperties = HHWIN_PROP_NOTITLEBAR|HHWIN_PROP_ONTOP;
		int n = (int)::HtmlHelp(hwnd, 0, HH_SET_WIN_TYPE, (DWORD)&hhwin);
		// Show window
		CString strTopicName(__topic_by_data(dwData));
		if (strTopicName.IsEmpty()) return TRUE;
		CString strHtmlFileName(g_strHtmlFileName);
		strHtmlFileName+="::/html/";
		strHtmlFileName+=strTopicName;
		strHtmlFileName+=".htm";
		strHtmlFileName+=">mainwnd";
		HWND hwndHelp = ::HtmlHelp(hwnd, strHtmlFileName, HH_DISPLAY_TOPIC, 0);
		::ShowWindow( hwndHelp, SW_SHOW );
#endif
		HelpDisplayTopic("Baumer",NULL,__topic_by_data(dwData),"$GLOBAL_main");

		return TRUE;
	}
	else
		return FALSE;;
}



BOOL RunContextHelp(HELPINFO* pHelpInfo)
{
	if(!pHelpInfo->iCtrlId || !pHelpInfo->dwContextId)
		return FALSE;
	if (IsUseHtmlHelp())
		return FALSE;
	int	arr[4];
	int	icount = 0;
	arr[icount++] = pHelpInfo->iCtrlId;
	arr[icount++] = pHelpInfo->dwContextId;
	arr[icount++] = 0;
	arr[icount++] = 0;
	if (!::WinHelp((HWND)pHelpInfo->hItemHandle, AfxGetApp()->m_pszHelpFilePath, HELP_WM_HELP, (DWORD)(void*)arr))
		AfxMessageBox(AFX_IDP_FAILED_TO_LAUNCH_HELP);
	return TRUE;
}

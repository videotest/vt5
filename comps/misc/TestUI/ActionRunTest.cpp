#include "StdAfx.h"
#include "TestUI.h"
#include "core5.h"
#include "actionruntest.h"
#include "RunTestsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
//CActionRunTestWindow
////////////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CActionRunTestWindow, CCmdTargetEx);

// {8BC0432C-FA85-4ad8-AE16-6ED5EBB6B756}
GUARD_IMPLEMENT_OLECREATE(CActionRunTestWindow, _T("TestUI.ActionRunTestWindow"), 
0x8bc0432c, 0xfa85, 0x4ad8, 0xae, 0x16, 0x6e, 0xd5, 0xeb, 0xb6, 0xb7, 0x56);


// {4714773D-3CA7-4b27-8F12-623ED0972529}
static const GUID guidActionRunTestWindow = 
{ 0x4714773d, 0x3ca7, 0x4b27, { 0x8f, 0x12, 0x62, 0x3e, 0xd0, 0x97, 0x25, 0x29 } };

ACTION_INFO_FULL(CActionRunTestWindow, ID_ACTION_RUNTESTWINDOW, -1, -1, guidActionRunTestWindow);
ACTION_TARGET(CActionRunTestWindow, szTargetApplication);
ACTION_ARG_LIST(CActionRunTestWindow)
END_ACTION_ARG_LIST();


CActionRunTestWindow::CActionRunTestWindow(void)
{
}

CActionRunTestWindow::~CActionRunTestWindow(void)
{
}

bool CActionRunTestWindow::Invoke()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	HWND	hwnd = 0;
	IApplicationPtr	ptrA(GetAppUnknown());
	ptrA->GetMainWindowHandle(&hwnd);
	CRunTestsDlg dlg(CWnd::FromHandle(hwnd));
	int iRet = dlg.DoModal();
	IUnknownPtr punkTM(_GetOtherComponent(GetAppUnknown(), IID_ITestManager), false);
	ITestProcessPtr sptrTP(punkTM);
	if (iRet == IDC_BUTTON_SAVE_TEST)
		sptrTP->RunTestSave();
	else if (iRet == IDC_BUTTON_TEST_SYSTEM)
		sptrTP->RunTestCompare();
	return true;
}
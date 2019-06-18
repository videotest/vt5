#include "StdAfx.h"
#include "resource.h"
#include "BatchInputAction.h"
#include "BatchInputDlg.h"
//#include "core5.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
//CActionBatchInput
////////////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CActionBatchInput, CCmdTargetEx);

// {0524581D-BD17-4605-83D8-E7E21E76B6B9}
GUARD_IMPLEMENT_OLECREATE(CActionBatchInput, _T("BatchInput.ActionBatchInput"), 
0x524581d, 0xbd17, 0x4605, 0x83, 0xd8, 0xe7, 0xe2, 0x1e, 0x76, 0xb6, 0xb9);

// {B128C8A6-F218-4D15-AC79-A492B104F20A}
static const GUID guidActionBatchInput = 
{ 0xb128c8a6, 0xf218, 0x4d15, { 0xac, 0x79, 0xa4, 0x92, 0xb1, 0x4, 0xf2, 0xa } };
ACTION_INFO_FULL(CActionBatchInput, ID_ACTION_BATCHINPUT, -1, -1, guidActionBatchInput);
ACTION_TARGET(CActionBatchInput, szTargetApplication);
ACTION_ARG_LIST(CActionBatchInput)
END_ACTION_ARG_LIST();


CActionBatchInput::CActionBatchInput()
{
}

CActionBatchInput::~CActionBatchInput()
{
}

static IUnknown *__GetMainWnd()
{
	return ::_GetOtherComponent(::GetAppUnknown(), IID_IMainWindow);
}

static IUnknownPtr __FindBatchInputBar()
{
	//get the main window
	IUnknown	*punkMainWnd = ::_GetOtherComponent( ::GetAppUnknown(), IID_IMainWindow );
	sptrIMainWindow	sptrM( punkMainWnd );
	if( !punkMainWnd )
		return 0;
	punkMainWnd->Release();
	//find the BatchInputDlg window between dockbars
	long	lPos =0;
	sptrM->GetFirstDockWndPosition( &lPos );
	while( lPos )
	{
		IUnknownPtr punkDockBar;
		sptrM->GetNextDockWnd(&punkDockBar, &lPos);
		INamedObject2Ptr sptrNODockBar(punkDockBar);
		BSTR bstr = NULL;
		if (FAILED(sptrNODockBar->GetName(&bstr)) || bstr == NULL)
			continue;
		CString s = bstr;
		::SysFreeString(bstr);
		if (s.CompareNoCase(_T("BatchInputDlg")) == 0)
			return punkDockBar;
	}
	//if no BatchInputDlg window avaible
	return IUnknownPtr(NULL);

}

bool CActionBatchInput::Invoke()
{
/*	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	HWND	hwnd = 0;
	IApplicationPtr	ptrA(App::application());
	ptrA->GetMainWindowHandle(&hwnd);
	BatchInputDlg dlg;
	dlg.DoModal();*/
	IUnknownPtr punkMain(__GetMainWnd(), false);
	sptrIMainWindow	sptrM(punkMain);
	IUnknownPtr punkDock = __FindBatchInputBar();
	IWindowPtr sptrWDock(punkDock);
	HWND hwndDock;
	sptrWDock->GetHandle((HANDLE *)&hwndDock);
	sptrM->ShowControlBar(hwndDock, SW_SHOW);
	return true;
}





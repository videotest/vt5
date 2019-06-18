// FormManager.cpp : implementation file
//
//FormManager.ExecuteDockBar "lookup"
/*
for i = 0 to 100 
  FormManager.Finalize "Lookup"
  FormManager.ExecuteDockBar "lookup"
next


Set frm = MainWnd.ActiveFrame
Set s = Frm.GetSplitter

for i = 0 to 5
  s.SetViewType 0, 0, "AxEditor.AxViewD"
  s.SetViewType 0, 0, "ImageDoc.ImageViewD" 
next

*/


#include "stdafx.h"
#include "axform.h"
#include "FormManager.h"
#include "FormDialog.h"
#include "FormDockDialog.h"
#include "FormPage.h"
#include "Page.h"
#include "afxpriv.h"
#include "WizButton.h"
#include "WizardDlg.h"
#include "button_messages.h"


bool CanDestroyForm(bool bOk = false)
{

	IApplication	*papp = GetAppUnknown();
	if( papp->IsNestedMessageLoop() == S_FALSE )
		return true;

	IUnknown	*punkAM = 0;
	papp->GetActionManager( &punkAM );
	IActionManagerPtr	ptrAM( punkAM );
	punkAM->Release();
	if (bOk)
		ptrAM->FinalizeInteractiveAction();
	else
		ptrAM->TerminateInteractiveAction();
	//AfxMessageBox( IDS_CANTEXIT_OPERATION );
	return false;
}

#define WizardBtnInterval	5
#define WizardBtnWidth		75
#define WizardBtnHeight		21

CString	g_strFormPath;

void _EnterSettingsMode(CSize size, UINT nIDtempl, HWND* phWnd)
{
	IUnknown	*punkMW = _GetOtherComponent( GetAppUnknown(), IID_IMainWindow );
	IMainWindowPtr	sptrMain(punkMW);
	if(punkMW)
		punkMW->Release();

	if(sptrMain == 0)return;
	
	CDialogTemplate	dt;
	dt.Load( MAKEINTRESOURCE(nIDtempl));
	HGLOBAL	hTempl = dt.Detach();
	((DLGTEMPLATE*)hTempl)->cx = (short)size.cx;
	
	int nHeight = size.cy/*+2*WizardBtnInterval*/;
	//if(size.cy < 4*WizardBtnInterval+3*WizardBtnHeight)
	//	size.cy = 2*WizardBtnInterval + 3*WizardBtnHeight;

	((DLGTEMPLATE*)hTempl)->cy = (short)size.cy;

	bool bEnableApply = ::GetValueInt( ::GetAppUnknown(), "\\FormManager", "EnableApply", 1  ) != 0;
	if( bEnableApply )
		nHeight = max(nHeight, 5*WizardBtnInterval+4*WizardBtnHeight);
	else
		nHeight = max(nHeight, 4*WizardBtnInterval+3*WizardBtnHeight);

	if(sptrMain != 0)
		sptrMain->EnterSettingsMode(phWnd, nHeight);
}

/////////////////////////////////////////////////////////////////////////////
// CFormManager

IMPLEMENT_DYNCREATE(CFormManager, CCmdTarget)

CFormManager*	CFormManager::m_pFormManager = 0;

CFormManager::CFormManager()
{
	m_sName = "FormManager";
	EnableAutomation();
	m_pFormPage = 0;

	// To keep the application running as long as an OLE automation 
	//	object is active, the constructor calls AfxOleLockApp.
	
	m_dlgOptions = new COptionsDlg();

	m_pBtnOk = 0;
	m_pBtnCancel = 0;
	m_pBtnApply = 0;
	m_pBtnHelp = 0;

	g_strFormPath = ::MakeAppPathName( "Forms" );
	g_strFormPath+= "\\";
	g_strFormPath = ::GetValueString( ::GetAppUnknown(), "Paths", "FormPath", g_strFormPath );

	_OleLockApp( this );

	HWND	m_hwndSettings = 0;

	m_pWizard = 0;

	m_bModalMode = false;

	m_pFormManager = this;
	m_pFormDialog = NULL;
	m_bDisableBtnMode = false;
}

CFormManager::~CFormManager()
{
	_OleUnlockApp( this );
	m_pFormManager = 0;
}

void CFormManager::_KillButtons()
{
	if(m_pBtnOk)
	{
		delete m_pBtnOk;
		m_pBtnOk = 0;
	}
	if(m_pBtnCancel)
	{
		delete m_pBtnCancel;
		m_pBtnCancel = 0;
	}
	if(m_pBtnApply)
	{
		delete m_pBtnApply;
		m_pBtnApply = 0;
	}
	if( m_pBtnHelp )
	{
		delete m_pBtnHelp;
		m_pBtnHelp = 0;
	}
}

void CFormManager::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	m_strStackPages.RemoveAll();

	if(m_dlgOptions)
	{
		delete m_dlgOptions;
		m_dlgOptions = 0;
	}
	if(m_pFormPage)
	{
		m_pFormPage->DestroyWindow();
		delete m_pFormPage;
		m_pFormPage = 0;
	}

	_KillButtons();

	if(m_pWizard)
	{
		delete m_pWizard;
		m_pWizard = 0;
	}
	
	CCmdTarget::OnFinalRelease();

}


BEGIN_MESSAGE_MAP(CFormManager, CCmdTarget)
	//{{AFX_MSG_MAP(CFormManager)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CFormManager, CCmdTarget)
	//{{AFX_DISPATCH_MAP(CFormManager)
	DISP_PROPERTY_EX(CFormManager, "ModalMode", GetModalMode, SetModalMode, VT_BOOL)
	DISP_FUNCTION(CFormManager, "ExecutePage", ExecutePage, VT_I4, VTS_BSTR)
	DISP_FUNCTION(CFormManager, "ExecuteModal", ExecuteModal, VT_I4, VTS_BSTR)
	DISP_FUNCTION(CFormManager, "ExecuteDockBar", ExecuteDockBar, VT_I4, VTS_BSTR)
	DISP_FUNCTION(CFormManager, "ExecuteDockBar2", ExecuteDockBar2, VT_I4, VTS_BSTR VTS_I4 VTS_BOOL)
	DISP_FUNCTION(CFormManager, "Finalize", Finalize, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CFormManager, "ExecuteOptions", ExecuteOptions, VT_I4, VTS_NONE)
	DISP_FUNCTION(CFormManager, "ResetOptions", ResetOptions, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CFormManager, "LoadCategoryImage", LoadCategoryImage, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CFormManager, "AddOptionsCategory", AddOptionsCategory, VT_EMPTY, VTS_I2 VTS_BSTR)
	DISP_FUNCTION(CFormManager, "SetActiveCategory", SetActiveCategory, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CFormManager, "GetActiveCategory", GetActiveCategory, VT_BSTR, VTS_NONE)
	DISP_FUNCTION(CFormManager, "AddOptionsPage", AddOptionsPage, VT_EMPTY, VTS_BSTR VTS_BSTR)
	DISP_FUNCTION(CFormManager, "SetActivePage", SetActivePage, VT_BOOL, VTS_BSTR)
	DISP_FUNCTION(CFormManager, "GetActivePage", GetActivePage, VT_BSTR, VTS_NONE)
	DISP_FUNCTION(CFormManager, "StoreOptionsConfig", StoreOptionsConfig, VT_BOOL, VTS_BSTR)
	DISP_FUNCTION(CFormManager, "LoadOptionsConfig", LoadOptionsConfig, VT_BOOL, VTS_BSTR)
	DISP_FUNCTION(CFormManager, "CreateNewWizard", CreateNewWizard, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CFormManager, "ExecuteWizard", ExecuteWizard, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CFormManager, "StackCurrentPage", StackCurrentPage, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CFormManager, "ExecutePage2", ExecutePage2, VT_EMPTY, VTS_BSTR VTS_BOOL VTS_BOOL VTS_BOOL)
	DISP_FUNCTION(CFormManager, "AddWizardPage", AddWizardPage, VT_EMPTY, VTS_BSTR VTS_BOOL)
	DISP_FUNCTION(CFormManager, "ExecuteNestedWizard", ExecuteNestedWizard, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CFormManager, "EnableNext", EnableNext, VT_EMPTY, VTS_BOOL)
	DISP_FUNCTION(CFormManager, "EnablePrev", EnablePrev, VT_EMPTY, VTS_BOOL)
	DISP_FUNCTION(CFormManager, "EnableOk", EnableOk, VT_EMPTY, VTS_BOOL)
	DISP_FUNCTION(CFormManager, "EnableApply", EnableApply, VT_EMPTY, VTS_BOOL)
	DISP_FUNCTION(CFormManager, "PressNext", PressNext, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CFormManager, "PressPrev", PressPrev, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CFormManager, "PressOK", PressOK, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CFormManager, "PressCancel", PressCancel, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CFormManager, "ExecutePageModal", ExecutePageModal, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CFormManager, "ExecutePageModal", ExecutePageModal, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CFormManager, "CreateHideDockBar", CreateHideDockBar, VT_I4, VTS_BSTR)
	DISP_FUNCTION(CFormManager, "PressHelp", PressHelp, VT_EMPTY, VTS_NONE)
	DISP_PROPERTY_EX(CFormManager, "PageActive", GetPageActive, SetPageActive, VT_BOOL)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IFormManager to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {C4633177-A0FD-440A-A2DE-73DBCAC188A5}
static const IID IID_IFormManager =
{ 0xc4633177, 0xa0fd, 0x440a, { 0xa2, 0xde, 0x73, 0xdb, 0xca, 0xc1, 0x88, 0xa5 } };

// {57F4F94D-7AD5-4821-AC4B-7939A6292D76}
GUARD_IMPLEMENT_OLECREATE(CFormManager, "axform.FormManager", 
0x57f4f94d, 0x7ad5, 0x4821, 0xac, 0x4b, 0x79, 0x39, 0xa6, 0x29, 0x2d, 0x76)

BEGIN_INTERFACE_MAP(CFormManager, CCmdTarget)
	INTERFACE_PART(CFormManager, IID_IFormManager, Dispatch)
	INTERFACE_PART(CFormManager, IID_INamedObject2, Name)
END_INTERFACE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CFormManager message handlers

long CFormManager::ExecutePage(LPCTSTR szFormName) 
{
	bool	bWasPage = false;
	if(m_pFormPage)
	{
		if( !CanDestroyForm() )return 0;
		CString strName = ::GetObjectName(m_pFormPage->GetControllingUnknown());
		//don't leave mode, only destroy window
		m_pFormPage->DestroyWindow();
		delete m_pFormPage;
		m_pFormPage = 0;
		bWasPage = true;
		//Finalize(strName);
	}

	IUnknown	*punk = CFormManager::_GetFormByName( szFormName );
	IUnknown	*punkMW = ::_GetOtherComponent( GetAppUnknown(), IID_IMainWindow );
	IMainWindowPtr	ptrMainWnd = punkMW;
	if( punkMW )punkMW->Release();

	IActiveXFormPtr sptrForm(punk);
	if( punk )punk->Release();
	
	if(sptrForm == 0)
	{
		if( bWasPage )
		{
			ptrMainWnd->LeaveSettingsMode();
			_KillButtons();
		}
		return 0;
	}

	SIZE size;	   
	sptrForm->GetSize( &size );

	
	HWND hwndSettings = 0;
	_EnterSettingsMode(size, IDD_CONTAINER_DOCKDIALOG, &hwndSettings);

	CString strBtnName;
	CRect	rcBtn = NORECT;
	strBtnName.LoadString(IDS_BTN_OK);
	if( !m_pBtnOk )m_pBtnOk = new CWizButton(this, hwndSettings, IDOK, strBtnName);
	rcBtn = CRect(size.cx+2*WizardBtnInterval, 1*WizardBtnInterval, 0, 0);
	rcBtn.right = rcBtn.left + WizardBtnWidth;
	rcBtn.bottom = rcBtn.top + WizardBtnHeight;
	m_pBtnOk->MoveWindow(&rcBtn);

	strBtnName.LoadString(IDS_BTN_CANCEL);
	if( !m_pBtnCancel )m_pBtnCancel = new CWizButton(this, hwndSettings, IDCANCEL, strBtnName);
	rcBtn = CRect(size.cx+2*WizardBtnInterval, 2*WizardBtnInterval+WizardBtnHeight, 0, 0);
	rcBtn.right = rcBtn.left + WizardBtnWidth;
	rcBtn.bottom = rcBtn.top + WizardBtnHeight;
	m_pBtnCancel->MoveWindow(&rcBtn);
		
	bool bEnableApply = ::GetValueInt( ::GetAppUnknown(), "\\FormManager", "EnableApply", 1  ) != 0;
	if( bEnableApply )
	{
		strBtnName.LoadString(IDS_BTN_APPLY);
		if( !m_pBtnApply )m_pBtnApply = new CWizButton(this, hwndSettings, IDAPPLY, strBtnName);
		rcBtn = CRect(size.cx+2*WizardBtnInterval, 3*WizardBtnInterval+2*WizardBtnHeight, 0, 0);
		rcBtn.right = rcBtn.left + WizardBtnWidth;
		rcBtn.bottom = rcBtn.top + WizardBtnHeight;
		m_pBtnApply->MoveWindow(&rcBtn);
	}

	strBtnName.LoadString(IDS_BTN_HELP);
	if( !m_pBtnHelp )m_pBtnHelp = new CWizButton(this, hwndSettings, IDHELP, strBtnName);
	if( bEnableApply )
		rcBtn = CRect(size.cx+2*WizardBtnInterval, 4*WizardBtnInterval+3*WizardBtnHeight, 0, 0);
	else
		rcBtn = CRect(size.cx+2*WizardBtnInterval, 3*WizardBtnInterval+2*WizardBtnHeight, 0, 0);

	rcBtn.right = rcBtn.left + WizardBtnWidth;
	rcBtn.bottom = rcBtn.top + WizardBtnHeight;
	m_pBtnHelp->MoveWindow(&rcBtn);

	


	m_pFormPage = new CFormPage(szFormName);
	m_pFormPage->SetForm( sptrForm );
	

	
	if(::IsWindow(hwndSettings))
	{
		CSize size = m_pFormPage->m_site.GetSize();
		if(size.cy < 2*WizardBtnInterval+3*WizardBtnHeight)
			size.cy = 2*WizardBtnInterval + 3*WizardBtnHeight;
		m_pFormPage->m_site.SetSize(size);

		m_pFormPage->Create(IDD_CONTAINER_DOCKDIALOG, CWnd::FromHandle(hwndSettings));

		
		CRect	rc = NORECT;
		CRect	rcWnd = NORECT;
		m_pFormPage->GetClientRect(&rc);
		::GetClientRect(hwndSettings, &rcWnd);
		m_pFormPage->SetWindowPos(0, 0, 0, rc.Width(), rcWnd.Height(), SWP_NOZORDER|SWP_SHOWWINDOW);
	}

	// --- поддержка методик: хитрая настройка в shell.data, запрещающая показ форм
	if( ::GetValueInt( ::GetAppUnknown(), "\\FormManager", "DisableFormShow", 0 ) != 0 )
	{
		::ExecuteScript( _bstr_t("ProcessMessage 0") ); // [SBT 1163]
		PressOK(); // если не показываем форму - сразу нажмем в ней Ok, пусть подавится хомяк
		::ExecuteScript( _bstr_t("ProcessMessage 0") );	// [SBT 1163]

		// 27.09.2005 build 91 SBT 1440
		// если помимо формы была запущена интерактивная акция - ее тоже прервем
		// не помогло (т.к. акция запускается уже после смерти формы), но пусть будет.
		IApplicationPtr sptrApp = ::GetAppUnknown();
		IUnknownPtr ptrActionMan = 0;
		if(sptrApp!=0) sptrApp->GetActionManager( &ptrActionMan );
		IActionManagerPtr sptrActionMan = ptrActionMan;
		if(sptrActionMan!=0) sptrActionMan->TerminateInteractiveAction();
		::ExecuteScript( _bstr_t("ProcessMessage 0") );	// [SBT 1163]
	}
	// ---

	// --- поддержка методик: отметим, что показывалась форма (чтобы при записи знать, что 'Has Property Page')
	::SetValue( ::GetAppUnknown(), "\\FormManager", "PageExecuted", 1L );
	

	return 0;
}

void CFormManager::ExecutePageModal(LPCTSTR szFormName) 
{
	bool bmode_old = m_bModalMode;
	m_bModalMode = true;

	ExecutePage( szFormName );

	IApplicationPtr ptrApp( GetAppUnknown() );
	if( ptrApp == 0 ) 
		return; 

	if( !m_pFormPage )
		return;

//	MSG msg;
	while( m_pFormPage && m_pFormPage->GetSafeHwnd()/* && GetMessage( &msg, 0, 0, 0 ) */)
	{
		ptrApp->ProcessMessage();

/*		TranslateMessage( &msg );
		DispatchMessage( &msg );
*/	}

	m_bModalMode = bmode_old;

}

BOOL CFormManager::GetModalMode() 
{
	return m_bModalMode == true;
}

void CFormManager::SetModalMode(BOOL bNewValue) 
{

}

BOOL CFormManager::GetPageActive()
{
	return m_pFormPage != NULL;
}

void CFormManager::SetPageActive(BOOL bNewValue)
{
}

class CManageDlgPointer
{
	CDialog **m_ppDlg;
	CDialog *m_pPrev;
public:
	CManageDlgPointer(CDialog **ppDlg, CDialog *pSet)
	{
		m_ppDlg = ppDlg;
		m_pPrev = *m_ppDlg;
		*m_ppDlg = pSet;
	}
	~CManageDlgPointer()
	{
		*m_ppDlg = m_pPrev;
	}
};

long CFormManager::ExecuteModal(LPCTSTR szFormName) 
{
	IUnknown	*punk = CFormManager::_GetFormByName( szFormName );

	if( !punk )
		return 0;


	HWND	hwnd;
	IApplicationPtr	ptrA( GetAppUnknown() );
	ptrA->GetMainWindowHandle( &hwnd );

	CFormDialog	dialog( 0/*CWnd::FromHandle( hwnd )*/ );
	dialog.SetForm( punk );

	CManageDlgPointer ManageDlgPointer(&m_pFormDialog, &dialog);
	bool bmode_old = m_bModalMode;
	m_bModalMode = true;
	int nRet = dialog.DoModal();
	m_bModalMode = bmode_old;

	punk->Release();

	return nRet;
}

long CFormManager::ExecuteDockBar(LPCTSTR szFormName) 
{
	IUnknown	*punkMW = _GetOtherComponent( GetAppUnknown(), IID_IMainWindow );
	if(!punkMW)	return 0;

	IMainWindowPtr	sptrMain(punkMW);
	punkMW->Release();

	IUnknown	*punk = CFormManager::_GetFormByName( szFormName );
	if( !punk )
		return 0;

	if(sptrMain != 0 && strcmp(szFormName, "") != 0)
		sptrMain->KillDockBar(_bstr_t(szFormName));

/*	{//??
		IUnknownPtr	ptrU( "ChooseColor.ChooseColorDlgD" );
		sptrMain->CreateDockBar(ptrU, AFX_IDW_CONTROLBAR_FIRST + 33, 0 );
		ptrU.Detach();

		return 1;
	}*/

 	

	
	CFormDockDialog* pDockDialog = new CFormDockDialog();
	pDockDialog->SetForm( punk );
	punk->Release();

	pDockDialog->SetDlgTitle(szFormName);

	pDockDialog->LockHideEvent( true );
	HWND	hwnd = 0;
	if(sptrMain != 0)
		sptrMain->CreateDockBar(pDockDialog->GetControllingUnknown(), AFX_IDW_CONTROLBAR_FIRST + 33, &hwnd);
	pDockDialog->LockHideEvent( false );

	

//	pDockDialog->CWnd::SetParent(CWnd::FromHandle(hwnd));

//	pDockDialog->ShowWindow(SW_SHOW);

	return 0;
}

long CFormManager::ExecuteDockBar2(LPCTSTR szFormName, LONG lDockSite, BOOL bCaption)
{
	IUnknown	*punkMW = _GetOtherComponent( GetAppUnknown(), IID_IMainWindow );
	if(!punkMW)	return 0;
	IMainWindowPtr	sptrMain(punkMW);
	punkMW->Release();
	IUnknown	*punk = CFormManager::_GetFormByName( szFormName );
	if( !punk )
		return 0;
	if(sptrMain != 0 && strcmp(szFormName, "") != 0)
		sptrMain->KillDockBar(_bstr_t(szFormName));
	switch(lDockSite)
	{
	case 0:
		lDockSite = AFX_IDW_DOCKBAR_FLOAT;
		break;
	case 1:
		lDockSite = AFX_IDW_DOCKBAR_TOP;
		break;
	case 2:
		lDockSite = AFX_IDW_DOCKBAR_LEFT;
		break;
	case 3:
		lDockSite = AFX_IDW_DOCKBAR_RIGHT;
		break;
	case 4:
		lDockSite = AFX_IDW_DOCKBAR_BOTTOM;
		break;
	default:
		lDockSite = 0;
	}
	CFormDockDialog* pDockDialog = new CFormDockDialog(lDockSite);
	pDockDialog->SetForm( punk );
	punk->Release();
	pDockDialog->SetDlgTitle(szFormName);
	HWND	hwnd = 0;
	if(sptrMain != 0)
	{
		pDockDialog->m_bLockHideEvent = true;
		sptrMain->CreateDockBarEx(pDockDialog->GetControllingUnknown(), AFX_IDW_CONTROLBAR_FIRST + 33,
		&hwnd, bCaption?1:3);
		pDockDialog->m_bLockHideEvent = false;
	}
	return 0;
}

long CFormManager::CreateHideDockBar(LPCTSTR szFormName)
{
	IUnknown	*punkMW = _GetOtherComponent( GetAppUnknown(), IID_IMainWindow );
	if(!punkMW)	return 0;

	IMainWindowPtr	sptrMain(punkMW);
	punkMW->Release();

	IUnknown	*punk = CFormManager::_GetFormByName( szFormName );
	if( !punk )
		return 0;

	if(sptrMain != 0 && strcmp(szFormName, "") != 0)
		sptrMain->KillDockBar(_bstr_t(szFormName));

	
	CFormDockDialog* pDockDialog = new CFormDockDialog();
	pDockDialog->SetForm( punk );
	punk->Release();

	pDockDialog->SetDlgTitle(szFormName);

	HWND	hwnd = 0;
	if(sptrMain != 0)
		sptrMain->CreateDockBarEx(pDockDialog->GetControllingUnknown(), AFX_IDW_CONTROLBAR_FIRST + 33, &hwnd, 0 );
	return 1;
}

void CFormManager::FinalizeEx(LPCTSTR nPageID, bool bOk) 
{
	//kill dock dialog
	IUnknown	*punkMW = _GetOtherComponent( GetAppUnknown(), IID_IMainWindow );
	IMainWindowPtr	sptrMain(punkMW);
	if(punkMW)
		punkMW->Release();
	if(sptrMain != 0)
		sptrMain->KillDockBar(_bstr_t(nPageID));
	
	//kill page
	if(m_pFormPage)
	{
		if(strcmp(::GetObjectName(m_pFormPage->GetControllingUnknown()), nPageID) == 0)
		{
			if( !CanDestroyForm(bOk) )
			{
				m_pFormPage->m_site.m_bClosingWindow = false;
				return;
			}
			m_pFormPage->m_site.m_bClosingWindow = true;
			m_pFormPage->DestroyWindow();
			delete m_pFormPage;
			m_pFormPage = 0;
			if(sptrMain != 0)
			{
				sptrMain->LeaveSettingsMode();
			}

			_KillButtons();
		}
	}

}

void CFormManager::Finalize(LPCTSTR nPageID) 
{
	FinalizeEx(nPageID, false);
}

IUnknown *CFormManager::_GetFormByName( const char *szName )
{
	IApplicationPtr	sptrA( GetAppUnknown() );

	long	lTemplPos;

	IUnknown	*punkObj = ::GetObjectByName( ::GetAppUnknown(), szName, 0 );

	if( punkObj )
		return punkObj;

	sptrA->GetFirstDocTemplPosition( &lTemplPos );

	while( lTemplPos )
	{
		long	lPosDoc = 0;
		sptrA->GetFirstDocPosition( lTemplPos, &lPosDoc );

		while( lPosDoc ) 
		{
			IUnknown	*punkDoc = 0;
			sptrA->GetNextDoc( lTemplPos, &lPosDoc, &punkDoc );

			IUnknown	*punkObj = ::GetObjectByName( punkDoc, szName, 0 );

			if( !CheckInterface(punkObj, IID_IActiveXForm ) )
				continue;

			if( punkDoc )
				punkDoc->Release();

			if( punkObj )
				return punkObj;
		}
		sptrA->GetNextDocTempl( &lTemplPos, 0, 0 );
	}

	//try to load from file
	CString strPath = g_strFormPath;
		
	strPath += szName;
	strPath += ".form";
/*
#ifdef _DEBUG
	IFileFilter2Ptr	sptrFF(_T("FileFilters.AXFileFilterD"));
#else
	IFileFilter2Ptr	sptrFF(_T("FileFilters.AXFileFilter"));
#endif
*/	IFileFilter2Ptr	sptrFF(_T("FileFilters.AXFileFilter"));
	IFileDataObjectPtr sptrFile(sptrFF);
	if(sptrFF != 0 && sptrFile != 0)
	{
		if(sptrFile->LoadFile(_bstr_t(strPath)) == S_OK)
		{
			int iCount = 0;
			sptrFF->GetCreatedObjectCount(&iCount);
			ASSERT(iCount == 1);
			sptrFF->GetCreatedObject(0,  &punkObj);
			if( punkObj )
				return punkObj;
		}
	}

	return 0;
}
long CFormManager::ExecuteOptions() 
{
	// TODO: Add your dispatch handler code here
	IApplicationPtr	ptrA( GetAppUnknown() );
	HWND	hwnd = 0;
	ptrA->GetMainWindowHandle( &hwnd );

	m_dlgOptions->SetParent( hwnd );
	return m_dlgOptions->DoModal();
}

void CFormManager::ResetOptions() 
{
	// TODO: Add your dispatch handler code here
	m_dlgOptions->ResetControls();
}

void CFormManager::LoadCategoryImage(LPCTSTR szFileName) 
{
	// TODO: Add your dispatch handler code here

	CString	strFileName = ::MakeFillFileName( g_strFormPath, szFileName );
	m_dlgOptions->LoadImageList( strFileName );
}

void CFormManager::AddOptionsCategory(short nImageIdx, LPCTSTR szTitle) 
{
	// TODO: Add your dispatch handler code here
	m_dlgOptions->AddItemToBar(nImageIdx, szTitle);
}

void CFormManager::SetActiveCategory(LPCTSTR szName) 
{
	// TODO: Add your dispatch handler code here
	m_dlgOptions->SetActiveCategory(szName);
}

BSTR CFormManager::GetActiveCategory() 
{
	CString strResult;
	// TODO: Add your dispatch handler code here
	m_dlgOptions->GetActiveCategory(strResult);
	return strResult.AllocSysString();
}

void CFormManager::AddOptionsPage(LPCTSTR szCategory, LPCTSTR szFormName) 
{
	// TODO: Add your dispatch handler code here
	m_dlgOptions->AddPageToCategory(szCategory, szFormName);
}

BOOL CFormManager::SetActivePage(LPCTSTR szFormName) 
{
	// TODO: Add your dispatch handler code here
	return m_dlgOptions->SetActivePage(szFormName);;
}

BSTR CFormManager::GetActivePage() 
{
	CString strResult;
	// TODO: Add your dispatch handler code here
	m_dlgOptions->GetActivePage(strResult);
	return strResult.AllocSysString();
}

BOOL CFormManager::StoreOptionsConfig(LPCTSTR szFileName) 
{
	// TODO: Add your dispatch handler code here
	return m_dlgOptions->StoreOptionsConfig(szFileName);
}

BOOL CFormManager::LoadOptionsConfig(LPCTSTR szFileName) 
{
	// TODO: Add your dispatch handler code here
	return m_dlgOptions->LoadOptionsConfig(szFileName);
}

void CFormManager::OnPageOk()
{
	if(m_pFormPage)
	{
		if( m_pFormPage->m_site.m_bClosingWindow )return;
		//чтобы игнорировала нажатия
		m_pFormPage->m_site.m_bClosingWindow = true;

		m_pFormPage->m_site.FireOnOK();
		if( !m_pFormPage->m_site.AllowClose() )
		{
			m_pFormPage->m_site.m_bClosingWindow = false;
			return;
		}

		FinalizeEx(::GetObjectName(m_pFormPage->GetControllingUnknown()), true);
	}

	_ProcessStackPages();
}

void CFormManager::OnPageCancel()
{
	if(m_pFormPage)
	{
		if(!m_pFormPage->m_site.m_bFormInited)
		{
			m_pFormPage->m_site.m_bCancelSheduled = true;
			return; // пока не проинитили - нах кнопки
		}
		m_pFormPage->m_site.m_bCancelSheduled = false;
		if( m_pFormPage->m_site.m_bClosingWindow )return;

		m_pFormPage->m_site.m_bClosingWindow = true;
		
		m_pFormPage->m_site.FireOnCancel();
		if( !m_pFormPage->m_site.AllowClose() )
		{
			m_pFormPage->m_site.m_bClosingWindow = false;
			return;
		}

		Finalize(::GetObjectName(m_pFormPage->GetControllingUnknown()));
	}

	_ProcessStackPages();
}

void CFormManager::OnPageApply()
{
	if(m_pFormPage)
	{
		if( ! m_pFormPage->m_site.m_bFormInited ) return; // пока не проинитили - нах кнопки
		m_pFormPage->m_site.FireOnOK();
	}
}


void CFormManager::_ProcessStackPages()
{
	int nStackSize = m_strStackPages.GetSize();
	if(nStackSize > 0)
	{
		ExecutePage(m_strStackPages[nStackSize-1]);
		m_strStackPages.RemoveAt(nStackSize-1);
	}
}


BOOL CFormManager::StackCurrentPage() 
{
	if(m_pFormPage)
	{
		CString strName = ::GetObjectName(m_pFormPage->GetControllingUnknown());
		m_strStackPages.Add(strName);
	}

	return TRUE;
}

void CFormManager::ExecutePage2(LPCTSTR bstrName, BOOL EnableOK, BOOL EnableCancel, BOOL EnableApply) 
{
	// TODO: Add your dispatch handler code here
	ExecutePage(bstrName);
	m_pBtnOk->EnableWindow(EnableOK);
	m_pBtnCancel->EnableWindow(EnableCancel);
	if( m_pBtnApply )
		m_pBtnApply->EnableWindow(EnableApply);
}


////////////////////////////////////////////////////////////////////////////
//Wizard support
void CFormManager::CreateNewWizard(LPCTSTR szWizardName) 
{
	// TODO: Add your dispatch handler code here
	if(m_pWizard)
		delete m_pWizard;
	m_pWizard = new CWizard( szWizardName, this );
}

void CFormManager::ExecuteWizard(LPCTSTR szWizardName) 
{
	// TODO: Add your dispatch handler code here
	bool bmode_old = m_bModalMode;
	m_bModalMode = true;
	if(m_pWizard)
		m_pWizard->ExecuteWizard();
	m_bModalMode = bmode_old;
}	

void CFormManager::ExecuteNestedWizard() 
{
	// TODO: Add your dispatch handler code here
	if(m_pWizard)
		m_pWizard->ExecuteNestedWizard();
}


/*BOOL CFormManager::LoadWizard(LPCTSTR szWizardName, LPCTSTR szFileName, LPCTSTR szSectionName) 
{
	// TODO: Add your dispatch handler code here
	if(m_pWizard)
		return m_pWizard->LoadWizard(szFileName, szSectionName);
	return FALSE;
}

BOOL CFormManager::StoreWizard(LPCTSTR szWizardName, LPCTSTR szFileName, LPCTSTR szSectionName) 
{
	// TODO: Add your dispatch handler code here
	if(m_pWizard)
		return m_pWizard->StoreWizard(szFileName, szSectionName); 
	return FALSE;
}*/

void CFormManager::AddWizardPage(LPCTSTR szFormName, BOOL bModalMode) 
{
	// TODO: Add your dispatch handler code here
	if(m_pWizard)
		m_pWizard->AddWizardPage(szFormName, bModalMode == TRUE);
}


////////////////////////////////////////////////////////////////////////////
//CWizard
CWizard::CWizard( CString strName, CFormManager *pFormMan ) : m_sizeLastSiteSize(0, 0)
{
	m_pFormMan = pFormMan;
	m_strName = strName;
	m_pBtnNext = 0;
	m_pBtnPrev = 0;
	m_pBtnCancel = 0;
	m_pBtnHelp = 0;
	m_hwndSettings = 0;
	m_hwndModal = 0;
	m_bNowExecute = false;
	m_pWizardPages = 0;
	m_pCurrentPage = 0;

	m_pDeleteList = 0;

	m_pWizardDlg = 0;

	m_yOffset = 0;
	
	/*m_nCurPage = 0;
	m_nPagesCount = 0;
	

	m_nNext = -1;
	m_nPrev = -1;*/
}

CWizard::~CWizard()
{
	_KillWizard();
}


void _DeleteNested(WizardNode* pNested)
{
	if(!pNested)
		return;

	long nPrevLevel = pNested->pPrevPage->nLevel;

	long nLevel = pNested->nLevel;
	if(nPrevLevel != nLevel)
	{
		while(pNested && pNested->nLevel == nLevel)
		{
			_DeleteNested(pNested->pNestedWizard);

			if(pNested->pPage)
			{
				if(pNested->pPage->m_hWnd)
					pNested->pPage->DestroyWindow();
				delete pNested->pPage;
				pNested->pPage = 0;
			}
			WizardNode* pT = pNested;
			pNested = pNested->pNextPage;
			delete pT;
		}
	}
}

void CWizard::_KillWizard()
{

	m_arrWizardSizes.RemoveAll();

	if(m_pWizardDlg)
	{
		delete m_pWizardDlg;
		m_pWizardDlg = 0;
		m_hwndModal = 0;
	}

	m_pCurrentPage = m_pWizardPages;
	while(m_pCurrentPage)
	{
		WizardNode* pTmpNode = m_pCurrentPage->pNextPage;
		if(m_pCurrentPage->pPage)
		{
			if(m_pCurrentPage->pPage->m_hWnd)
				m_pCurrentPage->pPage->DestroyWindow();
			delete m_pCurrentPage->pPage;
		}

		_DeleteNested(m_pCurrentPage->pNestedWizard);

		/*if(m_pCurrentPage->pNestedWizard)
		{
			WizardNode* pNested = m_pCurrentPage->pNestedWizard;
			long nLevel = pNested->nLevel;
			if(m_pCurrentPage->nLevel != nLevel)
			{
				while(pNested && pNested->nLevel == nLevel)
				{
					if(pNested->pPage)
					{
						if(pNested->pPage->m_hWnd)
							pNested->pPage->DestroyWindow();
						delete pNested->pPage;
						pNested->pPage = 0;
					}
					WizardNode* pT = pNested;
					pNested = pNested->pNextPage;
					delete pT;
				}
			}
		}*/

		delete m_pCurrentPage;
		m_pCurrentPage = pTmpNode;
	}

	m_pWizardPages = 0;
	m_pCurrentPage = 0;
	
	if(m_pBtnNext)
	{
		delete m_pBtnNext;
		m_pBtnNext = 0;
	}
	if(m_pBtnCancel)
	{
		delete m_pBtnCancel;
		m_pBtnCancel = 0;
	}
	if(m_pBtnPrev)
	{
		delete m_pBtnPrev;
		m_pBtnPrev = 0;
	}
	if(m_pBtnHelp)
	{
		delete m_pBtnHelp;
		m_pBtnHelp = 0;
	}
	m_mapWizardNodes.RemoveAll();
}

CSize CWizard::_GetNextPagesSize(bool bIncludeItself)
{
	CSize size(0, 0);
	if(m_pCurrentPage == m_pWizardPages || bIncludeItself)
		size = m_pCurrentPage->sizeForm;
	WizardNode* pNode = m_pCurrentPage->pNextPage;
	bool bModal = false;
	long nLevel = 0;
	if(pNode)
	{
		bModal = pNode->bModalMode;
		nLevel = pNode->nLevel;
	}

	while(pNode)
	{
		if(pNode->bModalMode == bModal)
		{
			if(pNode->nLevel != nLevel)
				break;
			size.cx = max(size.cx, pNode->sizeForm.cx);
			size.cy = max(size.cy, pNode->sizeForm.cy);
		}
		pNode = pNode->pNextPage;
	}

	return size;
}

void CWizard::AddWizardPage(CString strFormName, bool bModalMode)
{
	WizardNode* pNode = 0;

	if(m_mapWizardNodes.Lookup(strFormName, pNode) != TRUE)
	{
		IUnknown* punk = CFormManager::_GetFormByName(strFormName);
		if(punk)
		{
			pNode = new WizardNode(strFormName, bModalMode, punk);
			punk->Release();
			m_mapWizardNodes.SetAt(strFormName, pNode);
		}
	}

	if(!pNode)
		return;

	if(m_bNowExecute)
	{
		//nested wizard
		if(pNode->nLevel < 0)
			pNode->nLevel = m_pCurrentPage->nLevel+1;
		if(!m_pCurrentPage->pNestedWizard)
		{
			if(!pNode->pPrevPage && pNode->nLevel != m_pCurrentPage->nLevel)
				pNode->pPrevPage = m_pCurrentPage;
			m_pCurrentPage->pNestedWizard = pNode;
		}
		else
		{
			WizardNode* pInsertTo = m_pCurrentPage->pNestedWizard;
			while(pInsertTo->pNextPage)
				pInsertTo = pInsertTo->pNextPage;
			if(!pInsertTo->pNextPage)
				pInsertTo->pNextPage = pNode;
			if(pNode->nLevel == pInsertTo->nLevel && !pNode->pPrevPage)
				pNode->pPrevPage = pInsertTo;
		}
	}
	else
	{
		pNode->nLevel = 0;
		if(!m_pWizardPages)
			m_pWizardPages = pNode;
		if(!pNode->pPrevPage)
			pNode->pPrevPage = m_pCurrentPage;
		if(m_pCurrentPage && !m_pCurrentPage->pNextPage)
			m_pCurrentPage->pNextPage = pNode;
		m_pCurrentPage = pNode;
	}
	/*IUnknown	*punk = CFormManager::_GetFormByName( strFormName );
	if( !punk )
		return;
	IActiveXFormPtr sptrForm(punk);
	punk->Release();
	
	if(sptrForm == 0)
		return;
	CSize size(0,0);
	sptrForm->GetSize( &size );
	m_sizeMaxPage.cx = max(m_sizeMaxPage.cx, size.cx);
	m_sizeMaxPage.cy = max(m_sizeMaxPage.cy, size.cy);
	
	CFormPage* pPage = new CFormPage(strFormName);
	pPage->SetForm( sptrForm );

	m_arrPages.Add(pPage);
	m_nPagesCount = m_arrPages.GetSize();*/
}

void CWizard::ExecuteWizard()
{
	if(m_bNowExecute)return;
	if(!m_pWizardPages)return;

	m_bNowExecute = true;

	m_pCurrentPage = m_pWizardPages;

	m_sizeLastSiteSize = _GetNextPagesSize();

	m_arrWizardSizes.InsertAt(m_pCurrentPage->nLevel, m_sizeLastSiteSize);
	if(m_pCurrentPage->bModalMode)
	{
		/*{
		CDialogTemplate	dt;
		dt.Load( MAKEINTRESOURCE(CWizardDlg::IDD));
		HGLOBAL	hTempl = dt.Detach();
		((DLGTEMPLATE*)hTempl)->cx = m_sizeLastSiteSize.cx + WizardBtnWidth + 2*WizardBtnInterval;
		int nHeight = m_sizeLastSiteSize.cy+2*WizardBtnInterval;
		nHeight = max(nHeight, 4*WizardBtnInterval+3*WizardBtnHeight);
		((DLGTEMPLATE*)hTempl)->cy = nHeight;
		}*/

	
		m_sizeLastSiteSize.cy = m_pCurrentPage->sizeForm.cy;

		_EnterModalMode(CSize(0,0));

		_LocateButtons(true);

		//m_pBtnPrev->EnableWindow(FALSE);
	}
	else
	{
		
		_EnterSettingsMode(m_sizeLastSiteSize, IDD_CONTAINER_DOCKDIALOG, &m_hwndSettings);		

		_LocateButtons(false);

	}

	_SetCurModalessPage();

	IApplicationPtr ptrApp( GetAppUnknown() );
	if( ptrApp == 0 ) 
		return; 

	MSG msg;
	while( m_bNowExecute && GetMessage( &msg, 0, 0, 0 ) )
	{
		ptrApp->ProcessMessage();

//		TranslateMessage( &msg );
//		DispatchMessage( &msg );
	}
	
}

void CWizard::_EnterModalMode(CSize sizeOld)
{
	HWND	hwnd;
	IApplicationPtr	ptrA( GetAppUnknown() );
	ptrA->GetMainWindowHandle( &hwnd );

	int nHeight = m_sizeLastSiteSize.cy+2*WizardBtnInterval;
	nHeight = max(nHeight, 4*WizardBtnInterval+3*WizardBtnHeight);
	CSize size(m_sizeLastSiteSize.cx + WizardBtnWidth + 5*WizardBtnInterval, nHeight);

	if(!m_pWizardDlg)
		m_pWizardDlg = new CWizardDlg(NULL, size, m_strName);

	if(!m_hwndModal)
	{
		m_pWizardDlg->Create(CWizardDlg::IDD, CWnd::FromHandle(hwnd));
		m_hwndModal = m_pWizardDlg->GetSafeHwnd();
	}
	else
	{
		CRect rcWindow = NORECT;
		::GetWindowRect(m_hwndModal, &rcWindow);

		if(!m_yOffset)
		{
			CRect	rectClient = NORECT;
			CWnd* pwnd = CWnd::FromHandle(m_hwndModal);
			if(pwnd)
				pwnd->GetDlgItem(IDC_CONTROLS)->GetWindowRect(&rectClient);
			m_yOffset = rcWindow.Height() - rectClient.Height();
		}
		//if(sizeOld.cy < m_sizeLastSiteSize.cy)
		{
			//m_sizeLastSiteSize.cx = max(m_sizeLastSiteSize.cx, sizeOld.cx);
			//m_sizeLastSiteSize.cy = max(m_sizeLastSiteSize.cy, sizeOld.cy);
			int nHeight = m_sizeLastSiteSize.cy+2*WizardBtnInterval;
			nHeight = max(nHeight, 4*WizardBtnInterval+3*WizardBtnHeight);
			CSize size(m_sizeLastSiteSize.cx + WizardBtnWidth + 5*WizardBtnInterval, nHeight);
			
			::MoveWindow(m_hwndModal, rcWindow.left, rcWindow.top, rcWindow.Width(), size.cy + m_yOffset, TRUE);
		}
		::ShowWindow(m_hwndModal, SW_SHOW);
	}
	
	::EnableWindow(hwnd, FALSE); //do modal
}

void CWizard::_ExitModalMode()
{
	if(!m_hwndModal)
		return;

	HWND	hwnd;
	IApplicationPtr	ptrA( GetAppUnknown() );
	ptrA->GetMainWindowHandle( &hwnd );

	::EnableWindow(hwnd, TRUE);
	if(m_hwndModal)
	{
		::ShowWindow(m_hwndModal, SW_HIDE);
		//m_hwndModal = 0;
	}
	/*if(m_pWizardDlg)
		delete m_pWizardDlg;
	m_pWizardDlg = 0;*/
}

void CWizard::_ExitSettingsMode()
{
	if(!m_hwndSettings)
		return;
	IUnknown	*punkMW = _GetOtherComponent( GetAppUnknown(), IID_IMainWindow );
	IMainWindowPtr	sptrMain(punkMW);
	if(punkMW)
		punkMW->Release();
	if(sptrMain != 0)
		sptrMain->LeaveSettingsMode();

	m_hwndSettings = 0;
}


void CWizard::_ManageMode(bool bWasInModalMode, bool bNested)
{
	bool bCurModalMode = m_pCurrentPage->bModalMode;
	CSize size = _GetNextPagesSize(true);

	long nLevel = m_pCurrentPage->nLevel;

	if(nLevel > m_arrWizardSizes.GetUpperBound())
		m_arrWizardSizes.InsertAt(nLevel, size);
	else
		size = m_arrWizardSizes[nLevel];
	
	HWND	hwnd;
	IApplicationPtr	ptrA( GetAppUnknown() );
	ptrA->GetMainWindowHandle( &hwnd );

	if(bWasInModalMode)
	{
		if(bCurModalMode)
		{
			CSize sizeOld = m_sizeLastSiteSize;
			if(bNested)
			{
				m_sizeLastSiteSize = size;
			}
			else
			{
				
				m_sizeLastSiteSize.cx = size.cx;
				m_sizeLastSiteSize.cy = m_pCurrentPage->sizeForm.cy;
			}

			_EnterModalMode(sizeOld);

			/*//_ExitModalMode();
			CSize sizeOld = m_sizeLastSiteSize;
			m_sizeLastSiteSize.cx = max(size.cx, m_sizeLastSiteSize.cx);
			//m_sizeLastSiteSize.cy = max(size.cy, m_sizeLastSiteSize.cy);
			m_sizeLastSiteSize.cy = m_pCurrentPage->sizeForm.cy;
			//m_sizeLastSiteSize = m_pCurrentPage->sizeForm;
			_EnterModalMode(sizeOld);
			//_LocateButtons(true);*/
		}
		else
		{
			_ExitModalMode();
			_EnterSettingsMode(size, IDD_CONTAINER_DOCKDIALOG, &m_hwndSettings);
			m_sizeLastSiteSize = size;
			_LocateButtons(false);
		}
	}
	else
	{
		if(bCurModalMode)
		{
			_ExitSettingsMode();
			CSize sizeOld = m_sizeLastSiteSize;
			m_sizeLastSiteSize = size;
			m_sizeLastSiteSize.cy = m_pCurrentPage->sizeForm.cy;
			_EnterModalMode(sizeOld);
			_LocateButtons(true);
		}
		else
		{
			if(bNested)
			{
				if(size.cx > m_sizeLastSiteSize.cx && size.cy <= m_sizeLastSiteSize.cy)
				{
					//move buttons
					m_sizeLastSiteSize.cx = size.cx;
					_LocateButtons(false);
				}
				else if(size.cy > m_sizeLastSiteSize.cy)
				{
					//reset settings mode
					m_sizeLastSiteSize = size;
					_ExitSettingsMode();
					_EnterSettingsMode(size, IDD_CONTAINER_DOCKDIALOG, &m_hwndSettings);
					_LocateButtons(false);
				}
			}
		}
	}
}

void CWizard::_LocateButtons(bool bModal)
{
	bool bWasInModal = false;
	if(m_pBtnNext)
	{
		bWasInModal = m_pBtnNext->GetParent()->GetSafeHwnd() == m_hwndModal;
	}

	BOOL bPrev = FALSE;
	BOOL bNext = TRUE;
	if(m_pBtnPrev)
		bPrev = m_pBtnPrev->IsWindowEnabled();
	if(m_pBtnNext)
		bPrev = m_pBtnNext->IsWindowEnabled();

	if(bWasInModal != bModal)
	{
		if(m_pBtnNext)
		{
			delete m_pBtnNext;
			m_pBtnNext = 0;
		}
		if(m_pBtnPrev)
		{
			delete m_pBtnPrev;
			m_pBtnPrev = 0;
		}
		if(m_pBtnCancel)
		{
			delete m_pBtnCancel;
			m_pBtnCancel = 0;
		}
		if(m_pBtnHelp)
		{
			delete m_pBtnHelp;
			m_pBtnHelp = 0;
		}
	}

	HWND hwnd = bModal ? m_hwndModal : m_hwndSettings;
	CString strBtnName;
	CRect	rcBtn = NORECT;
	
	if(!m_pBtnNext)
	{
		if (m_pCurrentPage->pNextPage) 
			strBtnName.LoadString(IDS_BTN_NEXT);
		else
			strBtnName.LoadString(IDS_BTN_FINISH);
		m_pBtnNext = new CWizButton(this, hwnd, WIZB_NEXT, strBtnName);
	}
	CRect rcClient = NORECT;
	::GetClientRect(hwnd, &rcClient);
	rcBtn = CRect(m_sizeLastSiteSize.cx+3*WizardBtnInterval, WizardBtnInterval, 0, 0);
	rcBtn.right = rcBtn.left + WizardBtnWidth;
	/*rcBtn = CRect(0, WizardBtnInterval, rcClient.right - WizardBtnInterval, 0);
	rcBtn.left = rcBtn.right - WizardBtnWidth;*/
	rcBtn.bottom = rcBtn.top + WizardBtnHeight;
	m_pBtnNext->MoveWindow(&rcBtn);
	m_pBtnNext->EnableWindow(bNext);

	if(!m_pBtnPrev)
	{
		strBtnName.LoadString(IDS_BTN_PREV);
		m_pBtnPrev = new CWizButton(this, hwnd, WIZB_BACK, strBtnName);
	}
	rcBtn = CRect(m_sizeLastSiteSize.cx+3*WizardBtnInterval, 2*WizardBtnInterval+WizardBtnHeight, 0, 0);
	rcBtn.right = rcBtn.left + WizardBtnWidth;
	/*rcBtn = CRect(0, 2*WizardBtnInterval+WizardBtnHeight, rcClient.right - WizardBtnInterval, 0);
	rcBtn.left = rcBtn.right - WizardBtnWidth;*/
	rcBtn.bottom = rcBtn.top + WizardBtnHeight;
	m_pBtnPrev->MoveWindow(&rcBtn);
	m_pBtnPrev->EnableWindow(bPrev);
	
	if(!m_pBtnCancel)
	{
		strBtnName.LoadString(IDS_BTN_CANCEL);
		m_pBtnCancel = new CWizButton(this, hwnd, WIZB_FINISH, strBtnName);
	}
	rcBtn = CRect(m_sizeLastSiteSize.cx+3*WizardBtnInterval, 3*WizardBtnInterval+2*WizardBtnHeight, 0, 0);
	rcBtn.right = rcBtn.left + WizardBtnWidth;
	/*rcBtn = CRect(0, 3*WizardBtnInterval+2*WizardBtnHeight, rcClient.right - WizardBtnInterval, 0);
	rcBtn.left = rcBtn.right - WizardBtnWidth;*/
	rcBtn.bottom = rcBtn.top + WizardBtnHeight;
	m_pBtnCancel->MoveWindow(&rcBtn);

	if(!m_pBtnHelp)
	{
		strBtnName.LoadString(IDS_BTN_HELP);
		m_pBtnHelp = new CWizButton(this, hwnd, IDHELP, strBtnName);
	}
	rcBtn = CRect(m_sizeLastSiteSize.cx+3*WizardBtnInterval, 4*WizardBtnInterval+3*WizardBtnHeight, 0, 0);
	rcBtn.right = rcBtn.left + WizardBtnWidth;
	/*rcBtn = CRect(0, 4*WizardBtnInterval+3*WizardBtnHeight, rcClient.right - WizardBtnInterval, 0);
	rcBtn.left = rcBtn.right - WizardBtnWidth;*/
	rcBtn.bottom = rcBtn.top + WizardBtnHeight;
	m_pBtnHelp->MoveWindow(&rcBtn);

	
}

void CWizard::ExecuteNestedWizard()
{
	if(m_pCurrentPage->pNestedWizard)
	{
	
		bool bWasInModalMode = m_pCurrentPage->bModalMode;

		m_pCurrentPage->pPage->m_site.FireOnNext();
		m_pCurrentPage->pPage->ShowWindow(SW_HIDE);
		WizardNode* pCurrentPage = m_pCurrentPage->pNestedWizard;

		long nPrevLevel = m_pCurrentPage->nLevel;
		long nNewLevel = m_pCurrentPage->pNestedWizard->nLevel;


		if(nNewLevel < nPrevLevel)
		{
			WizardNode* pN = m_pCurrentPage;
			while(pN && pN->nLevel == nPrevLevel)
			{
				m_mapWizardNodes.RemoveKey(pN->strFormName);
				WizardNode* pDel = pN;
				pN = pN->pPrevPage;
				_AddToDeleteList(pDel);
			}
			pN->pNestedWizard = 0;
		}
		
		if(nPrevLevel == nNewLevel)
			m_pCurrentPage->pNestedWizard = 0;

		m_pCurrentPage = pCurrentPage;

		
		_ManageMode(bWasInModalMode, true);
		

		m_pBtnPrev->EnableWindow(m_pCurrentPage->pPrevPage != 0);
		if(!m_pCurrentPage->pNextPage)
		{
			CString strBtnName;
			strBtnName.LoadString(IDS_BTN_FINISH);
			m_pBtnNext->SetWindowText(strBtnName);
		}
		else
		{
			CString strBtnName;
			strBtnName.LoadString(IDS_BTN_NEXT);
			m_pBtnNext->SetWindowText(strBtnName);
		}
		_SetCurModalessPage();
	}
}

void CWizard::_SetCurModalessPage()
{
	HWND hwnd = m_pCurrentPage->bModalMode ? m_hwndModal : m_hwndSettings;
	if(hwnd && ::IsWindow(hwnd))
	{
		if(!m_pCurrentPage->pPage)
		{
			m_pCurrentPage->pPage = new CFormPage(m_pCurrentPage->strFormName);
			m_pCurrentPage->pPage->SetForm(m_pCurrentPage->sptrForm);
		}
		CSize size = m_pCurrentPage->sizeForm;
		if(size.cy < 2*WizardBtnInterval+3*WizardBtnHeight)
			size.cy = 2*WizardBtnInterval + 3*WizardBtnHeight;
		m_pCurrentPage->pPage->m_site.SetSize(size);

		if(m_pCurrentPage->pPage->m_hWnd)
			m_pCurrentPage->pPage->ShowWindow(SW_SHOW);
		else
			m_pCurrentPage->pPage->Create(IDD_CONTAINER_DOCKDIALOG, CWnd::FromHandle(hwnd));

		CRect	rc = NORECT;
		CRect	rcWnd = NORECT;
		m_pCurrentPage->pPage->GetClientRect(&rc);
		::GetClientRect(hwnd, &rcWnd); 
		m_pCurrentPage->pPage->SetWindowPos(0, WizardBtnInterval, WizardBtnInterval, rc.Width(), rcWnd.Height(), SWP_NOZORDER|SWP_SHOWWINDOW);

		m_pCurrentPage->pPage->m_site.FireOnShow();
	}
}

void CWizard::_EmptyDeleteList()
{
	while(m_pDeleteList)
	{
		WizardNode* pDel = m_pDeleteList;
		m_pDeleteList = m_pDeleteList->pNextPage;
		if(pDel->pPage)
		{
			pDel->pPage->m_site.FireOnOK();
			if(pDel->pPage->m_hWnd)
				pDel->pPage->DestroyWindow();
			delete pDel->pPage;
		}
		delete pDel;
	}
}

void CWizard::_AddToDeleteList(WizardNode* pNode)
{
	pNode->pNextPage = 0;
	pNode->pPrevPage = 0;
	if(!m_pDeleteList)
		m_pDeleteList = pNode;
	else
	{
		WizardNode* pNext = m_pDeleteList->pNextPage;
		m_pDeleteList->pNextPage = pNode;
		pNode->pPrevPage = m_pDeleteList;
		pNode->pNextPage = pNext;
		if(pNext)
			pNext->pPrevPage = pNode;
	}
		
}


void CWizard::Next()
{
	if(!m_pCurrentPage)
		return;
	bool bWasInModalMode = m_pCurrentPage->bModalMode;
	_EmptyDeleteList();

	m_pCurrentPage->pPage->m_site.FireOnNext();

	if(!IsNextPresent())
	{
		//finish
		ExitFromWizard(WIZB_NEXT);
		return;
	}

	if(!m_pCurrentPage->pNextPage->pNextPage)
	{
		CString strBtnName;
		strBtnName.LoadString(IDS_BTN_FINISH);
		m_pBtnNext->SetWindowText(strBtnName);
	}

	m_pCurrentPage->pPage->ShowWindow(SW_HIDE);

	long nPrevLevel = m_pCurrentPage->nLevel;
	long nNewLevel = m_pCurrentPage->pNextPage->nLevel;

	if(nNewLevel < nPrevLevel)
	{
		WizardNode* pN = m_pCurrentPage;
		m_pCurrentPage = m_pCurrentPage->pNextPage;
		while(pN && pN->nLevel == nPrevLevel)
		{
			m_mapWizardNodes.RemoveKey(pN->strFormName);
			if(pN->pPage)
			{
				pN->pPage->m_site.FireOnOK();
				if(pN->pPage->m_hWnd)
					pN->pPage->DestroyWindow();
				delete pN->pPage;
			}
			WizardNode* pDel = pN;
			pN = pN->pPrevPage;
			delete pDel;
		}
		pN->pNestedWizard = 0;
	}
	else
	{
		m_pCurrentPage = m_pCurrentPage->pNextPage;
	}

	_ManageMode(bWasInModalMode);

	m_pBtnPrev->EnableWindow(m_pCurrentPage->pPrevPage != 0);
	
	_SetCurModalessPage();
}

void CWizard::Prev()
{
	if(!m_pCurrentPage || !m_pCurrentPage->pPrevPage)
		return;
	bool bWasInModalMode = m_pCurrentPage->bModalMode;

	_EmptyDeleteList();

	m_pCurrentPage->pPage->m_site.FireOnPrev();

	m_pCurrentPage->pPage->ShowWindow(SW_HIDE);
	
	CString strBtnName;
	strBtnName.LoadString(IDS_BTN_NEXT);
	m_pBtnNext->SetWindowText(strBtnName);
	
	m_pBtnPrev->EnableWindow(IsPrevPresent());

	long nPrevLevel = m_pCurrentPage->nLevel;
	long nNewLevel = m_pCurrentPage->pPrevPage->nLevel;
	if(nNewLevel < nPrevLevel)
	{
		WizardNode* pN = m_pCurrentPage;
		m_pCurrentPage = m_pCurrentPage->pPrevPage;
		m_pCurrentPage->pNestedWizard = 0;
		while(pN && pN->nLevel == nPrevLevel)
		{
			m_mapWizardNodes.RemoveKey(pN->strFormName);
			if(pN->pPage)
			{
				pN->pPage->m_site.FireOnCancel();
				if(pN->pPage->m_hWnd)
					pN->pPage->DestroyWindow();
				delete pN->pPage;
			}
			WizardNode* pDel = pN;
			pN = pN->pNextPage;
			delete pDel;
		}
	}
	else
	{
		m_pCurrentPage = m_pCurrentPage->pPrevPage;
	}

	_ManageMode(bWasInModalMode);

	_SetCurModalessPage();
}

void CWizard::Cancel()
{
	ExitFromWizard(666);
}

void CWizard::ExitFromWizard(UINT nID)
{
	HWND	hwnd;
	IApplicationPtr	ptrA( GetAppUnknown() );
	ptrA->GetMainWindowHandle( &hwnd );
	::EnableWindow(hwnd, TRUE);


	m_pCurrentPage = m_pWizardPages;
	while(m_pCurrentPage)
	{
		if(m_pCurrentPage->pNestedWizard)
		{
			WizardNode* pNested = m_pCurrentPage->pNestedWizard;
			long nLevel = pNested->nLevel;
			if(m_pCurrentPage->nLevel != nLevel)
			{
				while(pNested && pNested->nLevel == nLevel)
				{
					if(pNested->pPage)
					{
						if(nID == 666)
							pNested->pPage->m_site.FireOnCancel();
						else if(nID == WIZB_NEXT)
							pNested->pPage->m_site.FireOnOK();
						if(pNested->pPage->m_hWnd)
							pNested->pPage->DestroyWindow();
					}
					pNested = pNested->pNextPage;
				}
			}
			else
				m_pCurrentPage->pNestedWizard = 0;
		}

		if(m_pCurrentPage->pPage)
		{
			if(nID == 666)
				m_pCurrentPage->pPage->m_site.FireOnCancel();
			else if(nID == WIZB_NEXT)
				m_pCurrentPage->pPage->m_site.FireOnOK();
			if(m_pCurrentPage->pPage->m_hWnd)
				m_pCurrentPage->pPage->DestroyWindow();
		}

		m_pCurrentPage = m_pCurrentPage->pNextPage;
	}

	_ExitSettingsMode();
	_ExitModalMode();

	_KillWizard();
	
	m_bNowExecute = false;
}

bool CWizard::IsNextPresent()
{
	return (m_pCurrentPage && m_pCurrentPage->pNextPage);
}

bool CWizard::IsPrevPresent()
{

	return (m_pCurrentPage && m_pCurrentPage->pPrevPage && m_pCurrentPage->pPrevPage->pPrevPage);
}


/*bool CWizard::StoreWizard(CString strFileName, CString strSectionName)
{
	INamedDataPtr sptrND = 0;
	sptrIFileDataObject	sptrF = 0;
	if(!strFileName.IsEmpty())
	{
		//create
		sptrF.CreateInstance( szNamedDataProgID );
		if( sptrF == 0 )
			return false;
		try
		{
			CFile	file(strFileName, CFile::modeCreate|CFile::modeNoTruncate);
		}
		catch(CFileException* e) 
		{
			e->Delete();
			return false;
		}
		if( FAILED(sptrF->LoadFile( _bstr_t(strFileName) )))
			return false;
		sptrND = sptrF;
	}
	else
		sptrND = GetAppUnknown();

	ASSERT(sptrND != 0);

	if(strSectionName.IsEmpty())
	{
		strSectionName = "Wizard_";
		strSectionName += m_strName;
	}

	CString strEntry;
	CString strFormName;
	for(int i = 0; i < m_nPagesCount; i++)
	{
		strEntry.Format("Page_%d_Name", i);
		strFormName = ::GetObjectName(m_arrPages[i]->GetControllingUnknown());
		::SetValue(sptrND, strSectionName, strEntry, _variant_t(strFormName));
	}

	return true;
}

bool CWizard::LoadWizard(CString strFileName, CString strSectionName)
{
	if(m_bNowExecute)return false;

	_KillWizard();

	INamedDataPtr sptrND = 0;
	sptrIFileDataObject	sptrF = 0;
	if(!strFileName.IsEmpty())
	{
		//create
		sptrF.CreateInstance( szNamedDataProgID );
		if( sptrF == 0 )
			return false;
		if( sptrF->LoadFile( _bstr_t(strFileName) ) != S_OK )
			return false;
		sptrND = sptrF;
	}
	else
		sptrND = GetAppUnknown();

	ASSERT(sptrND != 0);

	if(strSectionName.IsEmpty())
	{
		strSectionName = "Wizard_";
		strSectionName += m_strName;
	}

	CString strEntry;
	CString strFormName;
	int nIdx = 0;
	while(true)
	{
		strEntry.Format("Page_%d_Name", nIdx);
		strFormName = ::GetValueString(sptrND, strSectionName, strEntry, "");
		if(strFormName)
			break;

		AddWizardPage(strFormName);
		nIdx++;
	}

	return true;
}

*/

void CFormManager::EnableNext(BOOL bEnable) 
{
	if(m_pWizard)
	{
		if(m_pWizard->m_pBtnNext && m_pWizard->m_pBtnNext->m_hWnd)
			m_pWizard->m_pBtnNext->EnableWindow(bEnable);
	}
}

void CFormManager::EnablePrev(BOOL bEnable) 
{
	if(m_pWizard)
	{
		if(m_pWizard->m_pBtnPrev && m_pWizard->m_pBtnPrev->m_hWnd)
			m_pWizard->m_pBtnPrev->EnableWindow(bEnable);
	}
}

void CFormManager::EnableOk(BOOL bEnable) 
{
	if(m_pBtnOk && m_pBtnOk->m_hWnd)
		m_pBtnOk->EnableWindow(bEnable);
}

void CFormManager::EnableApply(BOOL bEnable) 
{
	if(m_pBtnApply && m_pBtnApply->m_hWnd)
		m_pBtnApply->EnableWindow(bEnable);
}

void CFormManager::PressNext() 
{
	if (m_bDisableBtnMode) return;
	if(m_pWizard && m_pWizard->IsNextPresent())
		m_pWizard->Next();
}

void CFormManager::PressPrev() 
{
	if (m_bDisableBtnMode) return;
	if(m_pWizard && m_pWizard->IsPrevPresent())
		m_pWizard->Prev();
}

void CFormManager::PressOK() 
{
	if (m_bDisableBtnMode) return;
	if( m_pBtnOk )
		::PostMessage( m_pBtnOk->GetParent()->GetSafeHwnd(), WM_COMMAND, MAKEWORD( IDOK, BN_CLICKED ), LPARAM( m_pBtnOk->m_hWnd ) );
	if( m_pFormDialog )
		::PostMessage( m_pFormDialog->GetSafeHwnd(), WM_COMMAND, MAKEWORD( IDOK, BN_CLICKED ), LPARAM( 0 ) );
}

void CFormManager::PressCancel() 
{
	if (m_bDisableBtnMode)
	{ // даже если кнопки задизаблены - Cancel будем обрабатывать (откладывая на потом)
		if(m_pFormPage)
			m_pFormPage->m_site.m_bCancelSheduled = true;
		return;
	}
	if( m_pBtnCancel )
		::PostMessage( m_pBtnCancel->GetParent()->GetSafeHwnd(), WM_COMMAND, MAKEWORD( IDCANCEL, BN_CLICKED ), LPARAM( m_pBtnCancel->m_hWnd ) );
	if( m_pFormDialog )
		::PostMessage( m_pFormDialog->GetSafeHwnd(), WM_COMMAND, MAKEWORD( IDCANCEL, BN_CLICKED ), LPARAM( 0 ) );
}

void CFormManager::PressHelp()
{
	if (m_bDisableBtnMode) return;
	if( m_pBtnHelp )
		::PostMessage( m_pBtnHelp->GetParent()->GetSafeHwnd(), WM_COMMAND, MAKEWORD( IDHELP, BN_CLICKED ), LPARAM( m_pBtnHelp->m_hWnd ) );
	if( m_pBtnHelp )
		::PostMessage( m_pFormDialog->GetSafeHwnd(), WM_COMMAND, MAKEWORD( IDHELP, BN_CLICKED ), LPARAM( 0 ) );
}


void CFormManager::OnPageHelp()
{
	if (m_bDisableBtnMode) return;
	if( m_pFormPage )
		m_pFormPage->OnHelp();
}

bool CFormManager::IsPageRunning()
{
	return ( m_pFormPage != 0 );
}

bool CFormManager::SetDisableButtonsMode(bool bMode)
{
	bool bPrev = m_bDisableBtnMode;
	m_bDisableBtnMode = bMode;
	return bPrev;
}

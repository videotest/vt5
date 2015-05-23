// SettingsWindow.cpp : implementation file
//

#include "stdafx.h"
#include "shell.h"
#include "SettingsWindow.h"
#include "MainFrm.h"


/////////////////////////////////////////////////////////////////////////////
// CSettingsWindow dialog


CSettingsWindow::CSettingsWindow(CWnd* pParent /*=NULL*/)
	: CDialog(CSettingsWindow::IDD, pParent)
{
	EnableAutomation();

	//{{AFX_DATA_INIT(CSettingsWindow)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CSettingsWindow::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CDialog::OnFinalRelease();
}

void CSettingsWindow::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSettingsWindow)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSettingsWindow, CDialog)
	//{{AFX_MSG_MAP(CSettingsWindow)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_CONTROL_RANGE(BN_CLICKED, 0, 600, OnBtnClick)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CSettingsWindow, CDialog)
	//{{AFX_DISPATCH_MAP(CSettingsWindow)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_ISettingsWindow to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {87F6BED3-11A3-11D3-A5BE-0000B493FF1B}
static const IID IID_ISettingsWindow =
{ 0x87f6bed3, 0x11a3, 0x11d3, { 0xa5, 0xbe, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b } };

BEGIN_INTERFACE_MAP(CSettingsWindow, CDialog)
	INTERFACE_PART(CSettingsWindow, IID_ISettingsWindow, Dispatch)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSettingsWindow message handlers

BOOL CSettingsWindow::Create( CWnd* pParentWnd ) 
{
	BOOL bRet = CDialog::Create(IDD, pParentWnd);


	//LONG dwStyle = GetWindowLong( GetSafeHwnd(), GWL_STYLE );
	
	//dwStyle |= (WS_CLIPCHILDREN|WS_CLIPSIBLINGS);
	//SetWindowLong( GetSafeHwnd(), GWL_STYLE, dwStyle );

	/*LONG dwExStyle = GetWindowLong(GetSafeHwnd(), GWL_EXSTYLE);
	dwExStyle |= WS_EX_WINDOWEDGE;
	SetWindowLong(GetSafeHwnd(), GWL_EXSTYLE, dwExStyle);
	dwExStyle = GetWindowLong(GetSafeHwnd(), GWL_EXSTYLE);*/
	return bRet;
}


void CSettingsWindow::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	//CDialog::OnCancel();
}

void CSettingsWindow::OnOK() 
{
	// TODO: Add extra validation here
	
	//CDialog::OnOK();
}

void CSettingsWindow::OnBtnClick(UINT nID)
{
	CWnd* pwnd = GetDlgItem(nID);
	if(pwnd)
		pwnd->SendMessage(WM_COMMAND, WPARAM(777), LPARAM(0));
}




BOOL CSettingsWindow::PreTranslateMessage(MSG* pMsg) 
{
	if( CWnd::PreTranslateMessage(pMsg) )
		return true;

	return false;
/*	if( CDialog::PreTranslateMessage(pMsg) )
		return true;

	CMainFrame	*pFrame = (CMainFrame*)AfxGetMainWnd();
	if( pFrame )pFrame->CheckActivePopup( pMsg );
	return false;
 */
}

BOOL CSettingsWindow::PreCreateWindow(CREATESTRUCT& cs) 
{
	if( !CDialog::PreCreateWindow(cs) )return false;
	cs.style |= WS_CLIPSIBLINGS|WS_CLIPCHILDREN;	
	return true;
}

void CSettingsWindow::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	HWND	hwndChild = ::GetWindow( GetSafeHwnd(), GW_CHILD );
	while( hwndChild )
	{
		if( ::GetDlgCtrlID( hwndChild ) == 0 ) 
		{
			::SetWindowPos( hwndChild, 0, 0, 0, cx, cy, SWP_NOZORDER|SWP_SHOWWINDOW);
			break;
		}
		hwndChild = ::GetWindow( hwndChild, GW_HWNDNEXT );
	}
}

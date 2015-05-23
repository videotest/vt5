// FormDockDialog.cpp : implementation file
//FormManager.ExecuteDockBar "Lookup"

#include "stdafx.h"
#include "axform.h"
#include "FormDockDialog.h"


/////////////////////////////////////////////////////////////////////////////
// CFormDockDialog dialog


CFormDockDialog::CFormDockDialog(long lDockSite, bool bFixed)
	: _CFormDialog()
{
	m_bLockHideEvent = false;
	EnableAutomation();
	_OleLockApp( this );
	//{{AFX_DATA_INIT(CFormDockDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_sName = "FormDockDialog";
	m_sUserName = m_sName;
	m_bFixed = bFixed;
	m_dwDockSide = lDockSite;

}	

CFormDockDialog::~CFormDockDialog()
{
	_OleUnlockApp( this );
	//TRACE0("CFormDockDialog released! \n");
}

void CFormDockDialog::DoDataExchange(CDataExchange* pDX)
{
	_CFormDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFormDockDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_INTERFACE_MAP(CFormDockDialog, _CFormDialog)
	INTERFACE_PART(CFormDockDialog, IID_IDockableWindow, Dock)
	INTERFACE_PART(CFormDockDialog, IID_IWindow, Wnd)
//	INTERFACE_PART(CFormDockDialog, IID_IRootedObject, Rooted)
	INTERFACE_PART(CFormDockDialog, IID_INamedObject2, Name)
END_INTERFACE_MAP()


BEGIN_MESSAGE_MAP(CFormDockDialog, _CFormDialog)
	//{{AFX_MSG_MAP(CFormDockDialog)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFormDockDialog message handlers

BOOL CFormDockDialog::OnInitDialog() 
{
	_CFormDialog::OnInitDialog();
	// TODO: Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

bool CFormDockDialog::DoCreate( DWORD dwStyle, CRect rc, CWnd *pparent, UINT nID )
{
	if (!Create( IDD, pparent) )
		return false;

	DoCaptionChange();

	CRect	rect = NORECT;
	GetClientRect(&rect);

	//m_size = CSize(rect.Width(), rect.Height());

	m_size = m_site.GetSize();
	m_size.cy -= 5;

	m_bFixed = true;

	return true;
}

void CFormDockDialog::DoCaptionChange()
{
	m_sUserName =  m_site.GetCaption();
	SetWindowText( m_sUserName );
	CWnd	*pwnd = CWnd::GetParent();
	while( pwnd->GetSafeHwnd() )
	{
		DWORD	dw = GetWindowLong( pwnd->GetSafeHwnd(), GWL_STYLE );
		if( dw & WS_POPUP )
			pwnd->SetWindowText( m_sUserName );
		pwnd = pwnd->GetParent();
	}
}

void CFormDockDialog::SetDlgTitle(CString strTitle)
{
	m_sName = strTitle;
	m_sUserName = strTitle;
}

void CFormDockDialog::OnFinalRelease() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	//_CFormDialog::OnFinalRelease();

	m_bLockHideEvent = true;
	if( IsWindow( GetSafeHwnd() ) )
		DestroyWindow();
	m_bLockHideEvent = false;

	delete this;
}

void CFormDockDialog::OnDestroy() 
{
	//??m_site.DestroyWindow();

	_CFormDialog::OnDestroy();
	
}

void CFormDockDialog::OnShow()
{
	m_site.FireOnShow();
}

void CFormDockDialog::OnHide()
{
	if( !m_bLockHideEvent )
		m_site.FireOnHide();
}

void CFormDockDialog::OnShowWindow(BOOL bShow, UINT nStatus)
{
	if( bShow )
		__super::OnShowWindow(bShow, nStatus);
	else
	{
		if( !m_bLockHideEvent )
			__super::OnShowWindow(bShow, nStatus);
		else
			CDialog::OnShowWindow(bShow, nStatus);
	}

	// TODO: Add your message handler code here
}

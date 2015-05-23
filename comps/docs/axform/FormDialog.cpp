// FormDialog.cpp : implementation file
//

#include "stdafx.h"
#include "axform.h"
#include "FormDialog.h"
#include "FormManager.h"
#include "afxpriv.h"


/////////////////////////////////////////////////////////////////////////////
// CFormDialog dialog


CFormDialog::CFormDialog( CWnd* pParent )
	: CDialog(CFormDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFormDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CFormDialog::SetForm( IUnknown *punkForm )
{	
	m_site.SetForm( punkForm );	
}



CFormDialog::~CFormDialog()
{
}


void CFormDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFormDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFormDialog, CDialog)
	//{{AFX_MSG_MAP(CFormDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_HELPINFO()
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_CAPTIONCHANGE, OnCaptionChange)
	ON_COMMAND(IDHELP, OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFormDialog message handlers

BOOL CFormDialog::OnInitDialog() 
{
	m_site.m_bFormInited = false;
	m_site.m_bCancelSheduled = false;
	CDialog::OnInitDialog();


//	SetWindowLong( GetSafeHwnd(), GWL_EXSTYLE, GetExStyle()|WS_EX_CONTEXTHELP );

	SetWindowText( m_site.GetCaption() );
	//SetWindowText( m_strTitle );
	
	
	CRect	rectSite;
	GetDlgItem( IDC_CONTROLS )->GetWindowRect( &rectSite );
	GetDlgItem( IDC_CONTROLS )->ShowWindow( SW_HIDE );

	ScreenToClient( rectSite );

	CSize	sizeOld = rectSite.Size();
	CSize	sizeNew = m_site.GetSize();
	sizeNew.cx = max( sizeNew.cx, sizeOld.cx );
	sizeNew.cy = max( sizeNew.cy, sizeOld.cy );

	rectSite.right = rectSite.left+sizeNew.cx;
	rectSite.bottom = rectSite.top+sizeNew.cy;

	CPoint	pointOffset( sizeNew.cx-sizeOld.cx, sizeNew.cy-sizeOld.cy );

	CRect	rectDialog;
	GetWindowRect( &rectDialog );
	rectDialog.right+= pointOffset.x;
	rectDialog.bottom+= pointOffset.y;

	MoveWindow( rectDialog );

	HWND	hwnd = ::GetWindow( GetSafeHwnd(), GW_CHILD );

	while( hwnd ) 
	{
		CRect	rect;
		::GetWindowRect( hwnd, &rect );
		ScreenToClient( rect );
		rect.OffsetRect( pointOffset );
		::MoveWindow( hwnd, rect.left, rect.top, rect.Width(), rect.Height(), 0 );

		hwnd = ::GetWindow( hwnd, GW_HWNDNEXT );
	}

	m_site.m_bLockFireEvent = true;
	m_site.SetEdgeType( 2 );
	m_site.DoCreate( WS_CHILD|WS_VISIBLE, rectSite, this, 101 );
	m_site.m_bLockFireEvent = false;

	m_site.m_bFormInited = true;
	if(m_site.m_bCancelSheduled)
	{
		CFormManager::m_pFormManager->PressCancel();
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFormDialog::OnCancel() 
{
	if(!m_site.m_bFormInited)
	{
		m_site.m_bCancelSheduled = true;
		return; // пока не проинитили - нах кнопки
	}
	m_site.m_bCancelSheduled = false;
	if( m_site.m_bClosingWindow )return;
	if( m_site.m_bContinueModal )return;

	m_site.FireOnCancel();
	m_site.FireOnHide();	

	if( m_site.AllowClose() )
		CDialog::OnCancel();
}

void CFormDialog::OnOK() 
{
	if(!m_site.m_bFormInited) return; // пока не проинитили - нах кнопки
	if( m_site.m_bClosingWindow )return;
	if( m_site.m_bContinueModal )return;

	m_site.FireOnOK();
	m_site.FireOnHide();	

	if( m_site.AllowClose() )
		CDialog::OnOK();
}

LRESULT CFormDialog::OnCaptionChange(WPARAM, LPARAM)
{
	DoCaptionChange();
	return 0;
}

void CFormDialog::DoCaptionChange()
{
	SetWindowText( m_site.GetCaption() );
}


void CFormDialog::OnSysCommand(UINT nID, LPARAM lParam) 
{
	//help
	/*if( nID == SC_CONTEXTHELP )m_site.SetHelpDisplayed( false );
	
	CDialog::OnSysCommand(nID, lParam);

	if( nID == SC_CONTEXTHELP && !m_site.IsHelpDisplayed() )
		m_site.SendMessage( WM_HELPHITTEST );*/

	if( nID == SC_CONTEXTHELP )
		m_site.SendMessage( WM_HELPHITTEST );
	else
		CDialog::OnSysCommand(nID, lParam);

}

BOOL CFormDialog::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	m_site.SendMessage( WM_HELPHITTEST );

	return true;
}

BOOL CFormDialog::PreTranslateMessage(MSG* pMsg) 
{

	return CDialog::PreTranslateMessage(pMsg);
}

void CFormDialog::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);

	if( bShow )
		m_site.FireOnShow();	
	else
		m_site.FireOnHide();	
}

void CFormDialog::OnHelp() 
{
	m_site.SendMessage( WM_HELPHITTEST );
}

LRESULT CFormDialog::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message >= WM_MOUSEFIRST && message <= WM_MOUSELAST )
	{
		HWND hwndParent = GetNotChildParent(m_hWnd);
		return hwndParent?::SendMessage(hwndParent,message,wParam,lParam):0;
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

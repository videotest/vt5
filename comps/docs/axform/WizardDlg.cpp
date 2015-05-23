// WizardDlg.cpp : implementation file
//

#include "stdafx.h"
#include "axform.h"
#include "WizardDlg.h"
#include "WizButton.h"
#include "button_messages.h"


/////////////////////////////////////////////////////////////////////////////
// CWizardDlg dialog


CWizardDlg::CWizardDlg(CWnd* pParent /*=NULL*/, CSize size, CString strName)
	: CDialog(CWizardDlg::IDD, pParent), m_size(size)
{
	//{{AFX_DATA_INIT(CWizardDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_strName = strName;
}


void CWizardDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWizardDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWizardDlg, CDialog)
	//{{AFX_MSG_MAP(CWizardDlg)
	ON_CONTROL_RANGE(BN_CLICKED, 0, 600, OnBtnClick)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWizardDlg message handlers

BOOL CWizardDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetWindowText(m_strName);
	
	// TODO: Add extra initialization here
	
	/*CRect	rectSite;
	GetDlgItem( IDC_CONTROLS )->GetWindowRect( &rectSite );
	GetDlgItem( IDC_CONTROLS )->ShowWindow( SW_HIDE );

	ScreenToClient( rectSite );

	CSize	sizeOld = rectSite.Size();
	CSize	sizeNew = m_size;

	rectSite.right = rectSite.left+sizeNew.cx;
	rectSite.bottom = rectSite.top+sizeNew.cy;

	CPoint	pointOffset( sizeNew.cx-sizeOld.cx, sizeNew.cy-sizeOld.cy );

	CRect	rectDialog;
	GetWindowRect( &rectDialog );
	rectDialog.right+= pointOffset.x;
	rectDialog.bottom+= pointOffset.y;*/

	CRect	rectClient = NORECT;
	GetDlgItem(IDC_CONTROLS)->GetClientRect(&rectClient);
	CRect	rectDialog = NORECT;
	GetWindowRect( &rectDialog );
	long nOffY = rectDialog.Height() - rectClient.Height();
	rectDialog.right = rectDialog.left + m_size.cx + rectDialog.Width() - rectClient.Width();
	rectDialog.bottom = rectDialog.top + m_size.cy + nOffY;
	rectClient.right = rectClient.left + m_size.cx;
	rectClient.bottom = rectClient.top + m_size.cy;
	GetDlgItem(IDC_CONTROLS)->MoveWindow(rectClient);
	GetDlgItem(IDC_CONTROLS)->ShowWindow(SW_HIDE);
	MoveWindow(rectDialog);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CWizardDlg::OnBtnClick(UINT nID)
{
	if(nID == IDCANCEL)
		nID = WIZB_FINISH;
	
	CWnd* pwnd = GetDlgItem(nID);
	if(pwnd)
		pwnd->SendMessage(WM_COMMAND, WPARAM(777), LPARAM(0));
}

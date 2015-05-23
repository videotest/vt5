// ClassNameDlg.cpp : implementation file
//

#include "stdafx.h"
#include "objects.h"
#include "ClassNameDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CClassNameDlg dialog


CClassNameDlg::CClassNameDlg(CWnd* pParent /*=NULL*/, CString strName)
	: CDialog(CClassNameDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CClassNameDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_strName = strName;
}


void CClassNameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CClassNameDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CClassNameDlg, CDialog)
	//{{AFX_MSG_MAP(CClassNameDlg)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClassNameDlg message handlers

void CClassNameDlg::OnDestroy() 
{
	GetDlgItem(IDC_EDIT_CLASS_NAME)->GetWindowText(m_strName);
	CDialog::OnDestroy();
}

BOOL CClassNameDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	GetDlgItem(IDC_EDIT_CLASS_NAME)->SetWindowText(m_strName);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CClassNameDlg::OnHelp() 
{
	HelpDisplay( "ClassNameDlg" );
}

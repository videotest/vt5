//*******************************************************************************
// COPYRIGHT NOTES
// ---------------
// This source code is a part of BCGControlBar library.
// You may use, compile or redistribute it as part of your application 
// for free. You cannot redistribute it as a part of a software development 
// library without the agreement of the author. If the sources are 
// distributed along with the application, you should leave the original 
// copyright notes in the source code without any changes.
// This code can be used WITHOUT ANY WARRANTIES on your own risk.
// 
// For the latest updates to this library, check my site:
// http://welcome.to/bcgsoft
// 
// Stas Levin <bcgsoft@yahoo.com>
//*******************************************************************************

// ToolbarNameDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ToolbarNameDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CToolbarNameDlg dialog


CToolbarNameDlg::CToolbarNameDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CToolbarNameDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CToolbarNameDlg)
	m_strToolbarName = _T("");
	//}}AFX_DATA_INIT
}


void CToolbarNameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CToolbarNameDlg)
	DDX_Control(pDX, IDOK, m_btnOk);
	DDX_Text(pDX, IDC_BCGBARRES_TOOLBAR_NAME, m_strToolbarName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CToolbarNameDlg, CDialog)
	//{{AFX_MSG_MAP(CToolbarNameDlg)
	ON_EN_UPDATE(IDC_BCGBARRES_TOOLBAR_NAME, OnUpdateToolbarName)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CToolbarNameDlg message handlers

void CToolbarNameDlg::OnUpdateToolbarName() 
{
	UpdateData ();
	m_btnOk.EnableWindow (!m_strToolbarName.IsEmpty ());
}
//**********************************************************************************
BOOL CToolbarNameDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_btnOk.EnableWindow (!m_strToolbarName.IsEmpty ());
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

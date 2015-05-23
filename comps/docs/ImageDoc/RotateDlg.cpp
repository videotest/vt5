// RotateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "imagedoc.h"
#include "RotateDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CRotateDlg dialog


CRotateDlg::CRotateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRotateDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRotateDlg)
	m_nAngle = 0;
	//}}AFX_DATA_INIT
}


void CRotateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRotateDlg)
	DDX_Control(pDX, IDC_EDIT_ANGLE, m_editAngle);
	DDX_Text(pDX, IDC_EDIT_ANGLE, m_nAngle);
	DDV_MinMaxUInt(pDX, m_nAngle, 0, 360);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRotateDlg, CDialog)
	//{{AFX_MSG_MAP(CRotateDlg)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRotateDlg message handlers

BOOL CRotateDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CString strText;
	strText.Format("%li", m_nAngle);
	m_editAngle.SetWindowText(strText);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CRotateDlg::OnOK() 
{
	// TODO: Add extra validation here
	CString strText;
	m_editAngle.GetWindowText(strText);
	m_nAngle = atol(strText);
	CDialog::OnOK();
}

void CRotateDlg::OnHelp() 
{
	HelpDisplay( "RotateDlg" );	
}

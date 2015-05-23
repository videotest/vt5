// DatePropDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PrintFieldAX.h"
#include "DatePropDlg.h"


// CDatePropDlg dialog

IMPLEMENT_DYNAMIC(CDatePropDlg, CDialog)
CDatePropDlg::CDatePropDlg(CWnd* pParent, print_field_params *pparams)
	: CDialog(CDatePropDlg::IDD, pParent)
{
	m_pparams = pparams;
}

CDatePropDlg::~CDatePropDlg()
{
}

void CDatePropDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDatePropDlg, CDialog)
END_MESSAGE_MAP()


// CDatePropDlg message handlers

BOOL CDatePropDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CheckDlgButton( IDC_SETDATE, (m_pparams->flags & PFF_TEXT_STYLE_INITBYDATE) == PFF_TEXT_STYLE_INITBYDATE );

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDatePropDlg::OnOK()
{
	m_pparams->flags &= ~PFF_TEXT_STYLE_MASK;
	if( IsDlgButtonChecked( IDC_SETDATE ) )
		m_pparams->flags |= PFF_TEXT_STYLE_INITBYDATE;

	CDialog::OnOK();
}

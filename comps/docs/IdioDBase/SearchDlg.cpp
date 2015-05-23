// SearchDlg.cpp : implementation file
//

#include "stdafx.h"
#include "IdioDBase.h"
#include "SearchDlg.h"


// CSearchDlg dialog

IMPLEMENT_DYNAMIC(CSearchDlg, CDialog)
CSearchDlg::CSearchDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSearchDlg::IDD, pParent)
	, m_Text(_T(""))
{
}

CSearchDlg::~CSearchDlg()
{
}

void CSearchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_Text);
}


BEGIN_MESSAGE_MAP(CSearchDlg, CDialog)
END_MESSAGE_MAP()


// CSearchDlg message handlers

BOOL CSearchDlg::OnInitDialog()
{
	__super::OnInitDialog();

	return TRUE;
}

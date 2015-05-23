// SelectParamDlg.cpp : implementation file
//

#include "stdafx.h"
#include "objects.h"
#include "SelectParamDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelectParamDlg dialog


CSelectParamDlg::CSelectParamDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectParamDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelectParamDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_strLastSelected = "";
}


void CSelectParamDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectParamDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelectParamDlg, CDialog)
	//{{AFX_MSG_MAP(CSelectParamDlg)
	ON_WM_DESTROY()
	ON_LBN_DBLCLK(IDC_LIST_PARAMS, OnDblclkListParams)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectParamDlg message handlers

BOOL CSelectParamDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CListBox* pListBox = (CListBox*)GetDlgItem(IDC_LIST_PARAMS);

	for(long i = 0; i < m_arrParams.GetSize(); i++)
		pListBox->AddString(m_arrParams[i]);

	pListBox->SetCurSel(0);
	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSelectParamDlg::OnDestroy() 
{
	CListBox* pListBox = (CListBox*)GetDlgItem(IDC_LIST_PARAMS);
	int nCurSel = pListBox->GetCurSel();
	if(nCurSel != LB_ERR)
		pListBox->GetText(nCurSel, m_strLastSelected);
	CDialog::OnDestroy();
}

void CSelectParamDlg::OnDblclkListParams() 
{
	EndDialog(IDOK);	
}

void CSelectParamDlg::OnHelp() 
{
	HelpDisplay( "SelectParamDlg" );
	
}

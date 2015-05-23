// WidthDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vtcontrols2.h"
#include "WidthDlg.h"
#include "VTWidthCtl.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWidthDlg dialog


CWidthDlg::CWidthDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWidthDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWidthDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CWidthDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWidthDlg)
	DDX_Control(pDX, IDC_EDIT1, m_edit);
	DDX_Control(pDX, IDC_SPIN1, m_spin);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWidthDlg, CDialog)
	//{{AFX_MSG_MAP(CWidthDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWidthDlg message handlers

void CWidthDlg::OnOK() 
{
	// TODO: Add extra validation here
	LPCTSTR str;
	CString string;
	m_edit.GetWindowText(string);
	str=string;
	int result=(int)strtod((char*)str,NULL);
	if(result>=m_Min && result<=m_Max)
	{
		m_nWidth=result;
		CDialog::EndDialog(IDOK);
	}
	else
		AfxMessageBox( IDS_WRONG_WIDTH );
}

BOOL CWidthDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
//	CVTWidthCtrl* parent=(CVTWidthCtrl*) GetParent();
	char str[10];
	sprintf(str,"%i",m_Width);
	m_edit.SetWindowText(str);
	m_spin.SetBuddy(&m_edit);
	m_spin.SetRange(m_Min,m_Max);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

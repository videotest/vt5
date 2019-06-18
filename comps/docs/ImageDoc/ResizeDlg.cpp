// ResizeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "imagedoc.h"
#include "ResizeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CResizeDlg dialog


CResizeDlg::CResizeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CResizeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CResizeDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_nWidth = 0;
	m_nHeight = 0;
}


void CResizeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CResizeDlg)
	DDX_Control(pDX, IDC_EDIT_WIDTH, m_editWidth);
	DDX_Control(pDX, IDC_EDIT_HEIGHT, m_editHeight);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CResizeDlg, CDialog)
	//{{AFX_MSG_MAP(CResizeDlg)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResizeDlg message handlers

BOOL CResizeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CString strText;
	strText.Format("%li", m_nWidth);
	m_editWidth.SetWindowText(strText);
	strText.Format("%li", m_nHeight);
	m_editHeight.SetWindowText(strText);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CResizeDlg::OnOK() 
{
	// TODO: Add extra validation here
	CString strText;
	m_editWidth.GetWindowText(strText);
	m_nWidth = atol(strText);
	m_editHeight.GetWindowText(strText);
	m_nHeight = atol(strText);

	CDialog::OnOK();
}

void CResizeDlg::OnHelp() 
{
	HelpDisplay( "ResizeDlg" );
}

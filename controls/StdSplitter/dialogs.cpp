// dialogs.cpp : implementation file
//

#include "stdafx.h"
#include "stdsplitter.h"
#include "dialogs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNameDialog dialog


CNameDialog::CNameDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CNameDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNameDialog)
	m_strName = _T("");
	//}}AFX_DATA_INIT
}


void CNameDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNameDialog)
	DDX_Text(pDX, IDC_NAME, m_strName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNameDialog, CDialog)
	//{{AFX_MSG_MAP(CNameDialog)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNameDialog message handlers

BOOL CNameDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetWindowText(m_strTitle);
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CNameDialog::OnHelp() 
{
	HelpDisplay( "NameDialog" );
	
}

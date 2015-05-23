// macrodialogs.cpp : implementation file
//

#include "stdafx.h"
#include "macros.h"
#include "macrodialogs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEnterNameDlg dialog


CEnterNameDlg::CEnterNameDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEnterNameDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEnterNameDlg)
	m_strName = _T("NewMacro");
	//}}AFX_DATA_INIT
}


void CEnterNameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEnterNameDlg)
	DDX_Text(pDX, IDC_NAME, m_strName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEnterNameDlg, CDialog)
	//{{AFX_MSG_MAP(CEnterNameDlg)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEnterNameDlg message handlers

void CEnterNameDlg::OnOK() 
{
	if( !UpdateData() )
		return;

	if( m_strName.IsEmpty() )
	{
		AfxMessageBox( IDS_EMPTYNAME );
		return;
	}
	CDialog::OnOK();
}

void CEnterNameDlg::OnHelp() 
{
	HelpDisplay( "EnterNameDlg" );
}

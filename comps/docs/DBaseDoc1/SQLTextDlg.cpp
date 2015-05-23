// SQLTextDlg.cpp : implementation file
//

#include "stdafx.h"
#include "dbasedoc.h"
#include "SQLTextDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CSQLTextDlg dialog


CSQLTextDlg::CSQLTextDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSQLTextDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSQLTextDlg)
	m_SQLText = _T("");
	//}}AFX_DATA_INIT
}


void CSQLTextDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSQLTextDlg)
	DDX_Text(pDX, IDC_SQL, m_SQLText);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSQLTextDlg, CDialog)
	//{{AFX_MSG_MAP(CSQLTextDlg)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSQLTextDlg message handlers


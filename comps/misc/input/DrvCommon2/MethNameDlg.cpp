// MethNameDlg.cpp : implementation file
//

#include "stdafx.h"
//#include "ProgRes.h"
#include "MethNameDlg.h"


// CMethNameDlg dialog

int CMethNameDlg::s_idd = -1;
int CMethNameDlg::s_idEdit = -1;


IMPLEMENT_DYNAMIC(CMethNameDlg, CDialog)
CMethNameDlg::CMethNameDlg(LPCTSTR lpstrDefName, CWnd* pParent /*=NULL*/)
	: CDialog(s_idd, pParent)
	, m_sName(lpstrDefName)
{
}

CMethNameDlg::~CMethNameDlg()
{
}

void CMethNameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, s_idEdit, m_sName);
}


BEGIN_MESSAGE_MAP(CMethNameDlg, CDialog)
END_MESSAGE_MAP()



// CMethNameDlg message handlers

void CMethNameDlg::OnOK()
{
	UpdateData();
	CDialog::OnOK();
}

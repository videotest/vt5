// ParamDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Metodics.h"
#include "ParamDialog.h"

namespace MetodicsTabSpace
{


// CAddPaneParamDlg dialog

IMPLEMENT_DYNAMIC(CAddPaneParamDlg, CDialog)
CAddPaneParamDlg::CAddPaneParamDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAddPaneParamDlg::IDD, pParent)
	, m_strName(_T(""))
{
}

CAddPaneParamDlg::~CAddPaneParamDlg()
{
}

void CAddPaneParamDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strName);
}


BEGIN_MESSAGE_MAP(CAddPaneParamDlg, CDialog)
END_MESSAGE_MAP()


// CAddPaneParamDlg message handlers

}
// WaitDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pixera.h"
#include "WaitDlg.h"
#include "StdProfile.h"
#include "PixSdk.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWaitDlg dialog


CWaitDlg::CWaitDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWaitDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWaitDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bClosed = false;
}


void CWaitDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWaitDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWaitDlg, CDialog)
	//{{AFX_MSG_MAP(CWaitDlg)
	ON_WM_TIMER()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWaitDlg message handlers

BOOL CWaitDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_nAdditionalInit = 0;
	int n = CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("AdditionalInitCount"), 5);
//	SetTimer(1, 1000, NULL);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CWaitDlg::OnTimer(UINT_PTR nIDEvent) 
{
/*	int n = CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("AdditionalInitCount"), 5);
	if (m_nAdditionalInit >= n+2)
	{
		PostMessage(WM_CLOSE);
	}*/
	CDialog::OnTimer(nIDEvent);
}

void CWaitDlg::OnClose() 
{
	m_bClosed = true;
	CDialog::OnClose();
}

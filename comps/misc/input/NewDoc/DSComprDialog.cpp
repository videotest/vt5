// DSComprDialog.cpp : implementation file
//

#include "stdafx.h"
#include "newdoc.h"
#include "DSComprDialog.h"
#include "DirectShowDriver.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDSComprDialog dialog


CDSComprDialog::CDSComprDialog(IDSCompressionSite *pSite, CWnd* pParent /*=NULL*/)
	: CDialog(CDSComprDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDSComprDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pSite = pSite;
}


void CDSComprDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDSComprDialog)
	DDX_Control(pDX, IDC_LIST, m_List);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDSComprDialog, CDialog)
	//{{AFX_MSG_MAP(CDSComprDialog)
	ON_BN_CLICKED(IDC_SETTINGS, OnSettings)
	ON_LBN_SELCHANGE(IDC_LIST, OnSelchangeList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDSComprDialog message handlers

BOOL CDSComprDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	int nCount = m_pSite->GetCompressorsCount();
	for (int i = 0; i < nCount; i++)
	{
		_bstr_t bstr(m_pSite->GetCompressorName(i));
		m_List.AddString((LPCTSTR)bstr);
	}
	int nCompr = g_DSProfile.GetProfileInt(_T("VideoDlg"), _T("Compression"), 0);
	m_List.SetCurSel(max(min(nCompr,nCount-1),0));
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDSComprDialog::OnOK() 
{
	int nCompr = m_List.GetCurSel();
	g_DSProfile.WriteProfileInt(_T("VideoDlg"), _T("Compression"), nCompr);
	
	CDialog::OnOK();
}

void CDSComprDialog::OnSettings() 
{
	m_pSite->CompressorDlg(m_List.GetCurSel());
}

void CDSComprDialog::OnSelchangeList() 
{
	bool bDlg = m_pSite->HasCompressorDialog(m_List.GetCurSel());
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_SETTINGS), bDlg);
}

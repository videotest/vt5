// FormatDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "mv500Grab.h"
#include "FormatDlg.h"
#include "Vt5Profile.h"
#include "Defs.h"
#include "ComDef.h"
#include "misc_utils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFormatDlg dialog


CFormatDlg::CFormatDlg(CWnd* pParent /*=NULL*/)
: CDialog(CFormatDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFormatDlg)
	m_bGrayScale = CMV500Grab::s_pGrab->IsGrayScale(false);
	m_bVerticalMirror = CStdProfileManager::m_pMgr->GetProfileInt(_T("Format"), _T("VerticalMirror"), TRUE);
	m_bHorizontalMirror = CStdProfileManager::m_pMgr->GetProfileInt(_T("Format"), _T("HorizontalMirror"), TRUE);
	m_nScale = CStdProfileManager::m_pMgr->GetProfileInt(_T("Format"), _T("Scale"), 0);
	//}}AFX_DATA_INIT
	if (m_nScale != 0 && m_nScale != 1)
		m_nScale = 0;
}


void CFormatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFormatDlg)
	DDX_Control(pDX, IDC_FORMAT, m_Format);
	DDX_Check(pDX, IDC_GRAY_SCALE, m_bGrayScale);
	DDX_Check(pDX, IDC_VERTICAL_MIRROR, m_bVerticalMirror);
	DDX_CBIndex(pDX, IDC_FORMAT, m_nScale);
	DDX_Check(pDX, IDC_HORIZONTAL_MIRROR, m_bHorizontalMirror);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFormatDlg, CDialog)
	//{{AFX_MSG_MAP(CFormatDlg)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFormatDlg message handlers

void CFormatDlg::OnOK() 
{
	UpdateData();
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Format"), _T("GrayScale"), m_bGrayScale);
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Format"), _T("VerticalMirror"), m_bVerticalMirror);
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Format"), _T("HorizontalMirror"), m_bHorizontalMirror);
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Format"), _T("Scale"), m_nScale);
	CDialog::OnOK();
}


BOOL CFormatDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	if (CMV500Grab::s_pGrab->GetBoardType() == CMV500Grab::Board_MV510)
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_GRAY_SCALE), FALSE);
	if (CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("DisableSize"), 0, true) ||
		CMV500Grab::s_pGrab->GetCamerasNum() > 1)
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_FORMAT), FALSE);
	if (CMV500Grab::s_pGrab->GetCamerasNum() > 1 && CMV500Grab::s_pGrab->GetBoardType() == CMV500Grab::Board_MV510)
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_HORIZONTAL_MIRROR), FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CFormatDlg::OnHelp() 
{
	if (((CVT5ProfileManager *)CStdProfileManager::m_pMgr)->IsVT5Profile())
		HelpDisplayTopic("MV500",NULL, "FormatDlg","$GLOBAL_main");
	else
		CDialog::OnHelp();
}

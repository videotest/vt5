// SettingsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "SettingsDialog.h"
#include "BaseDialog.h"
#include "input.h"


// CSettingsDialog dialog

IMPLEMENT_DYNAMIC(CSettingsDialog, CSettingsBase)
CSettingsDialog::CSettingsDialog(IUnknown *punk, int nDevice, CWnd* pParent /*=NULL*/)
	: CSettingsBase(punk, nDevice, CSettingsDialog::IDD,CSettingsIds(), pParent)
{
}

CSettingsDialog::~CSettingsDialog()
{
}

void CSettingsDialog::DoDataExchange(CDataExchange* pDX)
{
	CSettingsBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_Tab);
	DDX_Control(pDX, IDC_STATIC_PERIOD, m_StaticPeriod);
	DDX_Control(pDX, IDOK, m_Ok);
	DDX_Control(pDX, ID_HELP, m_Help);
}


BEGIN_MESSAGE_MAP(CSettingsDialog, CSettingsBase)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, OnTcnSelchangeTab1)
END_MESSAGE_MAP()


// CSettingsDialog message handlers

BOOL CSettingsDialog::OnInitDialog()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	CSettingsBase::OnInitDialog();
	CString s;
//	s.LoadString(IDS_CAMERA);
	m_Tab.InsertItem(0, "Bright/contrast");
//	s.LoadString(IDS_COLOR_BALANCE);
	m_Tab.InsertItem(1, "Color balance");
//	s.LoadString(IDS_IMAGE);
	m_Tab.InsertItem(2, "Image");
	AddDialog(new CBaseDialog(IDD_BRIGHTNESS_CONTRAST,this,NULL), IDD_BRIGHTNESS_CONTRAST);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSettingsDialog::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	int n = m_Tab.GetCurSel();
	if (n == 1)
		AddDialog(new CBaseDialog(IDD_COLOR_BALANCE,this,NULL), IDD_COLOR_BALANCE);
	else if (n == 2)
		AddDialog(new CBaseDialog(IDD_IMAGE,this,NULL), IDD_IMAGE);
	else
		AddDialog(new CBaseDialog(IDD_BRIGHTNESS_CONTRAST,this,NULL), IDD_BRIGHTNESS_CONTRAST);

	*pResult = 0;
}


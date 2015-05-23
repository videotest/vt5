// FramePage.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "SettingsSheet.h"
#include "FramePage.h"
#include "DriverWrp.h"
#include "Lock.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

static bool s_lock;

IMPLEMENT_DYNCREATE(CFramePage, CSettingsPage)

/////////////////////////////////////////////////////////////////////////////
// CFramePage property page

CFramePage::CFramePage() : CSettingsPage(CFramePage::IDD)
{
	//{{AFX_DATA_INIT(CFramePage)
	//}}AFX_DATA_INIT
}

CFramePage::~CFramePage()
{
}

void CFramePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFramePage)
	DDX_Control(pDX, IDC_E_BOTTOM_MARGIN, m_EBottomMargin);
	DDX_Control(pDX, IDC_E_TOP_MARGIN, m_ETopMargin);
	DDX_Control(pDX, IDC_E_RIGHT_MARGIN, m_ERightMargin);
	DDX_Control(pDX, IDC_E_LEFT_MARGIN, m_ELeftMargin);
	DDX_Control(pDX, IDC_S_TOP_MARGIN, m_STopMargin);
	DDX_Control(pDX, IDC_S_RIGHT_MARGIN, m_SRightMargin);
	DDX_Control(pDX, IDC_S_LEFT_MARGIN, m_SLeftMargin);
	DDX_Control(pDX, IDC_S_BOTTOM_MARGIN, m_SBottomMargin);
	//}}AFX_DATA_MAP
	if (pDX->m_bSaveAndValidate)
	{
		int n = IsDlgButtonChecked(IDC_ENABLE_FRAME);
		m_pDrv.SetInt(0, _T("Settings"), _T("EnableMargins"), n, 0); ;
		if (n)
		{
			int l = ::GetDlgItemInt(m_hWnd, IDC_E_LEFT_MARGIN, NULL, FALSE);
			int t = ::GetDlgItemInt(m_hWnd, IDC_E_TOP_MARGIN, NULL, FALSE);
			int r = ::GetDlgItemInt(m_hWnd, IDC_E_RIGHT_MARGIN, NULL, FALSE);
			int b = ::GetDlgItemInt(m_hWnd, IDC_E_BOTTOM_MARGIN, NULL, FALSE);
			m_pDrv.SetInt(0, _T("Settings"), _T("LeftMargin"), l, 0);
			m_pDrv.SetInt(0, _T("Settings"), _T("TopMargin"), t, 0);
			m_pDrv.SetInt(0, _T("Settings"), _T("RightMargin"), r, 0);
			m_pDrv.SetInt(0, _T("Settings"), _T("BottomMargin"), b, 0);
		}
	}
	else
	{
		BOOL bEnable = m_pDrv.GetInt(0, _T("Settings"), _T("EnableMargins"), 0);
		int l = m_pDrv.GetInt(0, _T("Settings"), _T("LeftMargin"), 0);
		int t = m_pDrv.GetInt(0, _T("Settings"), _T("TopMargin"), 0);
		int r = m_pDrv.GetInt(0, _T("Settings"), _T("RightMargin"), 0);
		int b = m_pDrv.GetInt(0, _T("Settings"), _T("BottomMargin"), 0);
		::CheckDlgButton(m_hWnd, IDC_ENABLE_FRAME, bEnable);
		::SetDlgItemInt(m_hWnd, IDC_E_LEFT_MARGIN, l, FALSE);
		::SetDlgItemInt(m_hWnd, IDC_E_TOP_MARGIN, t, FALSE);
		::SetDlgItemInt(m_hWnd, IDC_E_RIGHT_MARGIN, r, FALSE);
		::SetDlgItemInt(m_hWnd, IDC_E_BOTTOM_MARGIN, b, FALSE);
	}

}

void CFramePage::UpdateControls(int nHint)
{
	if (!m_hWnd) return;
	if (nHint == ESH_Start_Auto || nHint == ESH_Stop_Auto || nHint == 0)
	{
		BOOL bEnable = nHint!=ESH_Start_Auto;
		BOOL bEnableMargins = m_pDrv.GetInt(0, _T("Settings"), _T("EnableMargins"), 0, 0);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_ENABLE_FRAME), bEnable);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_PREVIEW_ALL_FRAME), bEnable&&bEnableMargins);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_E_LEFT_MARGIN), bEnable&&bEnableMargins);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_S_LEFT_MARGIN), bEnable&&bEnableMargins);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_E_RIGHT_MARGIN), bEnable&&bEnableMargins);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_S_RIGHT_MARGIN), bEnable&&bEnableMargins);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_E_TOP_MARGIN), bEnable&&bEnableMargins);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_S_TOP_MARGIN), bEnable&&bEnableMargins);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_E_BOTTOM_MARGIN), bEnable&&bEnableMargins);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_S_BOTTOM_MARGIN), bEnable&&bEnableMargins);
	}
}

BOOL CFramePage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	int nMaxMarginX = m_pDrv.GetInt(0, _T("Settings"), _T("MaxMarginX"), -1, ID2_BOARDIFDEFAULT);
	int nMaxMarginY = m_pDrv.GetInt(0, _T("Settings"), _T("MaxMarginY"), -1, ID2_BOARDIFDEFAULT);
	int l = m_pDrv.GetInt(0, _T("Settings"), _T("LeftMargin"), 0);
	int t = m_pDrv.GetInt(0, _T("Settings"), _T("TopMargin"), 0);
	int r = m_pDrv.GetInt(0, _T("Settings"), _T("RightMargin"), 0);
	int b = m_pDrv.GetInt(0, _T("Settings"), _T("BottomMargin"), 0);
	CLock lock(&s_lock);
	m_SLeftMargin.SetRange(0, nMaxMarginX);
	m_SLeftMargin.SetPos(l);
	m_SRightMargin.SetRange(0, nMaxMarginX);
	m_SRightMargin.SetPos(r);
	m_STopMargin.SetRange(0, nMaxMarginY);
	m_STopMargin.SetPos(t);
	m_SBottomMargin.SetRange(0, nMaxMarginY);
	m_SBottomMargin.SetPos(b);
	/*if (s_bSetup)
		UpdateControls(ESH_Start_Auto);
	else*/
		UpdateControls(0);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BEGIN_MESSAGE_MAP(CFramePage, CPropertyPage)
	//{{AFX_MSG_MAP(CFramePage)
	ON_EN_CHANGE(IDC_E_LEFT_MARGIN, OnChangeELeftMargin)
	ON_EN_CHANGE(IDC_E_RIGHT_MARGIN, OnChangeERightMargin)
	ON_EN_CHANGE(IDC_E_TOP_MARGIN, OnChangeETopMargin)
	ON_EN_CHANGE(IDC_E_BOTTOM_MARGIN, OnChangeEBottomMargin)
	ON_BN_CLICKED(IDC_PREVIEW_ALL_FRAME, OnPreviewAllFrame)
	ON_BN_CLICKED(IDC_ENABLE_FRAME, OnEnableFrame)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CFramePage::OnChangeELeftMargin()
{
	if (m_ELeftMargin.m_hWnd && !s_lock)
	{
		UpdateData();
		m_pDrv->ReinitPreview();
		m_pDrv->UpdateSizes();
	}
}

void CFramePage::OnChangeERightMargin() 
{
	if (m_ERightMargin.m_hWnd && !s_lock)
	{
		UpdateData();
		m_pDrv->ReinitPreview();
		m_pDrv->UpdateSizes();
	}
}

void CFramePage::OnChangeETopMargin() 
{
	if (m_ETopMargin.m_hWnd && !s_lock)
	{
		UpdateData();
		m_pDrv->ReinitPreview();
		m_pDrv->UpdateSizes();
	}
}

void CFramePage::OnChangeEBottomMargin() 
{
	if (m_EBottomMargin.m_hWnd && !s_lock)
	{
		UpdateData();
		m_pDrv->ReinitPreview();
		m_pDrv->UpdateSizes();
	}
}

void CFramePage::OnEnableFrame() 
{
	try {
	UpdateData();
	UpdateControls(0);
	m_pDrv->ReinitPreview();
	m_pDrv->UpdateSizes();
	SetModified();
	}
	catch(...) {}
}

BOOL CFramePage::OnApply() 
{
	m_pDrv->ApplyChanges();
	return CPropertyPage::OnApply();
}

void CFramePage::OnCancel() 
{
	m_pDrv->CancelChanges();
	m_pDrv->UpdateSizes();
	CPropertyPage::OnCancel();
}

void CFramePage::OnOK() 
{
	CPropertyPage::OnOK();
}

void CFramePage::OnPreviewAllFrame() 
{
	UpdateData();
	m_pDrv->UpdateSizes();
	SetModified();
}



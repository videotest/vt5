// SheetPages1.cpp : implementation file
//

#include "stdafx.h"
#include "baumer.h"
#include "SettingsPages1.h"
#include "SettingsSheet.h"
#include "cxlibwork.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Conversion function between:
// 1. Integer values at range 0..20
// 2. Double values at range 0.1..10.
const int g_nGammaMax = 10;
static int __PosByDValue(double dValue)
{
	int n = int(10.*log(dValue));
	return __Range(n, -g_nGammaMax, g_nGammaMax)+g_nGammaMax;
}

static double __DValueByPos(int nPos)
{
	double d = (nPos-g_nGammaMax)/10.;
	return pow(10., d);
}

static void __SetGamma(CSettingsSheet *p)
{
	EnterCriticalSection(&g_CritSectionCamera);
	g_CxLibWork.Gamma() = p->Gamma();
	g_CxLibWork.InitBitnessAndConversion(g_CxLibWork.m_nBitsMode, g_CxLibWork.m_nConversion, FALSE,
		g_CxLibWork.m_nConvStart, g_CxLibWork.m_nConvEnd, g_CxLibWork.m_dGamma,
		g_CxLibWork.m_nConvStart12,	g_CxLibWork.m_nConvEnd12, g_CxLibWork.m_dGamma);
	LeaveCriticalSection(&g_CritSectionCamera);
}

/////////////////////////////////////////////////////////////////////////////
// CManualPage property page

IMPLEMENT_DYNCREATE(CManualPage, CSettingsPage)

CManualPage::CManualPage() : CSettingsPage(CManualPage::IDD),
	m_Gamma(IDC_EDIT_GAMMA, IDC_SPIN_GAMMA, IDC_SLIDER_GAMMA)
{
	//{{AFX_DATA_INIT(CManualPage)
	//}}AFX_DATA_INIT
}

CManualPage::~CManualPage()
{
}

void CManualPage::DoDataExchange(CDataExchange* pDX)
{
	CSettingsPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CManualPage)
	DDX_Control(pDX, IDC_COMBO_BITS, m_Bits);
	DDX_Control(pDX, IDC_SLIDER_GAMMA, m_SliderGamma);
	DDX_Control(pDX, IDC_SPIN_GAMMA, m_SpinGamma);
	DDX_Control(pDX, IDC_SP_TOP, m_SpinTop);
	DDX_Control(pDX, IDC_SP_BOTTOM, m_SpinBottom);
	//}}AFX_DATA_MAP
	CSettingsSheet *p = GetSettingsParentSheet();
	if (g_CxLibWork.m_nBitsMode <= W10BIT)
	{
		DDX_Text(pDX, IDC_E_TOP, p->m_nConvStart);
		DDX_Text(pDX, IDC_E_BOTTOM, p->m_nConvEnd);
		DDX_Text(pDX, IDC_EDIT_GAMMA, p->m_dGamma);
	}
	else
	{
		DDX_Text(pDX, IDC_E_TOP, p->m_nConvStart12);
		DDX_Text(pDX, IDC_E_BOTTOM, p->m_nConvEnd12);
		DDX_Text(pDX, IDC_EDIT_GAMMA, p->m_dGamma12);
	}
	if (pDX->m_bSaveAndValidate)
	{
		if (p->m_nConvEnd < p->m_nConvStart)
			p->m_nConvEnd = p->m_nConvStart;
	}
}


void CManualPage::SetTopBottom()
{
	CSettingsSheet *p = GetSettingsParentSheet();
	if (g_CxLibWork.m_nBitsMode <= W10BIT)
	{
		m_SpinTop.SetRange(0, 1024);
		m_SpinBottom.SetRange(0, 1024);
		m_SpinTop.SetPos(p->m_nConvStart);
		m_SpinBottom.SetPos(p->m_nConvEnd);
	}
	else
	{
		m_SpinTop.SetRange(0, 4096);
		m_SpinBottom.SetRange(0, 4096);
		m_SpinTop.SetPos(p->m_nConvStart12);
		m_SpinBottom.SetPos(p->m_nConvEnd12);
	}
}

BEGIN_MESSAGE_MAP(CManualPage, CSettingsPage)
	//{{AFX_MSG_MAP(CManualPage)
	ON_EN_CHANGE(IDC_E_BOTTOM, OnChangeEBottom)
	ON_EN_CHANGE(IDC_E_TOP, OnChangeETop)
	ON_EN_CHANGE(IDC_EDIT_GAMMA, OnChangeEditGamma)
	ON_WM_HSCROLL()
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_GAMMA, OnDeltaposSpinGamma)
	ON_CBN_SELCHANGE(IDC_COMBO_BITS, OnSelchangeComboBits)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CManualPage message handlers

BOOL CManualPage::OnInitDialog() 
{
	CSettingsPage::OnInitDialog();
	SetTopBottom();
	m_Gamma.Init(this, NULL,this);
	m_Gamma.SetRange(10);
	InitComboByModes(&m_Bits, GetSettingsParentSheet()->m_nBitsMode);
	UpdateControls(ESH_BitMode);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CManualPage::OnOK() 
{
	GetSettingsParentSheet()->OnOK();	
	CPropertyPage::OnOK();
}


void CManualPage::OnCancel() 
{
	CSettingsSheet *p = GetSettingsParentSheet();
	p->OnCancel();
	if (p->m_pSettingsSite)
		p->m_pSettingsSite->OnChangeNaturalSize(p->m_bNaturalSize?true:false);
	CPropertyPage::OnCancel();
}

BOOL CManualPage::OnApply() 
{
	GetSettingsParentSheet()->OnApply();
	return CPropertyPage::OnApply();
}

void CManualPage::UpdateControls(int nHint)
{
	if (m_hWnd)
	{
		CSettingsSheet *p = GetSettingsParentSheet();
		BOOL bEnable = nHint!=ESH_Start_Auto;
		BOOL bEnableTB = BitsModeDescrByNum(p->m_nBitsMode)->nConversion == ManualConversion;
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_COMBO_BITS), bEnable);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_STATIC_TOP), bEnable&&bEnableTB);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_E_TOP), bEnable&&bEnableTB);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_SP_TOP), bEnable&&bEnableTB);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_STATIC_BOTTOM), bEnable&&bEnableTB);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_E_BOTTOM), bEnable&&bEnableTB);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_SP_BOTTOM), bEnable&&bEnableTB);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_STATIC_GAMMA), bEnable&&bEnableTB);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_EDIT_GAMMA), bEnable&&bEnableTB);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_SPIN_GAMMA), bEnable&&bEnableTB);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_SLIDER_GAMMA), bEnable&&bEnableTB);
		if (nHint == ESH_BitMode)
		{
			m_Gamma.SetPos(p->Gamma());
		}
	}
}

void CManualPage::OnChangeEBottom() 
{
	if (m_SpinTop.m_hWnd && m_SpinBottom.m_hWnd)
	{
		UpdateData();
		CSettingsSheet *p = GetSettingsParentSheet();
		EnterCriticalSection(&g_CritSectionCamera);
		if (g_CxLibWork.m_nBitsMode <= W10BIT)
			g_CxLibWork.m_nConvEnd = p->m_nConvEnd;
		else
			g_CxLibWork.m_nConvEnd12 = p->m_nConvEnd12;
		g_CxLibWork.InitBitnessAndConversion(g_CxLibWork.m_nBitsMode, g_CxLibWork.m_nConversion, FALSE,
			g_CxLibWork.m_nConvStart, g_CxLibWork.m_nConvEnd,g_CxLibWork.m_dGamma,
			g_CxLibWork.m_nConvStart12, g_CxLibWork.m_nConvEnd12, g_CxLibWork.m_dGamma12);
		LeaveCriticalSection(&g_CritSectionCamera);
	}
}

void CManualPage::OnChangeETop() 
{
	if (m_SpinTop.m_hWnd && m_SpinBottom.m_hWnd)
	{
		UpdateData();
		CSettingsSheet *p = GetSettingsParentSheet();
		EnterCriticalSection(&g_CritSectionCamera);
		if (g_CxLibWork.m_nBitsMode <= W10BIT)
			g_CxLibWork.m_nConvStart = p->m_nConvStart;
		else
			g_CxLibWork.m_nConvStart12 = p->m_nConvStart12;
		g_CxLibWork.InitBitnessAndConversion(g_CxLibWork.m_nBitsMode, g_CxLibWork.m_nConversion, FALSE,
			g_CxLibWork.m_nConvStart, g_CxLibWork.m_nConvEnd, g_CxLibWork.m_dGamma,
			g_CxLibWork.m_nConvStart12,	g_CxLibWork.m_nConvEnd12, g_CxLibWork.m_dGamma12);
		LeaveCriticalSection(&g_CritSectionCamera);
	}
}

void CManualPage::OnChangeEditGamma() 
{
	if (m_SpinTop.m_hWnd && m_SpinBottom.m_hWnd)
	{
		m_Gamma.OnEditChange();
		UpdateData();
	}
}

void CManualPage::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CSettingsSheet *p = GetSettingsParentSheet();
	if (pScrollBar == (CScrollBar*)&m_SliderGamma)
	{
		m_Gamma.OnHScroll();
		UpdateData();
	}
	CSettingsPage::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CManualPage::OnDeltaposSpinGamma(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	m_Gamma.OnSpinDelta(pNMUpDown->iDelta);
	UpdateData();
	*pResult = 0;
}

void CManualPage::OnChangeValue(CSmartIntEdit *pEdit, int nValue)
{
	CSettingsSheet *p = GetSettingsParentSheet();
	EnterCriticalSection(&g_CritSectionCamera);
	g_CxLibWork.Gamma() = p->Gamma();
	g_CxLibWork.InitBitnessAndConversion(g_CxLibWork.m_nBitsMode, g_CxLibWork.m_nConversion, FALSE,
		g_CxLibWork.m_nConvStart, g_CxLibWork.m_nConvEnd, g_CxLibWork.m_dGamma,
		g_CxLibWork.m_nConvStart12,	g_CxLibWork.m_nConvEnd12, g_CxLibWork.m_dGamma12);
	LeaveCriticalSection(&g_CritSectionCamera);
}

void CManualPage::OnSelchangeComboBits() 
{
	UpdateData();
	int n = m_Bits.GetCurSel();
	if (n < 0) return;
	CString s;
	m_Bits.GetLBText(n, s);
	BitsModeDescr *pBM = BitsModeDescrByName(s);
	if (pBM)
	{
		GetSettingsParentSheet()->m_nBitsMode = pBM->nNum;
		int nBitsMode = pBM->nBitsMode;
		if (Is12BitCamera(g_CxLibWork.m_nCamera) && nBitsMode == W10BIT)
			nBitsMode = W12BIT;
		EnterCriticalSection(&g_CritSectionCamera);
		g_CxLibWork.m_nBitsMode = nBitsMode;
		SetPixelWidth(pBM->nBitsMode);
		int p = g_CxLibWork.IsDC300()?4:3;
		SetScanMode(SM_SSMTC, &p);
		g_CxLibWork.m_bpp = GetImBpp();
		g_CxLibWork.m_nConversion = pBM->nConversion;
		g_CxLibWork.InitBitnessAndConversion(g_CxLibWork.m_nBitsMode,g_CxLibWork.m_nConversion,g_CxLibWork.m_bForCapture,
			g_CxLibWork.m_nConvStart,g_CxLibWork.m_nConvEnd,g_CxLibWork.m_dGamma,
			g_CxLibWork.m_nConvStart12,g_CxLibWork.m_nConvEnd12,g_CxLibWork.m_dGamma12);
		g_CxLibWork.ReinitSizes();
		GetCameraInfo(&g_CxLibWork.m_CameraType, sizeof(tCameraType), &g_CxLibWork.m_CameraStatus , sizeof(tCameraStatus));
		LeaveCriticalSection(&g_CritSectionCamera);
/*		if (g_CxLibWork.IsDC300()) // Strange gluck in ARC6000c : need to recall SetScanMode
		{
			Sleep(0);
			EnterCriticalSection(&g_CritSectionCamera);
				SetScanMode(SM_SSMTC, &p);
			LeaveCriticalSection(&g_CritSectionCamera);
		}*/
		SetTopBottom();
		GetSettingsParentSheet()->UpdateControls(ESH_BitMode);
		SetModified();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CTriggerDlg property page

IMPLEMENT_DYNCREATE(CTriggerDlg, CSettingsPage)

CTriggerDlg::CTriggerDlg() : CSettingsPage(CTriggerDlg::IDD)
{
	//{{AFX_DATA_INIT(CTriggerDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CTriggerDlg::~CTriggerDlg()
{
}

void CTriggerDlg::DoDataExchange(CDataExchange* pDX)
{
	CSettingsPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTriggerDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	CSettingsSheet *p = GetSettingsParentSheet();
	DDX_Check(pDX, IDC_TRIGGER_MODE, p->m_bTriggerMode);
	DDX_Check(pDX, IDC_DISCHARGE, p->m_bDischarge);
}


BEGIN_MESSAGE_MAP(CTriggerDlg, CSettingsPage)
	//{{AFX_MSG_MAP(CTriggerDlg)
	ON_BN_CLICKED(IDC_DISCHARGE, OnDischarge)
	ON_BN_CLICKED(IDC_TRIGGER_MODE, OnTriggerMode)
	ON_BN_CLICKED(IDC_SOFTWARE_TRIGGER, OnSoftwareTrigger)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTriggerDlg message handlers

void CTriggerDlg::OnDischarge() 
{
	UpdateData();
	CSettingsSheet *p = GetSettingsParentSheet();
	EnterCriticalSection(&g_CritSectionCamera);
	::SetTriggerMode(p->m_bTriggerMode, p->m_bDischarge);
	GetCameraInfo(&g_CxLibWork.m_CameraType, sizeof(tCameraType), &g_CxLibWork.m_CameraStatus , sizeof(tCameraStatus));
	LeaveCriticalSection(&g_CritSectionCamera);
	SetModified();
	UpdateControls();
}

void CTriggerDlg::OnTriggerMode() 
{
	UpdateData();
	CSettingsSheet *p = GetSettingsParentSheet();
	EnterCriticalSection(&g_CritSectionCamera);
	::SetTriggerMode(p->m_bTriggerMode, p->m_bDischarge);
	GetCameraInfo(&g_CxLibWork.m_CameraType, sizeof(tCameraType), &g_CxLibWork.m_CameraStatus , sizeof(tCameraStatus));
	LeaveCriticalSection(&g_CritSectionCamera);
	SetModified();
	p->UpdateControls(ESH_Trigger);
}

void CTriggerDlg::UpdateControls(int nHint)
{
	if (m_hWnd)
	{
		CSettingsSheet *p = GetSettingsParentSheet();
		BOOL bEnable = nHint!=ESH_Start_Auto && !p->m_bBinning && !g_CxLibWork.IsDC300();
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_TRIGGER_MODE), bEnable);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_DISCHARGE), bEnable&&p->m_bTriggerMode);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_SOFTWARE_TRIGGER), bEnable&&p->m_bTriggerMode);
	}
}


void CTriggerDlg::OnSoftwareTrigger() 
{
	EnterCriticalSection(&g_CritSectionCamera);
	::DoTrigger();
	LeaveCriticalSection(&g_CritSectionCamera);
}

BOOL CTriggerDlg::OnInitDialog() 
{
	CSettingsPage::OnInitDialog();
	
	UpdateControls(s_bSetup?ESH_Start_Auto:0);	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


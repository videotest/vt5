// SourceRGBPage.cpp : implementation file
//

#include "stdafx.h"
#include "IV460Drv.h"
#include "SourceRGBPage.h"
#include "iv4hlp.h"
#include "StdProfile.h"
#include "Lock.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int __Range(int n, int nMin, int nMax)
{
	if (n < nMin) return nMin;
	if (n > nMax) return nMax;
	return n;
}

void SetControls(CSliderCtrl *pSld, CSpinButtonCtrl *pSpin, BOOL bEnable, int nPos, int nMin, int nMax,
	BOOL bQuick = FALSE)
{
	if (pSld)
	{
		if (!bQuick) pSld->EnableWindow(bEnable);
		if (!bQuick) pSld->SetRange(nMin, nMax);
		if (!bQuick) pSld->SetTicFreq(max(1,(nMax-nMin)/10));
		pSld->SetPos(__Range(nPos, nMin, nMax));
	}
	if (pSpin)
	{
		if (!bQuick) pSpin->EnableWindow(bEnable);
		if (!bQuick) pSpin->SetRange(nMin, nMax);
		pSpin->SetPos(nPos);
	}
}

void SetDlgItemDouble(HWND hWnd, int nID, double dVal)
{
	CString s;
	s.Format("%.1f", dVal);
	::SetDlgItemText(hWnd, nID, s);
}

int __gaintoint(double dGain)
{
	return (int)(dGain*10.+30.);
}

double __inttogain(int nGain)
{
	return double(nGain-30)/10.;
}

static bool s_bInsideSpin;

/////////////////////////////////////////////////////////////////////////////
// CSourceRGBPage property page

IMPLEMENT_DYNCREATE(CSourceRGBPage, CSettingsPage)

CSourceRGBPage::CSourceRGBPage(IUnknown *punk) : CSettingsPage(CSourceRGBPage::IDD, punk)
{
	//{{AFX_DATA_INIT(CSourceRGBPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CSourceRGBPage::~CSourceRGBPage()
{
}

void CSourceRGBPage::DoDataExchange(CDataExchange* pDX)
{
	CSettingsPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSourceRGBPage)
	DDX_Control(pDX, IDC_SP_OFFSET_BLUE, m_SpOffsetBlue);
	DDX_Control(pDX, IDC_SP_GAIN_BLUE, m_SpGainBlue);
	DDX_Control(pDX, IDC_S_OFFSET_BLUE, m_SOffsetBlue);
	DDX_Control(pDX, IDC_S_GAIN_BLUE, m_SGainBlue);
	DDX_Control(pDX, IDC_SP_OFFSET_RED, m_SpOffsetRed);
	DDX_Control(pDX, IDC_SP_OFFSET_G, m_SpOffsetGreen);
	DDX_Control(pDX, IDC_SP_GAIN_RED, m_SpGainRed);
	DDX_Control(pDX, IDC_SP_GAIN_G, m_SpGainGreen);
	DDX_Control(pDX, IDC_S_OFFSET_RED, m_SOffsetRed);
	DDX_Control(pDX, IDC_S_OFFSET_G, m_SOffsetGreen);
	DDX_Control(pDX, IDC_S_GAIN_RED, m_SGainRed);
	DDX_Control(pDX, IDC_S_GAIN_GREEN, m_SGainGreen);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSourceRGBPage, CSettingsPage)
	//{{AFX_MSG_MAP(CSourceRGBPage)
	ON_BN_CLICKED(IDC_CHANNEL_RGB0, OnChannelRgb0)
	ON_BN_CLICKED(IDC_CHANNEL_RGB1, OnChannelRgb1)
	ON_EN_CHANGE(IDC_E_GAIN_G, OnChangeEGainG)
	ON_EN_CHANGE(IDC_E_GAIN_R, OnChangeEGainRb)
	ON_EN_CHANGE(IDC_E_OFFSET_G, OnChangeEOffsetG)
	ON_EN_CHANGE(IDC_E_OFFSET_R, OnChangeEOffsetRb)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SP_GAIN_G, OnDeltaposSpGainG)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SP_GAIN_RED, OnDeltaposSpGainRb)
	ON_WM_HSCROLL()
	ON_EN_CHANGE(IDC_E_GAIN_B, OnChangeEGainB)
	ON_EN_CHANGE(IDC_E_OFFSET_B, OnChangeEOffsetB)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SP_GAIN_BLUE, OnDeltaposSpGainBlue)
	ON_BN_CLICKED(IDC_DEFAULT, OnDefault)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSourceRGBPage message handlers

BOOL CSourceRGBPage::OnInitDialog() 
{
	CSettingsPage::OnInitDialog();
	
	CLock lock(&s_bInsideSpin);
	BOOL bUseTopBottom = m_pDrv.GetInt(0, _T("Settings"), _T("RGBUseTopBottom"), 1, ID2_BOARDIFDEFAULT);
	int nChannel = m_pDrv.GetInt(0, _T("Source"), _T("Channel"), 0, ID2_STORE);
	double dGainRed = m_pDrv.GetDouble(0, _T("Source"), _T("GainRed"), -3., ID2_STORE);
	double dGainGreen = m_pDrv.GetDouble(0, _T("Source"), _T("GainGreen"), -3., ID2_STORE);
	double dGainBlue = m_pDrv.GetDouble(0, _T("Source"), _T("GainBlue"), -3., ID2_STORE);
	int nOffsetRed = m_pDrv.GetInt(0, _T("Source"), _T("OffsetRed"), 0, ID2_STORE);
	int nOffsetGreen = m_pDrv.GetInt(0, _T("Source"), _T("OffsetGreen"), 0, ID2_STORE);
	int nOffsetBlue = m_pDrv.GetInt(0, _T("Source"), _T("OffsetBlue"), 0, ID2_STORE);
	CheckRadioButton(IDC_CHANNEL_RGB0, IDC_CHANNEL_RGB1, IDC_CHANNEL_RGB0+nChannel-IV4_Channel_RGB_0);
	SetControls(&m_SGainRed, &m_SpGainRed, bUseTopBottom, __gaintoint(dGainRed), 0, 210);
	SetControls(&m_SGainGreen, &m_SpGainGreen, bUseTopBottom, __gaintoint(dGainGreen), 0, 210);
	SetControls(&m_SGainBlue, &m_SpGainBlue, bUseTopBottom, __gaintoint(dGainBlue), 0, 210);
	SetControls(&m_SOffsetRed, &m_SpOffsetRed, bUseTopBottom, nOffsetRed, 0, 255);
	SetControls(&m_SOffsetGreen, &m_SpOffsetGreen, bUseTopBottom, nOffsetGreen, 0, 255);
	SetControls(&m_SOffsetBlue, &m_SpOffsetBlue, bUseTopBottom, nOffsetBlue, 0, 255);
	SetDlgItemDouble(m_hWnd, IDC_E_GAIN_R, dGainRed);
	SetDlgItemDouble(m_hWnd, IDC_E_GAIN_G, dGainGreen);
	SetDlgItemDouble(m_hWnd, IDC_E_GAIN_B, dGainBlue);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_E_GAIN_R), bUseTopBottom);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_E_GAIN_G), bUseTopBottom);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_E_GAIN_B), bUseTopBottom);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_E_OFFSET_R), bUseTopBottom);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_E_OFFSET_G), bUseTopBottom);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_E_OFFSET_B), bUseTopBottom);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_STATIC_GAIN_R), bUseTopBottom);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_STATIC_GAIN_G), bUseTopBottom);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_STATIC_GAIN_B), bUseTopBottom);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_STATIC_OFFSET_R), bUseTopBottom);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_STATIC_OFFSET_G), bUseTopBottom);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_STATIC_OFFSET_B), bUseTopBottom);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_STATIC_GAIN_OFFSET), bUseTopBottom);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_DEFAULT), bUseTopBottom);
	/*CString s;
	s.Format(_T("%.1f"), dGainRed);
	SetDlgItemText(IDC_E_GAIN_R, s);
	s.Format(_T("%.1f"), dGainGreen);
	SetDlgItemText(IDC_E_GAIN_G, s);
	s.Format(_T("%.1f"), dGainBlue);
	SetDlgItemText(IDC_E_GAIN_B, s);*/
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSourceRGBPage::OnChannelRgb0() 
{
	m_pDrv.SetInt(0, _T("Source"), _T("Channel"), IV4_Channel_RGB_0, 0);
	m_pDrv->ReinitPreview();
	SetModified();
}

void CSourceRGBPage::OnChannelRgb1() 
{
	m_pDrv.SetInt(0, _T("Source"), _T("Channel"), IV4_Channel_RGB_1, 0);
	m_pDrv->ReinitPreview();
	SetModified();
}

void CSourceRGBPage::OnChangeEGainG() 
{
	if (s_bInsideSpin) return;
	if (!m_hWnd || !m_SOffsetRed.m_hWnd) return;
	CLock lock(&s_bInsideSpin);
	CString s;
	GetDlgItemText(IDC_E_GAIN_G, s);
	double d = atof(s);
	m_SpGainGreen.SetPos(__gaintoint(d));
	m_SGainGreen.SetPos(__gaintoint(d));
	m_pDrv.SetDouble(0, _T("Source"), _T("GainGreen"), d, 0);
	m_pDrv->ReinitPreview();
	SetModified();
}

void CSourceRGBPage::OnChangeEGainRb() 
{
	if (s_bInsideSpin) return;
	if (!m_hWnd || !m_SOffsetRed.m_hWnd) return;
	CLock lock(&s_bInsideSpin);
	CString s;
	GetDlgItemText(IDC_E_GAIN_R, s);
	double d = atof(s);
	m_SpGainRed.SetPos(__gaintoint(d));
	m_SGainRed.SetPos(__gaintoint(d));
	m_pDrv.SetDouble(0, _T("Source"), _T("GainRed"), d, 0);
	m_pDrv->ReinitPreview();
	SetModified();
}

void CSourceRGBPage::OnChangeEGainB() 
{
	if (s_bInsideSpin) return;
	if (!m_hWnd || !m_SOffsetRed.m_hWnd) return;
	CLock lock(&s_bInsideSpin);
	CString s;
	GetDlgItemText(IDC_E_GAIN_B, s);
	double d = atof(s);
	m_SpGainBlue.SetPos(__gaintoint(d));
	m_SGainBlue.SetPos(__gaintoint(d));
	m_pDrv.SetDouble(0, _T("Source"), _T("GainBlue"), d, 0);
	m_pDrv->ReinitPreview();
	SetModified();
}


void CSourceRGBPage::OnChangeEOffsetG() 
{
	if (s_bInsideSpin) return;
	if (!m_hWnd || !m_SOffsetRed.m_hWnd) return;
	CLock lock(&s_bInsideSpin);
	int n = GetDlgItemInt(IDC_E_OFFSET_G);
	m_SOffsetGreen.SetPos(n);
	m_pDrv.SetInt(0, _T("Source"), _T("OffsetGreen"), n, 0);
	m_pDrv->ReinitPreview();
	SetModified();
}

void CSourceRGBPage::OnChangeEOffsetRb() 
{
	if (s_bInsideSpin) return;
	if (!m_hWnd || !m_SOffsetRed.m_hWnd) return;
	CLock lock(&s_bInsideSpin);
	int n = GetDlgItemInt(IDC_E_OFFSET_R);
	m_SOffsetRed.SetPos(n);
	m_pDrv.SetInt(0, _T("Source"), _T("OffsetRed"), n, 0);
	m_pDrv->ReinitPreview();
	SetModified();
}

void CSourceRGBPage::OnChangeEOffsetB() 
{
	if (s_bInsideSpin) return;
	if (!m_hWnd || !m_SOffsetBlue.m_hWnd) return;
	CLock lock(&s_bInsideSpin);
	int n = GetDlgItemInt(IDC_E_OFFSET_B);
	m_SOffsetBlue.SetPos(n);
	m_pDrv.SetInt(0, _T("Source"), _T("OffsetBlue"), n, 0);
	m_pDrv->ReinitPreview();
	SetModified();
}

void CSourceRGBPage::OnDeltaposSpGainG(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (!s_bInsideSpin)
	{
		CLock lock(&s_bInsideSpin);
		NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
		CString s;
		GetDlgItemText(IDC_E_GAIN_G, s);
		double d = atof(s);
		int nPos = __gaintoint(d)+pNMUpDown->iDelta;
		d = __inttogain(nPos);
		s.Format(_T("%.1f"), d);
		SetDlgItemText(IDC_E_GAIN_G, s);
		m_SGainGreen.SetPos(nPos);
		m_pDrv.SetDouble(0, _T("Source"), _T("GainGreen"), d, 0);
		m_pDrv->ReinitPreview();
		SetModified();
	}

	*pResult = 0;
}

void CSourceRGBPage::OnDeltaposSpGainRb(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (!s_bInsideSpin)
	{
		CLock lock(&s_bInsideSpin);
		NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
		CString s;
		GetDlgItemText(IDC_E_GAIN_R, s);
		double d = atof(s);
		int nPos = __gaintoint(d)+pNMUpDown->iDelta;
		d = __inttogain(nPos);
		s.Format(_T("%.1f"), d);
		SetDlgItemText(IDC_E_GAIN_R, s);
		m_SGainRed.SetPos(nPos);
		m_pDrv.SetDouble(0, _T("Source"), _T("GainRed"), d, 0);
		m_pDrv->ReinitPreview();
		SetModified();
	}
	*pResult = 0;
}

void CSourceRGBPage::OnDeltaposSpGainBlue(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (!s_bInsideSpin)
	{
		CLock lock(&s_bInsideSpin);
		NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
		CString s;
		GetDlgItemText(IDC_E_GAIN_B, s);
		double d = atof(s);
		int nPos = __gaintoint(d)+pNMUpDown->iDelta;
		d = __inttogain(nPos);
		s.Format(_T("%.1f"), d);
		SetDlgItemText(IDC_E_GAIN_B, s);
		m_SGainBlue.SetPos(nPos);
		m_pDrv.SetDouble(0, _T("Source"), _T("GainBlue"), d, 0);
		m_pDrv->ReinitPreview();
		SetModified();
	}
	*pResult = 0;
}


void CSourceRGBPage::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if (s_bInsideSpin) return;
	CLock lock(&s_bInsideSpin);
	CString s;
	if (pScrollBar == (CScrollBar*)&m_SGainRed)
	{
		int nPos = m_SGainRed.GetPos();
		double dGainRB = __inttogain(nPos);
		s.Format("%.1f", dGainRB); 
		SetDlgItemText(IDC_E_GAIN_R, s);
		m_SpGainRed.SetPos(nPos);
		m_pDrv.SetDouble(0, _T("Source"), _T("GainRed"), dGainRB, 0);
		m_pDrv->ReinitPreview();
	}
	else if (pScrollBar == (CScrollBar*)&m_SGainGreen)
	{
		int nPos = m_SGainGreen.GetPos();
		double dGainG = __inttogain(nPos);
		s.Format("%.1f", dGainG); 
		SetDlgItemText(IDC_E_GAIN_G, s);
		m_SpGainGreen.SetPos(nPos);
		m_pDrv.SetDouble(0, _T("Source"), _T("GainGreen"), dGainG, 0);
		m_pDrv->ReinitPreview();
	}
	else if (pScrollBar == (CScrollBar*)&m_SGainBlue)
	{
		int nPos = m_SGainBlue.GetPos();
		double dGainB = __inttogain(nPos);
		s.Format("%.1f", dGainB); 
		SetDlgItemText(IDC_E_GAIN_B, s);
		m_SpGainBlue.SetPos(nPos);
		m_pDrv.SetDouble(0, _T("Source"), _T("GainBlue"), dGainB, 0);
		m_pDrv->ReinitPreview();
	}
	else if (pScrollBar == (CScrollBar*)&m_SOffsetRed)
	{
		int nPos = m_SOffsetRed.GetPos();
		m_SpOffsetRed.SetPos(nPos);
		m_pDrv.SetInt(0, _T("Source"), _T("OffsetRed"), nPos, 0);
		m_pDrv->ReinitPreview();
	}
	else if (pScrollBar == (CScrollBar*)&m_SOffsetGreen)
	{
		int nPos = m_SOffsetGreen.GetPos();
		m_SpOffsetGreen.SetPos(nPos);
		m_pDrv.SetInt(0, _T("Source"), _T("OffsetGreen"), nPos, 0);
		m_pDrv->ReinitPreview();
	}
	else if (pScrollBar == (CScrollBar*)&m_SOffsetBlue)
	{
		int nPos = m_SOffsetBlue.GetPos();
		m_SpOffsetBlue.SetPos(nPos);
		m_pDrv.SetInt(0, _T("Source"), _T("OffsetBlue"), nPos, 0);
		m_pDrv->ReinitPreview();
	}
	SetModified();

	
	CSettingsPage::OnHScroll(nSBCode, nPos, pScrollBar);
}

BOOL CSourceRGBPage::OnApply() 
{
	m_pDrv->ApplyChanges();
	return CSettingsPage::OnApply();
}

void CSourceRGBPage::OnCancel() 
{
	m_pDrv->CancelChanges();
	m_pDrv->UpdateSizes();
	CSettingsPage::OnCancel();
}

void CSourceRGBPage::OnOK() 
{
	CSettingsPage::OnOK();
}

void CSourceRGBPage::OnDefault() 
{
	// 1. Gain
	// Red
	double dGainRed = g_StdProf.GetDefaultProfileDouble(_T("Source"), _T("GainRed"), 0.);
	SetControls(&m_SGainRed, &m_SpGainRed, TRUE, __gaintoint(dGainRed), 0, 210, TRUE);
	SetDlgItemDouble(m_hWnd, IDC_E_GAIN_R, dGainRed);
	m_pDrv.SetDouble(0, _T("Source"), _T("GainRed"), dGainRed, 0);
	// Green
	double dGainGreen = g_StdProf.GetDefaultProfileDouble(_T("Source"), _T("GainGreen"), 0.);
	SetControls(&m_SGainGreen, &m_SpGainGreen, TRUE, __gaintoint(dGainGreen), 0, 210, TRUE);
	SetDlgItemDouble(m_hWnd, IDC_E_GAIN_G, dGainGreen);
	m_pDrv.SetDouble(0, _T("Source"), _T("GainGreen"), dGainGreen, 0);
	// Blue
	double dGainBlue = g_StdProf.GetDefaultProfileDouble(_T("Source"), _T("GainBlue"), 0.);
	SetControls(&m_SGainBlue, &m_SpGainBlue, TRUE, __gaintoint(dGainBlue), 0, 210, TRUE);
	SetDlgItemDouble(m_hWnd, IDC_E_GAIN_B, dGainBlue);
	m_pDrv.SetDouble(0, _T("Source"), _T("GainBlue"), dGainBlue, 0);
	// 2. Offset
	int nOffsetRed = g_StdProf.GetDefaultProfileInt(_T("Source"), _T("OffsetRed"), 128);
	SetControls(&m_SOffsetRed, &m_SpOffsetRed, TRUE, nOffsetRed, 0, 255, TRUE);
	int nOffsetGreen = g_StdProf.GetDefaultProfileInt(_T("Source"), _T("OffsetGreen"), 128);
	SetControls(&m_SOffsetGreen, &m_SpOffsetGreen, TRUE, nOffsetGreen, 0, 255, TRUE);
	int nOffsetBlue = g_StdProf.GetDefaultProfileInt(_T("Source"), _T("OffsetBlue"), 128);
	SetControls(&m_SOffsetBlue, &m_SpOffsetBlue, TRUE, nOffsetBlue, 0, 255, TRUE);
}



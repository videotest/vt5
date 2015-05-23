// SettingsPages.cpp : implementation file
//

#include "stdafx.h"
#include "Baumer.h"
#include "SettingsSheet.h"
#include "SettingsPages.h"
#include "CxLibWork.h"
#include "ImLibApi.h"
#include "BaumUtils.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CSettingsPage, CPropertyPage);
IMPLEMENT_DYNCREATE(CAmplPage, CSettingsPage)
IMPLEMENT_DYNCREATE(CCameraPage, CSettingsPage)
IMPLEMENT_DYNCREATE(CWhitePage, CSettingsPage)
IMPLEMENT_DYNCREATE(CColorPage, CSettingsPage)
IMPLEMENT_DYNCREATE(CFramePage, CSettingsPage)

static void __Validate(CDataExchange* pDX, HWND hwnd, int nIDC, double &dValue, double dMin, double dMax)
{
	char szBuff[200];
	if (pDX->m_bSaveAndValidate)
	{
		if (dValue < dMin)
		{
			dValue = dMin;
			sprintf(szBuff, "%.2g", dValue);
			::SetDlgItemText(hwnd, nIDC, szBuff);
		}
		else if (dValue > dMax)
		{
			dValue = dMax;
			sprintf(szBuff, "%.2g", dValue);
			::SetDlgItemText(hwnd, nIDC, szBuff);
		}
	}
}

static void __Validate(CDataExchange* pDX, HWND hwnd, int nIDC, int &nValue, int nMin, int nMax)
{
	char szBuff[200];
	if (pDX->m_bSaveAndValidate)
	{
		if (nValue < nMin)
		{
			nValue = nMin;
			_itoa(nMin, szBuff, 10);
			::SetDlgItemText(hwnd, nIDC, szBuff);
		}
		else if (nValue > nMax)
		{
			nValue = nMax;
			_itoa(nMin, szBuff, 10);
			::SetDlgItemText(hwnd, nIDC, szBuff);
		}
	}
}

static int __FactorByUnit(LPCTSTR lpUnit)
{
	return _tcsicmp(lpUnit, _T("s"))==0?1000000:_tcsicmp(lpUnit,_T("ms"))==0?1000:1;
}

static void DDX_Text_Shutter(CDataExchange* pDX, int nIDC, int &nMcs)
{
	HWND hwnd = pDX->m_pDlgWnd->GetSafeHwnd();
	if (pDX->m_bSaveAndValidate)
	{
		TCHAR szBuff[20];
		GetDlgItemText(hwnd, nIDC, szBuff, 20);
		int nVal;
		TCHAR szUnit[20];
		_stscanf(szBuff, "%d%s", &nVal, szUnit);
		nVal = nVal*__FactorByUnit(szUnit);
		nMcs = Range(nVal,g_CxLibWork.m_sht.timeMin,g_CxLibWork.m_sht.timeMax);
	}
	else
	{
		CString s(_T("mks"));
		int nVal = nMcs;
		if (nVal >= 1000 && nVal/1000*1000 == nVal)
		{
			if (nVal >= 1000000 && nVal/1000000*1000000 == nVal)
			{
				nVal /= 1000000;
				s = _T("s");
			}
			else
			{
				nVal /= 1000;
				s = _T("ms");
			}
		}
		CString s1;
		s1.Format("%d%s", nVal, (LPCTSTR)s);
		SetDlgItemText(hwnd, nIDC, s1);
	}
}


CSettingsSheet *CSettingsPage::GetSettingsParentSheet()
{
	return m_pSheet;
}

void CSettingsPage::SetModified(BOOL bChanged)
{
	GetSettingsParentSheet()->m_bModified = bChanged;
	CPropertyPage::SetModified(bChanged);
}

static bool s_bInsideHScroll = false;
static bool s_bInsideSpin = false;

void SetExpSlider(CExponentSlider &Sld, CSpinButtonCtrl &Spin, BOOL bEnable, int nPos, int nMin, int nMax)
{
	Sld.EnableWindow(bEnable);
	Sld.SetRange(nMin, nMax);
	Sld.SetTicFreq(16);
	Sld.SetPos(__Range(nPos, nMin, nMax));
	Spin.EnableWindow(bEnable);
	Spin.SetRange(0, 100);
	Spin.SetPos(0);
}

/////////////////////////////////////////////////////////////////////////////
// CAmplPage property page

CAmplPage::CAmplPage() : CSettingsPage(CAmplPage::IDD)
{
	//{{AFX_DATA_INIT(CAmplPage)
	//}}AFX_DATA_INIT
	m_bHardwareSetup = FALSE;
}

CAmplPage::~CAmplPage()
{
}

void CAmplPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAmplPage)
	DDX_Control(pDX, IDC_SHUTTER_AUTO, m_ShutterAuto);
	DDX_Control(pDX, IDC_SP_SHUTTER, m_SpShutter);
	DDX_Control(pDX, IDC_SP_OFFSET, m_SpOffset);
	DDX_Control(pDX, IDC_SP_GAIN, m_SpGain);
	DDX_Control(pDX, IDC_SP_PROC_WHITE, m_SpProcWhite);
	DDX_Control(pDX, IDC_S_PROC_WHITE, m_SProcWhite);
	DDX_Control(pDX, IDC_E_PROC_WHITE, m_EProcWhite);
	DDX_Control(pDX, IDC_AUTO_SETUP, m_Auto);
	DDX_Control(pDX, IDC_STATIC_SHUTTER, m_StaticShutter);
	DDX_Control(pDX, IDC_S_SHUTTER, m_SShutter);
	DDX_Control(pDX, IDC_E_SHUTTER, m_EShutter);
	DDX_Control(pDX, IDC_S_OFFSET, m_SOffset);
	DDX_Control(pDX, IDC_E_OFFSET, m_EOffset);
	DDX_Control(pDX, IDC_S_GAIN, m_SGain);
	DDX_Control(pDX, IDC_E_GAIN, m_EGain);
	DDX_Control(pDX, IDC_PROGRESS_AUTO, m_ProgressAuto);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_E_GAIN, GetSettingsParentSheet()->m_nGain);
	DDX_Text(pDX, IDC_E_OFFSET, GetSettingsParentSheet()->m_nOffset);
	DDX_Text_Shutter(pDX, IDC_E_SHUTTER, GetSettingsParentSheet()->m_nShutter);
	__Validate(pDX, m_hWnd, IDC_E_SHUTTER, GetSettingsParentSheet()->m_nShutter, g_CxLibWork.m_sht.timeMin,
		g_CxLibWork.m_sht.timeMax); 
	DDX_Text(pDX, IDC_E_PROC_WHITE, GetSettingsParentSheet()->m_nProcWhite);
	DDX_Check(pDX, IDC_SHUTTER_AUTO, GetSettingsParentSheet()->m_bAutoShutter);
}


BEGIN_MESSAGE_MAP(CAmplPage, CPropertyPage)
	//{{AFX_MSG_MAP(CAmplPage)
	ON_BN_CLICKED(IDC_AUTO_SETUP, OnAutoSetup)
	ON_EN_CHANGE(IDC_E_GAIN, OnChangeEGain)
	ON_EN_CHANGE(IDC_E_OFFSET, OnChangeEOffset)
	ON_EN_CHANGE(IDC_E_SHUTTER, OnChangeEShutter)
	ON_WM_HSCROLL()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_EN_CHANGE(IDC_E_PROC_WHITE, OnChangeEProcWhite)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SP_SHUTTER, OnDeltaposSpShutter)
	ON_BN_CLICKED(IDC_SHUTTER_AUTO, OnShutterAuto)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CAmplPage::OnAutoSetup() 
{
	if (g_BaumerProfile.GetProfileInt(_T("Settings"), _T("HardwareFindShutter"), FALSE, true))
	{
		RECT r; r.left = r.top = r.right = r.bottom = 0;
		int nBrightness = g_BaumerProfile.GetProfileInt(_T("Settings"), _T("FindShutterBrightness"), 220, true);
		int nPercent = GetSettingsParentSheet()->m_nProcWhite;
		if (FindShutter(nBrightness, nPercent, r))
		{
			DWORD dwDelay = g_BaumerProfile.GetProfileInt(_T("Settings"), _T("FindShutterDelay"), 3000, true);
			m_bHardwareSetup = TRUE;
			SetTimer(1, dwDelay, NULL);
			GetSettingsParentSheet()->UpdateControls(ESH_Start_Auto);
		}
	}
	else
	{
		s_bSetup = !s_bSetup;
		if (s_bSetup)
		{
			CString s;
			s.LoadString(IDS_STOP);
			m_Auto.SetWindowText(s);
			m_ProgressAuto.ShowWindow(SW_SHOW);
			m_ProgressAuto.SetPos(0);
			s_hwndProgress = m_ProgressAuto.m_hWnd;
			GetSettingsParentSheet()->m_bFreezeGraffic = TRUE;
			if (GetSettingsParentSheet()->m_bBinning)
			{
				SetBinningMode(FALSE);
				Sleep(300);
				int p1 = 3;
				SetScanMode(SM_SSMTC, &p1);
				g_CxLibWork.m_cx = g_CxLibWork.m_lpbi->biWidth = GetImFrameX();
				g_CxLibWork.m_cy = g_CxLibWork.m_lpbi->biHeight = GetImFrameY();
				g_CxLibWork.m_bpp = GetImBpp();
				g_CxLibWork.SetWBalance(GetSettingsParentSheet()->m_bWBEnable, GetSettingsParentSheet()->m_dWBRed,
					GetSettingsParentSheet()->m_dWBGreen, GetSettingsParentSheet()->m_dWBBlue);
				/*if (GetSettingsParentSheet()->m_bWBEnableIm)
					ImRgbLut(GetSettingsParentSheet()->m_dWBRedIm, GetSettingsParentSheet()->m_dWBGreenIm,
					GetSettingsParentSheet()->m_dWBBlueIm, LUTALL);
				else
					ImRgbLut(1., 1., 1., LUTALL);*/
				SetRgbLut(GetSettingsParentSheet()->m_bCBEnable, 1, &GetSettingsParentSheet()->m_dCBRed, &GetSettingsParentSheet()->m_dCBGreen,
					&GetSettingsParentSheet()->m_dCBBlue);
			}
			GetSettingsParentSheet()->UpdateControls(ESH_Start_Auto);
		}
		SetModified();
	}
}

void CAmplPage::OnChangeEGain() 
{
	if (m_EGain.m_hWnd)
	{
		UpdateData();
		EnterCriticalSection(&g_CritSectionCamera);
		SetGain(GetSettingsParentSheet()->m_nGain);
		LeaveCriticalSection(&g_CritSectionCamera);
		if (!s_bInsideHScroll) m_SGain.SetPos(GetSettingsParentSheet()->m_nGain);
		SetModified();
	}
}

void CAmplPage::OnChangeEOffset() 
{
	if (m_EOffset.m_hWnd)
	{
		UpdateData();
		EnterCriticalSection(&g_CritSectionCamera);
		SetBlackLevel(GetSettingsParentSheet()->m_nOffset);
		LeaveCriticalSection(&g_CritSectionCamera);
		if (!s_bInsideHScroll) m_SOffset.SetPos(GetSettingsParentSheet()->m_nOffset);
		SetModified();
	}
}

void CAmplPage::OnChangeEShutter() 
{
	if (m_EShutter.m_hWnd)
	{
		UpdateData();
		EnterCriticalSection(&g_CritSectionCamera);
		CSettingsSheet *p = GetSettingsParentSheet();
		SetShutterTime(p->m_nShutter, NULL);
		LeaveCriticalSection(&g_CritSectionCamera);
		if (!s_bInsideHScroll) m_SShutter.SetPos(p->m_nShutter);
		SetModified();
	}
}

void CAmplPage::OnChangeEProcWhite() 
{
	if (m_EProcWhite.m_hWnd)
	{
		UpdateData();
		g_CxLibWork.m_nProcWhite = GetSettingsParentSheet()->m_nProcWhite;
		if (!s_bInsideHScroll) m_SProcWhite.SetPos(g_CxLibWork.m_nProcWhite);
		SetModified();
	}
}

void CAmplPage::OnShutterAuto() 
{
	UpdateData();
	CSettingsSheet *p = GetSettingsParentSheet();
	int nAutoBrightness = g_BaumerProfile.GetProfileInt(_T("Settings"), _T("AutoShutterBrightness"), 200);
	EnterCriticalSection(&g_CritSectionCamera);
	AutomaticShutter(p->m_bAutoShutter, nAutoBrightness);
	LeaveCriticalSection(&g_CritSectionCamera);
	if (!p->m_bAutoShutter)
	{
		if (Arc6000Gluck2())
		{
			tSHT sht;
			Sleep(200);
			EnterCriticalSection(&g_CritSectionCamera);
			SetShutterTime(p->m_nShutter, &sht);
			SetGain(GetSettingsParentSheet()->m_nGain);
			LeaveCriticalSection(&g_CritSectionCamera);
		}
		else
		{
			p->m_nShutter = g_CxLibWork.GetShutterTime();
			UpdateData(FALSE);
			m_SShutter.SetPos(p->m_nShutter);
			tSHT sht;
			EnterCriticalSection(&g_CritSectionCamera);
			SetShutterTime(p->m_nShutter, &sht);
			LeaveCriticalSection(&g_CritSectionCamera);
		}
	}
	m_SpShutter.EnableWindow(!p->m_bAutoShutter);
	m_SShutter.EnableWindow(!p->m_bAutoShutter);
	m_EShutter.EnableWindow(!p->m_bAutoShutter);
	m_EProcWhite.EnableWindow(!p->m_bAutoShutter);
	m_SProcWhite.EnableWindow(!p->m_bAutoShutter);
	m_SpProcWhite.EnableWindow(!p->m_bAutoShutter);
	m_Auto.EnableWindow(!p->m_bAutoShutter);
}

void CAmplPage::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	UpdateData();
	CSettingsSheet *p = GetSettingsParentSheet();
	CLock lock(&s_bInsideHScroll);
	if (pScrollBar == (CScrollBar*)&m_SGain)
	{
		p->m_nGain = m_SGain.GetPos();
		EnterCriticalSection(&g_CritSectionCamera);
		SetGain(p->m_nGain);
		LeaveCriticalSection(&g_CritSectionCamera);
	}
	else if (pScrollBar == (CScrollBar*)&m_SOffset)
	{
		p->m_nOffset = m_SOffset.GetPos();
		EnterCriticalSection(&g_CritSectionCamera);
		SetBlackLevel(p->m_nOffset);
		LeaveCriticalSection(&g_CritSectionCamera);
	}
	else if (pScrollBar == (CScrollBar*)&m_SShutter)
	{
		p->m_nShutter = m_SShutter.GetPos();
		tSHT sht;
		SetShutterTime(p->m_nShutter, &sht);
	}
	else if (pScrollBar == (CScrollBar*)&m_SProcWhite)
	{
		g_CxLibWork.m_nProcWhite = p->m_nProcWhite = m_SProcWhite.GetPos();
	}
	UpdateData(FALSE);
	SetModified();
	CPropertyPage::OnHScroll(nSBCode, nPos, pScrollBar);
}

BOOL CAmplPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	CSettingsSheet *p = GetSettingsParentSheet();
	m_Auto.GetWindowText(m_sAutoSetup);
	m_ProgressAuto.SetRange(0, 10);
	m_ProgressAuto.SetPos(0);
	int nCamera = CameraIds::CameraDescrByInternalId(p->m_nCamera)->nBaumerId;
	SetSlider(m_SGain, m_SpGain, GainEnabled(nCamera), p->m_nGain, g_CxLibWork.m_CameraType.vAmplMin,
		g_CxLibWork.m_CameraType.vAmplMax);
	m_EGain.EnableWindow(GainEnabled(nCamera));
	SetSlider(m_SOffset, m_SpOffset, TRUE, p->m_nOffset,  g_CxLibWork.m_CameraType.vOffsetMin, g_CxLibWork.m_CameraType.vOffsetMax);
	SetExpSlider(m_SShutter, m_SpShutter, !p->m_bAutoShutter, p->m_nShutter,  g_CxLibWork.m_sht.timeMin, g_CxLibWork.m_sht.timeMax);
	m_EShutter.EnableWindow(!p->m_bAutoShutter);
	SetSlider(m_SProcWhite, m_SpProcWhite, TRUE, GetSettingsParentSheet()->m_nProcWhite, 0, 100);
	m_EProcWhite.EnableWindow(!p->m_bAutoShutter);
	m_SProcWhite.EnableWindow(!p->m_bAutoShutter);
	m_SpProcWhite.EnableWindow(!p->m_bAutoShutter);
	UpdateData(FALSE);
	SetTimer(1, 200, NULL);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAmplPage::OnDeltaposSpShutter(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	CLock lock(&s_bInsideSpin);
	UpdateData();
	CSettingsSheet *p = GetSettingsParentSheet();
	p->m_nShutter = m_SShutter.NextValue(p->m_nShutter, pNMUpDown->iDelta);
	UpdateData(FALSE);
	m_SShutter.SetPos(p->m_nShutter);
	EnterCriticalSection(&g_CritSectionCamera);
	tSHT sht;
	SetShutterTime(p->m_nShutter, &sht);
	LeaveCriticalSection(&g_CritSectionCamera);
	*pResult = 0;
}


void CAmplPage::OnTimer(UINT_PTR nIDEvent) 
{
	if (m_bHardwareSetup)
	{
		m_bHardwareSetup = FALSE;
		KillTimer(1);
		CSettingsSheet *p = GetSettingsParentSheet();
		int nShutter = g_CxLibWork.GetShutterTime();
		UpdateData();
		p->m_nShutter = nShutter;
		UpdateData(FALSE);
		m_SShutter.SetPos(p->m_nShutter);
		GetSettingsParentSheet()->UpdateControls(ESH_Stop_Auto);
		SetModified();
	}
	else if (s_bSetupComplete)
	{
		s_bSetup = s_bSetupComplete = FALSE;
		EnterCriticalSection(&g_CritSectionCamera);
		GetCameraInfo(&g_CxLibWork.m_CameraType, sizeof(tCameraType), &g_CxLibWork.m_CameraStatus , sizeof(tCameraStatus));
		if (GetSettingsParentSheet()->m_bBinning)
		{
			SetBinningMode(TRUE);
			SetWhiteBalance(FALSE, FALSE, NULL, NULL, NULL);
			SetRgbLut(FALSE, 0, NULL, NULL, NULL);
		}
		LeaveCriticalSection(&g_CritSectionCamera);
		UpdateData();
		GetSettingsParentSheet()->m_nGain = g_CxLibWork.m_CameraStatus.vAmplif;
		GetSettingsParentSheet()->m_nOffset = g_CxLibWork.m_CameraStatus.vOffset;
		GetSettingsParentSheet()->m_nShutter = g_CxLibWork.m_nFoundShutter;
		m_SGain.SetPos(GetSettingsParentSheet()->m_nGain);
		m_SOffset.SetPos(GetSettingsParentSheet()->m_nOffset);
		m_SShutter.SetPos(GetSettingsParentSheet()->m_nShutter);
		UpdateData(FALSE);
		m_Auto.SetWindowText(m_sAutoSetup);
		m_ProgressAuto.ShowWindow(SW_HIDE);
		m_ProgressAuto.SetPos(0);
		s_hwndProgress = NULL;
		GetSettingsParentSheet()->m_bFreezeGraffic = FALSE;
		GetSettingsParentSheet()->UpdateControls(ESH_Stop_Auto);
	}

	CPropertyPage::OnTimer(nIDEvent);
}

void CAmplPage::OnDestroy() 
{
	CPropertyPage::OnDestroy();
	s_hwndProgress = NULL;
	KillTimer(1);
	if (s_bSetup || s_bSetupComplete)
		s_bSetup = s_bSetupComplete = FALSE;
}

void CAmplPage::OnOK() 
{
	GetSettingsParentSheet()->OnOK();
	CPropertyPage::OnOK();
}

void CAmplPage::OnCancel() 
{
	GetSettingsParentSheet()->OnCancel();
	CPropertyPage::OnCancel();
}

BOOL CAmplPage::OnApply() 
{
	GetSettingsParentSheet()->OnApply();
	return CPropertyPage::OnApply();
}


void CAmplPage::UpdateControls(int nHint)
{
	if (!m_hWnd) return;
	if (nHint == ESH_Start_Auto || nHint == ESH_Stop_Auto)
	{
		BOOL bEnable = nHint==ESH_Stop_Auto;
		int nCamera = CameraIds::CameraDescrByInternalId(GetSettingsParentSheet()->m_nCamera)->nBaumerId;
		m_SGain.EnableWindow(bEnable&&GainEnabled(nCamera));
		m_SpGain.EnableWindow(bEnable&&GainEnabled(nCamera));
		m_EGain.EnableWindow(bEnable&&GainEnabled(nCamera));
		m_SOffset.EnableWindow(bEnable);
		m_SpOffset.EnableWindow(bEnable);
		m_EOffset.EnableWindow(bEnable);
		m_SShutter.EnableWindow(bEnable);
		m_SpShutter.EnableWindow(bEnable);
		m_EShutter.EnableWindow(bEnable);
		m_SProcWhite.EnableWindow(bEnable);
		m_SpProcWhite.EnableWindow(bEnable);
		m_EProcWhite.EnableWindow(bEnable);
		m_ShutterAuto.EnableWindow(bEnable);
	}
	else if (nHint == ESH_Camera_Changed)
	{
		SetSlider(m_SGain, GainEnabled(GetSettingsParentSheet()->m_nCamera), GetSettingsParentSheet()->m_nGain, g_CxLibWork.m_CameraType.vAmplMin,
			g_CxLibWork.m_CameraType.vAmplMax);
		m_EGain.EnableWindow(GainEnabled(GetSettingsParentSheet()->m_nCamera));
		SetSlider(m_SOffset, TRUE, GetSettingsParentSheet()->m_nOffset, g_CxLibWork.m_CameraType.vOffsetMin, g_CxLibWork.m_CameraType.vOffsetMax);
		SetExpSlider(m_SShutter, m_SpShutter, !GetSettingsParentSheet()->m_bAutoShutter, GetSettingsParentSheet()->m_nShutter,
			g_CxLibWork.m_sht.timeMin, g_CxLibWork.m_sht.timeMax);
		m_EProcWhite.EnableWindow(!GetSettingsParentSheet()->m_bAutoShutter);
		m_SProcWhite.EnableWindow(!GetSettingsParentSheet()->m_bAutoShutter);
		m_SpProcWhite.EnableWindow(!GetSettingsParentSheet()->m_bAutoShutter);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CCameraPage property page

CCameraPage::CCameraPage() : CSettingsPage(CCameraPage::IDD)
{
	//{{AFX_DATA_INIT(CCameraPage)
	m_bGraffic = FALSE;
	//}}AFX_DATA_INIT
}

CCameraPage::~CCameraPage()
{
}

void CCameraPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCameraPage)
	DDX_Control(pDX, IDC_CAMERA, m_Camera);
	DDX_Control(pDX, IDC_COMBO_BITS, m_Bits);
	DDX_Check(pDX, IDC_GRAFFIC, m_bGraffic);
	//}}AFX_DATA_MAP
	DDX_CBIndex(pDX, IDC_CAMERA, GetSettingsParentSheet()->m_nCamera);
	DDX_Check(pDX, IDC_AGP, GetSettingsParentSheet()->m_bAGP);
	if (g_CxLibWork.IsDC300())
		DDX_Check(pDX, IDC_BINING, GetSettingsParentSheet()->m_bDC300QuickView);
	else
		DDX_Check(pDX, IDC_BINING, GetSettingsParentSheet()->m_bBinning);
	DDX_Check(pDX, IDC_NATURAL_SIZE, GetSettingsParentSheet()->m_bNaturalSize);
	DDX_Check(pDX, IDC_GRAY_SCALE, GetSettingsParentSheet()->m_bGrayScale);
	DDX_CBIndex(pDX, IDC_COMBO_COLOR_PLANE, GetSettingsParentSheet()->m_nPlane);
	if (pDX->m_bSaveAndValidate)
	{
		if (GetSettingsParentSheet()->m_nConvEnd < GetSettingsParentSheet()->m_nConvStart)
			GetSettingsParentSheet()->m_nConvEnd = GetSettingsParentSheet()->m_nConvStart;
	}
}

BEGIN_MESSAGE_MAP(CCameraPage, CPropertyPage)
	//{{AFX_MSG_MAP(CCameraPage)
	ON_CBN_SELCHANGE(IDC_CAMERA, OnSelchangeCamera)
	ON_BN_CLICKED(IDC_AGP, OnAgp)
	ON_BN_CLICKED(IDC_BINING, OnBining)
	ON_BN_CLICKED(IDC_GRAFFIC, OnGraffic)
	ON_CBN_SELCHANGE(IDC_COMBO_BITS, OnSelchangeComboBits)
	ON_CBN_SELCHANGE(IDC_COMBO_COLOR_PLANE, OnSelchangeComboColorPlane)
	ON_BN_CLICKED(IDC_GRAY_SCALE, OnGrayScale)
	ON_BN_CLICKED(IDC_NATURAL_SIZE, OnNaturalSize)
	ON_BN_CLICKED(IDC_CAMERA_SYSTEM_MANAGER, OnCameraSystemManager)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CCameraPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	CSettingsSheet *p = GetSettingsParentSheet();
	m_bGraffic = p->m_bGraffic;
	UpdateData(FALSE);
	bool bIntOk = g_CxLibWork.IsInitedOk();
	::EnableWindow(::GetDlgItem(m_hWnd,IDC_GRAY_SCALE), bIntOk&&g_CxLibWork.m_CameraType.bColor);
	BOOL bEnable = p->m_bGrayScale;
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_STATIC_COLOR_PLANE), bIntOk&&bEnable);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_COMBO_COLOR_PLANE), bIntOk&&bEnable);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_AGP), bIntOk&&!IsWin9x());
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_CAMERA), !p->m_bCameraDisabled);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_BINING), bIntOk&&!p->m_bTriggerMode&&(g_CxLibWork.m_CameraType.cBinnMode||g_CxLibWork.IsDC300()));
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_GRAFFIC), bIntOk);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_NATURAL_SIZE), bIntOk);
	BOOL bShowCSM = g_BaumerProfile.GetProfileInt(NULL, _T("ShowCameraSystemManager"), FALSE, true, true);
	::ShowWindow(::GetDlgItem(m_hWnd, IDC_CAMERA_SYSTEM_MANAGER), bShowCSM?SW_SHOW:SW_HIDE);
	if (s_bSetup) UpdateControls(ESH_Start_Auto);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCameraPage::InitCameraCB()
{
	m_Camera.ResetContent();
	int nCameras = CameraIds::GetCameraCount();
	for (int iCam = 0; iCam < nCameras; iCam++)
	{
		CameraIds *pIds = CameraIds::CameraDescrByInternalId(iCam);
		if (pIds)
			m_Camera.AddString(pIds->pszCameraName);
		else
			m_Camera.AddString(" ");
	}
}

void CCameraPage::OnOK() 
{
	GetSettingsParentSheet()->OnOK();
	CPropertyPage::OnOK();
}


void CCameraPage::OnCancel() 
{
	CSettingsSheet *p = GetSettingsParentSheet();
	p->OnCancel();
	if (p->m_pSettingsSite)
		p->m_pSettingsSite->OnChangeNaturalSize(p->m_bNaturalSize?true:false);
	CPropertyPage::OnCancel();
}

BOOL CCameraPage::OnApply() 
{
	GetSettingsParentSheet()->OnApply();
	return CPropertyPage::OnApply();
}

void CCameraPage::OnSelchangeCamera() 
{
	UpdateData();
	if (g_BaumerProfile.GetProfileInt(NULL,_T("ReinitCamera"),FALSE, true, true))
	{
		GetSettingsParentSheet()->m_bCameraChanged = true;
		GetSettingsParentSheet()->InitCamera();
		UpdateData(FALSE);
		GetSettingsParentSheet()->UpdateControls(ESH_Camera_Changed);
		g_CxLibWork.NotifyChangeSizes();
	}
	SetModified();
}


void CCameraPage::OnSelchangeComboBits() 
{
	UpdateData();
	int nBitsMode = m_Bits.GetCurSel();
	if (nBitsMode >= 0)
	{
		GetSettingsParentSheet()->m_nBitsMode = nBitsMode;
		BitsModeDescr *pBM = BitsModeDescrByNum(nBitsMode);
		EnterCriticalSection(&g_CritSectionCamera);
		g_CxLibWork.m_nBitsMode = pBM->nBitsMode;
		SetPixelWidth(pBM->nBitsMode);
		int p = 3;
		SetScanMode(SM_SSMTC, &p);
		g_CxLibWork.m_bpp = GetImBpp();
		g_CxLibWork.m_nConversion = pBM->nConversion;
		g_CxLibWork.InitBitnessAndConversion(g_CxLibWork.m_nBitsMode,g_CxLibWork.m_nConversion,g_CxLibWork.m_bForCapture,
			g_CxLibWork.m_nConvStart,g_CxLibWork.m_nConvEnd,g_CxLibWork.m_dGamma,
			g_CxLibWork.m_nConvStart12,g_CxLibWork.m_nConvEnd12,g_CxLibWork.m_dGamma12);
		GetCameraInfo(&g_CxLibWork.m_CameraType, sizeof(tCameraType), &g_CxLibWork.m_CameraStatus , sizeof(tCameraStatus));
		LeaveCriticalSection(&g_CritSectionCamera);
		GetSettingsParentSheet()->UpdateControls(ESH_BitMode);
//		SetTopBottom();
		SetModified();
	}
}

void CCameraPage::OnAgp() 
{
	UpdateData();
	EnterCriticalSection(&g_CritSectionCamera);
	CAMERADLLSETS settings;
	settings.VadSearchMode = GetSettingsParentSheet()->m_bAGP;
	DLLSettings(&settings);
	LeaveCriticalSection(&g_CritSectionCamera);
	SetModified();
}

void CCameraPage::OnBining() 
{
	UpdateData();
	CSettingsSheet *p = GetSettingsParentSheet();
	if (g_CxLibWork.IsDC300())
	{
		EnterCriticalSection(&g_CritSectionCamera);
		g_CxLibWork.m_nDC300QuickView = p->m_bDC300QuickView;
		g_CxLibWork.InitDC300();
		BitsModeDescr *pBM = BitsModeDescrByNum(GetSettingsParentSheet()->m_nBitsMode);
		SetPixelWidth(pBM->nBitsMode);
		int p = g_CxLibWork.IsDC300()?4:3;
		SetScanMode(SM_SSMTC, &p);
		g_CxLibWork.InitBitnessAndConversion(g_CxLibWork.m_nBitsMode,g_CxLibWork.m_nConversion,g_CxLibWork.m_bForCapture,
			g_CxLibWork.m_nConvStart,g_CxLibWork.m_nConvEnd,g_CxLibWork.m_dGamma,
			g_CxLibWork.m_nConvStart12,g_CxLibWork.m_nConvEnd12,g_CxLibWork.m_dGamma12);
		g_CxLibWork.ReinitSizes();
		GetCameraInfo(&g_CxLibWork.m_CameraType, sizeof(tCameraType), &g_CxLibWork.m_CameraStatus , sizeof(tCameraStatus));
		LeaveCriticalSection(&g_CritSectionCamera);
		if (g_CxLibWork.IsDC300()) // Strange gluck in ARC6000c : need to recall SetScanMode
		{
			Sleep(0);
			EnterCriticalSection(&g_CritSectionCamera);
			SetScanMode(SM_SSMTC, &p);
			LeaveCriticalSection(&g_CritSectionCamera);
		}
	}
	else
	{
		EnterCriticalSection(&g_CritSectionCamera);
		SetBinningMode(p->m_bBinning);
		Sleep(300);
		int p1 = 3;
		SetScanMode(SM_SSMTC, &p1);
		int _x = GetImFrameX();
		int _y = GetImFrameY();
		g_CxLibWork.m_cx = g_CxLibWork.m_lpbi->biWidth = GetImFrameX();
		g_CxLibWork.m_cy = g_CxLibWork.m_lpbi->biHeight = GetImFrameY();
		g_CxLibWork.m_bpp = GetImBpp();
		if (p->m_bBinning)
		{
			g_CxLibWork.SetWBalance(FALSE, 0, 0, 0);
			SetRgbLut(FALSE, FALSE, NULL, NULL, NULL);
		}
		else
		{
			g_CxLibWork.SetWBalance(p->m_bWBEnable, p->m_dWBRed, p->m_dWBGreen, p->m_dWBBlue);
			SetRgbLut(p->m_bCBEnable, 1, &p->m_dCBRed, &p->m_dCBGreen, &p->m_dCBBlue);
		}
		GetCameraInfo(&g_CxLibWork.m_CameraType, sizeof(tCameraType), &g_CxLibWork.m_CameraStatus , sizeof(tCameraStatus));
		g_CxLibWork.InitBitnessAndConversion(g_CxLibWork.m_nBitsMode,g_CxLibWork.m_nConversion,g_CxLibWork.m_bForCapture,
			g_CxLibWork.m_nConvStart,g_CxLibWork.m_nConvEnd,g_CxLibWork.m_dGamma,
			g_CxLibWork.m_nConvStart12,g_CxLibWork.m_nConvEnd12,g_CxLibWork.m_dGamma12);
		LeaveCriticalSection(&g_CritSectionCamera);
	}
	p->UpdateControls(ESH_Binning);
	g_CxLibWork.NotifyChangeSizes();
	SetModified();
	
}

void CCameraPage::OnSelchangeComboColorPlane() 
{
	UpdateData();
	if (GetSettingsParentSheet()->m_bGrayScale)
		g_CxLibWork.InitGray(GetSettingsParentSheet()->m_nPlane);
	SetModified();
}

void CCameraPage::OnGrayScale() 
{
	UpdateData();
	BOOL bEnable = GetSettingsParentSheet()->m_bGrayScale;
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_STATIC_COLOR_PLANE), bEnable);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_COMBO_COLOR_PLANE), bEnable);
	EnterCriticalSection(&g_CritSectionCamera);
	if (GetSettingsParentSheet()->m_bGrayScale)
		g_CxLibWork.InitGray(GetSettingsParentSheet()->m_nPlane);
	else
		g_CxLibWork.InitGray(NoneGrayMode);
	LeaveCriticalSection(&g_CritSectionCamera);
	SetModified();
}

void CCameraPage::OnNaturalSize() 
{
	UpdateData();
	CSettingsSheet *p = GetSettingsParentSheet();
	if (p->m_pSettingsSite)
		p->m_pSettingsSite->OnChangeNaturalSize(p->m_bNaturalSize?true:false);
	SetModified();
}


void CCameraPage::UpdateControls(int nHint)
{
	if (!m_hWnd) return;
//	if (nHint == ESH_Start_Auto || nHint == ESH_Stop_Auto)
	{
		CSettingsSheet *p = GetSettingsParentSheet();
		bool bIntOk = g_CxLibWork.IsInitedOk();
		BOOL bEnable = nHint!=ESH_Start_Auto;
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_CAMERA), bEnable&&!p->m_bCameraDisabled);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_AGP), bIntOk&&bEnable&&!IsWin9x());
		::EnableWindow(::GetDlgItem(m_hWnd,IDC_GRAY_SCALE), bIntOk&&bEnable&&g_CxLibWork.m_CameraType.bColor);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_BINING), bIntOk&&bEnable&&!p->m_bTriggerMode&&(g_CxLibWork.m_CameraType.cBinnMode||g_CxLibWork.IsDC300()));
		::EnableWindow(::GetDlgItem(m_hWnd,IDC_COMBO_COLOR_PLANE), bIntOk&&bEnable&&g_CxLibWork.m_CameraType.bColor);
	}
}

void CCameraPage::OnGraffic() 
{
	UpdateData();
	GetSettingsParentSheet()->SetGrafficMode(m_bGraffic);
}

void CCameraPage::OnCameraSystemManager() 
{
	EnterCriticalSection(&g_CritSectionCamera);
	CameraSystemManager();
	LeaveCriticalSection(&g_CritSectionCamera);
}


/////////////////////////////////////////////////////////////////////////////
// CWhitePage property page

CWhitePage::CWhitePage() : CSettingsPage(CWhitePage::IDD)
{
	//{{AFX_DATA_INIT(CWhitePage)
	//}}AFX_DATA_INIT
}

CWhitePage::~CWhitePage()
{
}

void CWhitePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWhitePage)
	DDX_Control(pDX, IDC_SP_RED, m_SpWBRed);
	DDX_Control(pDX, IDC_SP_BLUE, m_SpWBBlue);
	DDX_Control(pDX, IDC_SP_GREEN, m_SpWBGreen);
	DDX_Control(pDX, IDC_WB_DEFAULT, m_WBDefault);
	DDX_Control(pDX, IDC_S_WB_BLUE, m_SWBBlue);
	DDX_Control(pDX, IDC_S_WB_GREEN, m_SWBGreen);
	DDX_Control(pDX, IDC_S_WB_RED, m_SWBRed);
	DDX_Control(pDX, IDC_E_WB_BLUE, m_EWBBlue);
	DDX_Control(pDX, IDC_E_WB_GREEN, m_EWBGreen);
	DDX_Control(pDX, IDC_E_WB_RED, m_EWBRed);
	DDX_Control(pDX, IDC_WB_ENABLE, m_WBEnable);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_WB_ENABLE, GetSettingsParentSheet()->m_bWBEnable);
	DDX_Text(pDX, IDC_E_WB_BLUE, GetSettingsParentSheet()->m_dWBBlue);
	__Validate(pDX, m_hWnd, IDC_E_WB_BLUE, GetSettingsParentSheet()->m_dWBBlue, 0.01, 4.);
	DDX_Text(pDX, IDC_E_WB_GREEN, GetSettingsParentSheet()->m_dWBGreen);
	__Validate(pDX, m_hWnd, IDC_E_WB_GREEN, GetSettingsParentSheet()->m_dWBGreen, 0.01, 4.);
	DDX_Text(pDX, IDC_E_WB_RED, GetSettingsParentSheet()->m_dWBRed);
	__Validate(pDX, m_hWnd, IDC_E_WB_RED, GetSettingsParentSheet()->m_dWBRed, 0.01, 4.);
}


BEGIN_MESSAGE_MAP(CWhitePage, CPropertyPage)
	//{{AFX_MSG_MAP(CWhitePage)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_WB_ENABLE, OnWbEnable)
	ON_BN_CLICKED(IDC_WB_DEFAULT, OnWbDefault)
	ON_EN_CHANGE(IDC_E_WB_BLUE, OnChangeEWbBlue)
	ON_EN_CHANGE(IDC_E_WB_GREEN, OnChangeEWbGreen)
	ON_EN_CHANGE(IDC_E_WB_RED, OnChangeEWbRed)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SP_RED, OnDeltaposSpRed)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SP_GREEN, OnDeltaposSpGreen)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SP_BLUE, OnDeltaposSpBlue)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CWhitePage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	UpdateControls(0);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CWhitePage::UpdateControls(int nHint)
{
	if (!m_hWnd) return;
	if (nHint == ESH_Start_Auto || nHint == ESH_Stop_Auto || nHint == ESH_Binning || nHint == ESH_BitMode || nHint == 0)
	{
		m_WBEnable.EnableWindow(!s_bSetup&&!GetSettingsParentSheet()->m_bBinning);
		m_EWBRed.EnableWindow(!s_bSetup&&!GetSettingsParentSheet()->m_bBinning&&GetSettingsParentSheet()->m_bWBEnable);
		m_EWBGreen.EnableWindow(!s_bSetup&&!GetSettingsParentSheet()->m_bBinning&&GetSettingsParentSheet()->m_bWBEnable);
		m_EWBBlue.EnableWindow(!s_bSetup&&!GetSettingsParentSheet()->m_bBinning&&GetSettingsParentSheet()->m_bWBEnable);
		SetSlider(m_SWBRed, m_SpWBRed, !s_bSetup&&!GetSettingsParentSheet()->m_bBinning&&GetSettingsParentSheet()->m_bWBEnable, int(GetSettingsParentSheet()->m_dWBRed*100), 1, 400);
		SetSlider(m_SWBGreen, m_SpWBGreen, !s_bSetup&&!GetSettingsParentSheet()->m_bBinning&&GetSettingsParentSheet()->m_bWBEnable, int(GetSettingsParentSheet()->m_dWBGreen*100), 1, 400);
		SetSlider(m_SWBBlue, m_SpWBBlue, !s_bSetup&&!GetSettingsParentSheet()->m_bBinning&&GetSettingsParentSheet()->m_bWBEnable, int(GetSettingsParentSheet()->m_dWBBlue*100), 1, 400);
		m_WBDefault.EnableWindow(!s_bSetup&&!GetSettingsParentSheet()->m_bBinning&&GetSettingsParentSheet()->m_bWBEnable/*&& !IsImLibMode(GetSettingsParentSheet())*/);
	}
}

void CWhitePage::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CLock lock(&s_bInsideHScroll);
	UpdateData();
	CSettingsSheet *p = GetSettingsParentSheet();
	if (pScrollBar == (CScrollBar*)&m_SWBRed)
	{
		int n = m_SWBRed.GetPos();
		p->m_dWBRed = double(n)/100.;
		m_SpWBRed.SetPos(n);
		EnterCriticalSection(&g_CritSectionCamera);
		g_CxLibWork.SetWBalance(p->m_bWBEnable, p->m_dWBRed, p->m_dWBGreen, p->m_dWBBlue);
		LeaveCriticalSection(&g_CritSectionCamera);
	}
	else if (pScrollBar == (CScrollBar*)&m_SWBGreen)
	{
		int n = m_SWBGreen.GetPos();
		p->m_dWBGreen = double(n)/100.;
		m_SpWBGreen.SetPos(n);
		EnterCriticalSection(&g_CritSectionCamera);
		g_CxLibWork.SetWBalance(p->m_bWBEnable, p->m_dWBRed, p->m_dWBGreen, p->m_dWBBlue);
		LeaveCriticalSection(&g_CritSectionCamera);
	}
	else if (pScrollBar == (CScrollBar*)&m_SWBBlue)
	{
		int n = m_SWBBlue.GetPos();
		p->m_dWBBlue = double(n)/100.;
		m_SpWBBlue.SetPos(n);
		EnterCriticalSection(&g_CritSectionCamera);
		g_CxLibWork.SetWBalance(p->m_bWBEnable, p->m_dWBRed, p->m_dWBGreen, p->m_dWBBlue);
		LeaveCriticalSection(&g_CritSectionCamera);
	}
	UpdateData(FALSE);
	SetModified();
	CPropertyPage::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CWhitePage::OnWbEnable() 
{
	UpdateData();
	UpdateControls();
	EnterCriticalSection(&g_CritSectionCamera);
	CSettingsSheet *p = GetSettingsParentSheet();
	g_CxLibWork.SetWBalance(p->m_bWBEnable, p->m_dWBRed, p->m_dWBGreen, p->m_dWBBlue);
	LeaveCriticalSection(&g_CritSectionCamera);
	SetModified();
}

void CWhitePage::OnWbDefault() 
{
	CSettingsSheet *p = GetSettingsParentSheet();
	if (p->m_bWBEnable)
	{
		UpdateData();
		if (Arc6000Gluck2() && g_CxLibWork.m_nDC300QuickView != 1)
		{ // Yet another ARC6000 gluck - white balance doesn't work in full mode !
			EnterCriticalSection(&g_CritSectionCamera);
			g_CxLibWork.m_nDC300QuickView = 1;
			g_CxLibWork.InitDC300();
			LeaveCriticalSection(&g_CritSectionCamera);
			Sleep(200);
			EnterCriticalSection(&g_CritSectionCamera);
			g_CxLibWork.DoWBalance(p->m_dWBRed, p->m_dWBGreen, p->m_dWBBlue);
			g_CxLibWork.m_nDC300QuickView = 0;
			g_CxLibWork.InitDC300();
			LeaveCriticalSection(&g_CritSectionCamera);
		}
		else
		{
			EnterCriticalSection(&g_CritSectionCamera);
			g_CxLibWork.DoWBalance(p->m_dWBRed, p->m_dWBGreen, p->m_dWBBlue);
			LeaveCriticalSection(&g_CritSectionCamera);
		}
		SetSlider(m_SWBRed, m_SpWBRed, !s_bSetup&&!GetSettingsParentSheet()->m_bBinning&&GetSettingsParentSheet()->m_bWBEnable, int(GetSettingsParentSheet()->m_dWBRed*100), 1, 400);
		SetSlider(m_SWBGreen, m_SpWBGreen, !s_bSetup&&!GetSettingsParentSheet()->m_bBinning&&GetSettingsParentSheet()->m_bWBEnable, int(GetSettingsParentSheet()->m_dWBGreen*100), 1, 400);
		SetSlider(m_SWBBlue, m_SpWBBlue, !s_bSetup&&!GetSettingsParentSheet()->m_bBinning&&GetSettingsParentSheet()->m_bWBEnable, int(GetSettingsParentSheet()->m_dWBBlue*100), 1, 400);
		UpdateData(FALSE);
	}
	SetModified();
}

void CWhitePage::DoChangeValue()
{
	CSettingsSheet *p = GetSettingsParentSheet();
	UpdateData();
	EnterCriticalSection(&g_CritSectionCamera);
	g_CxLibWork.SetWBalance(p->m_bWBEnable, p->m_dWBRed, p->m_dWBGreen, p->m_dWBBlue);
	LeaveCriticalSection(&g_CritSectionCamera);
	SetModified();
}

void CWhitePage::OnChangeEWbBlue() 
{

	if (m_EWBBlue.m_hWnd)
	{
		DoChangeValue();
		int n = int(GetSettingsParentSheet()->m_dWBBlue*100);
		if (!s_bInsideHScroll) m_SWBBlue.SetPos(n);
		if (!s_bInsideSpin) m_SpWBBlue.SetPos(n);
	}
}


void CWhitePage::OnChangeEWbGreen() 
{
	if (m_EWBGreen.m_hWnd)
	{
		DoChangeValue();
		int n = int(/*IsImLibMode(GetSettingsParentSheet())?GetSettingsParentSheet()->m_dWBGreenIm*100:*/GetSettingsParentSheet()->m_dWBGreen*100);
		if (!s_bInsideHScroll) m_SWBGreen.SetPos(n);
		if (!s_bInsideSpin) m_SpWBGreen.SetPos(n);
	}
}

void CWhitePage::OnChangeEWbRed() 
{
	if (m_EWBRed.m_hWnd)
	{
		DoChangeValue();
		int n = int(/*IsImLibMode(GetSettingsParentSheet())?GetSettingsParentSheet()->m_dWBRedIm*100:*/GetSettingsParentSheet()->m_dWBRed*100);
		if (!s_bInsideHScroll) m_SWBRed.SetPos(n);
		if (!s_bInsideSpin) m_SpWBRed.SetPos(n);
	}
}

void CWhitePage::OnDeltaposSpRed(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	CLock lock(&s_bInsideSpin);
	UpdateData();
	CSettingsSheet *p = GetSettingsParentSheet();
	double dPos = (/*IsImLibMode(p)?p->m_dWBRedIm*100.:*/p->m_dWBRed*100.)+pNMUpDown->iDelta;
	dPos = max(min(dPos,400.),1.);
	/*if (IsImLibMode(p))
		p->m_dWBRedIm = dPos/100.;
	else*/
		p->m_dWBRed = dPos/100.;
	UpdateData(FALSE);
	m_SWBRed.SetPos(int(dPos));
	*pResult = 0;
}

void CWhitePage::OnDeltaposSpGreen(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	CLock lock(&s_bInsideSpin);
	UpdateData();
	CSettingsSheet *p = GetSettingsParentSheet();
	double dPos = (/*IsImLibMode(p)?p->m_dWBGreenIm*100.:*/p->m_dWBGreen*100.)+pNMUpDown->iDelta;
	dPos = max(min(dPos,400.),1.);
	/*if (IsImLibMode(p))
		p->m_dWBGreenIm = dPos/100.;
	else*/
		p->m_dWBGreen = dPos/100.;
	UpdateData(FALSE);
	m_SWBGreen.SetPos(int(dPos));
	*pResult = 0;
}

void CWhitePage::OnDeltaposSpBlue(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	CLock lock(&s_bInsideSpin);
	UpdateData();
	CSettingsSheet *p = GetSettingsParentSheet();
	double dPos = (/*IsImLibMode(p)?p->m_dWBBlueIm*100.:*/p->m_dWBBlue*100.)+pNMUpDown->iDelta;
	dPos = max(min(dPos,400.),1.);
	/*if (IsImLibMode(p))
		p->m_dWBBlueIm = dPos/100.;
	else*/
		p->m_dWBBlue = dPos/100.;
	UpdateData(FALSE);
	m_SWBBlue.SetPos(int(dPos));
	*pResult = 0;
}


BOOL CWhitePage::OnApply() 
{
	GetSettingsParentSheet()->OnApply();
	return CPropertyPage::OnApply();
}

void CWhitePage::OnCancel() 
{
	GetSettingsParentSheet()->OnCancel();
	CPropertyPage::OnCancel();
}

void CWhitePage::OnOK() 
{
	GetSettingsParentSheet()->OnOK();
	CPropertyPage::OnOK();
}


/////////////////////////////////////////////////////////////////////////////
// CColorPage property page

CColorPage::CColorPage() : CSettingsPage(CColorPage::IDD)
{
	//{{AFX_DATA_INIT(CColorPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CColorPage::~CColorPage()
{
}

void CColorPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CColorPage)
	DDX_Control(pDX, IDC_SP_CB_RED, m_SpCBRed);
	DDX_Control(pDX, IDC_SP_CB_GREEN, m_SpCBGreen);
	DDX_Control(pDX, IDC_SP_CB_BLUE, m_SpCBBlue);
	DDX_Control(pDX, IDC_S_CB_BLUE, m_SCBBlue);
	DDX_Control(pDX, IDC_S_CB_GREEN, m_SCBGreen);
	DDX_Control(pDX, IDC_S_CB_RED, m_SCBRed);
	DDX_Control(pDX, IDC_E_CB_BLUE, m_ECBBlue);
	DDX_Control(pDX, IDC_E_CB_GREEN, m_ECBGreen);
	DDX_Control(pDX, IDC_E_CB_RED, m_ECBRed);
	DDX_Control(pDX, IDC_CB_ENABLE, m_CBEnable);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_CB_ENABLE, GetSettingsParentSheet()->m_bCBEnable);
	DDX_Text(pDX, IDC_E_CB_BLUE, GetSettingsParentSheet()->m_dCBBlue);
	__Validate(pDX, m_hWnd, IDC_E_CB_BLUE, GetSettingsParentSheet()->m_dCBBlue, 0.01, 4.);
	DDX_Text(pDX, IDC_E_CB_GREEN, GetSettingsParentSheet()->m_dCBGreen);
	__Validate(pDX, m_hWnd, IDC_E_CB_GREEN, GetSettingsParentSheet()->m_dCBGreen, 0.01, 4.);
	DDX_Text(pDX, IDC_E_CB_RED, GetSettingsParentSheet()->m_dCBRed);
	__Validate(pDX, m_hWnd, IDC_E_CB_RED, GetSettingsParentSheet()->m_dCBRed, 0.01, 4.);
}


BEGIN_MESSAGE_MAP(CColorPage, CPropertyPage)
	//{{AFX_MSG_MAP(CColorPage)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_CB_ENABLE, OnCbEnable)
	ON_EN_CHANGE(IDC_E_CB_BLUE, OnChangeECbBlue)
	ON_EN_CHANGE(IDC_E_CB_GREEN, OnChangeECbGreen)
	ON_EN_CHANGE(IDC_E_CB_RED, OnChangeECbRed)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SP_CB_BLUE, OnDeltaposSpCbBlue)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SP_CB_GREEN, OnDeltaposSpCbGreen)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SP_CB_RED, OnDeltaposSpCbRed)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CColorPage::UpdateControls(int nHint)
{
	if (!m_hWnd) return;
	if (nHint == ESH_Start_Auto || nHint == ESH_Stop_Auto || nHint == ESH_Binning || nHint == 0)
	{
		CSettingsSheet *p = GetSettingsParentSheet();
		m_CBEnable.EnableWindow(!s_bSetup&&!p->m_bBinning);
		m_ECBRed.EnableWindow(!s_bSetup&&!p->m_bBinning&&p->m_bCBEnable);
		m_ECBGreen.EnableWindow(!s_bSetup&&!p->m_bBinning&&p->m_bCBEnable);
		m_ECBBlue.EnableWindow(!s_bSetup&&!p->m_bBinning&&p->m_bCBEnable);
		SetSlider(m_SCBRed, m_SpCBRed, !s_bSetup&&!p->m_bBinning&&p->m_bCBEnable, int(p->m_dCBRed*100), 1, 400);
		SetSlider(m_SCBGreen, m_SpCBGreen, !s_bSetup&&!p->m_bBinning&&p->m_bCBEnable, int(p->m_dCBGreen*100), 1, 400);
		SetSlider(m_SCBBlue, m_SpCBBlue, !s_bSetup&&!p->m_bBinning&&p->m_bCBEnable, int(p->m_dCBBlue*100), 1, 400);
	}
}

BOOL CColorPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	UpdateControls();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CColorPage::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CLock lock(&s_bInsideHScroll);
	UpdateData();
	CSettingsSheet *p = GetSettingsParentSheet();
	if (pScrollBar == (CScrollBar*)&m_SCBRed)
	{
		int n = m_SCBRed.GetPos();
		p->m_dCBRed = double(n)/100.;
		m_SpCBRed.SetPos(n);
		EnterCriticalSection(&g_CritSectionCamera);
		SetRgbLut(p->m_bCBEnable, 1, &p->m_dCBRed, &p->m_dCBGreen, &p->m_dCBBlue);
		LeaveCriticalSection(&g_CritSectionCamera);
	}
	else if (pScrollBar == (CScrollBar*)&m_SCBGreen)
	{
		int n = m_SCBGreen.GetPos();
		p->m_dCBGreen = double(n)/100.;
		m_SpCBGreen.SetPos(n);
		EnterCriticalSection(&g_CritSectionCamera);
		SetRgbLut(p->m_bCBEnable, 1, &p->m_dCBRed, &p->m_dCBGreen, &p->m_dCBBlue);
		LeaveCriticalSection(&g_CritSectionCamera);
	}
	else if (pScrollBar == (CScrollBar*)&m_SCBBlue)
	{
		int n = m_SCBBlue.GetPos();
		p->m_dCBBlue = double(n)/100.;
		m_SpCBBlue.SetPos(n);
		EnterCriticalSection(&g_CritSectionCamera);
		SetRgbLut(p->m_bCBEnable, 1, &p->m_dCBRed, &p->m_dCBGreen, &p->m_dCBBlue);
		LeaveCriticalSection(&g_CritSectionCamera);
	}
	UpdateData(FALSE);
	SetModified();
	CPropertyPage::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CColorPage::OnCbEnable() 
{
	UpdateData();
	UpdateControls();
	CSettingsSheet *p = GetSettingsParentSheet();
	EnterCriticalSection(&g_CritSectionCamera);
	SetRgbLut(p->m_bCBEnable, 1, &p->m_dCBRed, &p->m_dCBGreen, &p->m_dCBBlue);
	LeaveCriticalSection(&g_CritSectionCamera);
	SetModified();
}

void CColorPage::DoChangeValue()
{
	CSettingsSheet *p = GetSettingsParentSheet();
	UpdateData();
	EnterCriticalSection(&g_CritSectionCamera);
	SetRgbLut(p->m_bCBEnable, 1, &p->m_dCBRed, &p->m_dCBGreen, &p->m_dCBBlue);
	LeaveCriticalSection(&g_CritSectionCamera);
	SetModified();
}


void CColorPage::OnChangeECbBlue()
{
	if (m_ECBBlue.m_hWnd)
	{
		UpdateData();
		int n = int(GetSettingsParentSheet()->m_dCBBlue*100);
		if (!s_bInsideHScroll) m_SCBBlue.SetPos(n);
		if (!s_bInsideSpin) m_SpCBBlue.SetPos(n);
	}
}

void CColorPage::OnChangeECbGreen() 
{
	if (m_ECBGreen.m_hWnd)
	{
		UpdateData();
		int n = int(GetSettingsParentSheet()->m_dCBGreen*100);
		if (!s_bInsideHScroll) m_SCBGreen.SetPos(n);
		if (!s_bInsideSpin) m_SpCBGreen.SetPos(n);
	}
}

void CColorPage::OnChangeECbRed() 
{
	if (m_ECBBlue.m_hWnd)
	{
		UpdateData();
		int n = int(GetSettingsParentSheet()->m_dCBRed*100);
		if (!s_bInsideHScroll) m_SCBRed.SetPos(n);
		if (!s_bInsideSpin) m_SpCBRed.SetPos(n);
	}
}

void CColorPage::OnDeltaposSpCbBlue(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	CLock lock(&s_bInsideSpin);
	UpdateData();
	double dPos = GetSettingsParentSheet()->m_dCBBlue*100.+pNMUpDown->iDelta;
	dPos = max(min(dPos,400.),1.);
	GetSettingsParentSheet()->m_dCBBlue = dPos/100.;
	UpdateData(FALSE);
	m_SCBBlue.SetPos(int(dPos));
	*pResult = 0;
}

void CColorPage::OnDeltaposSpCbGreen(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	CLock lock(&s_bInsideSpin);
	UpdateData();
	double dPos = GetSettingsParentSheet()->m_dCBGreen*100.+pNMUpDown->iDelta;
	dPos = max(min(dPos,400.),1.);
	GetSettingsParentSheet()->m_dCBGreen = dPos/100.;
	UpdateData(FALSE);
	m_SCBGreen.SetPos(int(dPos));
	*pResult = 0;
}

void CColorPage::OnDeltaposSpCbRed(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	CLock lock(&s_bInsideSpin);
	UpdateData();
	double dPos = GetSettingsParentSheet()->m_dCBRed*100.+pNMUpDown->iDelta;
	dPos = max(min(dPos,400.),1.);
	GetSettingsParentSheet()->m_dCBRed = dPos/100.;
	UpdateData(FALSE);
	m_SCBRed.SetPos(int(dPos));
	*pResult = 0;
}


BOOL CColorPage::OnApply() 
{
	GetSettingsParentSheet()->OnApply();
	return CPropertyPage::OnApply();
}

void CColorPage::OnCancel() 
{
	GetSettingsParentSheet()->OnCancel();
	CPropertyPage::OnCancel();
}

void CColorPage::OnOK() 
{
	GetSettingsParentSheet()->OnOK();
	CPropertyPage::OnOK();
}

/////////////////////////////////////////////////////////////////////////////
// CFramePage property page

CFramePage::CFramePage() : CSettingsPage(CFramePage::IDD)
{
	//{{AFX_DATA_INIT(CFramePage)
	m_bPreviewAllFrameOld = m_bPreviewAllFrame = g_BaumerProfile.GetProfileInt(_T("Settings"), _T("PreviewAllFrame"), FALSE);
	//}}AFX_DATA_INIT
//	m_bEnableMargins = g_BaumerProfile.GetProfileInt(_T("Settings"), _T("EnableMargins"), FALSE);
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
	DDX_Check(pDX, IDC_PREVIEW_ALL_FRAME, m_bPreviewAllFrame);
	//}}AFX_DATA_MAP
	CSettingsSheet *p = GetSettingsParentSheet();
	DDX_Check(pDX, IDC_ENABLE_FRAME, p->m_bEnableMargins);
	DDX_Text(pDX, IDC_E_LEFT_MARGIN, p->m_nLeftMargin);
	DDX_Text(pDX, IDC_E_RIGHT_MARGIN, p->m_nRightMargin);
	DDX_Text(pDX, IDC_E_TOP_MARGIN, p->m_nTopMargin);
	DDX_Text(pDX, IDC_E_BOTTOM_MARGIN, p->m_nBottomMargin);
	if (pDX->m_bSaveAndValidate)
	{
		p->m_nLeftMargin = min(p->m_nLeftMargin,p->m_nMaxMarginX);
		p->m_nRightMargin = min(p->m_nRightMargin,p->m_nMaxMarginX);
		p->m_nTopMargin = min(p->m_nTopMargin,p->m_nMaxMarginY);
		p->m_nBottomMargin = min(p->m_nBottomMargin,p->m_nMaxMarginY);
	}
}

void CFramePage::UpdateControls(int nHint)
{
	if (!m_hWnd) return;
	if (nHint == ESH_Start_Auto || nHint == ESH_Stop_Auto || nHint == 0)
	{
		CSettingsSheet *p = GetSettingsParentSheet();
		BOOL bEnable = nHint!=ESH_Start_Auto;
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_ENABLE_FRAME), bEnable);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_PREVIEW_ALL_FRAME), bEnable&&p->m_bEnableMargins);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_E_LEFT_MARGIN), bEnable&&p->m_bEnableMargins);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_S_LEFT_MARGIN), bEnable&&p->m_bEnableMargins);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_E_RIGHT_MARGIN), bEnable&&p->m_bEnableMargins);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_S_RIGHT_MARGIN), bEnable&&p->m_bEnableMargins);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_E_TOP_MARGIN), bEnable&&p->m_bEnableMargins);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_S_TOP_MARGIN), bEnable&&p->m_bEnableMargins);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_E_BOTTOM_MARGIN), bEnable&&p->m_bEnableMargins);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_S_BOTTOM_MARGIN), bEnable&&p->m_bEnableMargins);
	}
}

BOOL CFramePage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	CSettingsSheet *p = GetSettingsParentSheet();
	m_SLeftMargin.SetRange(0, p->m_nMaxMarginX);
	m_SLeftMargin.SetPos(p->m_nLeftMargin);
	m_SRightMargin.SetRange(0, p->m_nMaxMarginX);
	m_SRightMargin.SetPos(p->m_nRightMargin);
	m_STopMargin.SetRange(0, p->m_nMaxMarginY);
	m_STopMargin.SetPos(p->m_nTopMargin);
	m_SBottomMargin.SetRange(0, p->m_nMaxMarginY);
	m_SBottomMargin.SetPos(p->m_nBottomMargin);
	if (s_bSetup)
		UpdateControls(ESH_Start_Auto);
	else
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
	if (m_ELeftMargin.m_hWnd)
	{
		UpdateData();
		CSettingsSheet *p = GetSettingsParentSheet();
		g_CxLibWork.m_rcMargins = CRect(p->m_nLeftMargin, p->m_nTopMargin, p->m_nRightMargin, p->m_nBottomMargin);
		SetModified();
		g_CxLibWork.NotifyChangeSizes();
	}
}

void CFramePage::OnChangeERightMargin() 
{
	if (m_ERightMargin.m_hWnd)
	{
		UpdateData();
		CSettingsSheet *p = GetSettingsParentSheet();
		g_CxLibWork.m_rcMargins = CRect(p->m_nLeftMargin, p->m_nTopMargin, p->m_nRightMargin, p->m_nBottomMargin);
		SetModified();
		g_CxLibWork.NotifyChangeSizes();
	}
}

void CFramePage::OnChangeETopMargin() 
{
	if (m_ETopMargin.m_hWnd)
	{
		UpdateData();
		CSettingsSheet *p = GetSettingsParentSheet();
		g_CxLibWork.m_rcMargins = CRect(p->m_nLeftMargin, p->m_nTopMargin, p->m_nRightMargin, p->m_nBottomMargin);
		SetModified();
		g_CxLibWork.NotifyChangeSizes();
	}
}

void CFramePage::OnChangeEBottomMargin() 
{
	if (m_EBottomMargin.m_hWnd)
	{
		UpdateData();
		CSettingsSheet *p = GetSettingsParentSheet();
		g_CxLibWork.m_rcMargins = CRect(p->m_nLeftMargin, p->m_nTopMargin, p->m_nRightMargin, p->m_nBottomMargin);
		SetModified();
		g_CxLibWork.NotifyChangeSizes();
	}
}

void CFramePage::OnEnableFrame() 
{
	UpdateData();
	UpdateControls(0);
	CSettingsSheet *p = GetSettingsParentSheet();
	g_CxLibWork.m_bEnableMargins = p->m_bEnableMargins;
	g_CxLibWork.NotifyChangeSizes();
	SetModified();
}

BOOL CFramePage::OnApply() 
{
	m_bPreviewAllFrameOld = m_bPreviewAllFrame;
	GetSettingsParentSheet()->OnApply();
	return CPropertyPage::OnApply();
}

void CFramePage::OnCancel() 
{
	g_BaumerProfile.WriteProfileInt(_T("Settings"), _T("PreviewAllFrame"), m_bPreviewAllFrameOld);
	GetSettingsParentSheet()->OnCancel();
	g_CxLibWork.m_bEnableMargins = GetSettingsParentSheet()->m_bEnableMargins;
	g_CxLibWork.NotifyChangeSizes();
	CPropertyPage::OnCancel();
}

void CFramePage::OnOK() 
{
	GetSettingsParentSheet()->OnOK();
	CPropertyPage::OnOK();
}

void CFramePage::OnPreviewAllFrame() 
{
	UpdateData();
	g_BaumerProfile.WriteProfileInt(_T("Settings"), _T("PreviewAllFrame"), m_bPreviewAllFrame);
	g_CxLibWork.NotifyChangeSizes();
	SetModified();
}











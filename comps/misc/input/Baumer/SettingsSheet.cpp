// SettingsSheet.cpp : implementation file
//

#include "stdafx.h"
#include "Baumer.h"
#include "SettingsSheet.h"
#include "CxLibWork.h"
#include "BaumerHelp.h"
#include "BaumUtils.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

BitsModeDescr arrBMs[] =
{
	{0, W8BIT, -1, IDS_8BIT},
	{1, W10BIT, AutoConversion, IDS_10BIT_AUTO},
	{2, W10BIT, ManualConversion, IDS_10BIT_MANUAL},
	{3, W10BIT, NoConversion, IDS_10BIT_NOCONV},
	{4, W12BIT, AutoConversion, IDS_12BIT_AUTO},
	{5, W12BIT, ManualConversion, IDS_12BIT_MANUAL},
	{6, W12BIT, NoConversion, IDS_12BIT_NOCONV},
};

#define BMNUMBER sizeof(arrBMs)/sizeof(BitsModeDescr)

BitsModeDescr *BitsModeDescrByNum(int nNum)
{
	for (int i = 0; i < BMNUMBER; i++)
	{
		if (arrBMs[i].nNum == nNum)
			return &arrBMs[i];
	}
	return &arrBMs[0];
}

BitsModeDescr *BitsModeDescrByBM(int nBitsMode, int nConversion)
{
	for (int i = 0; i < BMNUMBER; i++)
	{
		if (arrBMs[i].nBitsMode == nBitsMode && (arrBMs[i].nConversion == nConversion || arrBMs[i].nConversion == -1))
			return &arrBMs[i];
	}
	return &arrBMs[0];
}

BitsModeDescr *BitsModeDescrByName(LPCTSTR lpstrName)
{
	for (int i = 0; i < BMNUMBER; i++)
	{
		CString s;
		s.LoadString(arrBMs[i].idStr);
		if (s.Compare(lpstrName)==0)
			return &arrBMs[i];
	}
	return &arrBMs[0];
}

void InitComboByModes(CComboBox *pCombo, int nCurSelNum)
{
	pCombo->ResetContent();
	int iNum = 0;
	for (int i = 0; i < BMNUMBER; i++)
	{
		if (Is12BitCamera(g_CxLibWork.m_nCamera) && arrBMs[i].nBitsMode == W10BIT)
			continue;
		CString s;
		s.LoadString(arrBMs[i].idStr);
		pCombo->AddString(s);
		if (nCurSelNum == arrBMs[i].nNum)
			pCombo->SetCurSel(iNum);
		iNum++;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSettingsSheet

IMPLEMENT_DYNAMIC(CSettingsSheet, CPropertySheet)

CSettingsSheet::CSettingsSheet(ISettingsSite *pSite, IUnknown *punkDriver, CWnd* pWndParent)
	 : CPropertySheet(IDS_PROPSHT_CAPTION, pWndParent)
{
	// Initialize variables.
	m_nOffset = 0;
	m_nGain = 0;
	m_nShutter = 0;
	m_bCBEnable = FALSE;
	m_bWBEnable = FALSE;
	m_dWBBlue = 0.0;
	m_dWBGreen = 0.0;
	m_dWBRed = 0.0;
	m_dCBGreen = 0.0;
	m_dCBRed = 0.0;
	m_dCBBlue = 0.0;
	m_bOverlay = FALSE;
	m_nCamera = -1;
	m_nBitsMode = 0;
	m_bAGP = FALSE;
	m_bBinning = FALSE;
	m_nLeftMargin = 0;
	m_nRightMargin = 0;
	m_nTopMargin = 0;
	m_nBottomMargin = 0;
	m_bOvrMode = false;
	m_bCameraChanged = false;
	m_nMaxMarginX = m_nMaxMarginY = MAX_MARGIN;
	m_pSettingsSite = pSite;
	m_bInited = false;
	// Initialize shutter values array.
	ReadSettings();
	// Add pages
	if (g_CxLibWork.IsInitedOk())
	{
		AddPage(&m_PageAmpl);
		m_PageAmpl.m_pSheet = this;
	}
	AddPage(&m_PageCam);
	m_PageCam.m_pSheet = this;
	if (g_CxLibWork.IsInitedOk())
	{
		AddPage(&m_ManualPage);
		m_ManualPage.m_pSheet = this;
		if (g_CxLibWork.m_CameraType.bColor)
		{
			AddPage(&m_PageWB);
			m_PageWB.m_pSheet = this;
			if (g_BaumerProfile.GetProfileInt(NULL, _T("EnableColorBalancePage"), FALSE, true, true))
				AddPage(&m_PageCB);
			m_PageCB.m_pSheet = this;
		}
		AddPage(&m_PageFrame);
		m_PageFrame.m_pSheet = this;
		if (g_BaumerProfile.GetProfileInt(NULL, _T("EnableTriggerPage"), TRUE, true, true))
			AddPage(&m_PageTrigger);
		m_PageTrigger.m_pSheet = this;
	}
	m_bFinished = FALSE;
	m_bModified = FALSE;
	m_bGraffic = FALSE;
	m_bFreezeGraffic = FALSE;
	m_bOvrModeChanged = false;
	m_sptrDrv3 = punkDriver;
}

CSettingsSheet::~CSettingsSheet()
{
//	delete m_pSTT;
}

double &CSettingsSheet::Gamma()
{
	if (g_CxLibWork.m_nBitsMode <= W10BIT)
		return m_dGamma;
	else
		return m_dGamma12;
}

int &CSettingsSheet::ConvStart()
{
	if (g_CxLibWork.m_nBitsMode <= W10BIT)
		return m_nConvStart;
	else
		return m_nConvStart12;
}

int &CSettingsSheet::ConvEnd()
{
	if (g_CxLibWork.m_nBitsMode <= W10BIT)
		return m_nConvEnd;
	else
		return m_nConvEnd12;
}


BEGIN_MESSAGE_MAP(CSettingsSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CSettingsSheet)
	ON_WM_MOVE()
	ON_WM_DESTROY()
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_USER+10, OnUserMsg)
END_MESSAGE_MAP()


void CSettingsSheet::ReadSettings()
{
	m_nCamera = g_BaumerProfile.ReadCamera()->nInternalId;
	m_bCameraDisabled = g_BaumerProfile.GetProfileInt(_T("General"), _T("CameraDisabled"), 0, true, true);
	m_nOffset = g_BaumerProfile.GetProfileInt(_T("Settings"), _T("BlackLevel"), 0);
	m_nGain = g_BaumerProfile.GetProfileInt(_T("Settings"), _T("Gain"), 0);
	m_bAutoShutter = g_BaumerProfile.GetProfileInt(_T("Settings"), _T("AutoShutter"), FALSE);
	m_nShutter = g_BaumerProfile.GetProfileInt(_T("Settings"), _T("ShutterMcs"), 1000);
	m_bWBEnable = g_BaumerProfile.GetProfileInt(_T("Settings"), _T("WhiteBalanceEnable"), FALSE);
	m_dWBRed = g_BaumerProfile.GetProfileDouble(_T("Settings"), _T("WhiteBalance-Red"), 1.);
	m_dWBGreen = g_BaumerProfile.GetProfileDouble(_T("Settings"), _T("WhiteBalance-Green"), 1.);
	m_dWBBlue = g_BaumerProfile.GetProfileDouble(_T("Settings"), _T("WhiteBalance-Blue"), 1.);
	m_bCBEnable = g_BaumerProfile.GetProfileInt(_T("Settings"), _T("ColorBalanceEnable"), FALSE);
	m_dCBRed = g_BaumerProfile.GetProfileDouble(_T("Settings"), _T("ColorBalance-Red"), 1.);
	m_dCBGreen = g_BaumerProfile.GetProfileDouble(_T("Settings"), _T("ColorBalance-Green"), 1.);
	m_dCBBlue = g_BaumerProfile.GetProfileDouble(_T("Settings"), _T("ColorBalance-Blue"), 1.);
	m_bOverlay = g_BaumerProfile.GetProfileInt(_T("Settings"), _T("UseOverlay"), 0);
	int nBitsMode = g_BaumerProfile.GetProfileInt(_T("Settings"), _T("BitsMode"), W8BIT);
	int nConversion = g_BaumerProfile.GetProfileInt(_T("Settings"), _T("Conversion"), AutoConversion);
	m_nBitsMode = BitsModeDescrByBM(nBitsMode, nConversion)->nNum;
	m_bAGP = g_BaumerProfile.GetProfileInt(_T("Settings"), _T("DirectX"), TRUE);
	m_bBinning = g_CxLibWork.m_CameraType.cBinnMode?g_BaumerProfile.GetProfileInt(_T("Settings"), _T("Binning"), FALSE):FALSE;
	m_bGrayScale = g_BaumerProfile.GetProfileInt(_T("Settings"), _T("GrayScale"), FALSE);
	m_nPlane = g_BaumerProfile.GetProfileInt(_T("Settings"), _T("Plane"), TRUE);
	m_nLeftMargin = g_BaumerProfile.GetProfileInt(_T("Margins"), _T("Left"), 0);
	m_nLeftMargin = min(m_nLeftMargin,MAX_MARGIN);
	m_nRightMargin = g_BaumerProfile.GetProfileInt(_T("Margins"), _T("Right"), 0);
	m_nRightMargin = min(m_nRightMargin,MAX_MARGIN);
	m_nTopMargin = g_BaumerProfile.GetProfileInt(_T("Margins"), _T("Top"), 0);
	m_nTopMargin = min(m_nTopMargin,MAX_MARGIN);
	m_nBottomMargin = g_BaumerProfile.GetProfileInt(_T("Margins"), _T("Bottom"), 0);
	m_nBottomMargin = min(m_nBottomMargin,MAX_MARGIN);
	m_bEnableMargins = g_BaumerProfile.GetProfileInt(_T("Margins"), _T("Enable"), FALSE);
	m_nProcWhite = g_BaumerProfile.GetProfileInt(_T("Settings"), _T("ProcentWhite"), 0, true);
	m_bNaturalSize = g_BaumerProfile.GetProfileInt(_T("PreviewDlg"), _T("NaturalSize"), FALSE);
	m_nConvStart = g_BaumerProfile.GetProfileInt(_T("Settings"), _T("ConversionStart"), 0, true);
	m_nConvEnd = g_BaumerProfile.GetProfileInt(_T("Settings"), _T("ConversionEnd"), 0x400, true);
	m_dGamma = g_BaumerProfile.GetProfileDouble(_T("Settings"), _T("Gamma"), 1., NULL, true);
	m_nConvStart12 = g_BaumerProfile.GetProfileInt(_T("Settings"), _T("ConversionStart12"), 0, true);
	m_nConvEnd12 = g_BaumerProfile.GetProfileInt(_T("Settings"), _T("ConversionEnd12"), 0x1000, true);
	m_dGamma12 = g_BaumerProfile.GetProfileDouble(_T("Settings"), _T("Gamma12"), 1., NULL, true);
	m_bTriggerMode = !m_bBinning&&!IsDC300(m_nCamera)?g_BaumerProfile.GetProfileInt(_T("Trigger"), _T("TriggerMode"), FALSE, true):FALSE;
	m_bDischarge = g_BaumerProfile.GetProfileInt(_T("Trigger"), _T("Discharge"), FALSE, true);
	m_bDC300QuickView = g_BaumerProfile.GetProfileInt(NULL, _T("DC300QuickView"), 1, true, true);
}

void CSettingsSheet::WriteSettings()
{
	LPCTSTR lpName = CameraIds::CameraDescrByInternalId(m_nCamera)->pszCameraName;
	g_BaumerProfile.WriteProfileString(_T("Settings"), _T("Camera"), lpName, true);
	g_BaumerProfile.WriteProfileInt(_T("Settings"), _T("BlackLevel"), m_nOffset);
	g_BaumerProfile.WriteProfileInt(_T("Settings"), _T("Gain"), m_nGain);
	g_BaumerProfile.WriteProfileInt(_T("Settings"), _T("AutoShutter"), m_bAutoShutter);
	g_BaumerProfile.WriteProfileInt(_T("Settings"), _T("ShutterMcs"), m_nShutter);
	g_BaumerProfile.WriteProfileInt(_T("Settings"), _T("WhiteBalanceEnable"), m_bWBEnable);
	g_BaumerProfile.WriteProfileDouble(_T("Settings"), _T("WhiteBalance-Red"), m_dWBRed);
	g_BaumerProfile.WriteProfileDouble(_T("Settings"), _T("WhiteBalance-Green"), m_dWBGreen);
	g_BaumerProfile.WriteProfileDouble(_T("Settings"), _T("WhiteBalance-Blue"), m_dWBBlue);
	g_BaumerProfile.WriteProfileInt(_T("Settings"), _T("ColorBalanceEnable"), m_bCBEnable);
	g_BaumerProfile.WriteProfileDouble(_T("Settings"), _T("ColorBalance-Red"), m_dCBRed);
	g_BaumerProfile.WriteProfileDouble(_T("Settings"), _T("ColorBalance-Green"), m_dCBGreen);
	g_BaumerProfile.WriteProfileDouble(_T("Settings"), _T("ColorBalance-Blue"), m_dCBBlue);
	g_BaumerProfile.WriteProfileInt(_T("Settings"), _T("UseOverlay"), m_bOverlay);
	BitsModeDescr *pBM = BitsModeDescrByNum(m_nBitsMode);
	g_BaumerProfile.WriteProfileInt(_T("Settings"), _T("BitsMode"), pBM->nBitsMode);
	g_BaumerProfile.WriteProfileInt(_T("Settings"), _T("Conversion"), pBM->nConversion);
	if (!IsWin9x()) g_BaumerProfile.WriteProfileInt(_T("Settings"), _T("DirectX"), m_bAGP);
	g_BaumerProfile.WriteProfileInt(_T("Settings"), _T("Binning"), m_bBinning);
	g_BaumerProfile.WriteProfileInt(_T("Settings"), _T("GrayScale"), m_bGrayScale);
	g_BaumerProfile.WriteProfileInt(_T("Settings"), _T("Plane"), m_nPlane);
	g_BaumerProfile.WriteProfileInt(_T("Margins"), _T("Left"), m_nLeftMargin);
	g_BaumerProfile.WriteProfileInt(_T("Margins"), _T("Right"), m_nRightMargin);
	g_BaumerProfile.WriteProfileInt(_T("Margins"), _T("Top"), m_nTopMargin);
	g_BaumerProfile.WriteProfileInt(_T("Margins"), _T("Bottom"), m_nBottomMargin);
	g_BaumerProfile.WriteProfileInt(_T("Margins"), _T("Enable"), m_bEnableMargins);
	g_BaumerProfile.WriteProfileInt(_T("Settings"), _T("ProcentWhite"), m_nProcWhite);
	g_BaumerProfile.WriteProfileInt(_T("PreviewDlg"), _T("NaturalSize"), m_bNaturalSize);
	g_BaumerProfile.WriteProfileInt(_T("Settings"), _T("ConversionStart"), m_nConvStart);
	g_BaumerProfile.WriteProfileInt(_T("Settings"), _T("ConversionEnd"), m_nConvEnd);
	g_BaumerProfile.WriteProfileDouble(_T("Settings"), _T("Gamma"), m_dGamma);
	g_BaumerProfile.WriteProfileInt(_T("Settings"), _T("ConversionStart12"), m_nConvStart12);
	g_BaumerProfile.WriteProfileInt(_T("Settings"), _T("ConversionEnd12"), m_nConvEnd12);
	g_BaumerProfile.WriteProfileDouble(_T("Settings"), _T("Gamma12"), m_dGamma12);
	g_BaumerProfile.WriteProfileInt(_T("Trigger"), _T("TriggerMode"), m_bTriggerMode);
	g_BaumerProfile.WriteProfileInt(_T("Trigger"), _T("Discharge"), m_bDischarge);
	g_BaumerProfile.WriteProfileInt(NULL, _T("DC300QuickView"), m_bDC300QuickView, true);
}

int __Range(int n, int nMin, int nMax)
{
	if (n < nMin) return nMin;
	if (n > nMax) return nMax;
	return n;
}

void SetSlider(CSliderCtrl &Sld, BOOL bEnable, int nPos, int nMin, int nMax)
{
	Sld.EnableWindow(bEnable);
	Sld.SetRange(nMin, nMax);
	Sld.SetTicFreq(max(1,(nMax-nMin)/10));
	Sld.SetPos(__Range(nPos, nMin, nMax));
}

void SetSlider(CSliderCtrl &Sld, CSpinButtonCtrl &Spin, BOOL bEnable, int nPos, int nMin, int nMax)
{
	Sld.EnableWindow(bEnable);
	Sld.SetRange(nMin, nMax);
	Sld.SetTicFreq(max(1,(nMax-nMin)/10));
	Sld.SetPos(__Range(nPos, nMin, nMax));
	Spin.EnableWindow(bEnable);
	Spin.SetRange(nMin, nMax);
	Spin.SetPos(nPos);
}

void CSettingsSheet::InitCamera()
{
	int nCamera = CameraIds::CameraDescrByInternalId(m_nCamera)->nBaumerId;
	BitsModeDescr *pBitsMode = BitsModeDescrByNum(m_nBitsMode);
	EnterCriticalSection(&g_CritSectionCamera);
	if (g_BaumerProfile.GetProfileInt(NULL,_T("ReinitCamera"),FALSE, true, true) && !m_bCameraDisabled)
		g_CxLibWork.InitCamera(nCamera);
	else if (pBitsMode->nBitsMode != g_CxLibWork.m_nBitsMode && pBitsMode->nConversion != g_CxLibWork.m_nConversion)
	{
		SetPixelWidth(pBitsMode->nBitsMode);
		int p = 3;
		SetScanMode(SM_SSMTC, &p);
		g_CxLibWork.m_nBitsMode = pBitsMode->nBitsMode;
		g_CxLibWork.m_nConversion = pBitsMode->nConversion;
		g_CxLibWork.InitBitnessAndConversion(pBitsMode->nBitsMode, pBitsMode->nConversion, FALSE, -1, -1, 1., -1, -1, 1.);
	}
	else if (m_bDC300QuickView != g_CxLibWork.m_nDC300QuickView)
	{
		g_CxLibWork.m_nDC300QuickView = m_bDC300QuickView;
		g_CxLibWork.InitDC300();
	}
	if (GainEnabled(m_nCamera))
	{
		m_nGain = __Range(m_nGain, g_CxLibWork.m_CameraType.vAmplMin, g_CxLibWork.m_CameraType.vAmplMax);
		SetGain(m_nGain);
	}
	m_nOffset = __Range(m_nOffset, g_CxLibWork.m_CameraType.vOffsetMin, g_CxLibWork.m_CameraType.vOffsetMax);
	SetBlackLevel(m_nOffset);
	int nAutoBrightness = g_BaumerProfile.GetProfileInt(_T("Settings"), _T("AutoShutterBrightness"), 200);
	AutomaticShutter(m_bAutoShutter, nAutoBrightness);
	m_nShutter = Range(m_nShutter, g_CxLibWork.m_sht.timeMin, g_CxLibWork.m_sht.timeMax);
	tSHT sht;
	if (!m_bAutoShutter) SetShutterTime(m_nShutter, &sht);
	CAMERADLLSETS settings;
	settings.VadSearchMode = m_bAGP;
	DLLSettings(&settings);
	SetBinningMode(m_bBinning);
	int p1 = 3; // Second call SetScanMode has set in order to override problem - incorrect size of image
	SetScanMode(SM_SSMTC, &p1); // after cancel of binning 
	if (m_bBinning && m_bWBEnable)
	{
		SetWhiteBalance(FALSE, FALSE, NULL, NULL, NULL);
		SetRgbLut(FALSE, 0, NULL, NULL, NULL);
	}
	else if (!m_bBinning)
	{
		SetWhiteBalance(m_bWBEnable, 1, &m_dWBRed, &m_dWBGreen, &m_dWBBlue);
		SetRgbLut(m_bCBEnable, 1, &m_dCBRed, &m_dCBGreen, &m_dCBBlue);
	}
	if (g_CxLibWork.m_lpbi)
	{
		g_CxLibWork.ReinitSizes();
		g_CxLibWork.m_lpbi->biSizeImage = 0;
		GetCameraInfo(&g_CxLibWork.m_CameraType, sizeof(tCameraType), &g_CxLibWork.m_CameraStatus , sizeof(tCameraStatus));
	}
	g_CxLibWork.m_rcMargins = CRect(m_nLeftMargin, m_nTopMargin, m_nRightMargin, m_nBottomMargin);
	g_CxLibWork.m_bEnableMargins = m_bEnableMargins;
	g_CxLibWork.m_nConvStart = m_nConvStart;
	g_CxLibWork.m_nConvEnd = m_nConvEnd;
	LeaveCriticalSection(&g_CritSectionCamera);
}

void CSettingsSheet::UpdateControls(int nHint)
{
	int nPages = GetPageCount();
	for (int i = 0; i < nPages; i++)
	{
		CSettingsPage *p = (CSettingsPage *)GetPage(i);
		p->UpdateControls(nHint);
	}
	m_Graffic.UpdateControls(nHint);
}

void CSettingsSheet::OnOK()
{
	if (m_bFinished) return;
	m_bFinished = TRUE;
	g_BaumerProfile.WriteProfileInt(_T("SettingsDlg"), _T("Graffic"), m_bGraffic);
	WriteSettings();
}

void CSettingsSheet::OnCancel()
{
	if (m_bFinished) return;
	if (!g_CxLibWork.IsInitedOk()) return;
	m_bFinished = TRUE;
	ReadSettings();
	InitCamera();
	if (m_bOvrModeChanged && m_pSettingsSite)
		m_pSettingsSite->OnChangeOvrMode(m_bOverlay?true:false);
}

void CSettingsSheet::OnApply()
{
	if (!m_bModified) return;
	m_bModified = FALSE;
	WriteSettings();
}


/////////////////////////////////////////////////////////////////////////////
// CSettingsSheet message handlers



BOOL CSettingsSheet::OnInitDialog() 
{
	BOOL bResult = CPropertySheet::OnInitDialog();

	m_bOvrModeChanged = false;
	RestorePosition();
	// Restore active page
	int nActive = g_BaumerProfile.GetProfileInt(_T("SettingsDlg"), _T("ActivePage"), 0);
	SetActivePage(nActive);
	m_bInited = true;
	// Initialize graffic mode
	BOOL bGraffic = g_BaumerProfile.GetProfileInt(_T("SettingsDlg"), _T("Graffic"), 0);
	SetGrafficMode(bGraffic);
	if (m_PageCam.m_hWnd)
	{
		m_PageCam.UpdateData();
		m_PageCam.m_bGraffic = bGraffic;
		m_PageCam.UpdateData(FALSE);
	}
	if (g_BaumerProfile.GetProfileInt(_T("Settings"), _T("HideHelp"), 0))
		::ShowWindow(::GetDlgItem(m_hWnd, IDHELP), SW_HIDE);

	m_sptrDrv3->AddDriverSite(0,this);

	return bResult;
}

void CSettingsSheet::RestorePosition()
{
	// Move to saved position
	if (m_bOvrMode)
	{
		int nSavedX1 = g_BaumerProfile.GetProfileInt(_T("SettingsDlg"), _T("OvrSavedX1"), -1);
		int nSavedY1 = g_BaumerProfile.GetProfileInt(_T("SettingsDlg"), _T("OvrSavedY1"), -1);
		if (nSavedX1 != -1 && nSavedY1 != -1)
			SetWindowPos(NULL, nSavedX1, nSavedY1, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
	}
	else
	{
		int nSavedX1 = g_BaumerProfile.GetProfileInt(_T("SettingsDlg"), _T("NonOvrSavedX1"), -1);
		int nSavedY1 = g_BaumerProfile.GetProfileInt(_T("SettingsDlg"), _T("NonOvrSavedY1"), -1);
		if (nSavedX1 != -1 && nSavedY1 != -1)
			SetWindowPos(NULL, nSavedX1, nSavedY1, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
	}
}

void CSettingsSheet::OnMove(int x, int y) 
{
	CPropertySheet::OnMove(x, y);
	
	if (m_bInited)
	{
		CRect rc;
		GetWindowRect(rc);
		if (m_bOvrMode)
		{
			g_BaumerProfile.WriteProfileInt(_T("SettingsDlg"), _T("OvrSavedX1"), rc.left);
			g_BaumerProfile.WriteProfileInt(_T("SettingsDlg"), _T("OvrSavedY1"), rc.top);
		}
		else
		{
			g_BaumerProfile.WriteProfileInt(_T("SettingsDlg"), _T("NonOvrSavedX1"), rc.left);
			g_BaumerProfile.WriteProfileInt(_T("SettingsDlg"), _T("NonOvrSavedY1"), rc.top);
		}
	}
	
}


void CSettingsSheet::OnDestroy() 
{
	m_sptrDrv3->RemoveDriverSite(0,this);
	CPropertySheet::OnDestroy();
	g_BaumerProfile.WriteProfileInt(_T("SettingsDlg"), _T("ActivePage"), GetActiveIndex());
}

void CSettingsSheet::SetGrafficMode(BOOL bGrafficMode)
{
	if (bGrafficMode != m_bGraffic)
	{
		if (bGrafficMode)
		{
			GetWindowRect(m_rcNonGraffic);
			CRect rcClient,rc1;
			GetClientRect(rcClient);
			::GetWindowRect(::GetDlgItem(m_hWnd, IDOK), m_rcOk);
			ScreenToClient(m_rcOk);
			::GetWindowRect(::GetDlgItem(m_hWnd, IDCANCEL), m_rcCancel);
			ScreenToClient(m_rcCancel);
			::GetWindowRect(::GetDlgItem(m_hWnd, ID_APPLY_NOW), m_rcApply);
			ScreenToClient(m_rcApply);
			::GetWindowRect(::GetDlgItem(m_hWnd, IDHELP), m_rcHelp);
			ScreenToClient(m_rcHelp);
			CRect rc(0, m_rcOk.top, rcClient.Width(), m_rcOk.top);
			m_Graffic.Create(IDD_HISTO, this);
			m_Graffic.UpdateControls(ESH_Camera_Changed);
			m_Graffic.GetWindowRect(rc1);
			rc.bottom = rc.top+rc1.Height();
			m_Graffic.MoveWindow(rc);
			SetWindowPos(NULL, 0, 0, m_rcNonGraffic.Width(), m_rcNonGraffic.Height()+rc.Height()+5, SWP_NOMOVE|SWP_NOZORDER);
			::SetWindowPos(::GetDlgItem(m_hWnd, IDOK), NULL, m_rcOk.left, m_rcOk.top+rc.Height(), 0, 0, SWP_NOSIZE|SWP_NOZORDER);
			::SetWindowPos(::GetDlgItem(m_hWnd, IDCANCEL), NULL, m_rcCancel.left, m_rcCancel.top+rc.Height(), 0, 0, SWP_NOSIZE|SWP_NOZORDER);
			::SetWindowPos(::GetDlgItem(m_hWnd, ID_APPLY_NOW), NULL, m_rcApply.left, m_rcApply.top+rc.Height(), 0, 0, SWP_NOSIZE|SWP_NOZORDER);
			::SetWindowPos(::GetDlgItem(m_hWnd, IDHELP), NULL, m_rcHelp.left, m_rcHelp.top+rc.Height(), 0, 0, SWP_NOSIZE|SWP_NOZORDER);
		}
		else
		{
			SetWindowPos(NULL, 0, 0, m_rcNonGraffic.Width(), m_rcNonGraffic.Height(), SWP_NOMOVE|SWP_NOZORDER);
			m_Graffic.DestroyWindow();
			::SetWindowPos(::GetDlgItem(m_hWnd, IDOK), NULL, m_rcOk.left, m_rcOk.top, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
			::SetWindowPos(::GetDlgItem(m_hWnd, IDCANCEL), NULL, m_rcCancel.left, m_rcCancel.top, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
			::SetWindowPos(::GetDlgItem(m_hWnd, ID_APPLY_NOW), NULL, m_rcApply.left, m_rcApply.top, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
			::SetWindowPos(::GetDlgItem(m_hWnd, IDHELP), NULL, m_rcHelp.left, m_rcHelp.top, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
		}
		m_bGraffic = bGrafficMode;
	}
}

HRESULT CSettingsSheet::Invalidate()
{
	if (m_bGraffic && !m_bFreezeGraffic && m_Graffic.m_hWnd)
	{
		m_Graffic.m_HistoBox.InitHisto(g_CxLibWork.m_lpbi);
		PostMessage(WM_USER+10);
	}
	return S_OK;
}

HRESULT CSettingsSheet::OnChangeSize()
{
	return S_OK;
}

LRESULT CSettingsSheet::OnUserMsg(WPARAM wParam, LPARAM lParam)
{
	if (m_bGraffic && !m_bFreezeGraffic && m_Graffic.m_hWnd)
		m_Graffic.ResetMinMax();
	return 0L;
}

BOOL CSettingsSheet::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	if (!RunContextHelp(pHelpInfo))
		return CPropertySheet::OnHelpInfo( pHelpInfo );
	return FALSE;
}

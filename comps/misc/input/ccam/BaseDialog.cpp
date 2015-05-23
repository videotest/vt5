// BaseDialog.cpp: implementation of the CBaseDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CCam.h"
#include "BaseDialog.h"
#include "StdProfile.h"
#include "Ctrls.h"
#include "CCamUtils.h"
#include "CCamHelp.h"
#include "ccapi.h"
#include "SmartEdit.h"
#include "Utils.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CBaseDialog, CBaseDlg);


static bool s_bChanging;
BOOL CBaseDialog::m_bHistogram;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBaseDialog::CBaseDialog(int idd, CWnd *pParent, CBaseDialog *pBase) : CBaseDlg(idd, pParent, pBase)
{
	m_bHistogram = FALSE;
	m_nCurColor = 0;
	m_nCurSlope = 0;
	m_bVideo = false;
}

CBaseDialog::~CBaseDialog()
{
}


BEGIN_MESSAGE_MAP(CBaseDialog, CBaseDlg)
	//{{AFX_MSG_MAP(CPreviewDialog)
	ON_WM_HSCROLL()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_HISTOGRAM, OnHistogram)
	ON_BN_CLICKED(IDC_CHECK_GRAYSCALE, OnGrayscale)
	ON_BN_CLICKED(IDC_WB_BUILD, OnWBBuild)
	ON_BN_CLICKED(IDC_DEFAULT, OnDefault)
	ON_CBN_SELCHANGE(IDC_BITNESS, OnBitnessChanged)
	ON_CBN_SELCHANGE(IDC_BITS, OnBitsChanged)
	ON_BN_CLICKED(IDC_GRAY, OnGray)
	ON_BN_CLICKED(IDC_RED, OnRed)
	ON_BN_CLICKED(IDC_GREEN, OnGreen)
	ON_BN_CLICKED(IDC_BLUE, OnBlue)
	ON_CBN_SELCHANGE(IDC_CURRENT_SLOPE, OnCurrentSlopeChanged)
	ON_BN_CLICKED(IDC_BROWSE, OnChangePathName)
	ON_CBN_SELCHANGE(IDC_CAPTURE_RATE_SCALE, OnFPSTypeChanged)
	ON_BN_CLICKED(IDC_CHECK_SAME_AS_CAPTURE, OnSameAsCapture)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static CString _TimeStrByFPSType(int nFPSType)
{
	int nRes = nFPSType==1?IDS_TIME_IN_MINUTES:nFPSType==2?IDS_TIME_IN_HOURS:IDS_TIME_IN_SECONDS;
	CString s;
	s.LoadString(nRes);
	return s;
}

void CBaseDialog::InitControls()
{
	// Init common camera controls
	InitControl(new CSmartIntEdit(IDC_EDIT_GAIN, IDC_SPIN_GAIN, IDC_SLIDER_GAIN, _T("Settings"),
		_T("Gain")));
	InitControl(new CSmartIntEdit(IDC_EDIT_OFFSET, IDC_SPIN_OFFSET, IDC_SLIDER_OFFSET, _T("Settings"),
		_T("Offset")));
	if (CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("UseAnaval2InsteadOffsetBFine"), FALSE, true, true))
		InitControl(new CSmartIntEdit(IDC_EDIT_ODD_OFFSET, IDC_SPIN_ODD_OFFSET, IDC_SLIDER_ODD_OFFSET, _T("Settings"),
			_T("Anaval2")));
	else
		InitControl(new CSmartIntEdit(IDC_EDIT_ODD_OFFSET, IDC_SPIN_ODD_OFFSET, IDC_SLIDER_ODD_OFFSET, _T("Settings"),
			_T("OffsetBFine")));
	// Init multislope - related controls
	VALUEINFOEX vie;
	if (m_sptrDrv != 0 && SUCCEEDED(m_sptrDrv->GetValueInfoByName(0, "Settings", "TotalSlopes", &vie)) &&
		vie.Info.nMax > 1)
	{
		InitControl(new CSmartIntEdit(IDC_EDIT_TOTAL_SLOPES, IDC_SPIN_TOTAL_SLOPES, -1, _T("Settings"),
			_T("TotalSlopes"), this));
		int nSlopes = CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("TotalSlopes"), vie.Info.nDefault, true);;
		InitSlopesCB(nSlopes);
		InitCurSlope(0);
	}
	else
	{
		InitControl(new CSmartSpanEdit(IDC_EDIT_EXP_TIME, IDC_SPIN_EXP_TIME, IDC_SLIDER_TIME,
			_T("Settings"),	_T("IntegrationTime"), 100, 1000000, 50000));
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_EDIT_TOTAL_SLOPES), FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_SPIN_TOTAL_SLOPES), FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_CURRENT_SLOPE), FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_EDIT_LEVEL), FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_SPIN_LEVEL), FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_SLIDER_LEVEL), FALSE);
	}

	// Init color balance controls
	if (CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("UseContrastForWhiteBalance"), TRUE, true, true))
	{
		InitControl(new CSmartIntEdit(IDC_EDIT_WB_RED, IDC_SPIN_WB_RED, IDC_SLIDER_WB_RED, _T("Settings"),
			_T("RedContrast"), -200, 200, 0));
		InitControl(new CSmartIntEdit(IDC_EDIT_WB_GREEN, IDC_SPIN_WB_GREEN, IDC_SLIDER_WB_GREEN, _T("Settings"),
			_T("GreenContrast"), -200, 200, 0));
		InitControl(new CSmartIntEdit(IDC_EDIT_WB_BLUE, IDC_SPIN_WB_BLUE, IDC_SLIDER_WB_BLUE, _T("Settings"),
			_T("BlueContrast"), -200, 200, 0));
	}
	else
	{
		InitControl(new CSmartIntEdit(IDC_EDIT_WB_RED, IDC_SPIN_WB_RED, IDC_SLIDER_WB_RED, _T("Settings"),
			_T("RedLuminance"), -200, 200, 0));
		InitControl(new CSmartIntEdit(IDC_EDIT_WB_GREEN, IDC_SPIN_WB_GREEN, IDC_SLIDER_WB_GREEN, _T("Settings"),
			_T("GreenLuminance"), -200, 200, 0));
		InitControl(new CSmartIntEdit(IDC_EDIT_WB_BLUE, IDC_SPIN_WB_BLUE, IDC_SLIDER_WB_BLUE, _T("Settings"),
			_T("BlueLuminance"), -200, 200, 0));
	}
	InitCurColor(m_nCurColor);
	InitControl(new CSmartIntEdit(IDC_EDIT_SATURATION, IDC_SPIN_SATURATION, IDC_SLIDER_SATURATION, _T("Settings"),
		_T("Saturation"), 0, 8, 0));
	// Inite grab window controls
	CString sFrameSec = m_bVideo?_T("Video"):_T("Settings");
	InitControl(new CSmartIntEdit(IDC_EDIT_LEFT, IDC_SPIN_LEFT, IDC_SLIDER_LEFT, sFrameSec,
		_T("StartX"), 0, 640, 0, CSmartIntEdit::ThumbPosition));
	InitControl(new CSmartIntEdit(IDC_EDIT_WIDTH, IDC_SPIN_WIDTH, IDC_SLIDER_WIDTH, sFrameSec,
		_T("Width"), 640, 1280, 1280, CSmartIntEdit::ThumbPosition));
	InitControl(new CSmartIntEdit(IDC_EDIT_TOP, IDC_SPIN_TOP, IDC_SLIDER_TOP, sFrameSec,
		_T("StartY"), 0, 512, 0, CSmartIntEdit::ThumbPosition));
	InitControl(new CSmartIntEdit(IDC_EDIT_HEIGHT, IDC_SPIN_HEIGHT, IDC_SLIDER_HEIGHT, sFrameSec,
		_T("Height"), 512, 1024, 1024, CSmartIntEdit::ThumbPosition));
	InitControl(new CCheckboxHandler(IDC_CAPTURE_FULL_FRAME, sFrameSec, _T("CaptureFullFrame"), FALSE));
	InitControl(new CComboboxHandler(IDC_COMBO_FORMATS, sFrameSec, _T("GrabWindowSize"), 0));
	InitControl(new CCheckboxHandler(IDC_CHECK_VERTICAL_MIRROR, _T("Mirror"), _T("Vertical"), FALSE));
	InitControl(new CCheckboxHandler(IDC_CHECK_HORIZONTAL_MIRROR, _T("Mirror"), _T("Horizontal"), FALSE));
	InitControl(new CSmartIntEdit(IDC_FPS,-1,-1,_T("Video"),_T("FPS"),1,60,15,0));
	int nFPSType = CStdProfileManager::m_pMgr->GetProfileInt(_T("Video"), _T("FPSType"), 0, true);
	::InitComboBox(m_hWnd,IDC_CAPTURE_RATE_SCALE,nFPSType);
	BOOL bSameAsCapture = CStdProfileManager::m_pMgr->GetProfileInt(_T("Video"), _T("PlaybackSameAsCapture"), TRUE, true);
	CheckDlgButton(IDC_CHECK_SAME_AS_CAPTURE, bSameAsCapture);
	InitControl(new CSmartIntEdit(IDC_PLAYBACK_FPS,-1,-1,_T("Video"),_T("PlaybackFPS"),1,60,15,0));
	InitControl(new CComboboxHandler(IDC_PLAYBACK_RATE_SCALE, _T("Video"), _T("PlaybackFPSType"), 0));
	if (bSameAsCapture)
	{
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_PLAYBACK_FPS), FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_PLAYBACK_RATE_SCALE), FALSE);
	}
	::SetWindowText(::GetDlgItem(m_hWnd, IDC_TIME), _TimeStrByFPSType(nFPSType));
	InitControl(new CSmartIntEdit(IDC_TIME_LIM,-1,-1,_T("Video"),_T("Time"),1,60,1,0));
	CString sFile = CStdProfileManager::m_pMgr->GetProfileString(_T("Video"), _T("PathName"), _T("c:\\capture.avi"), true);
	::SetWindowText(::GetDlgItem(m_hWnd,IDC_FILE),sFile);
}

void CBaseDialog::InitCurColor(int n)
{
	m_nCurColor = n;
	RemoveControl(IDC_EDIT_BRIGHTNESS);
	RemoveControl(IDC_EDIT_CONTRAST);
	RemoveControl(IDC_EDIT_GAMMA);
	if (m_nCurColor == 0)
	{
		InitControl(new CSmartIntEdit(IDC_EDIT_BRIGHTNESS, IDC_SPIN_BRIGHTNESS, IDC_SLIDER_BRIGHTNESS,
			_T("Settings"),	_T("RedLuminance"), -200, 200, 0));
		InitControl(new CSmartIntEdit(IDC_EDIT_CONTRAST, IDC_SPIN_CONTRAST, IDC_SLIDER_CONTRAST,
			_T("Settings"),	_T("RedContrast"), -200, 200, 0));
		InitControl(new CSmartIntEdit(IDC_EDIT_GAMMA, IDC_SPIN_GAMMA, IDC_SLIDER_GAMMA,
			_T("Settings"),	_T("RedGamma"), 0, 200, 100));
	}
	else if (m_nCurColor == 1)
	{
		InitControl(new CSmartIntEdit(IDC_EDIT_BRIGHTNESS, IDC_SPIN_BRIGHTNESS, IDC_SLIDER_BRIGHTNESS,
			_T("Settings"),	_T("GreenLuminance"), -200, 200, 0));
		InitControl(new CSmartIntEdit(IDC_EDIT_CONTRAST, IDC_SPIN_CONTRAST, IDC_SLIDER_CONTRAST,
			_T("Settings"),	_T("GreenContrast"), -200, 200, 0));
		InitControl(new CSmartIntEdit(IDC_EDIT_GAMMA, IDC_SPIN_GAMMA, IDC_SLIDER_GAMMA,
			_T("Settings"),	_T("GreenGamma"), 0, 200, 100));
	}
	else if (m_nCurColor == 2)
	{
		InitControl(new CSmartIntEdit(IDC_EDIT_BRIGHTNESS, IDC_SPIN_BRIGHTNESS, IDC_SLIDER_BRIGHTNESS,
			_T("Settings"),	_T("BlueLuminance"), -200, 200, 0));
		InitControl(new CSmartIntEdit(IDC_EDIT_CONTRAST, IDC_SPIN_CONTRAST, IDC_SLIDER_CONTRAST,
			_T("Settings"),	_T("BlueContrast"), -200, 200, 0));
		InitControl(new CSmartIntEdit(IDC_EDIT_GAMMA, IDC_SPIN_GAMMA, IDC_SLIDER_GAMMA,
			_T("Settings"),	_T("BlueGamma"), 0, 200, 100));
	}
	::CheckRadioButton(m_hWnd, IDC_RED, IDC_BLUE, IDC_RED+m_nCurColor);
	if (CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("DisableCaptureFullFrame"), TRUE, true, true) &&
		::GetDlgItem(m_hWnd, IDC_CAPTURE_FULL_FRAME))
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_CAPTURE_FULL_FRAME), FALSE);
}

void CBaseDialog::InitCurSlope(int n)
{
	m_nCurSlope = n;
	RemoveControl(IDC_EDIT_EXP_TIME);
	RemoveControl(IDC_EDIT_LEVEL);
	if (m_nCurSlope == 0)
	{
		InitControl(new CSmartSpanEdit(IDC_EDIT_EXP_TIME, IDC_SPIN_EXP_TIME, IDC_SLIDER_TIME,
			_T("Settings"),	_T("IntegrationTime")));
		InitControl(new CSmartIntEdit(IDC_EDIT_LEVEL, IDC_SPIN_LEVEL, IDC_SLIDER_LEVEL,
			_T("Settings"),	_T("Anaval3")));
	}
	else
	{
		// Calculate maximal integration time and reset values
		int nIntTime,nIntTimeMax,nIntTimeDef;
		int nResetValue,nResetValueMin,nResetValueDef;
		m_sptrDrv->GetValueInt(m_nDev, _bstr_t("Settings"), _bstr_t("IntegrationTime"), &nIntTimeMax, 0);
		m_sptrDrv->GetValueInt(m_nDev, _bstr_t("Settings"), _bstr_t("Anaval3"), &nResetValueMin, 0);
		char szBuff[255];
		int n1;
		for (int i = 1; i < m_nCurSlope; i++)
		{
			sprintf(szBuff, "IntegrationTimeProc_%d", i);
			n1 = -1;
			m_sptrDrv->GetValueInt(m_nDev, _bstr_t("Settings"), _bstr_t(szBuff), &n1, 0);
			nIntTimeMax = nIntTimeMax*n1/1000;
			sprintf(szBuff, "Anaval1Extra_%d", i);
			n1 = -1;
			m_sptrDrv->GetValueInt(m_nDev, _bstr_t("Settings"), _bstr_t(szBuff), &n1, 0);
			nResetValueMin += n1;
		}
		// Obtain current and default values of the integration time and reset voltage
		VALUEINFOEX vie;
		sprintf(szBuff, "IntegrationTimeProc_%d", m_nCurSlope);
		n1 = -1;
		m_sptrDrv->GetValueInt(m_nDev, _bstr_t("Settings"), _bstr_t(szBuff), &n1, 0);
		nIntTime = nIntTimeMax*n1/1000;
		m_sptrDrv->GetValueInfoByName(0, "Settings", szBuff, &vie);
		nIntTimeDef = nIntTime*vie.Info.nDefault/1000;
		m_nIntTimeMax = nIntTimeMax;
		sprintf(szBuff, "Anaval1Extra_%d", m_nCurSlope);
		n1 = -1;
		m_sptrDrv->GetValueInt(m_nDev, _bstr_t("Settings"), _bstr_t(szBuff), &n1, 0);
		nResetValue = nResetValueMin+n1;
		m_sptrDrv->GetValueInfoByName(0, "Settings", szBuff, &vie);
		nResetValueDef = nResetValueMin+vie.Info.nDefault;
		m_nResetValueMin = nResetValueMin;
		// Obtain default integration time and reset values
		InitControl(new CSmartSpanEdit(IDC_EDIT_EXP_TIME, IDC_SPIN_EXP_TIME, IDC_SLIDER_TIME,
			1, nIntTimeMax, nIntTimeDef, nIntTime, this));
		InitControl(new CSmartIntEdit(IDC_EDIT_LEVEL, IDC_SPIN_LEVEL, IDC_SLIDER_LEVEL,
			nResetValueMin, 255, nResetValueDef, nResetValue, this));
	}
}

void CBaseDialog::InitSlopesCB(int nSlopes)
{
	ResetComboBox(m_hWnd, IDC_CURRENT_SLOPE);
	for (int i = 0; i < nSlopes; i++)
	{
		char szBuff[25];
		_itoa(i+1, szBuff, 10);
		AddComboBoxString(m_hWnd,IDC_CURRENT_SLOPE,szBuff);
	}
	::InitComboBox(m_hWnd,IDC_CURRENT_SLOPE,m_nCurSlope);
}

void CBaseDialog::OnCurrentSlopeChanged()
{
	int n = ::GetComboBoxValue(m_hWnd, IDC_CURRENT_SLOPE);
	if (n >= 0)
		InitCurSlope(n);
}

void CBaseDialog::OnChangeValue(CSmartIntEdit *pEdit, int nValue)
{
	char szBuff[256];
	if (pEdit)
	{
		if (pEdit->IsHandler(IDC_EDIT_TOTAL_SLOPES))
		{
			if (m_nCurSlope >= nValue)
				InitCurSlope(nValue-1);
			InitSlopesCB(nValue);
		}
		else if (pEdit->IsHandler(IDC_EDIT_EXP_TIME))
		{
			int nValProc = nValue*1000/m_nIntTimeMax;
			sprintf(szBuff, "IntegrationTimeProc_%d", m_nCurSlope);
			m_sptrDrv->SetValueInt(m_nDev, _bstr_t("Settings"), _bstr_t(szBuff), nValProc, 1);
		}
		else if (pEdit->IsHandler(IDC_EDIT_LEVEL))
		{
			int nValExtra = nValue-m_nResetValueMin;
			sprintf(szBuff, "Anaval1Extra_%d", m_nCurSlope);
			m_sptrDrv->SetValueInt(m_nDev, _bstr_t("Settings"), _bstr_t(szBuff), nValExtra, 1);
		}
	}
}


void CBaseDialog::InitBitsCB(int datamode)
{
	int bpp = BitnessByDatamode(datamode);
	ResetComboBox(m_hWnd, IDC_BITS);
	int nSel = 0;
	if (bpp==8)
	{
		AddComboBoxString(m_hWnd, IDC_BITS, IDS_7TO0);
		AddComboBoxString(m_hWnd, IDC_BITS, IDS_8TO1);
		AddComboBoxString(m_hWnd, IDC_BITS, IDS_9TO2);
		AddComboBoxString(m_hWnd, IDC_BITS, IDS_10TO3);
		AddComboBoxString(m_hWnd, IDC_BITS, IDS_11TO4);
		nSel = datamode==CC_DATA_8BIT_11_DOWNTO_4?4:datamode==CC_DATA_8BIT_10_DOWNTO_3?3:
			datamode==CC_DATA_8BIT_9_DOWNTO_2?2:datamode==CC_DATA_8BIT_8_DOWNTO_1?1:0;
	}
	else if (bpp==10)
	{
		AddComboBoxString(m_hWnd, IDC_BITS, IDS_9TO0);
		AddComboBoxString(m_hWnd, IDC_BITS, IDS_11TO2);
		nSel = datamode==CC_DATA_16BIT_9_DOWNTO_0?0:1;
	}
	else
		AddComboBoxString(m_hWnd, IDC_BITS, IDS_11TO0);
	InitComboBox(m_hWnd, IDC_BITS, nSel);
}


void CBaseDialog::InitControlsState()
{
	CLock lock(&s_bChanging);
	InitControls();
	int datamode = CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("Datamode"), CC_DATA_8BIT_11_DOWNTO_4, true);
	int bpp = BitnessByDatamode(datamode);
	InitComboBox(m_hWnd, IDC_BITNESS, bpp==12?2:bpp==10?1:0);
	InitBitsCB(datamode);
	if (!CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("ColorCamera"), TRUE, true, true))
	{
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_COLOR_BALANCE), FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_CHECK_GRAYSCALE), FALSE);
		::CheckDlgButton(m_hWnd, IDC_CHECK_GRAYSCALE, TRUE);
	}
	else
	{
		BOOL bColor = CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("Color"), false, true);
		::CheckDlgButton(m_hWnd, IDC_CHECK_GRAYSCALE, !bColor);
	}
	m_bHistogram = CStdProfileManager::m_pMgr->GetProfileInt(_T("Histogram"), _T("Show"), FALSE);
	::CheckDlgButton(m_hWnd, IDC_HISTOGRAM, m_bHistogram?1:0);
	::CheckRadioButton(m_hWnd, IDC_RED, IDC_BLUE, IDC_RED+m_nCurColor);
	CBaseDlg::InitControlsState();
}

void CBaseDialog::OnBitnessChanged()
{
	int nBitness = GetComboBoxValue(m_hWnd, IDC_BITNESS);
	int datamode = nBitness==0?CC_DATA_8BIT_11_DOWNTO_4:nBitness==1?
		CC_DATA_16BIT_11_DOWNTO_2:CC_DATA_16BIT_11_DOWNTO_0;
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Settings"), _T("Datamode"), datamode);
	m_sptrDrv->SetValueInt(m_nDev, _bstr_t("Settings"), _bstr_t("Datamode"), datamode, 0);
	InitBitsCB(datamode);
}

void CBaseDialog::OnBitsChanged()
{
	int nBitness = GetComboBoxValue(m_hWnd, IDC_BITNESS);
	int nBits = GetComboBoxValue(m_hWnd, IDC_BITS);
	int datamode;
	if (nBitness==0) // 8 bit
		datamode = nBits==0?CC_DATA_8BIT_7_DOWNTO_0:nBits==1?CC_DATA_8BIT_8_DOWNTO_1:
			nBits==2?CC_DATA_8BIT_9_DOWNTO_2:nBits==3?CC_DATA_8BIT_10_DOWNTO_3:
			CC_DATA_8BIT_11_DOWNTO_4;
	else if (nBitness==1) // 10 bit
		datamode = nBits==0?CC_DATA_16BIT_9_DOWNTO_0:CC_DATA_16BIT_11_DOWNTO_2;
	else // 12 bit
		datamode = CC_DATA_16BIT_11_DOWNTO_0;
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Settings"), _T("Datamode"), datamode);
	m_sptrDrv->SetValueInt(m_nDev, _bstr_t("Settings"), _bstr_t("Datamode"), datamode, 0);
	InitBitsCB(datamode);
}

void CBaseDialog::OnGrayscale()
{
	BOOL bGrayScale = IsDlgButtonChecked(IDC_CHECK_GRAYSCALE);
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Settings"), _T("Color"), !bGrayScale);
	m_sptrDrv->SetValueInt(m_nDev, _bstr_t("Settings"), _bstr_t("Color"), !bGrayScale, 0);
}

void CBaseDialog::OnWBBuild()
{
	m_sptrDrv->SetValueInt(m_nDev, _bstr_t("Settings"), _bstr_t("###WBBuild"), 0, 0);
	CDrvControl *p = FindControl(IDC_EDIT_WB_RED);
	if (p) p->Reset();
	p = FindControl(IDC_EDIT_WB_GREEN);
	if (p) p->Reset();
	p = FindControl(IDC_EDIT_WB_BLUE);
	if (p) p->Reset();
	p = FindControl(IDC_EDIT_BRIGHTNESS);
	if (p) p->Reset();
	p = FindControl(IDC_EDIT_CONTRAST);
	if (p) p->Reset();
	p = FindControl(IDC_EDIT_GAMMA);
	if (p) p->Reset();
}

void CBaseDialog::OnHistogram()
{
	if (s_bChanging) return;
	m_bHistogram = IsDlgButtonChecked(IDC_HISTOGRAM);
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Histogram"), _T("Show"), m_bHistogram);
	OnLayoutChanged();
	if (m_pBase)
		m_pBase->OnLayoutChanged();
}

void CBaseDialog::OnGray()
{
}

void CBaseDialog::OnRed()
{
	InitCurColor(0);
}

void CBaseDialog::OnGreen()
{
	InitCurColor(1);
}

void CBaseDialog::OnBlue()
{
	InitCurColor(2);
}

void CBaseDialog::OnFPSTypeChanged()
{
	int nFPSType = GetComboBoxValue(m_hWnd, IDC_CAPTURE_RATE_SCALE);
	nFPSType = Range(nFPSType,0,2);
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Video"), _T("FPSType"), nFPSType);
	::SetWindowText(::GetDlgItem(m_hWnd, IDC_TIME), _TimeStrByFPSType(nFPSType));
}

void CBaseDialog::OnChangePathName()
{
	CString	strFilter = "AVI files (*.AVI)|*.AVI|All files (*.*)|*.*||";
	CString sFile = CStdProfileManager::m_pMgr->GetProfileString(_T("Video"), _T("PathName"), _T("c:\\capture.avi"), true);
	CFileDialog dlg( FALSE, ".AVI", sFile, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		 strFilter, 0, _FILE_OPEN_SIZE_ );  // common/defs.h
	if( dlg.DoModal()==IDOK )
	{
		sFile = dlg.GetPathName();
		TCHAR szDrive[_MAX_DRIVE],szRootPath[_MAX_PATH];
		_tsplitpath(sFile, szDrive, NULL, NULL, NULL);
		_tmakepath(szRootPath, szDrive, _T("\\"), NULL, NULL);
		UINT nDriveType = GetDriveType(szRootPath);
		if (nDriveType == DRIVE_FIXED || nDriveType == DRIVE_RAMDISK || nDriveType == DRIVE_REMOTE &&
			CStdProfileManager::m_pMgr->GetProfileInt(_T("Video"), _T("EnableRemote"), FALSE, true, false))
		{
			CStdProfileManager::m_pMgr->WriteProfileString(_T("Video"), _T("PathName"), sFile);
			::SetWindowText(::GetDlgItem(m_hWnd,IDC_FILE),sFile);
		}
		else
			AfxMessageBox(IDS_NETWORK_PATH, MB_OK|MB_ICONEXCLAMATION);
	}
}

void CBaseDialog::OnSameAsCapture()
{
	BOOL bSameAsCapture = IsDlgButtonChecked(IDC_CHECK_SAME_AS_CAPTURE);
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Video"), _T("PlaybackSameAsCapture"), TRUE, true);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_PLAYBACK_FPS), !bSameAsCapture);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_PLAYBACK_RATE_SCALE), !bSameAsCapture);
}


void CBaseDialog::OnDefault()
{
	CBaseDlg::OnDefault();
	if (IsControl(m_hWnd, IDC_BITNESS)||IsControl(m_hWnd, IDC_BITS))
	{
		InitComboBox(m_hWnd, IDC_BITNESS, 0);
		InitBitsCB(CC_DATA_8BIT_11_DOWNTO_4);
		CStdProfileManager::m_pMgr->WriteProfileInt(_T("Settings"), _T("Datamode"), CC_DATA_8BIT_11_DOWNTO_4);
		m_sptrDrv->SetValueInt(m_nDev, _bstr_t("Settings"), _bstr_t("Datamode"), CC_DATA_8BIT_11_DOWNTO_4, 0);
	}
	if (IsControl(m_hWnd, IDC_CHECK_GRAYSCALE))
	{
		::CheckDlgButton(m_hWnd, IDC_CHECK_GRAYSCALE, TRUE);
		CStdProfileManager::m_pMgr->WriteProfileInt(_T("Settings"), _T("Color"), FALSE);
		m_sptrDrv->SetValueInt(m_nDev, _bstr_t("Settings"), _bstr_t("Color"), FALSE, 0);
	}
	if (IsControl(m_hWnd, IDC_COMBO_FORMATS))
	{
		CStdProfileManager::m_pMgr->WriteProfileInt(_T("Settings"), _T("StartX1"), -1); // 1024 x 768
		CStdProfileManager::m_pMgr->WriteProfileInt(_T("Settings"), _T("StartY1"), -1);
		CStdProfileManager::m_pMgr->WriteProfileInt(_T("Settings"), _T("StartX2"), -1); // 800 x 600
		CStdProfileManager::m_pMgr->WriteProfileInt(_T("Settings"), _T("StartY2"), -1);
		CStdProfileManager::m_pMgr->WriteProfileInt(_T("Settings"), _T("StartX3"), -1); // 640 x 480
		CStdProfileManager::m_pMgr->WriteProfileInt(_T("Settings"), _T("StartY3"), -1);
	}
}


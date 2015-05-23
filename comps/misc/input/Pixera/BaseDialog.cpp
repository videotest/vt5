// BaseDialog.cpp: implementation of the CBaseDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pixera.h"
#include "BaseDialog.h"
#include "PixSdk.h"
#include "StdProfile.h"
#include "Settings.h"
#include "PixHelp.h"
#include <math.h>
#include "PreviewDialog.h"
#include "DrvPreview.h"




//sergey 24/01/06

#include "MethCombo.h"
#include "MethNameDlg.h"
#include "SmartEdit.h"
#include "CamValues.h"
//#include "Ctrls.h"
//end

#include "comutil.h"
//#include "Utils.h"



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
FILE *fp;




IMPLEMENT_DYNAMIC(CBaseDialog, CDialog);

static int Gradations[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 20, 30, 40, 50, 60, 70, 80, 90,
100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000,
9000, 10000, 20000, 30000, 40000, 50000, 60000, 70000, 80000, 90000, 100000, 200000, 300000,
400000, 500000, 600000};
const int nGradations = sizeof(Gradations)/sizeof(Gradations[0]);

int GradationByValue(int nValue)
{
	for (int i = 0; i < nGradations; i++)
	{
		if (nValue <= Gradations[i])
			return i;
	}
	return nGradations-1;
}

int ValueByGradation(int nGradation) {return Gradations[nGradation];}

static CString TimeString(int nTime)
{
	CString s;
	CString sMs,sMks,sS;
	sMs.LoadString(IDS_MS);
	sMks.LoadString(IDS_MKS);
	sS.LoadString(IDS_SEC);
	if (nTime > 1000000)
		s.Format(_T("%d%s"), nTime/1000000, (LPCTSTR)sS);
	else if (nTime > 1000)
		s.Format(_T("%d%s"), nTime/1000, (LPCTSTR)sMs);
	else
		s.Format(_T("%d%s"), nTime, (LPCTSTR)sMks);
	return s;
}

static int StringToTime(LPCTSTR lpstrTime)
{
	CString s(lpstrTime);
	int n = 0;
	while(_istdigit(s[n]))
		n++;
	if (n > 0)
	{
		CString sVal = s.Left(n);
		CString sUnit = s.Mid(n);
		CString sMs,sMks,sS;
		sMs.LoadString(IDS_MS);
		sMks.LoadString(IDS_MKS);
		sS.LoadString(IDS_SEC);
		int nVal = _ttoi(sVal);
		if (sUnit.CompareNoCase(sS) == 0)
			nVal *= 1000000;
		else if (sUnit.CompareNoCase(sMs) == 0)
			nVal *= 1000;
		return nVal;
	}
	return 0;
};

static LPCTSTR aszNames[13] =
{_T("-2"), _T("-1 2/3"), _T("-1 1/3"), _T("-1"), _T("-2/3"), _T("-1/3"), _T("0"),
_T("1/3"), _T("2/3"), _T("1"), _T("1 1/3"), _T("1 2/3"), _T("2")};

void SetDlgItemDouble1(HWND hWnd, int nID, double dVal, int nDigits = 3)
{
	CString s;
	if ((double)(int)dVal == dVal)
		s.Format("%.0f", dVal);
	else
	{
		CString sFmt;
		sFmt.Format(_T("%%.%df"), nDigits);
		s.Format(sFmt, dVal);
	}
	::SetDlgItemText(hWnd, nID, s);
}

double GetDlgItemDouble(HWND hWnd, int nID)
{
	TCHAR szBuff[50];
	::GetDlgItemText(hWnd, nID, szBuff, 50);
	return _tcstod(szBuff, NULL);
}
/*
class CLock
{
public:
	bool *m_pbLockVar;
	bool m_bPrev;
	CLock(bool *pbLockVar)
	{
		m_bPrev = *pbLockVar;
		m_pbLockVar = pbLockVar;
		*pbLockVar = true;
	};
	~CLock()
	{
		*m_pbLockVar = m_bPrev;
	}
};*/

static bool s_bChanging;
BOOL CBaseDialog::m_bHistogram;
//sergey 05/02//06
int CBaseDialog::m_bChancedProf;
int CBaseDialog::m_bbuld;
int CBaseDialog::m_bNotB;
//end


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBaseDialog::CBaseDialog(int idd, CWnd *pParent, CBaseDialog *pBase) : CDialog(idd, pParent)
{
	m_bDelayedInitCtrllSite = false;
	m_bFocus = CAM_FF_IsRunning()?true:false;
	m_pBase = pBase;
	m_bHistogram = FALSE;
	m_bDisableAllCtrls = true;
	m_bInited = false;
	//sergey 24/01/06
	
	g_Methodics.SetId(IDC_LIST11);

	//CMethNameDlg::s_idd = IDD_DIALOG_SETTING_NAME11;
	//CMethNameDlg::s_idEdit = IDC_EDIT11;

	 m_nIdNewMeth = IDC_BUTTON_NEW_SETTINGS11;
    	m_nIdDeleteMeth = IDC_BUTTON_DELETE_SETTINGS11;
		m_bSettingsChanged = true; // Settings not initialized
		//CListboxHandler ListHend(IDC_LIST11,_T("Methodics"), _T("Default"), 0);

		//CMethodicsComboValue::OnInitControl();
		
		
	
	
	//end
}

CBaseDialog::~CBaseDialog()
{
	DestroyControls();
}


BEGIN_MESSAGE_MAP(CBaseDialog, CDialog)
	//{{AFX_MSG_MAP(CPreviewDialog)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_SHUTTER_AUTO,OnAutoExposure)
	ON_EN_CHANGE(IDC_EDIT_EXP_TIME,OnExpChange)
	ON_EN_CHANGE(IDC_EDIT_SENSITIVITY,OnSensitivityChange)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_EXP_TIME, OnDeltaposExpTime)
	ON_CBN_SELCHANGE(IDC_COMBO_SENSITIVITY, OnSelchangeComboSensitivity)
	ON_CBN_SELCHANGE(IDC_COMBO_CHANNEL, OnSelchangeLevelAdjustment)
	ON_EN_CHANGE(IDC_EDIT_SHADOW,OnShadowChange)
	ON_EN_CHANGE(IDC_EDIT_GAMMA,OnGammaChange)
	ON_EN_CHANGE(IDC_EDIT_HIGHLIGHT,OnHighlightChange)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_GAMMA, OnDeltaposSpinGamma)
	ON_CBN_SELCHANGE(IDC_COMBO_ACCUMULATION, OnSelchangeAccumulationMode)
	ON_EN_CHANGE(IDC_EDIT_ACCUMULATION_TIMES,OnAccumulateTimes)
	ON_EN_KILLFOCUS(IDC_EDIT_ACCUMULATION_TIMES,OnKillFocusAccumulateTimes)
	ON_BN_CLICKED(IDC_WB_OFF,OnWBChange)
	ON_BN_CLICKED(IDC_WB_AUTO,OnWBChange)
	ON_BN_CLICKED(IDC_WB_ONE_PUSH,OnWBChange)
	ON_BN_CLICKED(IDC_WB_MANUAL,OnWBChange)
	ON_EN_CHANGE(IDC_EDIT_WB_RED,OnWBRedChange)
	ON_EN_CHANGE(IDC_EDIT_WB_GREEN,OnWBGreenChange)
	ON_EN_CHANGE(IDC_EDIT_WB_BLUE,OnWBBlueChange)
	ON_BN_CLICKED(IDC_SHOW_WB_RECT,OnWBShowRect)
	ON_BN_CLICKED(IDC_WB_BUILD,OnWBBuild)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_WB_RED, OnDeltaposSpinRed)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_WB_GREEN, OnDeltaposSpinGreen)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_WB_BLUE, OnDeltaposSpinBlue)
	ON_BN_CLICKED(IDC_CHECK_FLIP_VERT,OnFlip)
	ON_BN_CLICKED(IDC_CHECK_FLIP_HORZ,OnFlip)
	ON_CBN_SELCHANGE(IDC_COMBO_PREVIEW, OnSelchangePreviewMode)
	ON_CBN_SELCHANGE(IDC_COMBO_CAPTURE, OnSelchangeCaptureMode)
	ON_BN_CLICKED(IDC_CHECK_GRAYSCALE,OnGrayScale)
	ON_BN_CLICKED(IDC_MICRO_BRIGHT,OnMicro)
	ON_BN_CLICKED(IDC_MICRO_FLUORESCENT,OnMicro)
	ON_BN_CLICKED(IDC_CHECK_FOCUS,OnFocus)
	ON_BN_CLICKED(IDC_SHOW_FOCUS_RECT,OnFocusRect)
	ON_BN_CLICKED(IDC_CHECK_BB,OnBBChange)
	ON_BN_CLICKED(IDC_HISTOGRAM,OnHistogram)
	ON_BN_CLICKED(IDC_SHOW_BB_RECT,OnBBShowRect)
	ON_BN_CLICKED(IDC_SHOW_EXPOSURE_RECTANGLE,OnAEShowRect)
	ON_BN_CLICKED(IDC_DEFAULT,OnDefault)
	ON_BN_CLICKED(IDOK,OnOK)
	ON_BN_CLICKED(IDCANCEL,OnCancel)
	ON_WM_TIMER()
	//sergey 11/01/2006
	ON_EN_CHANGE(IDC_EDIT1,OnEdit1Change)
	ON_BN_CLICKED (IDC_BUTTON_NEW_SETTINGS,OnNew)
	ON_BN_CLICKED (IDC_BUTTON_DELETE_SETTINGS,OnDelete)
	ON_BN_CLICKED (IDC_BUTTON_SAVE_SETTINGS,OnSave)
	ON_BN_CLICKED(IDC_BUTTON_NEW_SETTINGS,OnFileName)
	ON_LBN_DBLCLK(IDC_LIST1,OnRestoreMetodick)
	//ON_BN_CLICKED (IDC_BUTTON_NEW_SETTINGS11,OnNew11)
	//ON_BN_CLICKED(IDCANCEL11,OnCancel11)

	
	ON_EN_KILLFOCUS(IDC_EDIT_SHADOW,OnShadowChange1)
	ON_EN_KILLFOCUS(IDC_EDIT_GAMMA,OnGammaChange1)
	ON_EN_KILLFOCUS(IDC_EDIT_HIGHLIGHT,OnHighlightChange1)
	ON_EN_KILLFOCUS(IDC_EDIT_EXP_TIME,OnExpChange1)
	//end
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


	

void CBaseDialog::InitControls()
{
	HWND hwndChild = ::GetWindow(m_hWnd, GW_CHILD);
	while (hwndChild)
	{
		int nId = GetWindowLong(hwndChild, GWL_ID);
		CWnd *pCtrl = CreateControlForDialog(m_hWnd, nId, hwndChild);
		if (pCtrl)
			m_arrCtrls.Add(pCtrl);
		hwndChild = ::GetWindow(hwndChild, GW_HWNDNEXT);
	}
}

void CBaseDialog::DestroyControls()
{
	for (int i = 0; i < m_arrCtrls.GetSize(); i++)
		delete m_arrCtrls[i];
	m_arrCtrls.RemoveAll();
}

void CBaseDialog::InitAutoExposure()
{
	CString s;
	// Set time/adjust static
	s.LoadString(IDS_ADJUST);
	SetDlgItemText(IDC_STATIC_TIMEADJ, s);
	// Init edit box
	int n = CAM_AE_GetAdjust();
	if (n >= -6 && n <= 6)
		SetDlgItemText(IDC_EDIT_EXP_TIME, aszNames[n+6]);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_EDIT_EXP_TIME), FALSE);
	InitSpin(IDC_SPIN_EXP_TIME, 0, 12, n+6);
	InitSlider(IDC_SLIDER_TIME, 0, 12, n+6, 1);
	// Init spin
}

void CBaseDialog::InitManualExposure()
{
	CString s;
	// Set time/adjust static
	s.LoadString(IDS_TIME);
	SetDlgItemText(IDC_STATIC_TIMEADJ, s);
	// Init edit box
	double d;
	CAM_EXP_GetSpeed(&d); // 1..10000
	s = TimeString(int(d*100.)); // 1..1000000
	//sergey 18/01/06
	if(cwndGlob)
	cwndGlob->SetDlgItemText(IDC_EDIT_EXP_TIME,s);
	//edd
	else SetDlgItemText(IDC_EDIT_EXP_TIME, s);
	
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_EDIT_EXP_TIME), TRUE);
	InitSpin(IDC_SPIN_EXP_TIME, 0, nGradations-1, GradationByValue(int(d)));
	//sergey 24/01/06
	//InitSlider(IDC_SLIDER_TIME, 0, nGradations-1, GradationByValue(int(d)), 1);	
	InitSlider(IDC_SLIDER_TIME, 0, nGradations-1, GradationByValue(int(d)), 3);	
	//end
	
}

void CBaseDialog::InitSlider(int id, int nMin, int nMax, int nPos, int nTicFreq, BOOL bEnable)
{
	CSliderCtrl *pSlider = (CSliderCtrl *)GetDlgItem(id);
	if (pSlider)
	{
		pSlider->SetRange(nMin, nMax);
		pSlider->SetPos(nPos);
		pSlider->SetTicFreq(nTicFreq);
		pSlider->EnableWindow(bEnable);
	}
}

void CBaseDialog::InitSpin(int id, int nMin, int nMax, int nPos, BOOL bEnable)
{
	CSpinButtonCtrl *pSpin = (CSpinButtonCtrl *)GetDlgItem(id);
	if (pSpin)
	{
		pSpin->SetRange32(nMin, nMax);
		pSpin->SetPos(nPos);
		pSpin->EnableWindow(bEnable);
	}
}

void CBaseDialog::SetSliderPos(int id, int nPos)
{
	CSliderCtrl *pSlider = (CSliderCtrl *)GetDlgItem(id);
	if (pSlider)
		pSlider->SetPos(nPos);
}

int  CBaseDialog::GetSliderPos(int id)
{
	CSliderCtrl *pSlider = (CSliderCtrl *)GetDlgItem(id);
	if (pSlider)
		return pSlider->GetPos();
	else
		return 0;
}

void CBaseDialog::SetSpinPos(int id, int nPos)
{
	CSpinButtonCtrl *pSpin = (CSpinButtonCtrl *)GetDlgItem(id);
	if (pSpin)
		pSpin->SetPos(nPos);
}

void CBaseDialog::InitSmartEdit(int idSlider, int idSpin, int idEdit, int nMin, int nMax, int nPos, int nTicFreq, BOOL bEnable)
{
	InitSlider(idSlider, nMin, nMax, nPos, nTicFreq, bEnable);
	InitSpin(idSpin, nMin, nMax, nPos, bEnable);
	SetDlgItemInt(idEdit, nPos);
}

bool CBaseDialog::IsSmartEdit(int idSlider, int idEdit)
{
	if (idSlider != -1 && ::GetDlgItem(m_hWnd, idSlider) != 0)
		return true;
	if (idEdit != -1 && ::GetDlgItem(m_hWnd, idEdit) != 0)
		return true;
	return false;
}

void CBaseDialog::InitComboBox(int id, int nCurSel)
{
	CComboBox *pCombo = (CComboBox *)GetDlgItem(id);
	if (pCombo)
		pCombo->SetCurSel(nCurSel);
}

void CBaseDialog::InitComboBoxValue(int id, int nValue)
{
	CComboBox *pCombo = (CComboBox *)GetDlgItem(id);
	if (!pCombo) return;
	TCHAR szBuff[20];
	_itot(nValue, szBuff, 10);
	int nCurSel = pCombo->FindStringExact(-1, szBuff);
	pCombo->SetCurSel(nCurSel);
}

int CBaseDialog::GetComboBoxValue(int id)
{
	CComboBox *pCombo = (CComboBox *)GetDlgItem(id);
	if (pCombo)
		return pCombo->GetCurSel();
	else
		return 0;
}

void CBaseDialog::ChangeValue(int idSlider, int idSpin, int idEdit, int nValue)
{
	CSliderCtrl *pSlider = (CSliderCtrl *)GetDlgItem(idSlider);
	if (pSlider)
		pSlider->SetPos(nValue);
	CSpinButtonCtrl *pSpin = (CSpinButtonCtrl *)GetDlgItem(idSpin);
	if (pSpin)
		pSpin->SetPos(nValue);
	SetDlgItemInt(idEdit, nValue);
}


void CBaseDialog::InitLevelAdjustment(int nLevel)
{
	LevelAdjEx level;
	memset(&level, 0, sizeof(level));
	level.nChannel = (ChannelEx)(nLevel+kLuminanceChannel);
	CAM_IP_GetLuminanceLevels(&level);
	InitComboBox(IDC_COMBO_CHANNEL,level.nChannel-kLuminanceChannel);
	InitSmartEdit(IDC_SLIDER_SHADOW,IDC_SPIN_SHADOW,IDC_EDIT_SHADOW,0,255,level.nInShadow,16);
	InitSmartEdit(IDC_SLIDER_HIGHLIGHT,IDC_SPIN_HIGHLIGHT,IDC_EDIT_HIGHLIGHT,0,255,level.nInHighlight,16);
	//sergey 27/02/06
	int nGamma = int(log10(level.nInGamma)*10.);
	//int nGamma = int(log(level.nInGamma)*10.);
	//end
	InitSlider(IDC_SLIDER_GAMMA, -10, 10, nGamma, 1);
	InitSpin(IDC_SPIN_GAMMA, -10, 10, nGamma);
	SetDlgItemDouble1(m_hWnd, IDC_EDIT_GAMMA, level.nInGamma);
}

void CBaseDialog::ChangeLevelAdjustment(int nHighlight, int nShadow, double dGamma, unsigned nMask)
{
	CComboBox *pComboBox = (CComboBox *)GetDlgItem(IDC_COMBO_CHANNEL);
	if (!pComboBox) return;
	int nLevel = pComboBox->GetCurSel();
	LevelAdjEx level;
	memset(&level, 0, sizeof(level));
	level.nChannel = (ChannelEx)(nLevel+kLuminanceChannel);
	CAM_IP_GetLuminanceLevels(&level);
	if (nMask&1)
		level.nInShadow = nShadow;
	if (nMask&2)
		level.nInHighlight = nHighlight;
	if (nMask&4)
		level.nInGamma = dGamma;
	CAM_IP_SetLuminanceLevels(&level);
}

void CBaseDialog::InitCBFactor(int idEdit, int idSpin, int idSlider, double dFactor, BOOL bEnable)
{
	SetDlgItemDouble1(m_hWnd,idEdit,dFactor);
	::EnableWindow(::GetDlgItem(m_hWnd, idEdit), bEnable);
	InitSpin(idSpin,0,20,int(dFactor*10.),bEnable);
	InitSlider(idSlider,0,20,int(dFactor*10.),1,bEnable);
}

void CBaseDialog::ChangeWBValues(int nMode)
{
	double dWBRed = GetDlgItemDouble(m_hWnd,IDC_EDIT_WB_RED);
	double dWBGreen = GetDlgItemDouble(m_hWnd,IDC_EDIT_WB_GREEN);
	double dWBBlue = GetDlgItemDouble(m_hWnd,IDC_EDIT_WB_BLUE);
	//sergey 16/05/06
	if(!m_bNotB)//16/05/06
	{
		//sergey 18/05/06
	if (nMode == kManualWB)
	//if (nMode >= kWBOff && nMode <= kManualWB)
	{
		//CAM_WB_SetMode(kWBOff);
		CAM_WB_SetMode(kManualWB);
		//CAM_WB_SetFactors(1.,1.,1.);
		//Sleep(15);
		//CAM_WB_SetMode((WhiteBalanceModeEx)nMode);
	}	
	}
	//end
	CAM_WB_SetFactors(dWBRed,dWBGreen,dWBBlue);
	//sergey 17/05/06
	m_bNotB=0;
	//end
}

void CBaseDialog::EnableWB(int nWBMode)
{
	BOOL bColorCamera = CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("ColorCamera"), TRUE, true, true);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_WB_OFF), bColorCamera);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_WB_AUTO), bColorCamera);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_WB_ONE_PUSH), bColorCamera);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_WB_MANUAL), bColorCamera);
	BOOL bEnable = nWBMode == kAutoWB && bColorCamera;
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_SHOW_WB_RECT), bEnable);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_MOVE_WB_RECT), bEnable);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_CENTER_WB_RECT), bEnable);
	bEnable = nWBMode == kOneTouchWB && bColorCamera;
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_WB_BUILD), bEnable);
	bEnable = nWBMode == kManualWB && bColorCamera;
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_EDIT_WB_RED), bEnable);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_SPIN_WB_RED), bEnable);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_SLIDER_WB_RED), bEnable);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_EDIT_WB_GREEN), bEnable);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_SPIN_WB_GREEN), bEnable);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_SLIDER_WB_GREEN), bEnable);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_EDIT_WB_BLUE), bEnable);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_SPIN_WB_BLUE), bEnable);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_SLIDER_WB_BLUE), bEnable);
}

void CBaseDialog::EnableBB(BOOL bBB)
{
	::EnableWindow(::GetDlgItem(m_hWnd,IDC_SHOW_BB_RECT),bBB);
	::EnableWindow(::GetDlgItem(m_hWnd,IDC_MOVE_BB_RECT),bBB);
	::EnableWindow(::GetDlgItem(m_hWnd,IDC_CENTER_BB_RECT),bBB);
	::EnableWindow(::GetDlgItem(m_hWnd,IDC_BB_BUILD),bBB);
	double dBBRed,dBBGreen,dBBBlue;
	CAM_BB_GetFactors(&dBBRed,&dBBGreen,&dBBBlue);
	InitCBFactor(IDC_EDIT_BB_RED,IDC_SPIN_BB_RED,IDC_SLIDER_BB_RED,dBBRed,bBB);
	InitCBFactor(IDC_EDIT_BB_GREEN,IDC_SPIN_BB_GREEN,IDC_SLIDER_BB_GREEN,dBBGreen,bBB);
	InitCBFactor(IDC_EDIT_BB_BLUE,IDC_SPIN_BB_BLUE,IDC_SLIDER_BB_BLUE,dBBBlue,bBB);
}

void CBaseDialog::InitControlsState()
{
	//sergey 18/01/06
	cwndGlob=::AfxGetMainWnd();
	//end	
	
	CLock lock(&s_bChanging);
	if(!CAM_IsDriverLoaded())
	{
		if (m_bDisableAllCtrls)
		{
			HWND hwnd = ::GetWindow(m_hWnd, GW_CHILD);
			while (hwnd)
			{
				::EnableWindow(hwnd, FALSE);
				hwnd = ::GetWindow(hwnd, GW_HWNDNEXT);
			}
		}
		return;
	}
	BOOL bColorCamera = CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("ColorCamera"), TRUE, true, true);
	//sergey 17/01/06
	int color=CAM_IP_IsColorCapture();
	//end
	// 1. Initialize exposure settings
	AutoExposureModeEx Mode = CAM_AE_GetMode();
	CheckDlgButton(IDC_SHUTTER_AUTO, Mode != kManualExposure);
	CString s;
	if (Mode == kManualExposure)
		InitManualExposure();
	else
		InitAutoExposure();
	if (CStdProfileManager::m_pMgr->GetProfileInt(_T("Exposure"), _T("DisableAutoExposure"), TRUE, true))
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_SHUTTER_AUTO), FALSE);
	// 2. Initialize sensitivity
	int nSensitivity;
	CAM_EXP_GetSensitivity(&nSensitivity);
	InitComboBoxValue(IDC_COMBO_SENSITIVITY, nSensitivity);
	if (CStdProfileManager::m_pMgr->GetProfileInt(_T("Exposure"), _T("DisableSensitivity"), TRUE, true))
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_COMBO_SENSITIVITY), FALSE);
	// 3. Initialize level adjustments
	int nLevel = CStdProfileManager::m_pMgr->_GetProfileInt(_T("LevelAdjustment"), _T("CurrentLevel"), 0);
	InitLevelAdjustment(bColorCamera?nLevel:0);
	if (!bColorCamera)
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_COMBO_CHANNEL), FALSE);
	// 4. Initialize accumulation modes
	//sergey 11/05/06
	//BOOL bGetImageFromPreview = CStdProfileManager::m_pMgr->GetProfileInt(_T("Image"), _T("CaptureFromPreview"), 0, true,true);
	BOOL bGetImageFromPreview = CStdProfileManager::m_pMgr->GetProfileInt(_T("Image"), _T("CaptureFromPreview"), 0, TRUE);
	//end
	AccumulateMethodEx AccMode;
	CAM_SIA_GetAccumulateMode(&AccMode);
	InitComboBox(IDC_COMBO_ACCUMULATION, AccMode-kNotAccumulating);
	//sergey 21/01/06
	if(GetComboBoxValue(IDC_COMBO_ACCUMULATION)==kNotAccumulating) ::EnableWindow(::GetDlgItem(m_hWnd, IDC_EDIT_ACCUMULATION_TIMES), FALSE);
	//end
	int nAccTimes;
	CAM_SIA_GetAccumulateTimes(&nAccTimes);
	//sergey 23/01/06
	/*InitSpin(IDC_SPIN_ACCUMULATION_TIMES, 1, CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"),
		_T("MaxAccumulation"), 10, true, true),	nAccTimes, !bGetImageFromPreview);*/
	InitSpin(IDC_SPIN_ACCUMULATION_TIMES, 2, CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"),
		_T("MaxAccumulation"), 10, true, true),	nAccTimes, !bGetImageFromPreview);
	//end
	CString sAcc;
	GetDlgItemText(IDC_STATIC_ACCUMULATION, sAcc);
	if (!sAcc.IsEmpty())
	{
		CString s;
		s.Format(_T(" (2-%d):"), CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"),
			_T("MaxAccumulation"), 10, true, true));
		int n = sAcc.Find(_T(':'));
		if (n != -1)
			sAcc = sAcc.Left(n)+s;
		else
			sAcc += s;
		SetDlgItemText(IDC_STATIC_ACCUMULATION, sAcc);
	}
	if (bGetImageFromPreview)
	{
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_STATIC_ACCUMULATION), FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_EDIT_ACCUMULATION_TIMES), FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_COMBO_ACCUMULATION), FALSE);
	}
	// 5. Init white balance
	if (bColorCamera)
	{
	WhiteBalanceModeEx WBMode = (WhiteBalanceModeEx)g_nWBMode;//CAM_WB_GetMode();
	CheckRadioButton(IDC_WB_OFF, IDC_WB_MANUAL, IDC_WB_OFF+WBMode-kWBOff);
	bool bWBShow;
	PxRectEx WBrect;
	UCHAR byWBR,byWBG,byWBB;
	CAM_WB_GetRegion(&bWBShow,&WBrect,&byWBR,&byWBG,&byWBB);
	CheckDlgButton(IDC_SHOW_WB_RECT, bWBShow?1:0);
	double dWBRed,dWBGreen,dWBBlue;
		CAM_WB_SetMode(WBMode);
	CAM_WB_GetFactors(&dWBRed,&dWBGreen,&dWBBlue);
		TRACE("InitControlState, mode = %d,%d, factors are (%f,%f,%f)\n", g_nWBMode,
			(int)CAM_WB_GetMode(), dWBRed, dWBGreen, dWBBlue);
	InitCBFactor(IDC_EDIT_WB_RED,IDC_SPIN_WB_RED,IDC_SLIDER_WB_RED,dWBRed,WBMode==kManualWB);
	InitCBFactor(IDC_EDIT_WB_GREEN,IDC_SPIN_WB_GREEN,IDC_SLIDER_WB_GREEN,dWBGreen,WBMode==kManualWB);
	InitCBFactor(IDC_EDIT_WB_BLUE,IDC_SPIN_WB_BLUE,IDC_SLIDER_WB_BLUE,dWBBlue,WBMode==kManualWB);
		
	EnableWB(WBMode);

		
	}
	else
	{
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_COLOR_BALANCE), FALSE);
		CheckRadioButton(IDC_WB_OFF, IDC_WB_MANUAL, IDC_WB_OFF);
		EnableWB(kWBOff);
	}
	// 6. Init image characteristics
	OrientationEx Orient = CAM_IP_GetOrientation();
	CheckDlgButton(IDC_CHECK_FLIP_VERT, Orient==kFlipVertical||Orient==kRotate180);
	CheckDlgButton(IDC_CHECK_FLIP_HORZ, Orient==kFlipHorizontal||Orient==kRotate180);
	PreviewResolutionEx PrvMode;
	PxSizeEx pxMIA,pxSIA;
	CAM_MIA_GetResolution(&PrvMode,&pxMIA);
	InitComboBox(IDC_COMBO_PREVIEW, PrvMode-kFast);
	StillResolutionEx CapMode;
	CAM_SIA_GetResolution(&CapMode, &pxSIA);
	InitComboBox(IDC_COMBO_CAPTURE, CapMode-kWarp);
	if (bGetImageFromPreview)
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_COMBO_CAPTURE), FALSE);
	if (bColorCamera)
	{
	CheckDlgButton(IDC_CHECK_GRAYSCALE, CAM_IP_IsColorCapture()==kColorCamColor?0:1);
		//sergey 17/01/06
		CWnd *s;
		if(CAM_IP_IsColorCapture()==kColorCamMono)
		{
			s=::AfxGetMainWnd();
		    ::EnableWindow(::GetDlgItem(*s, IDC_COLOR_BALANCE), FALSE);
		}
		//end
	}
	else
	{
		CWnd *s;
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_CHECK_GRAYSCALE), FALSE);
		CheckDlgButton(IDC_CHECK_GRAYSCALE, 1);
		//sergey 12/01/06
		//s=::AfxGetMainWnd();
		//::EnableWindow(::GetDlgItem(*s, IDC_COLOR_BALANCE), FALSE);	
		//end
	}
	MicroModeEx MicroMode;
	CAM_AE_GetMicroMode(&MicroMode);
	//sergey 17/01/06
	if(MicroMode==kBF)
	{
		CWnd *s;
		s=::AfxGetMainWnd();
		::EnableWindow(::GetDlgItem(*s, IDC_BLACK_BALANCE), FALSE);

		//CWnd *s1=s->GetDlgItem(IDD_BLACK_BALANCE);
	   //s1->CheckDlgButton(IDC_CHECK_BB,0);

	}
	//end
	CheckRadioButton(IDC_MICRO_BRIGHT,IDC_MICRO_FLUORESCENT,IDC_MICRO_BRIGHT+MicroMode-kBF);
	CheckDlgButton(IDC_CHECK_FOCUS, CAM_FF_IsRunning()?1:0);
	bool bDrawingOnOff;
	PxRectEx RectFocus;
	UCHAR byFocR, byFocG, byFocB;
	CAM_FF_GetRegion(&bDrawingOnOff, &RectFocus, &byFocR, &byFocG, &byFocB);
	CheckDlgButton(IDC_SHOW_FOCUS_RECT, bDrawingOnOff?1:0);
	m_bHistogram = CStdProfileManager::m_pMgr->GetProfileInt(_T("Histogram"), _T("Show"), FALSE);
	CheckDlgButton(IDC_HISTOGRAM, m_bHistogram);
	bool bAERect;
	PxRectEx AERect;
	UCHAR byAER, byAEG, byAEB;
	CAM_AE_GetSpotRegion(&bAERect, &AERect, &byAER, &byAEG, &byAEB);
	if (CStdProfileManager::m_pMgr->GetProfileInt(_T("Exposure"), _T("DisableAutoExposure"), TRUE, true))
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_SHOW_EXPOSURE_RECTANGLE), FALSE);
	CheckDlgButton(IDC_SHOW_EXPOSURE_RECTANGLE, bAERect);
	// 7. Init black balance
	BOOL bBB = CStdProfileManager::m_pMgr->_GetProfileInt(_T("BlackBalance"), _T("UseBlackBalance"), 0);
	CheckDlgButton(IDC_CHECK_BB, bBB?1:0);
	bool bBBShow;
	PxRectEx BBrect;
	UCHAR byBBR,byBBG,byBBB;
	CAM_BB_GetRegion(&bBBShow,&BBrect,&byBBR,&byBBG,&byBBB);
	CheckDlgButton(IDC_SHOW_BB_RECT, bBBShow?1:0);
	EnableBB(bBB);
	m_bInited = true;

	//sergey 13/01/06
	COMBOBOXREPRDATA *pReprData=(COMBOBOXREPRDATA *)g_Methodics.GetRepr();
	InitListBox1(IDC_LIST11,0,pReprData);
	
	InitListBox(IDC_LIST1, 0);
	InitEdit(IDC_EDIT1);
	//??????????????????????????????????????????????
	//RestoreSettings();
	//end
}

BOOL CBaseDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	if (!m_bDelayedInitCtrllSite) InitControlsState();
	return TRUE;
}

void CBaseDialog::OnDelayedNewImage()
{
	if (m_bFocus)
	{
		int nCurrent,nPeak;
		CAM_FF_GetEvalValue(&nCurrent,&nPeak);
		SetDlgItemInt(IDC_STATIC_FOCUS_CURRENT, nCurrent);
		//SetDlgItemInt(IDC_STATIC_FOCUS_PEAK, nPeak);
	}
}

void CBaseDialog::OnAutoExposure()
{
	if (s_bChanging) return;
	if (IsDlgButtonChecked(IDC_SHUTTER_AUTO))
	{
		CAM_AE_Start();
		int n = CStdProfileManager::m_pMgr->GetProfileInt(_T("Exposure"), _T("Adjust"), 0);
		n = min(max(n, -6,), 6);
		CAM_AE_SetAdjust(n);
		InitAutoExposure();
	}
	else
	{
		CAM_AE_Stop();
		InitManualExposure();
	}
	//sergey 05/02/06
	m_bChancedProf=1;
	//end
}

void CBaseDialog::OnHScroll( UINT nSBCode, UINT nPos1, CScrollBar* pScrollBar)
{
	if (s_bChanging) return;
	CLock lock(&s_bChanging);
//	if (nSBCode != SB_THUMBPOSITION && nSBCode != SB_THUMBTRACK)
//		return;
	int nID = pScrollBar->GetDlgCtrlID();
	int nPos = ((CSliderCtrl*)pScrollBar)->GetPos();
	if (nID == IDC_SLIDER_TIME)
	{
		if (nSBCode != SB_THUMBTRACK)
		{
			AutoExposureModeEx Mode = CAM_AE_GetMode();
			if (Mode == kManualExposure)
			{
				int nTime = ValueByGradation(nPos); // 1..10000
				CString s = TimeString(nTime*100);
				SetDlgItemText(IDC_EDIT_EXP_TIME, s);
				CSpinButtonCtrl *pSpin = (CSpinButtonCtrl *)GetDlgItem(IDC_SPIN_EXP_TIME);
				if (pSpin)
					pSpin->SetPos(GradationByValue(nTime));
				CAM_EXP_SetSpeed(nTime);
			}
			else
			{
				SetDlgItemText(IDC_EDIT_EXP_TIME, aszNames[nPos]);
				CSpinButtonCtrl *pSpin = (CSpinButtonCtrl *)GetDlgItem(IDC_SPIN_EXP_TIME);
				if (pSpin)
					pSpin->SetPos(nPos);
				CAM_AE_SetAdjust(nPos-6);
			}
		}
	}
	else if (nID == IDC_SLIDER_SENSITIVITY)
	{
		CAM_EXP_SetSensitivity(nPos);
		CSpinButtonCtrl *pSpin = (CSpinButtonCtrl *)GetDlgItem(IDC_SPIN_SENSITIVITY);
		if (pSpin)
			pSpin->SetPos(nPos);
		SetDlgItemInt(IDC_EDIT_SENSITIVITY, nPos);
	}
	else if (nID == IDC_SLIDER_SHADOW)
	{
		ChangeValue(0, IDC_SPIN_SHADOW, IDC_EDIT_SHADOW, nPos);
		ChangeLevelAdjustment(0, nPos, 0., 1);
	}
	else if (nID == IDC_SLIDER_HIGHLIGHT)
	{
		ChangeValue(0, IDC_SPIN_HIGHLIGHT, IDC_EDIT_HIGHLIGHT, nPos);
		ChangeLevelAdjustment(nPos, 0, 0., 2);
	}
	else if (nID == IDC_SLIDER_GAMMA)
	{
		int n = (int)nPos;
		double d = pow(10.,double(n)/10.);
		SetSpinPos(IDC_SPIN_GAMMA, nPos);
		SetDlgItemDouble1(m_hWnd, IDC_EDIT_GAMMA, d);
		ChangeLevelAdjustment(0, 0, d, 4);
	}
	else if (nID == IDC_SLIDER_WB_RED)
	{
		SetSpinPos(IDC_SPIN_WB_RED, nPos);
		SetDlgItemDouble1(m_hWnd, IDC_EDIT_WB_RED, nPos/10.);
		//sergey 17/05/06
	   // m_bNotB=1;
		ChangeWBValues(-1);	    
		//ChangeWBValues(kManualWB);
		//end
	}
	else if (nID == IDC_SLIDER_WB_GREEN)
	{
		SetSpinPos(IDC_SPIN_WB_GREEN, nPos);
		SetDlgItemDouble1(m_hWnd, IDC_EDIT_WB_GREEN, nPos/10.);
		//sergey 17/05/06
	   // m_bNotB=1;
		ChangeWBValues(-1);	    
		//ChangeWBValues(kManualWB);
		//end
	}
	else if (nID == IDC_SLIDER_WB_BLUE)
	{
		SetSpinPos(IDC_SPIN_WB_BLUE, nPos);
		SetDlgItemDouble1(m_hWnd, IDC_EDIT_WB_BLUE, nPos/10.);
		//sergey 17/05/06
	   // m_bNotB=1;
		ChangeWBValues(-1);	    
		//ChangeWBValues(kManualWB);
		//end
	}
	//sergey 05/02/06
	m_bChancedProf=1;
	//end
}

void CBaseDialog::OnExpChange()
{
	if (s_bChanging) return;
	CLock lock(&s_bChanging);
	AutoExposureModeEx Mode = CAM_AE_GetMode();
	if (Mode == kManualExposure)
	{
		CString s;
		GetDlgItemText(IDC_EDIT_EXP_TIME, s);
		if (!s.IsEmpty())
		{
			int nVal = StringToTime(s);
			nVal /= 100;
			CAM_EXP_SetSpeed(double(nVal));
			CSpinButtonCtrl *pSpin = (CSpinButtonCtrl *)GetDlgItem(IDC_SPIN_EXP_TIME);
			if (pSpin)
				pSpin->SetPos(GradationByValue(nVal));
			CSliderCtrl *pSlider = (CSliderCtrl *)GetDlgItem(IDC_SLIDER_TIME);
			if (pSlider)
				pSlider->SetPos(GradationByValue(nVal));
		}
	}
	//sergey 05/02/06
	//m_bChancedProf=1;
	//end
}

void CBaseDialog::OnSensitivityChange()
{
	if (s_bChanging) return;
	CLock lock(&s_bChanging);
	int nSensitivity = GetDlgItemInt(IDC_EDIT_SENSITIVITY);
	CSpinButtonCtrl *pSpin = (CSpinButtonCtrl *)GetDlgItem(IDC_SPIN_SENSITIVITY);
	if (pSpin)
		pSpin->SetPos(nSensitivity);
	CSliderCtrl *pSlider = (CSliderCtrl *)GetDlgItem(IDC_SLIDER_SENSITIVITY);
	if (pSlider)
		pSlider->SetPos(nSensitivity);
	CAM_EXP_SetSensitivity(nSensitivity);
	//sergey 05/02/06
	m_bChancedProf=1;
	//end
}

void CBaseDialog::OnDeltaposExpTime(NMHDR* pNMHDR, LRESULT* pResult)
{
	
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	if (!s_bChanging)
	{
		CLock lock(&s_bChanging);
		AutoExposureModeEx Mode = CAM_AE_GetMode();
		if (Mode == kManualExposure)
		{
			CString s;
			GetDlgItemText(IDC_EDIT_EXP_TIME, s);
			int nVal = StringToTime(s);
			nVal /= 100;
			int nGrad = GradationByValue(nVal);
			nGrad += pNMUpDown->iDelta;
			nGrad = max(min(nGrad,nGradations-1),0);
			nVal = ValueByGradation(nGrad);
			CSliderCtrl *pSlider = (CSliderCtrl *)GetDlgItem(IDC_SLIDER_TIME);
			if (pSlider)
				pSlider->SetPos(nGrad);
			s = TimeString(nVal*100);
			SetDlgItemText(IDC_EDIT_EXP_TIME, s);
			CAM_EXP_SetSpeed(double(nVal));
		}
		else
		{
			int n = CAM_AE_GetAdjust();
			n += pNMUpDown->iDelta;
			n = max(min(n,6),-6);
			CAM_AE_SetAdjust(n);
			CSliderCtrl *pSlider = (CSliderCtrl *)GetDlgItem(IDC_SLIDER_TIME);
			if (pSlider)
				pSlider->SetPos(n+6);
			SetDlgItemText(IDC_EDIT_EXP_TIME, aszNames[n+6]);
		}
	}
	*pResult = 0;
	//sergey 05/02/06
	m_bChancedProf=1;
	//end
}

void CBaseDialog::OnSelchangeComboSensitivity()
{
	if (s_bChanging) return;
	CLock lock(&s_bChanging);
	int nSensitivity = GetDlgItemInt(IDC_COMBO_SENSITIVITY);
	CAM_EXP_SetSensitivity(nSensitivity);
	//sergey 05/02/06
	m_bChancedProf=1;
	//end
}

void CBaseDialog::OnSelchangeLevelAdjustment()
{
	if (s_bChanging) return;
	CLock lock(&s_bChanging);
	CComboBox *pComboBox = (CComboBox *)GetDlgItem(IDC_COMBO_CHANNEL);
	if (!pComboBox) return;
	int nLevel = pComboBox->GetCurSel();
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("LevelAdjustment"), _T("CurrentLevel"), nLevel);
	InitLevelAdjustment(nLevel);
	//sergey 05/02/06
	m_bChancedProf=1;
	//end
}

void CBaseDialog::OnShadowChange()
{
	if (s_bChanging) return;
	CLock lock(&s_bChanging);
	int nShadow = GetDlgItemInt(IDC_EDIT_SHADOW);
	ChangeValue(IDC_SLIDER_SHADOW, IDC_SPIN_SHADOW, 0, nShadow);
	ChangeLevelAdjustment(0, nShadow, 0., 1);
	//sergey 05/02/06
	//m_bChancedProf=1;
	//end
}

void CBaseDialog::OnGammaChange()
{
	if (s_bChanging) return;
	CLock lock(&s_bChanging);
	double d = GetDlgItemDouble(m_hWnd, IDC_EDIT_GAMMA);
	//sergey 27/02/06	
	int nGamma = int(log10(d)*10.);
	//int nGamma = int(log(d)*10.);
	//end
	ChangeValue(IDC_SLIDER_GAMMA, IDC_SPIN_GAMMA, 0, nGamma);
	ChangeLevelAdjustment(0, 0, d, 4);
	//sergey 05/02/06
	m_bChancedProf=1;
	//end
}

void CBaseDialog::OnHighlightChange()
{
	if (s_bChanging) return;
	CLock lock(&s_bChanging);
	int nHighlight = GetDlgItemInt(IDC_EDIT_HIGHLIGHT);
	ChangeValue(IDC_SLIDER_HIGHLIGHT, IDC_SPIN_HIGHLIGHT, 0, nHighlight);
	ChangeLevelAdjustment(nHighlight, 0, 0., 2);
	//sergey 05/02/06
	//m_bChancedProf=1;
	//end
}

//sergey 05/02/06
void CBaseDialog::OnShadowChange1()
{
	//m_bChancedProf=1;
	
}

void CBaseDialog::OnHighlightChange1()
{
	m_bChancedProf=1;
}
void CBaseDialog::OnGammaChange1()
{
	m_bChancedProf=1;
}
void CBaseDialog::OnExpChange1()
{
	m_bChancedProf=1;
}
//end

void CBaseDialog::OnDeltaposSpinGamma(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	if (!s_bChanging)
	{
		CLock lock(&s_bChanging);
		int nGamma = pNMUpDown->iPos+pNMUpDown->iDelta;
		if (nGamma > 10)
		{
			nGamma = 10;
		}
		else if (nGamma < -10)
		{
			nGamma = -10;
		}
		double d = pow(10.,double(nGamma)/10.);
		SetSliderPos(IDC_SLIDER_GAMMA, nGamma);
		SetDlgItemDouble1(m_hWnd, IDC_EDIT_GAMMA, d);
		ChangeLevelAdjustment(0, 0, d, 4);
	}
	*pResult = 0;
	//sergey 05/02/06
	m_bChancedProf=1;
	//end
}

void CBaseDialog::OnSelchangeAccumulationMode()
{
	if (s_bChanging) return;

	int add=GetComboBoxValue(IDC_COMBO_ACCUMULATION);

	CAM_SIA_SetAccumulateMode((AccumulateMethodEx)(GetComboBoxValue(IDC_COMBO_ACCUMULATION)+kNotAccumulating));
	//sergey 21/01/06
	if(GetComboBoxValue(IDC_COMBO_ACCUMULATION)==kNotAccumulating)::EnableWindow(::GetDlgItem(m_hWnd, IDC_EDIT_ACCUMULATION_TIMES), FALSE);
	else ::EnableWindow(::GetDlgItem(m_hWnd, IDC_EDIT_ACCUMULATION_TIMES), TRUE);
	//end
	//sergey 05/02/06
	m_bChancedProf=1;
	//end
}

void CBaseDialog::OnAccumulateTimes()
{
	if (s_bChanging || !m_bInited) return;
	int n = GetDlgItemInt(IDC_EDIT_ACCUMULATION_TIMES);
	//check(n, 1, CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("MaxAccumulation"), 10, true, true));
	//sergey 23/01/06
	check(n, 2, CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("MaxAccumulation"), 10, true, true));
	//end
	CAM_SIA_SetAccumulateTimes(n);
	//sergey 05/02/06
	m_bChancedProf=1;
	//end
}

void CBaseDialog::OnKillFocusAccumulateTimes()
{
	int n = GetDlgItemInt(IDC_EDIT_ACCUMULATION_TIMES);
	/*if (check(n, 1, CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("MaxAccumulation"), 10, true, true)))
		InitSpin(IDC_SPIN_ACCUMULATION_TIMES, 1, CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("MaxAccumulation"), 10, true, true), n);
//		SetDlgItemInt(IDC_EDIT_ACCUMULATION_TIMES, n);*/
	//sergey 23/01/06
	 if (check(n, 2, CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("MaxAccumulation"), 10, true, true)))
		InitSpin(IDC_SPIN_ACCUMULATION_TIMES, 2, CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("MaxAccumulation"), 10, true, true), n);
	//end
	 //sergey 05/02/06
	m_bChancedProf=1;
	//end
}

void CBaseDialog::OnWBChange()
{
	if (s_bChanging) return;
	WhiteBalanceModeEx WBModeOld,WBMode;
	if (IsDlgButtonChecked(IDC_WB_OFF))
		WBMode = kWBOff;
	else if (IsDlgButtonChecked(IDC_WB_AUTO))
		WBMode = kAutoWB;
	else if (IsDlgButtonChecked(IDC_WB_ONE_PUSH))
		WBMode = kOneTouchWB;
	else //if (IsDlgButtonChecked(IDC_WB_MANUAL))
		WBMode = kManualWB;
	WBModeOld = (WhiteBalanceModeEx)g_nWBMode;
	double dWBRed,dWBGreen,dWBBlue;
	if (WBModeOld == kAutoWB || WBModeOld == kOneTouchWB)
		CAM_WB_GetFactors(&dWBRed,&dWBGreen,&dWBBlue);
	
	//sergey 16/05/06  
	if (WBMode == kWBOff)
	{
	CAM_WB_SetMode(kWBOff);
	m_bbuld=0;
	m_bNotB=0;
	}
   //end
	if (WBMode == kManualWB)
	{
		//sergey 16/05/06
		
        if (WBModeOld == kAutoWB || m_bbuld == 1)
		//if (WBModeOld == kAutoWB || WBModeOld == kOneTouchWB)
		{
			//sergey 16/05/06
			
			
			/*dWBRed = GetDlgItemDouble(m_hWnd,IDC_EDIT_WB_RED);
	        dWBGreen = GetDlgItemDouble(m_hWnd,IDC_EDIT_WB_GREEN);
	        dWBBlue = GetDlgItemDouble(m_hWnd,IDC_EDIT_WB_BLUE);
			//end*/
			/*CAM_WB_SetFactors(dWBRed,dWBGreen,dWBBlue);
			InitCBFactor(IDC_EDIT_WB_RED,IDC_SPIN_WB_RED,IDC_SLIDER_WB_RED,dWBRed,WBMode==kManualWB);
			InitCBFactor(IDC_EDIT_WB_GREEN,IDC_SPIN_WB_GREEN,IDC_SLIDER_WB_GREEN,dWBGreen,WBMode==kManualWB);
			InitCBFactor(IDC_EDIT_WB_BLUE,IDC_SPIN_WB_BLUE,IDC_SLIDER_WB_BLUE,dWBBlue,WBMode==kManualWB);*/
			ChangeWBValues(kManualWB);
		}
		else
		//end
			ChangeWBValues(-1);
		//sergey 16/05/06
		m_bbuld=0;
		m_bNotB=0;
		//end
	}
	//sergey 16/05/06
	  
	else if (WBMode == kAutoWB)
	{
		if (WBModeOld != kAutoWB)
		{
		//sergey 16/05/06
		CAM_WB_SetMode(kAutoWB);		
			
		//end
		bool bWBShow;
		PxRectEx WBrect;
		UCHAR byWBR,byWBG,byWBB;
		CAM_WB_GetRegion(&bWBShow,&WBrect,&byWBR,&byWBG,&byWBB);
		bWBShow = IsDlgButtonChecked(IDC_SHOW_WB_RECT)==1;
		//sergey 16/05/06
		m_bNotB=0;
		m_bbuld=1;
		//if(bWBShow)
		//end
		CAM_WB_SetRegion(bWBShow,&WBrect,byWBR,byWBG,byWBB);
		
	}
	}
	EnableWB(WBMode);
	g_nWBMode = WBMode;
	//sergey 05/02/06
	if (WBMode == kOneTouchWB)
	{
        m_bNotB=0;
	//if (WBModeOld == kAutoWB)m_bbuld=1;
	}
	m_bChancedProf=1;
	//end
}

void CBaseDialog::OnWBRedChange()
{
	if (s_bChanging) return;
	double d = GetDlgItemDouble(m_hWnd, IDC_EDIT_WB_RED);
	SetSpinPos(IDC_SPIN_WB_RED, int(d*10.));
	SetSliderPos(IDC_SLIDER_WB_RED, int(d*10.));
	//sergey 17/05/06
	   // m_bNotB=1;
		ChangeWBValues(-1);	    
		//ChangeWBValues(kManualWB);
		//end
	//sergey 05/02/06
	m_bChancedProf=1;
	//end
}

void CBaseDialog::OnWBGreenChange()
{
	if (s_bChanging) return;
	double d = GetDlgItemDouble(m_hWnd, IDC_EDIT_WB_GREEN);
	SetSpinPos(IDC_SPIN_WB_GREEN, int(d*10.));
	SetSliderPos(IDC_SLIDER_WB_GREEN, int(d*10.));
	//sergey 17/05/06
	   // m_bNotB=1;
		ChangeWBValues(-1);	    
		//ChangeWBValues(kManualWB);
		//end
	//sergey 05/02/06
	m_bChancedProf=1;
	//end
}

void CBaseDialog::OnWBBlueChange()
{
	if (s_bChanging) return;
	double d = GetDlgItemDouble(m_hWnd, IDC_EDIT_WB_BLUE);
	SetSpinPos(IDC_SPIN_WB_BLUE, int(d*10.));
	SetSliderPos(IDC_SLIDER_WB_BLUE, int(d*10.));
	//sergey 17/05/06
	   // m_bNotB=1;
		ChangeWBValues(-1);	    
		//ChangeWBValues(kManualWB);
		//end
	//sergey 05/02/06
	m_bChancedProf=1;
	//end
}

void CBaseDialog::OnWBShowRect()
{
	if (s_bChanging) return;
	bool bWBShow;
	PxRectEx WBrect;
	UCHAR byWBR,byWBG,byWBB;
	CAM_WB_GetRegion(&bWBShow,&WBrect,&byWBR,&byWBG,&byWBB);
	bWBShow = IsDlgButtonChecked(IDC_SHOW_WB_RECT)==1;
	CAM_WB_SetMode(kAutoWB);
	double dWBRed,dWBGreen,dWBBlue;		
		CAM_WB_GetFactors(&dWBRed,&dWBGreen,&dWBBlue);
	CAM_WB_SetRegion(bWBShow,&WBrect,byWBR,byWBG,byWBB);
	//sergey 05/02/06
	m_bChancedProf=1;
	//end
}

void CBaseDialog::OnWBBuild()
{
	if (s_bChanging) return;
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_WB_BUILD), FALSE);
	SetTimer(1, 1000, NULL);
	CAM_WB_CalibrateFactors();
	//CAM_WB_SetMode(kAutoWB);
	CAM_WB_SetMode(kOneTouchWB);
	//sergey 05/02/06
	m_bChancedProf=1;
	m_bbuld=1;
	//m_bNotB=1;
	//end
}

void CBaseDialog::OnDeltaposSpinRed(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	if (!s_bChanging)
	{
		CLock lock(&s_bChanging);
		int nPos = GetSliderPos(IDC_SLIDER_WB_RED)+pNMUpDown->iDelta;
		if (nPos > 20)
			nPos = 20;
		else if (nPos < 0)
			nPos = 0;
		double d = nPos/10.;
		SetSliderPos(IDC_SLIDER_WB_RED, nPos);
		SetDlgItemDouble1(m_hWnd, IDC_EDIT_WB_RED, d);
		//sergey 17/05/06
	   // m_bNotB=1;
		ChangeWBValues(-1);	    
		//ChangeWBValues(kManualWB);
		//end
	}
	*pResult = 0;
	//sergey 05/02/06
	m_bChancedProf=1;
	//end
}

void CBaseDialog::OnDeltaposSpinGreen(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	if (!s_bChanging)
	{
		CLock lock(&s_bChanging);
		int nPos = GetSliderPos(IDC_SLIDER_WB_GREEN)+pNMUpDown->iDelta;
		if (nPos > 20)
			nPos = 20;
		else if (nPos < 0)
			nPos = 0;
		double d = nPos/10.;
		SetSliderPos(IDC_SLIDER_WB_GREEN, nPos);
		SetDlgItemDouble1(m_hWnd, IDC_EDIT_WB_GREEN, d);
		//sergey 17/05/06
	   // m_bNotB=1;
		ChangeWBValues(-1);	    
		//ChangeWBValues(kManualWB);
		//end
	}
	*pResult = 0;
	//sergey 05/02/06
	m_bChancedProf=1;
	//end
}

void CBaseDialog::OnDeltaposSpinBlue(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	if (!s_bChanging)
	{
		CLock lock(&s_bChanging);
		int nPos = GetSliderPos(IDC_SLIDER_WB_BLUE)+pNMUpDown->iDelta;
		if (nPos > 20)
			nPos = 20;
		else if (nPos < 0)
			nPos = 0;
		double d = nPos/10.;
		SetSliderPos(IDC_SLIDER_WB_BLUE, nPos);
		SetDlgItemDouble1(m_hWnd, IDC_EDIT_WB_BLUE, d);
		//sergey 17/05/06
	   // m_bNotB=1;
		ChangeWBValues(-1);	    
		//ChangeWBValues(kManualWB);
		//end
	}
	*pResult = 0;
	//sergey 05/02/06
	m_bChancedProf=1;
	//end
}

void CBaseDialog::OnFlip()
{
	if (s_bChanging) return;
	BOOL bCheckV = IsDlgButtonChecked(IDC_CHECK_FLIP_VERT);
	BOOL bCheckH = IsDlgButtonChecked(IDC_CHECK_FLIP_HORZ);
	OrientationEx Orient;
	if (bCheckV)
		if (bCheckH)
			Orient = kRotate180;
		else
			Orient = kFlipVertical;
	else
		if (bCheckH)
			Orient = kFlipHorizontal;
		else
			Orient = kNoTransform;
	CAM_IP_SetOrientation(Orient);
	//sergey 05/02/06
	m_bChancedProf=1;
	//end
}

void CBaseDialog::OnSelchangePreviewMode()
{
	if (s_bChanging) return;
	PreviewResolutionEx PrvReso = (PreviewResolutionEx)(GetComboBoxValue(IDC_COMBO_PREVIEW)+kFast);
	CAM_MIA_SetResolution(PrvReso);
	//sergey 05/02/06
	m_bChancedProf=1;
	//end
}

void CBaseDialog::OnSelchangeCaptureMode()
{
	if (s_bChanging) return;
	StillResolutionEx CapReso = (StillResolutionEx)(GetComboBoxValue(IDC_COMBO_CAPTURE)+kWarp);
	CAM_SIA_SetResolution(CapReso);
	//sergey 05/02/06
	m_bChancedProf=1;
	//end
}

void CBaseDialog::OnGrayScale()
{
	CWnd *s;
	if (s_bChanging) return;
	//sergey 16/03/06
	//ColorModeEx Color = IsDlgButtonChecked(IDC_CHECK_GRAYSCALE)==1?kColorCamMono:kColorCamColor;
	ColorModeEx Color = IsDlgButtonChecked(IDC_CHECK_GRAYSCALE)==1?kMonochromeCam:kColorCamColor;
	//end
	//sergey 27/12/05
	if(Color==kColorCamMono)
	{		
		s=::AfxGetMainWnd();
		::EnableWindow(::GetDlgItem(*s, IDC_COLOR_BALANCE), FALSE);		
	}
	else
	{
		s=::AfxGetMainWnd();
		::EnableWindow(::GetDlgItem(*s, IDC_COLOR_BALANCE), TRUE);		
	}
	//end
	CAM_IP_ColorCapture(Color);
	//sergey 05/02/06
	m_bChancedProf=1;
	//end
	
}

void CBaseDialog::OnMicro()
{
	CWnd *s;
	if (s_bChanging) return;
	if (IsDlgButtonChecked(IDC_MICRO_BRIGHT)==1)
	{
		CAM_AE_SetMicroMode(kBF);
	   //sergey 27/12/05
	     s=::AfxGetMainWnd();

		// CheckDlgButton(IDC_SHOW_BB_RECT, 0);
		 //::EnableWindow(::GetDlgItem(m_hWnd,IDC_SHOW_BB_RECT),FALSE);

	    UCHAR byBBR = 255, byBBG = 255,byBBB = 255;
		bool bBB;
		PxRectEx BBrect;
//		RestoreRect(_T("BlackBalance\\Rect"), bBB, BBrect, byBBR, byBBG, byBBB);
		CAM_BB_GetRegion(&bBB, &BBrect, &byBBR, &byBBG, &byBBB);
		CAM_BB_SetRegion(0,&BBrect,byBBR,byBBG,byBBB);
		 
		
		::EnableWindow(::GetDlgItem(*s, IDC_BLACK_BALANCE), FALSE);	 
		
	  //end
	}
	else
	{
		CAM_AE_SetMicroMode(kFL);
		//sergey 27/12/05
	     s=::AfxGetMainWnd();
		::EnableWindow(::GetDlgItem(*s, IDC_BLACK_BALANCE), TRUE);	
	  //end
	}
	//sergey 05/02/06
	m_bChancedProf=1;
	//end
}

void CBaseDialog::OnFocus()
{
	if (s_bChanging) return;
	m_bFocus = IsDlgButtonChecked(IDC_CHECK_FOCUS)==1;
	if (m_bFocus)
		CAM_FF_Start();
	else
	{
		CAM_FF_Stop();
		//sergey 06/02/06
        CheckDlgButton(IDC_SHOW_FOCUS_RECT, 0);
		this->OnFocusRect();
		//end
	}
	SetDlgItemText(IDC_STATIC_FOCUS_CURRENT, _T(""));
	SetDlgItemText(IDC_STATIC_FOCUS_PEAK, _T(""));
	//sergey 05/02/06
	m_bChancedProf=1;
	//end
}

void CBaseDialog::OnFocusRect()
{
	if (s_bChanging) return;
	bool bFocusRect = IsDlgButtonChecked(IDC_SHOW_FOCUS_RECT)==1;
	bool bPrev;
	PxRectEx rect;
	UCHAR r,g,b;
	CAM_FF_GetRegion(&bPrev, &rect, &r, &g, &b);
	CAM_FF_SetRegion(bFocusRect, &rect, r, g, b);
	//sergey 05/02/06
	m_bChancedProf=1;
	//end
}

void CBaseDialog::OnHistogram()
{
	if (s_bChanging) return;
	m_bHistogram = IsDlgButtonChecked(IDC_HISTOGRAM);
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Histogram"), _T("Show"), m_bHistogram);
	OnLayoutChanged();
	if (m_pBase)
		m_pBase->OnLayoutChanged();
	//sergey 05/02/06
	m_bChancedProf=1;
	//end
}


void CBaseDialog::OnBBChange()
{
	if (s_bChanging) return;
	BOOL bBB = IsDlgButtonChecked(IDC_CHECK_BB)==1;
	if (bBB)
	{
		CAM_BB_Start();
		UCHAR byBBR = 255, byBBG = 255,byBBB = 255;
		bool bBB;
		PxRectEx BBrect;
		CAM_BB_GetRegion(&bBB, &BBrect, &byBBR, &byBBG, &byBBB);
		CAM_BB_SetRegion(bBB,&BBrect,byBBR,byBBG,byBBB);
	}
	else
		CAM_BB_Stop();
	EnableBB(bBB);
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("BlackBalance"), _T("UseBlackBalance"), bBB);
	//sergey 05/02/06
	m_bChancedProf=1;
	//end
}

void CBaseDialog::OnBBShowRect()
{
	if (CStdProfileManager::m_pMgr->GetProfileInt(_T("BlackBalance"), _T("UseBlackBalance"), 0))
	{
		UCHAR byBBR = 255, byBBG = 255,byBBB = 255;
		bool bBB;
		PxRectEx BBrect;
//		RestoreRect(_T("BlackBalance\\Rect"), bBB, BBrect, byBBR, byBBG, byBBB);
		CAM_BB_GetRegion(&bBB, &BBrect, &byBBR, &byBBG, &byBBB);
		bBB = IsDlgButtonChecked(IDC_SHOW_BB_RECT)==1;
		CAM_BB_SetRegion(bBB,&BBrect,byBBR,byBBG,byBBB);
	}
	//sergey 05/02/06
	m_bChancedProf=1;
	//end
}

void CBaseDialog::OnAEShowRect()
{
	bool bAERect;
	PxRectEx AERect;
	UCHAR byAER, byAEG, byAEB;
	RestoreRect(_T("Exposure\\Rect"), bAERect, AERect, byAER, byAEG, byAEB);
	bAERect = IsDlgButtonChecked(IDC_SHOW_EXPOSURE_RECTANGLE)==1;
	CAM_AE_SetSpotPosition(bAERect, &AERect, byAER, byAEG, byAEB);
	//sergey 05/02/06
	m_bChancedProf=1;
	//end
}

void CBaseDialog::OnDefault()
{
	BOOL bColorCamera = CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("ColorCamera"), TRUE, true, true);
	if (IsSmartEdit(IDC_EDIT_SHADOW, IDC_SLIDER_SHADOW) ||
		IsSmartEdit(IDC_EDIT_GAMMA, IDC_SLIDER_GAMMA) ||
		IsSmartEdit(IDC_EDIT_HIGHLIGHT, IDC_SLIDER_HIGHLIGHT))
	{
		LevelAdjEx level;
		memset(&level, 0, sizeof(level));
		level.nChannel = kLuminanceChannel;
		level.nInHighlight = 255;
		level.nInShadow = 0;
		level.nInGamma = 1.0;
		level.nChannel = kLuminanceChannel;
		CAM_IP_SetLuminanceLevels(&level);
		if (bColorCamera)
		{
		level.nChannel = kRedChannel;
		CAM_IP_SetLuminanceLevels(&level);
		level.nChannel = kGreenChannel;
		CAM_IP_SetLuminanceLevels(&level);
		level.nChannel = kBlueChannel;
		CAM_IP_SetLuminanceLevels(&level);
		}
		InitLevelAdjustment(bColorCamera?CStdProfileManager::m_pMgr->_GetProfileInt(_T("LevelAdjustment"), _T("CurrentLevel"), 0):0);
	}
	if (IsControl(IDC_EDIT_ACCUMULATION_TIMES))
	{
		CAM_SIA_SetAccumulateTimes(1);
		//InitSpin(IDC_SPIN_ACCUMULATION_TIMES, 1, 10, 1);
		//sergey 23/01/06
		InitSpin(IDC_SPIN_ACCUMULATION_TIMES, 2, 10, 1);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_EDIT_ACCUMULATION_TIMES), FALSE);
		//end
	}
	if (IsControl(IDC_COMBO_ACCUMULATION))
	{
		InitComboBox(IDC_COMBO_ACCUMULATION, 0);
		CAM_SIA_SetAccumulateMode(kNotAccumulating);
	}
	if (bColorCamera && (IsControl(IDC_WB_OFF) || IsControl(IDC_WB_AUTO) || IsControl(IDC_WB_ONE_PUSH) || IsControl(IDC_WB_MANUAL)))
	{
		g_nWBMode = kWBOff;
		CAM_WB_SetMode(kWBOff);
		CheckRadioButton(IDC_WB_OFF, IDC_WB_MANUAL, IDC_WB_OFF);
		EnableWB(kWBOff);
	}
	if (bColorCamera && IsControl(IDC_SHOW_WB_RECT))
	{
		bool bWBShow;
		PxRectEx WBrect;
		UCHAR byWBR,byWBG,byWBB;
		CAM_WB_GetRegion(&bWBShow,&WBrect,&byWBR,&byWBG,&byWBB);
		CAM_WB_SetRegion(false,&WBrect,byWBR,byWBG,byWBB);
		CheckDlgButton(IDC_SHOW_WB_RECT, 0);
	}
	if (bColorCamera &&
		(IsSmartEdit(IDC_SLIDER_WB_RED,IDC_EDIT_WB_RED) ||
		IsSmartEdit(IDC_SLIDER_WB_GREEN,IDC_EDIT_WB_GREEN) || 
		IsSmartEdit(IDC_SLIDER_WB_BLUE,IDC_EDIT_WB_BLUE)))
	{
		WhiteBalanceModeEx WBMode = CAM_WB_GetMode();
		//sergey 11/05/06
		//CAM_WB_SetFactors(1.,1.,1.);
		//end		
      
		InitCBFactor(IDC_EDIT_WB_RED,IDC_SPIN_WB_RED,IDC_SLIDER_WB_RED,1.,WBMode==kManualWB);
		InitCBFactor(IDC_EDIT_WB_GREEN,IDC_SPIN_WB_GREEN,IDC_SLIDER_WB_GREEN,1.,WBMode==kManualWB);
		InitCBFactor(IDC_EDIT_WB_BLUE,IDC_SPIN_WB_BLUE,IDC_SLIDER_WB_BLUE,1.,WBMode==kManualWB);
	}
	if (IsControl(IDC_CHECK_FLIP_VERT) || IsControl(IDC_CHECK_FLIP_HORZ))
	{
		CAM_IP_SetOrientation(kNoTransform);
		CheckDlgButton(IDC_CHECK_FLIP_VERT, 0);
		CheckDlgButton(IDC_CHECK_FLIP_HORZ, 0);
	}
	if (bColorCamera && IsControl(IDC_CHECK_GRAYSCALE))
	{
		CheckDlgButton(IDC_CHECK_GRAYSCALE, 0);
		CAM_IP_ColorCapture(kColorCamColor);
	
	//sergey 12/01/06
	
	
		CWnd *s;
		s=::AfxGetMainWnd();
		::EnableWindow(::GetDlgItem(*s, IDC_COLOR_BALANCE), TRUE);	
	}
	//end
	if (IsControl(IDC_COMBO_PREVIEW))
	{
		CAM_MIA_SetResolution(kFast);
		InitComboBox(IDC_COMBO_PREVIEW, 0);
	}
	if (IsControl(IDC_COMBO_CAPTURE))
	{
		CAM_SIA_SetResolution(kWarp);
		InitComboBox(IDC_COMBO_CAPTURE, 0);
	}
	if (IsControl(IDC_CHECK_FOCUS))
	{
		CheckDlgButton(IDC_CHECK_FOCUS, 0);
		CAM_FF_Stop();
		m_bFocus = false;
		SetDlgItemText(IDC_STATIC_FOCUS_CURRENT, _T(""));
		SetDlgItemText(IDC_STATIC_FOCUS_PEAK, _T(""));
	}
	if (IsControl(IDC_SHOW_FOCUS_RECT))
	{
		CheckDlgButton(IDC_SHOW_FOCUS_RECT, 0);
		PixRectData rd = RestoreDefaultRect(_T("Focus\\Rect"));
		CAM_FF_SetRegion(false, &rd.rect, rd.r, rd.g, rd.b);
	}
	if (IsControl(IDC_SHOW_EXPOSURE_RECTANGLE))
	{
		CheckDlgButton(IDC_SHOW_EXPOSURE_RECTANGLE, 0);
		PixRectData rd = RestoreDefaultRect(_T("Exposure\\Rect"));
		CAM_AE_SetSpotPosition(false, &rd.rect, rd.r, rd.g, rd.b);
		SaveRect(_T("Exposure\\Rect"), false, rd.rect, rd.r, rd.g, rd.b);
	}
	if (IsControl(IDC_MICRO_BRIGHT) || IsControl(IDC_MICRO_FLUORESCENT))
	{
		CheckRadioButton(IDC_MICRO_BRIGHT,IDC_MICRO_FLUORESCENT,IDC_MICRO_BRIGHT);
		CAM_AE_SetMicroMode(kBF);
		//sergey 12/01/06
		CWnd *s;
         s=::AfxGetMainWnd();
		::EnableWindow(::GetDlgItem(*s, IDC_BLACK_BALANCE), FALSE);

		//CWnd *s1=s->GetDlgItem(IDD_BLACK_BALANCE);
	   //s1->CheckDlgButton(IDC_CHECK_BB,0);
		//end
	}
	if (IsControl(IDC_HISTOGRAM))
	{
		m_bHistogram = false;
		CheckDlgButton(IDC_HISTOGRAM, 0 );
		CStdProfileManager::m_pMgr->WriteProfileInt(_T("Histogram"), _T("Show"), m_bHistogram);
		OnLayoutChanged();
		if (m_pBase)
			m_pBase->OnLayoutChanged();
	}
	if (IsControl(IDC_CHECK_BB) && IsDlgButtonChecked(IDC_CHECK_BB)==1)
	{
		CheckDlgButton(IDC_CHECK_BB, 0);
		CAM_BB_Stop();
		EnableBB(false);
		CStdProfileManager::m_pMgr->WriteProfileInt(_T("BlackBalance"), _T("UseBlackBalance"), false);
	}
}

void CBaseDialog::OnTimer(UINT_PTR id)
{
//	WhiteBalanceModeEx WBModeNew = CAM_WB_GetMode();
	if (id == 1)
	{
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_WB_BUILD), g_nWBMode == kOneTouchWB);
		KillTimer(1);
	}
	CDialog::OnTimer(id);
}

void CBaseDialog::OnOK()
{
	CWnd *pParent = GetParent();
	if (!pParent || !pParent->IsKindOf(RUNTIME_CLASS(CBaseDialog)))
		CDialog::OnOK();
}

void CBaseDialog::OnCancel()
{
	CWnd *pParent = GetParent();
	if (!pParent || !pParent->IsKindOf(RUNTIME_CLASS(CBaseDialog)))
		CDialog::OnCancel();
}


//sergey 11.01.06
void CBaseDialog::OnCancel11()
{
	CWnd *pParent = GetParent();
	if (!pParent || !pParent->IsKindOf(RUNTIME_CLASS(CBaseDialog)))
		CDialog::OnCancel();
}
void CBaseDialog::OnNew()
{
	char buffer[50];
	//if((fp=fopen("Metod.prf","w"))==NULL)		
		//VTMessageBox("ERROR OPEN FILE SERGEY.txt", "VT5", MB_OK);
	pEdit->GetLine(1,buffer);
	
	if(strlen(buffer)==0) ;
		//::AfxMessageBox("Input File Name");
		
	else 
		SaveSettingsS();
	SetDlgItemText(IDC_EDIT1,"");
}

void CBaseDialog::OnDelete()
{
	CFile myFile;

	CString prof;
	TCHAR szBuffName[50];
	TCHAR szBuffName1[50];
	TCHAR szPath[_MAX_PATH];

	    //prof.Format("C:\\Program Files\\¬идео“ес“\\%s.prof",fd.m_szFileName);
		//szBuffName=fd.m_pOFN->lpstrFile;
		//sprintf(szBuffName,"%s",fd.m_pOFN->lpstrFile);
	 

	     int nLiFi=GetListBoxValue(IDC_LIST1);	
		 if(nLiFi)
		 {	    	
	    GetCurrentDirectory(_MAX_PATH,szPath);	
        prof.Format("%s\\%s.prof",szPath,csListTextItem);
		//prof.Format("C:\\Program Files\\¬идео“ес“\\%s.prof",csListTextItem);	
		 myFile.Remove(prof);
		 InitListBox(IDC_LIST1, 0);
		 }
}

void CBaseDialog::OnSave()
{
               //RestoreSettingsS();
              //InitManualExposure();

	/*char szPath1[_MAX_PATH];
	LPCTSTR lpstrAdd="DEbag";
	static TCHAR szPath[_MAX_PATH],szDrive[_MAX_DRIVE],szDir[_MAX_DIR],szFName[_MAX_FNAME];
	static bool bInit = false;
	if (GetModuleFileName(0, szPath1, _MAX_PATH) == 0)
		{
			ASSERT(FALSE);			
		}
		GetLongPathName(szPath1, szPath, _MAX_PATH);
		_tsplitpath(szPath, szDrive, szDir, szFName, NULL);

		//_bstr_t bstrPath = GetValueString(GetAppUnknown(), "\\Input", "IniDir", NULL);
		_bstr_t bstrPath=L"YES";
		
		if (bstrPath.length() >0)
		{
			TCHAR szIniPath[_MAX_PATH];
			_tcsncpy(szIniPath, bstrPath, _MAX_PATH);
			int n = _tcslen(szIniPath);
			if (szIniPath[n-1] != _T('\\') && szIniPath[n-1] != _T('/'))
				_tcsncat(szIniPath, _T("\\"), _MAX_PATH);
			LPCTSTR lpDir = _tcschr(szIniPath, _T(':'));
			if (lpDir)
			{
				_tcsncpy(szDrive, szIniPath, _MAX_DRIVE);
				szDrive[lpDir-szIniPath+1] = 0;
				lpDir++;
			}
			else
				lpDir = szIniPath;
			if (lpDir[0] != 0)
			{
				if (lpDir[0] == '\\' || lpDir[0] == '/')
					_tcscpy(szDir, lpDir);
				else
				{
					int n = _tcslen(szDir);
					if (szDir[n-1] != '\\' && szDir[n-1] != '/')
						_tcsncat(szDir, _T("\\"), _MAX_DIR);
					_tcsncat(szDir, lpDir, _MAX_DIR);
				}
			}
		}
		bInit = true;
	
	if (lpstrAdd == NULL)
		_tmakepath(szPath, szDrive, szDir, szFName, _T(".ini"));
	else
	{
		TCHAR szFName1[_MAX_FNAME];
		_tcscpy(szFName1, szFName);
		_tcsncat(szFName1, _T("-"), _MAX_FNAME);
		_tcsncat(szFName1, lpstrAdd, _MAX_FNAME);
		_tmakepath(szPath, szDrive, szDir, szFName1, _T(".ini"));
	}
	//return szPath;


	CAM_AE_SetMicroMode((MicroModeEx)1);*/
}

inline LPCTSTR SecOrGen(LPCTSTR lpstrSecName)
{
	return lpstrSecName==NULL?_T("General"):lpstrSecName;
}

/*bool check(int &nVal, int nMin, int nMax)
{
	if (nVal < nMin)
	{
		nVal = nMin;
		return true;
	}
	else if (nVal > nMax)
	{
		nVal = nMax;
		return true;
	}
	else
		return false;
}*/

int CBaseDialog::GetProfileInt(LPCTSTR lpSection, LPCTSTR lpEntry, int nDef, bool bWriteToReg, bool bIgnoreMethodic)
{
	CFileDialog fd(false);
	CString prof;
	TCHAR szBuffName[50];
	TCHAR szBuffName1[50];

	    //prof.Format("C:\\Program Files\\¬идео“ес“\\%s.prof",fd.m_szFileName);
		//szBuffName=fd.m_pOFN->lpstrFile;
		//sprintf(szBuffName,"%s",fd.m_pOFN->lpstrFile);
	 

	     //int nLiFi=GetListBoxValue(IDC_LIST1);
	   TCHAR szPath[_MAX_PATH];	
	GetCurrentDirectory(_MAX_PATH,szPath);	
   // prof.Format("%s\\%s.prof",szPath,csListTextItem);// ѕ–≈ƒџƒ”ўјя
	prof.Format("%s",CStdProfileManager::m_pMgr->m_sMethodicIni);

		//prof.Format("C:\\Program Files\\¬идео“ес“\\%s.prof",csListTextItem);	 
	int n=GetPrivateProfileInt(SecOrGen(lpSection), lpEntry, -1,prof );
	if (n == -1)
	{
		n = CStdProfileManager::m_pMgr->GetDefaultProfileInt(lpSection, lpEntry, -1);
		if (n == -1)
		{
			n = nDef;
			if (bWriteToReg)
				CStdProfileManager::m_pMgr->WriteProfileInt(lpSection, lpEntry, n, bIgnoreMethodic);
		}
	}
	CStdProfileManager::m_pMgr->ValidateInt(lpSection, lpEntry, n);
	return n;
}

/*CString CStdProfileManager::_GetProfileString(LPCTSTR lpSection, LPCTSTR lpEntry, LPCTSTR lpDefault, bool bIgnoreMethodic)
{
	CString s;
	GetPrivateProfileString(SecOrGen(lpSection), lpEntry, lpDefault, s.GetBuffer(4096), 4095,
		GetIniName(bIgnoreMethodic));
	s.ReleaseBuffer();
	return s;
}*/


double CBaseDialog::_GetProfileDouble(LPCTSTR lpSection, LPCTSTR lpEntry, double dDef, bool *pbDefValueUsed, bool bIgnoreMethodic)
{
	CString prof;
	 //int nLiFi=GetListBoxValue(IDC_LIST1);

	TCHAR szBuffName1[50],szPath1[100],szPath[_MAX_PATH];	
	GetCurrentDirectory(_MAX_PATH,szPath);	
    //prof.Format("%s\\%s.prof",szPath,csListTextItem);// ѕ–≈ƒџƒ”ўјя
	prof.Format("%s",CStdProfileManager::m_pMgr->m_sMethodicIni);

		//prof.Format("C:\\Program Files\\¬идео“ес“\\%s.prof",csListTextItem);	

	CString sSection(lpSection);
	//CString s(_GetProfileString(sSection, lpEntry, NULL));
    CString s1;
	GetPrivateProfileString(SecOrGen(lpSection), lpEntry, NULL, s1.GetBuffer(4096),4095,prof);
	s1.ReleaseBuffer();
	CString s(s1);

	double dRet = dDef;
	if (!s.IsEmpty())
	{
		dRet= atof(s);
		if (pbDefValueUsed)
			*pbDefValueUsed = false;
	}
	else
	{
		if (pbDefValueUsed)
			*pbDefValueUsed = true;
	};
	return dRet;
}

double CBaseDialog::GetProfileDouble(LPCTSTR lpSection, LPCTSTR lpEntry, double dDef, bool *pbDefValueUsed,bool bWriteToReg, bool bIgnoreMethodic)
{
	bool bDefValueUsed;
	double dRet = _GetProfileDouble(lpSection, lpEntry, dDef, &bDefValueUsed, bIgnoreMethodic);
	if (bDefValueUsed)
	{
		dRet = CStdProfileManager::m_pMgr->GetDefaultProfileDouble(lpSection, lpEntry, dDef);
		if (bWriteToReg)
			CStdProfileManager::m_pMgr->WriteProfileDouble(lpSection, lpEntry, dRet);
	}
	CStdProfileManager::m_pMgr->ValidateDouble(lpSection, lpEntry, dRet);
	return dRet;
}

//static void RestoreLevelAdjustment(int nLevel)
void CBaseDialog::RestoreLevelAdjustment(int nLevel)
{
	static LPCTSTR szLvlNames[] =
	{
		_T("Luminance"),
		_T("Red"),
		_T("Green"),
		_T("Blue")
	};
	LevelAdjEx level;
	memset(&level, 0, sizeof(level));
	level.nChannel = (ChannelEx)(nLevel);
	CAM_IP_GetLuminanceLevels(&level);
	LPCTSTR lpszLvl = szLvlNames[nLevel-kLuminanceChannel];
	CString sSec;
	sSec.Format(_T("LevelAdustment\\%s"), lpszLvl);
	level.nInHighlight = GetProfileInt(sSec, _T("Highlight"), level.nInHighlight);
	level.nInShadow = GetProfileInt(sSec, _T("Shadow"), level.nInShadow);
	level.nInGamma = GetProfileDouble(sSec, _T("Gamma"), level.nInGamma);
	//level.nInGamma =GetProfileInt(sSec, _T("Gamma"), level.nInGamma);
	CAM_IP_SetLuminanceLevels(&level);
}

static void SaveLevelAdjustment(int nLevel,CString prof)
{
	static LPCTSTR szLvlNames[] =
	{
		_T("Luminance"),
		_T("Red"),
		_T("Green"),
		_T("Blue")
	};
	LevelAdjEx level;
	memset(&level, 0, sizeof(level));
	level.nChannel = (ChannelEx)(nLevel);
	CAM_IP_GetLuminanceLevels(&level);
	LPCTSTR lpszLvl = szLvlNames[nLevel-kLuminanceChannel];
	CString sSec;
	sSec.Format(_T("LevelAdustment\\%s"), lpszLvl);
	//CStdProfileManager::m_pMgr->WriteProfileInt(sSec, _T("Highlight"), level.nInHighlight);
	TCHAR szBuff[100];
	_itot(level.nInHighlight, szBuff, 10);
	WritePrivateProfileString(sSec, _T("Highlight"), szBuff, prof);
	//CStdProfileManager::m_pMgr->WriteProfileInt(sSec, _T("Shadow"), level.nInShadow);
	_itot(level.nInShadow, szBuff, 10);
	WritePrivateProfileString(sSec, _T("Shadow"), szBuff, prof);
	//CStdProfileManager::m_pMgr->WriteProfileDouble(sSec, _T("Gamma"), level.nInGamma);
	CString s;
	s.Format("%f", level.nInGamma);
	//_itot(level.nInGamma, szBuff, 10);
	WritePrivateProfileString(sSec, _T("Gamma"), s, prof);
}


void CBaseDialog:: RestoreSettingsS()
{
	
}


static void SaveRect(LPCTSTR sSec, bool bDraw, PxRectEx &rect, UCHAR r, UCHAR g, UCHAR b,CString prof)
{
	//CStdProfileManager::m_pMgr->WriteProfileInt(sSec, _T("Show"), bDraw);
	TCHAR szBuff[100];
	_itot(bDraw, szBuff, 10);
	WritePrivateProfileString(sSec, _T("Show"), szBuff, prof);
	//CStdProfileManager::m_pMgr->WriteProfileInt(sSec, _T("left"), rect.left);
	_itot(rect.left, szBuff, 10);
	WritePrivateProfileString(sSec,_T("left"), szBuff, prof);
	//CStdProfileManager::m_pMgr->WriteProfileInt(sSec, _T("top"), rect.top);
	_itot(rect.top, szBuff, 10);
	WritePrivateProfileString(sSec,_T("top"), szBuff, prof);
	//CStdProfileManager::m_pMgr->WriteProfileInt(sSec, _T("right"), rect.right);
	_itot(rect.right, szBuff, 10);
	WritePrivateProfileString(sSec,_T("right"), szBuff, prof);
	//CStdProfileManager::m_pMgr->WriteProfileInt(sSec, _T("bottom"), rect.bottom);
	_itot(rect.bottom, szBuff, 10);
	WritePrivateProfileString(sSec,_T("bottom"), szBuff, prof);
	//CStdProfileManager::m_pMgr->WriteProfileInt(sSec, _T("red"), (int)r);
	_itot((int)r, szBuff, 10);
	WritePrivateProfileString(sSec,_T("red"), szBuff, prof);
	//CStdProfileManager::m_pMgr->WriteProfileInt(sSec, _T("green"), (int)g);
	_itot((int)g, szBuff, 10);
	WritePrivateProfileString(sSec,_T("green"), szBuff, prof);
	//CStdProfileManager::m_pMgr->WriteProfileInt(sSec, _T("blue"), (int)b);
	_itot((int)b, szBuff, 10);
	WritePrivateProfileString(sSec,_T("blue"), szBuff, prof);
}


void CBaseDialog:: SaveSettingsS()
{
	
   }


 int CBaseDialog::_InitListBoxByReprData(CListBox *pLB,COMBOBOXREPRDATA *pReprData)
{
	int nCurSel11=0;
	CString s1,s2,s3;
	
	pLB->ResetContent();
	if (pReprData->ComboType == CBox_Integer)
	{
		/*for (int i = 0; i < pReprData->nValues; i++)
		{
			
			if (pReprData->pnValues[i].lpstrValue)
				pLB->AddString(pReprData->pnValues[i].lpstrValue);
			else if (pReprData->pnValues[i].idString > 0)
			{
				CString s;
				s.LoadString(pReprData->pnValues[i].idString);
				pLB->AddString(s);
			}
		}*/
		
		
       INTCOMBOITEMDESCR * r=g_Methodics.GetMethodics();
	CStringArray *Metodics=g_Methodics.GetMethodics1();
	CString strLast,strPixera;
	strLast.Format("%s",CStdProfileManager::m_pMgr->m_sMethodicIni);
	strPixera.Format("%s",CStdProfileManager::m_pMgr->m_sMethodic);	

	TCHAR szBuffer[512];
	CString str,str1,str2;
	str2=strPixera.GetString();

	if(str2=="")
	{
     str1.Format("%s","Predefined");
	}
	else
	{
	GetPrivateProfileString(_T("MethodicTitles"), _T("ru"), _T("ћои настройки"),szBuffer,512, strLast);	
	str1.Format("%s",szBuffer);
	}
	int nCount=g_Methodics.GetMethodicsCount();
	for(int i=0;i<g_Methodics.GetMethodicsCount();i++)
	
	{
		//if(g_Methodics.m_pDescr[i].lpstrValue)
		//pLB->AddString(g_Methodics.m_pDescr[i].lpstrValue);
        if(r[i].lpstrValue)
		{	
		//CString sPath=m_saMethodics1[i];	
		pLB->AddString(r[i].lpstrValue);
		str.Format("%s",r[i].lpstrValue);
		if(str==str1)
			nCurSel11=i;
		}
		else if (r[i].idString > 0)
			{
				CString s;
				s.LoadString(r[i].idString);
				pLB->AddString(s);
			}
	}
	
	}
	return nCurSel11;
}
//void InitListBox1(HWND hWnd, int id, int nCurSel, COMBOBOXREPRDATA *pReprData)
void CBaseDialog::InitListBox1( int id, int nCurSel,COMBOBOXREPRDATA *pReprData)
{
	//CWnd *pWnd = CWnd::FromHandle(hWnd);
	int nCurSel11=nCurSel;
	g_Methodics.OnInitControl();
	pCombo1 = (CListBox *)GetDlgItem(id);
	if (pCombo1)
	{
		if (g_Methodics.GetRepr())
			nCurSel11= _InitListBoxByReprData(pCombo1,pReprData);
		pCombo1->SetCurSel(nCurSel11);
	}
}


void CBaseDialog::InitListBox(int id, int nCurSel)
{	
	CString s;
	CFileFind finder;
	char szBuffName[50],szBuffName1[50],szPath[_MAX_PATH];
	
	GetCurrentDirectory(_MAX_PATH,szPath);
//static const TCHAR szFileToFind[] = _T("C:\\Program Files\\¬идео“ес“\\*.prof");
	 TCHAR szFileToFind[500];
	sprintf(szFileToFind,"%s\\*prof",szPath);



	//CListBox *pCombo = (CListBox *)GetDlgItem(id);
	pCombo = (CListBox *)GetDlgItem(id);
	if (pCombo)	
		pCombo->SetCurSel(nCurSel);
	if (pCombo!=NULL)
	{
		pCombo->ResetContent();
		BOOL bResult = finder.FindFile(szFileToFind);	
		
			
           while(bResult)
		   { 
			   bResult=finder.FindNextFile();
          s=finder.GetFileName();
		 
		  int i=s.Find(".");
		  s.Delete(i,5);         
		  
        pCombo->AddString((LPCTSTR)s );	// добавить строку			   
		
	       }
	}
	//::GetValueString( ::GetAppUnknown(), _T("Paths"), _T("HelpPath"), szPathName );
	

}


int CBaseDialog::GetListBoxValue(int id)
{
	//CListBox *pCombo = (CListBox *)GetDlgItem(id);
	pCombo = (CListBox *)GetDlgItem(id);
	/*if (pCombo)
		return pCombo->GetCurSel();
	else
		return 0;*/
	 if (pCombo->GetCurSel()==LB_ERR )	// если в списке ничего не выбрано
	{
		//AfxMessageBox("Select Item List Box");	// сообщить о ошибке
  //::AfxMessageBox("Select Item List Box");
		return 0;
	}
	else	// иначе, то есть что-то выбрано
	{
		pCombo->GetText(pCombo->GetCurSel(),csListTextItem); // получить результат
		return 1;
		
	}
}


void CBaseDialog::InitEdit(int id)
{
	CWnd* s=::AfxGetMainWnd();
	
	char buffer[50];
	CString str;
	//CWnd* Edit=s->GetDlgItem(IDD_SETTING);
	//Edit->GetDlgItemText(id,str);
    pEdit=(CEdit *)GetDlgItem(id);
	if(pEdit)
	{
	pEdit->SetFocus();
	pEdit->GetLine(1,buffer);
	
	if(strlen(buffer))
	{
	pEdit->Clear();
	UpdateData(FALSE);
	}
	}
}

void CBaseDialog::OnEdit1Change()
{	
	char buffer[50];
	pEdit=(CEdit *)GetDlgItem(IDC_EDIT1);	
	pEdit->GetLine(0,buffer);
    csEdit1TextItem.Format("%s",buffer);
	
	
	
}
void CBaseDialog::OnFileName()
{
	CDialog dlg(IDD_DIALOG1);
	int nResult;
	if(dlg.DoModal()==IDOK)
	{
		dlg.EndDialog(nResult);
	}
	else dlg.EndDialog(nResult);
}

void CBaseDialog::OnRestoreMetodick()
{
	//RestoreSettingsS();
    //InitManualExposure();
}

void CBaseDialog::DoSelectMethodic(int nMethodic)
{
	//XStopPreview StopGrab(this, m_CamState==Succeeded&&m_nGrabCount>0, true);
	//StopGrab(this, m_CamState==Succeeded&&m_nGrabCount>0, true);	
	g_Methodics.SetValueInt(nMethodic,false);	
	CStdProfileManager::m_pMgr->WriteProfileString(_T("Methodics"), _T("Default"),
		CStdProfileManager::m_pMgr->GetMethodic(), true);
	
	ReadSettings();
	//OnResetValues();
	ResetAllControls();
	UpdateCmdUI();
	//Sleep(200);
}

void CBaseDialog::OnNew11()
{
	//int n = g_Methodics.NewMethodic();
	
}



BOOL CBaseDialog::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
//HRESULT CBaseDialog::OnCmdMsg(int nCode, UINT nID, long lHWND)
{
	if (nID > 0)
	{
		
		if(nID!=1014)
		{
		//int no=g_Methodics.id();

		if (nID == g_Methodics.GetRepr()->nDlgCtrlId && nCode == LBN_SELCHANGE)		
		{
			int nWBMode = CStdProfileManager::m_pMgr->GetProfileInt(_T("WhiteBalance"), _T("Mode"), kAutoWB);
			//int nCurSel = (int)::SendMessage((HWND)GetDlgItem(nID), LB_GETCURSEL, 0, 0);
            pCombo1 = (CListBox *)GetDlgItem(IDC_LIST11);
			int nCurSel=pCombo1->GetCurSel();			
			int met=(int)g_Methodics.GetValueInt();
            //sergey 10/02/06
			  if(m_bChancedProf)
	           {
	           CDialog dlgList(IDD_DIALOG_SETTING_NAME12);	
	           if(dlgList.DoModal()==IDOK)
			   {
				   ::SaveSettings();
			       
			   }
			   m_bChancedProf=0;
			   }
			 if (nCurSel != (int)g_Methodics.GetValueInt())
			 {
				
			  DoSelectMethodic(nCurSel);
			 
			  InitManualExposure();
			 ::RestoreSettings();
			 // Sleep(500);
			 RestoreSettingsLite();
			 //InitControlsState();
			 //????????????????????????????????
			 if (nWBMode == kAutoWB)	
				{
					bool bWBShow;
				PxRectEx WBrect;
				UCHAR byWBR,byWBG,byWBB;
				CAM_WB_GetRegion(&bWBShow,&WBrect,&byWBR,&byWBG,&byWBB);
				//bWBShow = IsDlgButtonChecked(IDC_SHOW_WB_RECT)==1;
				CAM_WB_SetMode(kAutoWB);
				double dWBRed,dWBGreen,dWBBlue;		
					CAM_WB_GetFactors(&dWBRed,&dWBGreen,&dWBBlue);
				CAM_WB_SetRegion(bWBShow,&WBrect,byWBR,byWBG,byWBB);
				}
				if (nWBMode == kOneTouchWB)	
				{
					CAM_WB_CalibrateFactors();
				//CAM_WB_SetMode(kAutoWB);
				CAM_WB_SetMode(kOneTouchWB);
				}
				if (nWBMode == kManualWB)
		        {
					double dWBRed,dWBGreen,dWBBlue;
					CAM_WB_GetFactors(&dWBRed,&dWBGreen,&dWBBlue);
					//sergey 11/05/06
					CAM_WB_SetFactors(1.,1.,1.);
					//CAM_WB_SetMode(kAutoWB);
		//          Sleep(200);
					Sleep(300);
					CAM_WB_SetFactors(dWBRed,dWBGreen,dWBBlue);
					TRACE("TimerProc, factors are %f,%f,%f\n", dWBRed, dWBGreen, dWBBlue);
					Sleep(500);
					//end
				}
			 //??????????????????????????????????
			//InitControlsState();		 
			        
			  //sergey 11/05/06
			   BOOL bColorCamera = CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("ColorCamera"), FALSE, true, true);
			  	             
			  //end		
			   if (nWBMode != kManualWB)
		        {
			  Sleep(500);			
				}
				 
			//::RestoreSettings();
			RestoreSettingsLite();
			InitControlsState();
			 Sleep(800);
			}				
			return S_OK;			
		}       
		
		/*if (nID == g_Methodics.id() && nCode ==4) //LBN_SELCHANGE)
		{
		if(m_bChancedProf)
	           {
	           CDialog dlgList(IDD_DIALOG_SETTING_NAME12);	
	           if(dlgList.DoModal()==IDOK)
			   {
				   ::SaveSettings();
			       
			   }
			   m_bChancedProf=0;
			   }
			int nCurSel = (int)::SendMessage((HWND)this, LB_GETCURSEL, 0, 0);
			if (nCurSel != (int)g_Methodics)
			{
				DoSelectMethodic(nCurSel);
				::RestoreSettings();			 
			 InitManualExposure();
			 InitControlsState();
			}
			//return S_OK;
			return TRUE;
		}*/
		else if (nID == m_nIdNewMeth)
		{			
			int n = g_Methodics.NewMethodic();
			if(n==-1)
			{
			::SaveSettings();
			m_bChancedProf=0;
			}			
	        COMBOBOXREPRDATA *pReprData=(COMBOBOXREPRDATA *)g_Methodics.GetRepr();
			InitListBox1(IDC_LIST11,0,pReprData);

			//return S_OK;
			return TRUE;
		}
		else if (nID == m_nIdDeleteMeth && g_Methodics.GetMethodicsCount() > 1)
		{
			CString sMethIni = CStdProfileManager::m_pMgr->GetIniName(false);
			CString sMethDefIni = CStdProfileManager::m_pMgr->GetDefIniName(false);
			if (sMethDefIni == CStdProfileManager::m_pMgr->GetDefIniName(true))
				sMethDefIni.Empty();
			int nCurMethodic = g_Methodics;
			DoSelectMethodic(nCurMethodic>=1?nCurMethodic-1:nCurMethodic+1);
			remove(sMethIni);
			if (!sMethDefIni.IsEmpty())
				remove(sMethDefIni);
			g_Methodics.OnInitControl();
			g_Methodics.ReinitControls();

			 COMBOBOXREPRDATA *pReprData=(COMBOBOXREPRDATA *)g_Methodics.GetRepr();
			InitListBox1(IDC_LIST11,0,pReprData);
			//return S_OK;
			return TRUE;
		}
		 
		}
	}
	//return S_FALSE;

	 return __super::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CBaseDialog::ResetAllControls()
{
	for (int i = 0; i < m_arrDialogs.GetSize(); i++)
		m_arrDialogs[i]->OnResetSettings(NULL, NULL);
}



HRESULT CBaseDialog::OnUpdateCmdUI(HWND hwnd, int nID)
{
	if (nID > 0)
	{
		if (nID == m_nIdDeleteMeth)
		{
			BOOL bCanDelete = CStdProfileManager::m_pMgr->GetProfileInt(_T("Methodic"),_T("CanDelete"),0,false,false);
			::EnableWindow(hwnd, bCanDelete&&g_Methodics.GetMethodicsCount()>1);
		}
		/*else if (nID == g_FocusRect.GetRepr()->nDlgCtrlId)
			::EnableWindow(hwnd, (bool)g_Focus);
		else if (nID == g_FocusValue.id()/* || nID == g_MaxFocus.id() || nID == m_nIdStaticFocusMax||
			nID == m_nIdStaticFocusCur*///)
			/*::ShowWindow(hwnd, (bool)g_Focus?SW_SHOW:SW_HIDE);*/
	}
	return S_FALSE;
}

void CBaseDialog::ReadSettings()
{
	CCamValue::InitChain(0, true);
	m_ExposureMask.Reinit();
	m_bSettingsChanged = true;
}

void CBaseDialog::UpdateCmdUI()
{
	for (int i = 0; i < m_arrDialogs.GetSize(); i++)
		m_arrDialogs[i]->UpdateCmdUI();
}







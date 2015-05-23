// BaseDialog.cpp: implementation of the CBaseDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "BaseDialog.h"
#include "StdProfile.h"
#include "Ctrls.h"
#include "SmartEdit.h"
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
}

CBaseDialog::~CBaseDialog()
{
}


BEGIN_MESSAGE_MAP(CBaseDialog, CBaseDlg)
	//{{AFX_MSG_MAP(CPreviewDialog)
	ON_BN_CLICKED(IDC_HISTOGRAM, OnHistogram)
	ON_BN_CLICKED(IDC_CHECK_GRAYSCALE, OnGrayscale)
	ON_BN_CLICKED(IDC_GRAY, OnGray)
	ON_BN_CLICKED(IDC_RED, OnRed)
	ON_BN_CLICKED(IDC_GREEN, OnGreen)
	ON_BN_CLICKED(IDC_BLUE, OnBlue)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CBaseDialog::InitControls()
{
	InitControl(new CSmartRangeDoubleEdit(IDC_EDIT_BRIGHTNESS, IDC_SPIN_BRIGHTNESS, IDC_SLIDER_BRIGHTNESS,
		_T("Settings"),	_T("Brightness"), -1., 1., .1, 0.));
	InitControl(new CSmartRangeDoubleEdit(IDC_EDIT_CONTRAST, IDC_SPIN_CONTRAST, IDC_SLIDER_CONTRAST,
		_T("Settings"),	_T("Contrast"), .5, 2., .1, 1.));
	if (CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("UseContrastForWhiteBalance"), TRUE, true, true))
	{
		InitControl(new CSmartRangeDoubleEdit(IDC_EDIT_WB_RED, IDC_SPIN_WB_RED, IDC_SLIDER_WB_RED, _T("Settings"),
			_T("RedContrast"), 0.5, 2., 0.1, 1.));
		InitControl(new CSmartRangeDoubleEdit(IDC_EDIT_WB_GREEN, IDC_SPIN_WB_GREEN, IDC_SLIDER_WB_GREEN, _T("Settings"),
			_T("GreenContrast"), 0.5, 2., 0.1, 1.));
		InitControl(new CSmartRangeDoubleEdit(IDC_EDIT_WB_BLUE, IDC_SPIN_WB_BLUE, IDC_SLIDER_WB_BLUE, _T("Settings"),
			_T("BlueContrast"), 0.5, 2., 0.1, 1.));
	}
	else
	{
		InitControl(new CSmartRangeDoubleEdit(IDC_EDIT_WB_RED, IDC_SPIN_WB_RED, IDC_SLIDER_WB_RED, _T("Settings"),
			_T("RedLuminance"), -1., 1., 0.1, 0.));
		InitControl(new CSmartRangeDoubleEdit(IDC_EDIT_WB_GREEN, IDC_SPIN_WB_GREEN, IDC_SLIDER_WB_GREEN, _T("Settings"),
			_T("GreenLuminance"), -1., 1., 0.1, 0.));
		InitControl(new CSmartRangeDoubleEdit(IDC_EDIT_WB_BLUE, IDC_SPIN_WB_BLUE, IDC_SLIDER_WB_BLUE, _T("Settings"),
			_T("BlueLuminance"), -1., 1., 0.1, 0.));
	}
	InitCurColor(0);
}

void CBaseDialog::InitCurColor(int n)
{
	m_nCurColor = n;
	RemoveControl(IDC_EDIT_WB_BRIGHTNESS);
	RemoveControl(IDC_EDIT_WB_CONTRAST);
	if (m_nCurColor == 0)
	{
		InitControl(new CSmartRangeDoubleEdit(IDC_EDIT_WB_BRIGHTNESS, IDC_SPIN_WB_BRIGHTNESS, IDC_SLIDER_WB_BRIGHTNESS,
			_T("Settings"),	_T("RedLuminance"), -1., 1., .1, 0.));
		InitControl(new CSmartRangeDoubleEdit(IDC_EDIT_WB_CONTRAST, IDC_SPIN_WB_CONTRAST, IDC_SLIDER_WB_CONTRAST,
			_T("Settings"),	_T("RedContrast"), .5, 2., .1, 1.));
	}
	else if (m_nCurColor == 1)
	{
		InitControl(new CSmartRangeDoubleEdit(IDC_EDIT_WB_BRIGHTNESS, IDC_SPIN_WB_BRIGHTNESS, IDC_SLIDER_WB_BRIGHTNESS,
			_T("Settings"),	_T("GreenLuminance"), -1., 1., .1, 0.));
		InitControl(new CSmartRangeDoubleEdit(IDC_EDIT_WB_CONTRAST, IDC_SPIN_WB_CONTRAST, IDC_SLIDER_WB_CONTRAST,
			_T("Settings"),	_T("GreenContrast"), .5, 2., .1, 1.));
	}
	else if (m_nCurColor == 2)
	{
		InitControl(new CSmartRangeDoubleEdit(IDC_EDIT_WB_BRIGHTNESS, IDC_SPIN_WB_BRIGHTNESS, IDC_SLIDER_WB_BRIGHTNESS,
			_T("Settings"),	_T("BlueLuminance"), -1., 1., .1, 0.));
		InitControl(new CSmartRangeDoubleEdit(IDC_EDIT_WB_CONTRAST, IDC_SPIN_WB_CONTRAST, IDC_SLIDER_WB_CONTRAST,
			_T("Settings"),	_T("BlueContrast"), .5, 2., .1, 1.));
	}
	::CheckRadioButton(m_hWnd, IDC_RED, IDC_BLUE, IDC_RED+m_nCurColor);
}

void CBaseDialog::InitControlsState()
{
	CLock lock(&s_bChanging);
	InitControls();
	BOOL bColor = CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("Color"), false, true);
	::CheckDlgButton(m_hWnd, IDC_CHECK_GRAYSCALE, !bColor);
	m_bHistogram = CStdProfileManager::m_pMgr->GetProfileInt(_T("Histogram"), _T("Show"), FALSE);
	::CheckDlgButton(m_hWnd, IDC_HISTOGRAM, m_bHistogram?1:0);
	::CheckRadioButton(m_hWnd, IDC_RED, IDC_BLUE, IDC_RED+m_nCurColor);
	m_bInited = true;
}

void CBaseDialog::OnGrayscale()
{
	BOOL bGrayScale = IsDlgButtonChecked(IDC_CHECK_GRAYSCALE);
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Settings"), _T("Color"), !bGrayScale);
	m_sptrDrv->SetValueInt(m_nDev, _bstr_t("Settings"), _bstr_t("Color"), !bGrayScale, 0);
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


// SourceDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "MV500Grab.h"
#include "SourceDlg.h"
#include "Vt5Profile.h"
#include "Defs.h"
#include "ComDef.h"
#include "misc_utils.h"
#include "Htmlhelp.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSourceDlg dialog


CSourceDlg::CSourceDlg(int nDevice, CWnd* pParent /*=NULL*/, int idd /*= false*/)
	: CDialog(idd, pParent)
{
	//{{AFX_DATA_INIT(CSourceDlg)
	m_bSVideo = CStdProfileManager::m_pMgr->GetProfileInt(_T("Source"), _T("2SVideo"), FALSE);
	m_bAGC = CStdProfileManager::m_pMgr->GetProfileInt(_T("Source"), _T("AGC"), FALSE);
	m_nHue = CStdProfileManager::m_pMgr->GetProfileInt(_T("Source"), _T("Hue"), 0);
	m_nSaturation = CStdProfileManager::m_pMgr->GetProfileInt(_T("Source"), _T("Saturation"), 128);
	m_nBrightness = CStdProfileManager::m_pMgr->GetProfileInt(_T("Source"), _T("Brightness"), 128);
	m_nContrast = CStdProfileManager::m_pMgr->GetProfileInt(_T("Source"), _T("Contrast"), 128);
	m_bVCR = CStdProfileManager::m_pMgr->GetProfileInt(_T("Source"), _T("VCRMode"), 0);
	m_nVideoStandard = CStdProfileManager::m_pMgr->GetProfileInt(_T("Source"), _T("VideoStandard"), 0);
	//}}AFX_DATA_INIT
	m_nIDD = idd;
	m_nDevice = idd==IDD_SOURCE2?0:nDevice;
	if (m_bSVideo)
	{
		m_nConnector1 = -1;
		m_nConnector2 = CMV500Grab::s_pGrab->GetConnector(m_nIDD==IDD_SOURCE2?0:m_nDevice);
	}
	else
	{
		m_nConnector1 = CMV500Grab::s_pGrab->GetConnector(m_nIDD==IDD_SOURCE2?0:m_nDevice);
		m_nConnector2 = -1;
	}
	m_nGWX  = CStdProfileManager::m_pMgr->GetProfileInt(_T("Source"), _T("GrabWindowX"), 0);
	m_nGWY  = CStdProfileManager::m_pMgr->GetProfileInt(_T("Source"), _T("GrabWindowY"), 0);
	m_nGWDX = CStdProfileManager::m_pMgr->GetProfileInt(_T("Source"), _T("GrabWindowDX"), MaxDX());
	m_nGWDY = CStdProfileManager::m_pMgr->GetProfileInt(_T("Source"), _T("GrabWindowDY"), MaxDY());
	m_nGWScope = CStdProfileManager::m_pMgr->GetProfileInt("Source", "GrabWindowScope", 20, true);
	m_nGWStep = max(m_nGWScope/5,1);
	m_pSite = NULL;
	// Save previous settings
	m_bPrevSVideo = m_bSVideo;
	m_nPrevConnector2 = m_nConnector2;
	m_nPrevConnector1 = m_nConnector1;
	if (m_nIDD = IDD_SOURCE2)
		m_nPrevConnectorCam2 = CMV500Grab::s_pGrab->GetConnector(1);
	m_bPrevAGC = m_bAGC;
	m_nPrevHue = m_nHue;
	m_nPrevSaturation = m_nSaturation;
	m_nPrevBrightness = m_nBrightness;
	m_nPrevContrast = m_nContrast;
	m_bPrevVCR = m_bVCR;
	m_nPrevGWDX = m_nGWDX;
	m_nPrevGWDY = m_nGWDY;
	m_nPrevGWX = m_nGWX;
	m_nPrevGWY = m_nGWY;
	m_nPrevVideoStandard = m_nVideoStandard;
}

bool CSourceDlg::IsPalFormat()
{
	if (m_nVideoStandard == 1 || m_nVideoStandard == 2 || m_nVideoStandard == 4 || m_nVideoStandard == 5 ||
		m_nVideoStandard == 6 || m_nVideoStandard == 8)
		return true;
	else
		return false;
}

int CSourceDlg::MaxDX()
{
	int nMaxX,nMaxY;
	MV5InquireMaxGrabWindowSize(&nMaxX, &nMaxY);
	return nMaxX;
/*	if (IsPalFormat())
		return 768;
	else
		return 640;*/
}

int CSourceDlg::MaxDY()
{
	int nMaxX,nMaxY;
	MV5InquireMaxGrabWindowSize(&nMaxX, &nMaxY);
	return nMaxY;
	/*if (IsPalFormat())
		return 576;
	else
		return 480;*/
}

void CSourceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSourceDlg)
	DDX_Control(pDX, IDC_S_GWDY, m_SGWDY);
	DDX_Control(pDX, IDC_S_GWDX, m_SGWDX);
	DDX_Control(pDX, IDC_S_GWY, m_SGWY);
	DDX_Control(pDX, IDC_S_GWX, m_SGWX);
	DDX_Control(pDX, IDC_S_SATURATION, m_Saturation);
	DDX_Control(pDX, IDC_S_HUE, m_Hue);
	DDX_Control(pDX, IDC_S_CONTRAST, m_Contrast);
	DDX_Control(pDX, IDC_S_BRIGHTNESS, m_Brightness);
	DDX_Check(pDX, IDC_2SVIDEO, m_bSVideo);
	DDX_Radio(pDX, IDC_1STSVIDEO, m_nConnector2);
	DDX_Radio(pDX, IDC_RED, m_nConnector1);
	DDX_Check(pDX, IDC_AGC, m_bAGC);
	DDX_Text(pDX, IDC_E_HUE, m_nHue);
	DDX_Text(pDX, IDC_E_SATURATION, m_nSaturation);
	DDX_Text(pDX, IDC_E_BRIGHTNESS, m_nBrightness);
	DDX_Text(pDX, IDC_E_CONTRAST, m_nContrast);
	DDX_Check(pDX, IDC_VCR, m_bVCR);
	DDX_Text(pDX, IDC_E_GWDX, m_nGWDX);
	DDX_Text(pDX, IDC_E_GWDY, m_nGWDY);
	DDX_Text(pDX, IDC_E_GWX, m_nGWX);
	DDX_Text(pDX, IDC_E_GWY, m_nGWY);
	DDX_Radio(pDX, IDC_STANDARD_NTSC, m_nVideoStandard);
	//}}AFX_DATA_MAP
	if (!pDX->m_bSaveAndValidate)
	{
		if (!::CheckRadioButton(m_hWnd, IDC_STANDARD_NTSC, IDC_STANDARD_PAL, IDC_STANDARD_NTSC+m_nVideoStandard))
		{
			int n = GetLastError();
		}
		ASSERT(::IsDlgButtonChecked(m_hWnd,IDC_STANDARD_NTSC+m_nVideoStandard));
	}
}


BEGIN_MESSAGE_MAP(CSourceDlg, CDialog)
	//{{AFX_MSG_MAP(CSourceDlg)
	ON_BN_CLICKED(IDC_2SVIDEO, On2svideo)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_1STSVIDEO, On1stsvideo)
	ON_BN_CLICKED(IDC_2NDSVIDEO, On2ndsvideo)
	ON_BN_CLICKED(IDC_AGC, OnAgc)
	ON_BN_CLICKED(IDC_ANALOG, OnAnalog)
	ON_BN_CLICKED(IDC_BLUE, OnBlue)
	ON_EN_CHANGE(IDC_E_BRIGHTNESS, OnChangeEBrightness)
	ON_EN_CHANGE(IDC_E_CONTRAST, OnChangeEContrast)
	ON_EN_CHANGE(IDC_E_HUE, OnChangeEHue)
	ON_EN_CHANGE(IDC_E_SATURATION, OnChangeESaturation)
	ON_BN_CLICKED(IDC_GREEN, OnGreen)
	ON_BN_CLICKED(IDC_RED, OnRed)
	ON_BN_CLICKED(IDC_SVIDEO, OnSvideo)
	ON_BN_CLICKED(IDC_VCR, OnVcr)
	ON_BN_CLICKED(IDC_DEFAULT, OnDefault)
	ON_BN_CLICKED(IDC_STANDARD_NTSC, OnStandardNtsc)
	ON_BN_CLICKED(IDC_STANDARD_NTSC44, OnStandardNtsc44)
	ON_BN_CLICKED(IDC_STANDARD_NTSCCOMB, OnStandardNtsccomb)
	ON_BN_CLICKED(IDC_STANDARD_NTSCMONO, OnStandardNtscmono)
	ON_BN_CLICKED(IDC_STANDARD_PAL, OnStandardPal)
	ON_BN_CLICKED(IDC_STANDARD_PALM, OnStandardPalm)
	ON_BN_CLICKED(IDC_STANDARD_PALN, OnStandardPaln)
	ON_BN_CLICKED(IDC_STANDARD_SECAM, OnStandardSecam)
	ON_BN_CLICKED(IDC_STANDARD_PAL60, OnStandardPal60)
	ON_BN_CLICKED(IDC_STANDARD_SECAMMONO, OnStandardSecammono)
	ON_EN_CHANGE(IDC_E_GWDX, OnChangeEGwdx)
	ON_EN_CHANGE(IDC_E_GWDY, OnChangeEGwdy)
	ON_EN_CHANGE(IDC_E_GWX, OnChangeEGwx)
	ON_EN_CHANGE(IDC_E_GWY, OnChangeEGwy)
	ON_BN_CLICKED(IDC_RED2, OnRed2)
	ON_BN_CLICKED(IDC_GREEN2, OnGreen2)
	ON_BN_CLICKED(IDC_BLUE2, OnBlue2)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSourceDlg message handlers

BOOL CSourceDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	UpdateControls();
	m_Brightness.SetRange(0, 255);
	m_Brightness.SetPos(m_nBrightness);
	m_Brightness.SetTicFreq(16);
	m_Contrast.SetRange(0, 255);
	m_Contrast.SetPos(m_nContrast);
	m_Contrast.SetTicFreq(16);
	m_Hue.SetRange(-128, 127);
	m_Hue.SetPos(m_nHue);
	m_Hue.SetTicFreq(16);
	m_Hue.EnableWindow(CMV500Grab::s_pGrab->GetBoardType()==CMV500Grab::Board_MV500);
	m_Saturation.SetRange(0, 255);
	m_Saturation.SetPos(m_nSaturation);
	m_Saturation.SetTicFreq(16);
	m_Saturation.EnableWindow(CMV500Grab::s_pGrab->GetBoardType()==CMV500Grab::Board_MV500);
//	if (IsPalFormat())
//	{
		m_SGWX.SetRange(0,m_nGWScope);
		m_SGWX.SetPos(m_nGWX);
		m_SGWX.SetTicFreq(m_nGWStep);
		m_SGWY.SetRange(0,m_nGWScope);
		m_SGWY.SetPos(m_nGWY);
		m_SGWY.SetTicFreq(m_nGWStep);
		m_SGWDX.SetRange(MaxDX()-m_nGWScope,MaxDX());
		m_SGWDX.SetPos(m_nGWDX);
		m_SGWDX.SetTicFreq(m_nGWStep);
		m_SGWDY.SetRange(MaxDY()-m_nGWScope,MaxDY());
		m_SGWDY.SetPos(m_nGWDY);
		m_SGWDY.SetTicFreq(m_nGWStep);
/*	}
	else
	{
		m_SGWX.SetRange(0,m_nGWScope);
		m_SGWX.SetPos(m_nGWX);
		m_SGWX.SetTicFreq(m_nGWStep);
		m_SGWY.SetRange(0,m_nGWScope);
		m_SGWY.SetPos(m_nGWY);
		m_SGWY.SetTicFreq(m_nGWStep);
		m_SGWDX.SetRange(640-m_nGWScope,640);
		m_SGWDX.SetPos(m_nGWDX);
		m_SGWDX.SetTicFreq(m_nGWStep);
		m_SGWDY.SetRange(480-m_nGWScope,480);
		m_SGWDY.SetPos(m_nGWDY);
		m_SGWDY.SetTicFreq(m_nGWStep);
	}*/
	m_SGWX.SetLineSize(4);
	m_SGWY.SetLineSize(4);
	m_SGWDX.SetLineSize(4);
	m_SGWDY.SetLineSize(4);
	if (m_nIDD == IDD_SOURCE2)
		::CheckRadioButton(m_hWnd, IDC_RED2, IDC_BLUE2, IDC_RED2+CMV500Grab::s_pGrab->GetConnector(1));
	BOOL bCustomCamera = CStdProfileManager::m_pMgr->GetProfileInt(_T("Source"), _T("CustomCamera"), FALSE, true, true);
	if (bCustomCamera)
	{
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_STANDARD_NTSC), FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_STANDARD_PAL), FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_STANDARD_SECAM), FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_STANDARD_NTSC44), FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_STANDARD_PALM), FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_STANDARD_PALN), FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_STANDARD_PAL60), FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_STANDARD_NTSCCOMB), FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_STANDARD_SECAMMONO), FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_STANDARD_NTSCMONO), FALSE);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSourceDlg::OnDefault() 
{
	UpdateData();
	m_nHue = CStdProfileManager::m_pMgr->GetDefaultProfileInt(_T("Source"), _T("Hue"), 0);
	m_Hue.SetPos(m_nHue);
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("Hue"), m_nHue);
	m_nBrightness = CStdProfileManager::m_pMgr->GetDefaultProfileInt(_T("Source"), _T("Brightness"), 128);
	m_Brightness.SetPos(m_nBrightness);
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("Brightness"), m_nBrightness);
	m_nContrast = CStdProfileManager::m_pMgr->GetDefaultProfileInt(_T("Source"), _T("Contrast"), 128);
	m_Contrast.SetPos(m_nContrast);
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("Contrast"), m_nContrast);
	m_nSaturation = CStdProfileManager::m_pMgr->GetDefaultProfileInt(_T("Source"), _T("Saturation"), 128);
	m_Saturation.SetPos(m_nSaturation);
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("Saturation"), m_nSaturation);
	UpdateData(FALSE);
	if (m_pSite) m_pSite->Reinit(2);
}

void CSourceDlg::UpdateControls() 
{
	if (m_bSVideo)
	{
		::ShowWindow(::GetDlgItem(m_hWnd, IDC_RED), SW_HIDE);
		::ShowWindow(::GetDlgItem(m_hWnd, IDC_GREEN), SW_HIDE);
		::ShowWindow(::GetDlgItem(m_hWnd, IDC_BLUE), SW_HIDE);
		::ShowWindow(::GetDlgItem(m_hWnd, IDC_SVIDEO), SW_HIDE);
		::ShowWindow(::GetDlgItem(m_hWnd, IDC_ANALOG), SW_SHOW);
		::ShowWindow(::GetDlgItem(m_hWnd, IDC_1STSVIDEO), SW_SHOW);
		::ShowWindow(::GetDlgItem(m_hWnd, IDC_2NDSVIDEO), SW_SHOW);
	}
	else
	{
		::ShowWindow(::GetDlgItem(m_hWnd, IDC_RED), SW_SHOW);
		::ShowWindow(::GetDlgItem(m_hWnd, IDC_GREEN), SW_SHOW);
		::ShowWindow(::GetDlgItem(m_hWnd, IDC_BLUE), SW_SHOW);
		::ShowWindow(::GetDlgItem(m_hWnd, IDC_SVIDEO), SW_SHOW);
		::ShowWindow(::GetDlgItem(m_hWnd, IDC_ANALOG), SW_HIDE);
		::ShowWindow(::GetDlgItem(m_hWnd, IDC_1STSVIDEO), SW_HIDE);
		::ShowWindow(::GetDlgItem(m_hWnd, IDC_2NDSVIDEO), SW_HIDE);
	}
}

void CSourceDlg::On2svideo() 
{
	UpdateData();
	UpdateControls();
//	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("Connector"), 0);
	CMV500Grab::s_pGrab->SetDefaultConnectors();
	if (m_pSite) m_pSite->Reinit(0);
}

void CSourceDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
//	if (nSBCode != SB_THUMBPOSITION)
	if (nSBCode == SB_THUMBTRACK || nSBCode == SB_ENDSCROLL)
	{
		CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
		return;
	}
	if (pScrollBar == (CScrollBar *)&m_Brightness)
	{
		UpdateData();
		m_nBrightness = m_Brightness.GetPos();
		UpdateData(FALSE);
		CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("Brightness"), m_nBrightness);
		if (m_pSite) m_pSite->Reinit(2);
	}
	if (pScrollBar == (CScrollBar *)&m_Contrast)
	{
		UpdateData();
		m_nContrast = m_Contrast.GetPos();
		UpdateData(FALSE);
		CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("Contrast"), m_nContrast);
		if (m_pSite) m_pSite->Reinit(2);
	}
	if (pScrollBar == (CScrollBar *)&m_Hue)
	{
		UpdateData();
		m_nHue = m_Hue.GetPos();
		UpdateData(FALSE);
		CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("Hue"), m_nHue);
		if (m_pSite) m_pSite->Reinit(2);
	}
	if (pScrollBar == (CScrollBar *)&m_Saturation)
	{
		UpdateData();
		m_nSaturation = m_Saturation.GetPos();
		UpdateData(FALSE);
		CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("Saturation"), m_nSaturation);
		if (m_pSite) m_pSite->Reinit(2);
	}
	if (pScrollBar == (CScrollBar *)&m_SGWX)
	{
		UpdateData();
		m_nGWX = m_SGWX.GetPos();
		m_nGWX = ((m_nGWX>>2)<<2);
		CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("GrabWindowX"), m_nGWX);
		unsigned nMax = max(MaxDX()-m_nGWX,MaxDX()-m_nGWScope);
		m_SGWDX.SetRange(MaxDX()-m_nGWScope, nMax);
		m_SGWDX.SetTicFreq(m_nGWStep);
		if (m_nGWDX > nMax)
		{
			m_nGWDX = nMax;
			m_SGWDX.SetPos(m_nGWDX);
			CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("GrabWindowDX"), m_nGWDX);
		}
		UpdateData(FALSE);
		if (m_pSite) m_pSite->Reinit(1);
	}
	if (pScrollBar == (CScrollBar *)&m_SGWY)
	{
		UpdateData();
		m_nGWY = m_SGWY.GetPos();
		m_nGWY = ((m_nGWY>>2)<<2);
		CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("GrabWindowY"), m_nGWY);
		unsigned nMax = max(MaxDY()-m_nGWY,MaxDY()-m_nGWScope);
		m_SGWDY.SetRange(MaxDY()-m_nGWScope, nMax);
		m_SGWDY.SetTicFreq(m_nGWStep);
		if (m_nGWDY > nMax)
		{
			m_nGWDY = nMax;
			m_SGWDY.SetPos(m_nGWDY);
			CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("GrabWindowDY"), m_nGWDY);
		}
		UpdateData(FALSE);
		if (m_pSite) m_pSite->Reinit(1);
	}
	if (pScrollBar == (CScrollBar *)&m_SGWDX)
	{
		UpdateData();
		m_nGWDX = m_SGWDX.GetPos();
		m_nGWDX = ((m_nGWDX>>2)<<2);
		CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("GrabWindowDX"), m_nGWDX);
		if (m_nGWX > MaxDX()-m_nGWDX)
		{
			m_nGWX = MaxDX()-m_nGWDX;
			CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("GrabWindowX"), m_nGWX);
		}
		UpdateData(FALSE);
		if (m_pSite) m_pSite->Reinit(1);
	}
	if (pScrollBar == (CScrollBar *)&m_SGWDY)
	{
		UpdateData();
		m_nGWDY = m_SGWDY.GetPos();
		m_nGWDY = ((m_nGWDY>>2)<<2);
		CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("GrabWindowDY"), m_nGWDY);
		if (m_nGWY > MaxDY()-m_nGWDY)
		{
			m_nGWY = MaxDY()-m_nGWDY;
			CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("GrabWindowY"), m_nGWY);
		}
		UpdateData(FALSE);
		if (m_pSite) m_pSite->Reinit(1);
	}
	
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CSourceDlg::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

void CSourceDlg::On1stsvideo() 
{
	UpdateData();
	CMV500Grab::s_pGrab->SetConnector(m_nDevice, 0);
//	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("Connector"), 0);
	if (m_pSite) m_pSite->Reinit(0);
}

void CSourceDlg::On2ndsvideo() 
{
	UpdateData();
	CMV500Grab::s_pGrab->SetConnector(m_nDevice, 1);
//	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("Connector"), 1);
	if (m_pSite) m_pSite->Reinit(0);
}

void CSourceDlg::OnAgc() 
{
	UpdateData();
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("AGC"), m_bAGC);
	if (m_pSite) m_pSite->Reinit(1);
}

void CSourceDlg::OnAnalog() 
{
	UpdateData();
	CMV500Grab::s_pGrab->SetConnector(m_nDevice, 3);
//	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("Connector"), 2);
	if (m_pSite) m_pSite->Reinit(0);
}

void CSourceDlg::OnBlue() 
{
	UpdateData();
	CMV500Grab::s_pGrab->SetConnector(m_nDevice, 2);
//	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("Connector"), 2);
	if (m_pSite) m_pSite->Reinit(0);
}

void CSourceDlg::OnChangeEBrightness() 
{
	UpdateData();
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("Brightness"), m_nBrightness);
	if (m_pSite) m_pSite->Reinit(2);
}

void CSourceDlg::OnChangeEContrast() 
{
	UpdateData();
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("Contrast"), m_nContrast);
	if (m_pSite) m_pSite->Reinit(2);
}

void CSourceDlg::OnChangeEHue() 
{
	UpdateData();
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("Hue"), m_nHue);
	if (m_pSite) m_pSite->Reinit(2);
}

void CSourceDlg::OnChangeESaturation() 
{
	UpdateData();
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("Saturation"), m_nSaturation);
	if (m_pSite) m_pSite->Reinit(2);
}

void CSourceDlg::OnGreen() 
{
	UpdateData();
	CMV500Grab::s_pGrab->SetConnector(m_nDevice, 1);
//	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("Connector"), 1);
	if (m_pSite) m_pSite->Reinit(0);
}

void CSourceDlg::OnRed() 
{
	UpdateData();
	CMV500Grab::s_pGrab->SetConnector(m_nDevice, 0);
//	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("Connector"), 0);
	if (m_pSite) m_pSite->Reinit(0);
}

void CSourceDlg::OnSvideo() 
{
	UpdateData();
	CMV500Grab::s_pGrab->SetConnector(m_nDevice, 3);
//	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("Connector"), 3);
	if (m_pSite) m_pSite->Reinit(0);
}

void CSourceDlg::OnRed2()
{
	CMV500Grab::s_pGrab->SetConnector(1, 0);
	if (m_pSite) m_pSite->Reinit(0);
}

void CSourceDlg::OnGreen2()
{
	CMV500Grab::s_pGrab->SetConnector(1, 1);
	if (m_pSite) m_pSite->Reinit(0);
}

void CSourceDlg::OnBlue2()
{
	CMV500Grab::s_pGrab->SetConnector(1, 2);
	if (m_pSite) m_pSite->Reinit(0);
}


void CSourceDlg::OnVcr() 
{
	UpdateData();
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("VCRMode"), m_bVCR);
	if (m_pSite) m_pSite->Reinit(1);
}


void CSourceDlg::OnStandardNtsc() 
{
	UpdateData();
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("VideoStandard"), m_nVideoStandard);
	SetGrabWindow(0,0,640,480);
	if (m_pSite) m_pSite->Reinit(0);
}

void CSourceDlg::OnStandardNtsc44() 
{
	UpdateData();
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("VideoStandard"), m_nVideoStandard);
	SetGrabWindow(0,0,640,480);
	if (m_pSite) m_pSite->Reinit(0);
}

void CSourceDlg::OnStandardNtsccomb() 
{
	UpdateData();
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("VideoStandard"), m_nVideoStandard);
	SetGrabWindow(0,0,640,480);
	if (m_pSite) m_pSite->Reinit(0);
}

void CSourceDlg::OnStandardNtscmono() 
{
	UpdateData();
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("VideoStandard"), m_nVideoStandard);
	SetGrabWindow(0,0,640,480);
	if (m_pSite) m_pSite->Reinit(0);
}

void CSourceDlg::OnStandardPal() 
{
	UpdateData();
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("VideoStandard"), m_nVideoStandard);
	SetGrabWindow(0,0,768,576);
	if (m_pSite) m_pSite->Reinit(0);
}

void CSourceDlg::OnStandardPalm() 
{
	UpdateData();
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("VideoStandard"), m_nVideoStandard);
	SetGrabWindow(0,0,768,576);
	if (m_pSite) m_pSite->Reinit(0);
}

void CSourceDlg::OnStandardPaln() 
{
	UpdateData();
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("VideoStandard"), m_nVideoStandard);
	SetGrabWindow(0,0,768,576);
	if (m_pSite) m_pSite->Reinit(0);
}

void CSourceDlg::OnStandardSecam() 
{
	UpdateData();
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("VideoStandard"), m_nVideoStandard);
	SetGrabWindow(0,0,768,576);
	if (m_pSite) m_pSite->Reinit(0);
}

void CSourceDlg::SetGrabWindow(int x, int y, int cx, int cy)
{
	UpdateData();
	m_nGWX  = x;
	m_nGWY  = y;
	m_nGWDX = cx;
	m_nGWDY = cy;
	UpdateData(FALSE);
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("GrabWindowX"), m_nGWX);
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("GrabWindowY"), m_nGWY);
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("GrabWindowDX"), m_nGWDX);
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("GrabWindowDY"), m_nGWDY);
}




void CSourceDlg::OnStandardPal60() 
{
	UpdateData();
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("VideoStandard"), m_nVideoStandard);
	SetGrabWindow(0,0,768,576);
	if (m_pSite) m_pSite->Reinit(0);
}

void CSourceDlg::OnStandardSecammono() 
{
	UpdateData();
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("VideoStandard"), m_nVideoStandard);
	SetGrabWindow(0,0,768,576);
	if (m_pSite) m_pSite->Reinit(0);
}

void CSourceDlg::OnChangeEGwdx() 
{
	UpdateData();
	m_SGWDX.SetPos(m_nGWDX);
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("GrabWindowDX"), m_nGWDX);
	if (m_nGWX > MaxDX()-m_nGWDX)
	{
		m_nGWX = MaxDX()-m_nGWDX;
		CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("GrabWindowX"), m_nGWX);
	}
	UpdateData(FALSE);
	if (m_pSite) m_pSite->Reinit(1);
}

void CSourceDlg::OnChangeEGwdy() 
{
	UpdateData();
	m_SGWDY.SetPos(m_nGWDY);
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("GrabWindowDY"), m_nGWDY);
	if (m_nGWY > MaxDY()-m_nGWDY)
	{
		m_nGWY = MaxDY()-m_nGWDY;
		CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("GrabWindowY"), m_nGWY);
	}
	UpdateData(FALSE);
	if (m_pSite) m_pSite->Reinit(1);
}

void CSourceDlg::OnChangeEGwx() 
{
	UpdateData();
	m_SGWX.SetPos(m_nGWX);
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("GrabWindowX"), m_nGWX);
	unsigned nMax = max(MaxDX()-m_nGWX,MaxDX()-m_nGWScope);
	m_SGWDX.SetRange(MaxDX()-m_nGWScope, nMax);
	m_SGWDX.SetTicFreq(m_nGWStep);
	if (m_nGWDX > nMax)
	{
		m_nGWDX = nMax;
		m_SGWDX.SetPos(m_nGWDX);
		CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("GrabWindowDX"), m_nGWDX);
	}
	UpdateData(FALSE);
	if (m_pSite) m_pSite->Reinit(1);
}

void CSourceDlg::OnChangeEGwy() 
{
	UpdateData();
	m_SGWY.SetPos(m_nGWY);
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("GrabWindowY"), m_nGWY);
	unsigned nMax = max(MaxDY()-m_nGWY,MaxDY()-m_nGWScope);
	m_SGWDY.SetRange(MaxDY()-m_nGWScope, nMax);
	m_SGWDY.SetTicFreq(m_nGWStep);
	if (m_nGWDY > nMax)
	{
		m_nGWDY = nMax;
		m_SGWDY.SetPos(m_nGWDY);
		CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("GrabWindowDY"), m_nGWDY);
	}
	UpdateData(FALSE);
	if (m_pSite) m_pSite->Reinit(1);
}

void CSourceDlg::OnCancel() 
{
	// Restore
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("2SVideo"), m_bPrevSVideo);
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("AGC"), m_bPrevAGC);
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("Hue"), m_nPrevHue);
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("Saturation"), m_nPrevSaturation);
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("Brightness"), m_nPrevBrightness);
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("Contrast"), m_nPrevContrast);
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("VCRMode"), m_bPrevVCR);
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("VideoStandard"), m_nPrevVideoStandard);
	if (m_bPrevSVideo)
		CMV500Grab::s_pGrab->SetConnector(m_nDevice, m_nPrevConnector2);
	else
		CMV500Grab::s_pGrab->SetConnector(m_nDevice, m_nPrevConnector1);
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("GrabWindowX"), m_nPrevGWX);
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("GrabWindowY"), m_nPrevGWY);
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("GrabWindowDX"), m_nPrevGWDX);
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("GrabWindowDY"), m_nPrevGWDY);
	if (m_nIDD == IDD_SOURCE2)
		CMV500Grab::s_pGrab->SetConnector(1, m_nPrevConnectorCam2);
	// Reinit site
	if (m_pSite) m_pSite->Reinit(0);
	CDialog::OnCancel();
}

void CSourceDlg::OnHelp() 
{
	if (((CVT5ProfileManager *)CStdProfileManager::m_pMgr)->IsVT5Profile())
		HelpDisplayTopic("MV500",NULL, "SourceDlg","$GLOBAL_main");
//::HtmlHelp( NULL, "e:\\karyo\\help\\shell.col>$GLOBAL_main", HH_DISPLAY_TOPIC, (DWORD)"MV500.chm::/MV500/SourceDlg.html");
//::HtmlHelp( NULL, "e:\\karyo\\help\\MV500.chm::/MV500/SourceDlg.html>$GLOBAL_main", HH_DISPLAY_TOPIC, (DWORD)0);
	else
		CDialog::OnHelp();
}

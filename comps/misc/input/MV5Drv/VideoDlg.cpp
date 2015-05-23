// VideoDlg.cpp : implementation file
//

#include "stdafx.h"
#include <mv5.h>
#include "resource.h"
#include "MV500Grab.h"
#include "VideoDlg.h"
#include "StdProfile.h"
#include "Defs.h"
//#include "ProgLog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVideoDlg dialog


CVideoDlg::CVideoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVideoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CVideoDlg)
	m_strFile = _T("");
	m_iFPS = 25;
	m_iTime = 1;
	m_nRateScale = 0;
	m_iPlaybackFPS = 25;
	m_nPlaybackRateScale = 0;
	m_nCaptureMemory = CStdProfileManager::m_pMgr->GetProfileInt(_T("AVI"), _T("CaptureToMemory"), 0);
	m_bGrayScale = CMV500Grab::s_pGrab->IsGrayScale(true);
	m_nFrameSize = CStdProfileManager::m_pMgr->GetProfileInt(_T("AVI"), _T("ProcGrabbedImage"), 75);
	//}}AFX_DATA_INIT
}


void CVideoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVideoDlg)
	DDX_Control(pDX, IDC_S_FRAME_SIZE, m_SFrameSize);
	DDX_Control(pDX, IDC_FPS_MESSAGE, m_FPSMessage);
	DDX_Text(pDX, IDC_FILE, m_strFile);
	DDX_Text(pDX, IDC_FPS, m_iFPS);
	DDV_MinMaxUInt(pDX, m_iFPS, 1, 50);
	DDX_Text(pDX, IDC_TIME_LIM, m_iTime);
	DDX_CBIndex(pDX, IDC_CAPTURE_RATE_SCALE, m_nRateScale);
	DDX_Text(pDX, IDC_PLAYBACK_FPS, m_iPlaybackFPS);
	DDX_CBIndex(pDX, IDC_PLAYBACK_RATE_SCALE, m_nPlaybackRateScale);
	DDX_Check(pDX, IDC_GRAYSCALE, m_bGrayScale);
	DDX_Text(pDX, IDC_E_FRAME_SIZE, m_nFrameSize);
	//}}AFX_DATA_MAP
	if (pDX->m_bSaveAndValidate)
	{
		int n;
		DDX_CBIndex(pDX, IDC_CAPTURE_MEMORY, n);
		m_nCaptureMemory = n==1?2:0;
	}
	else
	{
		int n = m_nCaptureMemory==2?1:0;
		DDX_CBIndex(pDX, IDC_CAPTURE_MEMORY, n);
	}
}


BEGIN_MESSAGE_MAP(CVideoDlg, CDialog)
	//{{AFX_MSG_MAP(CVideoDlg)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_EN_CHANGE(IDC_FPS, OnChangeFps)
	ON_CBN_SELCHANGE(IDC_CAPTURE_RATE_SCALE, OnSelchangeCaptureRateScale)
	ON_WM_HSCROLL()
	ON_CBN_SELCHANGE(IDC_CAPTURE_MEMORY, OnSelchangeCaptureMemory)
	ON_BN_CLICKED(IDC_DEFAULT, OnDefault)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVideoDlg message handlers

void CVideoDlg::OnBrowse() 
{
	UpdateData( TRUE );
	CString	strFilter = "AVI files (*.AVI)|*.AVI|All files (*.*)|*.*||";
	CFileDialog dlg( FALSE, ".AVI", m_strFile, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		 strFilter, 0, _FILE_OPEN_SIZE_ );  // common/defs.h
	if( dlg.DoModal()==IDOK )
	{
		m_strFile = dlg.GetPathName();
		UpdateData( FALSE );
		if (m_strFile.IsEmpty())
			GetDlgItem(IDOK)->EnableWindow(FALSE);
		else
			GetDlgItem(IDOK)->EnableWindow(TRUE);
	}
}

void CVideoDlg::OnOK() 
{
	UpdateData();
	CStdProfileManager::m_pMgr->WriteProfileString(_T("AVI"), _T("FileName"), m_strFile);
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("AVI"), _T("FPS"), int(m_iFPS));
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("AVI"), _T("Time"), int(m_iTime));
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("AVI"), _T("RateScale"), m_nRateScale);
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("AVI"), _T("PlaybackFPS"), int(m_iPlaybackFPS));
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("AVI"), _T("PlaybackRateScale"), m_nPlaybackRateScale);
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("AVI"), _T("CaptureToMemory"), m_nCaptureMemory);
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("AVI"), _T("GrayScale"), m_bGrayScale);
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("AVI"), _T("ProcGrabbedImage"), m_nFrameSize);
	CDialog::OnOK();
}

BOOL CVideoDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_strFile = CStdProfileManager::m_pMgr->GetProfileString(_T("AVI"), _T("FileName"), "");
	m_iFPS = CStdProfileManager::m_pMgr->GetProfileInt(_T("AVI"), _T("FPS"), 25);
	m_iTime = CStdProfileManager::m_pMgr->GetProfileInt(_T("AVI"), _T("Time"), 1);
	m_nRateScale = CStdProfileManager::m_pMgr->GetProfileInt(_T("AVI"), _T("RateScale"), 0);
	if (m_nRateScale < 0 || m_nRateScale > 3)
		m_nRateScale = 0;
	m_iPlaybackFPS = CStdProfileManager::m_pMgr->GetProfileInt(_T("AVI"), _T("PlaybackFPS"), -1);
	if (m_iPlaybackFPS == -1)
		m_iPlaybackFPS = m_iFPS;
	m_nPlaybackRateScale = CStdProfileManager::m_pMgr->GetProfileInt(_T("AVI"), _T("PlaybackRateScale"), -1);
	if (m_nPlaybackRateScale < 0 || m_nPlaybackRateScale > 3)
		m_nPlaybackRateScale = m_nRateScale;
	m_nCaptureMemory = CStdProfileManager::m_pMgr->GetProfileInt(_T("AVI"), _T("CaptureToMemory"), 0);
	UpdateData(FALSE);
	CString s;
	if (m_nRateScale == 1)
		s.LoadString(IDS_TIME_IN_MINUTES);
	else if (m_nRateScale == 2)
		s.LoadString(IDS_TIME_IN_HOURS);
	else
		s.LoadString(IDS_TIME_IN_SECONDS);
	SetDlgItemText(IDC_TIME,s);
	if (m_strFile.IsEmpty())
		GetDlgItem(IDOK)->EnableWindow(FALSE);
	m_SFrameSize.SetRange(0, 100);
	m_SFrameSize.SetTicFreq(10);
	m_SFrameSize.SetPos(m_nFrameSize);
	UpdateControls();
	int nVideoStd = CStdProfileManager::m_pMgr->GetProfileInt(_T("Source"), _T("VideoStandard"), 0);
	nVideoStd += MV5_VideoStandard_NTSC;
	CString sMsg;
	if (nVideoStd == MV5_VideoStandard_PAL || nVideoStd == MV5_VideoStandard_SECAM || nVideoStd == MV5_VideoStandard_PALm ||
		nVideoStd == MV5_VideoStandard_PALn || nVideoStd == MV5_VideoStandard_PAL60 || nVideoStd == MV5_VideoStandard_SECAMmono)
		sMsg.LoadString(IDS_FPS_MESSAGE_PAL);
	else
		sMsg.LoadString(IDS_FPS_MESSAGE_NTSC);
	m_FPSMessage.SetWindowText(sMsg);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


DWORD CVideoDlg::CalcFrameDelay()
{
	if (m_nRateScale == 1)
		return 60000000L/((DWORD)m_iFPS); // frames per minute
	else if (m_nRateScale == 2)
		return 3600000000L/((DWORD)m_iFPS); // frames per hour
	else
		return 1000000L/m_iFPS;
}

void CVideoDlg::OnChangeFps() 
{
	UpdateData();
	m_iPlaybackFPS = m_iFPS;
	m_nPlaybackRateScale = m_nRateScale;
	UpdateData(FALSE);
}

void CVideoDlg::OnSelchangeCaptureRateScale() 
{
	UpdateData();
	m_iPlaybackFPS = m_iFPS;
	m_nPlaybackRateScale = m_nRateScale;
	CString s;
	if (m_nRateScale == 1)
		s.LoadString(IDS_TIME_IN_MINUTES);
	else if (m_nRateScale == 2)
		s.LoadString(IDS_TIME_IN_HOURS);
	else
		s.LoadString(IDS_TIME_IN_SECONDS);
	SetDlgItemText(IDC_TIME,s);
	UpdateData(FALSE);
}

DWORD CVideoDlg::CalcPlaybackRate()
{
	return m_iPlaybackFPS;
}

DWORD CVideoDlg::CalcPlaybackScale()
{
	if (m_nPlaybackRateScale == 1)
		return 60; // frames per minute
	else if (m_nPlaybackRateScale == 2)
		return 3600; // frames per hour
	else
		return 1;
}

int CVideoDlg::CalcTime()
{
	if (m_nRateScale == 1)
		return m_iTime*60; // minutes
	else if (m_nRateScale == 2)
		return m_iTime*3600; // hours
	else
		return m_iTime;
}

void CVideoDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if (pScrollBar == (CScrollBar *)&m_SFrameSize)
	{
		UpdateData();
		m_nFrameSize = m_SFrameSize.GetPos();
		UpdateData(FALSE);
	}
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CVideoDlg::UpdateControls()
{
	int nShow = m_nCaptureMemory==2?SW_SHOW:SW_HIDE;
	::ShowWindow(::GetDlgItem(m_hWnd, IDC_GRAYSCALE), nShow);
	::ShowWindow(::GetDlgItem(m_hWnd, IDC_E_FRAME_SIZE), nShow);
	::ShowWindow(::GetDlgItem(m_hWnd, IDC_S_FRAME_SIZE), nShow);
	::ShowWindow(::GetDlgItem(m_hWnd, IDC_STATIC_FRAME_SIZE), nShow);
	::ShowWindow(::GetDlgItem(m_hWnd, IDC_STATIC_PROCENT), nShow);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_GRAYSCALE), CMV500Grab::s_pGrab->GetBoardType()==CMV500Grab::Board_MV500);
}

void CVideoDlg::OnSelchangeCaptureMemory() 
{
	UpdateData();
	UpdateControls();
}

void CVideoDlg::OnDefault() 
{
	UpdateData();
	m_iFPS = CStdProfileManager::m_pMgr->GetDefaultProfileInt(_T("AVI"), _T("FPS"), 25);
	m_iTime = CStdProfileManager::m_pMgr->GetDefaultProfileInt(_T("AVI"), _T("Time"), 1);
	m_nRateScale = CStdProfileManager::m_pMgr->GetDefaultProfileInt(_T("AVI"), _T("RateScale"), 0);
	m_iPlaybackFPS = CStdProfileManager::m_pMgr->GetDefaultProfileInt(_T("AVI"), _T("PlaybackFPS"), 25);
	m_nPlaybackRateScale = CStdProfileManager::m_pMgr->GetDefaultProfileInt(_T("AVI"), _T("PlaybackRateScale"), 0);
	m_nCaptureMemory = CStdProfileManager::m_pMgr->GetDefaultProfileInt(_T("AVI"), _T("CaptureToMemory"), 0);
	m_bGrayScale = CStdProfileManager::m_pMgr->GetDefaultProfileInt(_T("AVI"), _T("GrayScale"), FALSE);
	m_nFrameSize = CStdProfileManager::m_pMgr->GetDefaultProfileInt(_T("AVI"), _T("ProcGrabbedImage"), 75);
	UpdateData(FALSE);
	UpdateControls();
}




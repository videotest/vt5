#include "StdAfx.h"
#include "resource.h"
#include "InputDlg.h"
#include "VideoDlg.h"
//#include "VfWProfile.h"
#include <vfw.h>
#include <io.h>
#include <fcntl.h>
#include "input.h"
#include "HelpBase.h"
//#include "VTAvi.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CVideoDlg dialog


CVideoDlg::CVideoDlg(CStdProfileManager *pProf, IUnknown *punkDrv, CWnd* pParent /*=NULL*/)
	: CDialog(CVideoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CVideoDlg)
	m_strFile = _T("");
	m_iFPS = 25;
	m_iTime = 1;
	m_nRateScale = 0;
	m_iPlaybackFPS = 25;
	m_nPlaybackRateScale = 0;
	m_nFileSize = 0;
	m_bAllocFile = FALSE;
	//}}AFX_DATA_INIT
	m_pProf = pProf;
	m_sptrDrv = punkDrv;
}


void CVideoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVideoDlg)
	DDX_Text(pDX, IDC_FILE, m_strFile);
	DDX_Text(pDX, IDC_FPS, m_iFPS);
	DDV_MinMaxUInt(pDX, m_iFPS, 1, 50);
	DDX_Text(pDX, IDC_TIME_LIM, m_iTime);
	DDX_CBIndex(pDX, IDC_CAPTURE_RATE_SCALE, m_nRateScale);
	DDX_Text(pDX, IDC_PLAYBACK_FPS, m_iPlaybackFPS);
	DDX_CBIndex(pDX, IDC_PLAYBACK_RATE_SCALE, m_nPlaybackRateScale);
	DDX_Text(pDX, IDC_EDIT_FILE_SIZE, m_nFileSize);
	DDX_Check(pDX, IDC_ALLOC_FILE, m_bAllocFile);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CVideoDlg, CDialog)
	//{{AFX_MSG_MAP(CVideoDlg)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_EN_CHANGE(IDC_FPS, OnChangeFps)
	ON_CBN_SELCHANGE(IDC_CAPTURE_RATE_SCALE, OnSelchangeCaptureRateScale)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_BN_CLICKED(IDC_COMPRESSION, OnCompression)
	ON_BN_CLICKED(IDC_ALLOC_FILE, OnAllocFile)
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
		CString sFile = dlg.GetPathName();
		TCHAR szDrive[_MAX_DRIVE],szRootPath[_MAX_PATH];
		_tsplitpath(sFile, szDrive, NULL, NULL, NULL);
		_tmakepath(szRootPath, szDrive, _T("\\"), NULL, NULL);
		UINT nDriveType = GetDriveType(szRootPath);
		if (nDriveType == DRIVE_FIXED || nDriveType == DRIVE_RAMDISK)
			m_strFile = sFile;
		else if (nDriveType == DRIVE_REMOTE &&
			m_pProf->GetProfileInt(_T("VideoDlg"), _T("EnableRemote"), FALSE, true, false))
			m_strFile = sFile;
		else
			AfxMessageBox(IDS_NETWORK_PATH, MB_OK|MB_ICONEXCLAMATION);
		UpdateData( FALSE );
		if (m_strFile.IsEmpty())
			GetDlgItem(IDOK)->EnableWindow(FALSE);
		else
			GetDlgItem(IDOK)->EnableWindow(TRUE);
	}
}

void CVideoDlg::ReadSettings() 
{
	m_strFile = m_pProf->_GetProfileString(_T("VideoDlg"), _T("FileName"), "");
	m_iFPS = m_pProf->_GetProfileInt(_T("VideoDlg"), _T("FPS"), 25);
	m_iTime = m_pProf->_GetProfileInt(_T("VideoDlg"), _T("Time"), 1);
	m_nRateScale = m_pProf->_GetProfileInt(_T("VideoDlg"), _T("RateScale"), 0);
	if (m_nRateScale < 0 || m_nRateScale > 3)
		m_nRateScale = 0;
	m_iPlaybackFPS = m_pProf->_GetProfileInt(_T("VideoDlg"), _T("PlaybackFPS"), -1);
	if (m_iPlaybackFPS == -1)
		m_iPlaybackFPS = m_iFPS;
	m_nPlaybackRateScale = m_pProf->_GetProfileInt(_T("VideoDlg"), _T("PlaybackRateScale"), -1);
	if (m_nPlaybackRateScale < 0 || m_nPlaybackRateScale > 3)
		m_nPlaybackRateScale = m_nRateScale;
	m_bAllocFile = m_pProf->_GetProfileInt(_T("VideoDlg"), _T("AllocFile"), FALSE);
	m_nFileSize = m_pProf->_GetProfileInt(_T("VideoDlg"), _T("FileSizeMB"), 256);
}

void CVideoDlg::OnOK() 
{
	UpdateData();
	m_pProf->WriteProfileString(_T("VideoDlg"), _T("FileName"), m_strFile);
	m_pProf->WriteProfileInt(_T("VideoDlg"), _T("FPS"), int(m_iFPS));
	m_pProf->WriteProfileInt(_T("VideoDlg"), _T("Time"), int(m_iTime));
	m_pProf->WriteProfileInt(_T("VideoDlg"), _T("RateScale"), m_nRateScale);
	m_pProf->WriteProfileInt(_T("VideoDlg"), _T("PlaybackFPS"), int(m_iPlaybackFPS));
	m_pProf->WriteProfileInt(_T("VideoDlg"), _T("PlaybackRateScale"), m_nPlaybackRateScale);
	m_pProf->WriteProfileInt(_T("VideoDlg"), _T("AllocFile"), m_bAllocFile);
	m_pProf->WriteProfileInt(_T("VideoDlg"), _T("FileSizeMB"), m_nFileSize);
	CDialog::OnOK();
}

BOOL CVideoDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	ReadSettings();
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
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_EDIT_FILE_SIZE), m_bAllocFile);

	BOOL bEnableCompress = m_sptrDrv!=0&&m_sptrDrv->ExecuteDriverDialog(-1, _T("?Compression"))==S_OK;
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_COMPRESSION), bEnableCompress);
	
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

void CVideoDlg::OnHelp() 
{
	HelpDisplay( "VideoDlg" );	
}

void CVideoDlg::OnCompression() 
{
	m_sptrDrv->ExecuteDriverDialog(-1, _T("Compression"));
}

void CVideoDlg::OnAllocFile() 
{
	UpdateData();	
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_EDIT_FILE_SIZE), m_bAllocFile);
}



// InputDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "InputDlg.h"
#include "VideoDlg.h"
#include "VfWProfile.h"
#include "Accumulate.h"
#include <vfw.h>
#include "input.h"
#include "HelpBase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CVT5ProfileManager g_VfWProfile(_T("VideoForWindows"),false);


/////////////////////////////////////////////////////////////////////////////
// CInputDlg dialog

CInputDlg *CInputDlg::m_pInputDlg = NULL;


#define DIST     20


CInputDlg::CInputDlg(int iNum, const char *pszDescr, CWnd* pParent /*=NULL*/)
	: CDialog(CInputDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInputDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_DialogPurpose = ForInput;
	m_hWndCap = NULL;
	m_lpbi = NULL;
	m_bModalState = false;
	m_nDrvNum = iNum;
	m_sDrvDescr = pszDescr;
}

CInputDlg::~CInputDlg()
{
	if (m_lpbi)
		free(m_lpbi);
}

void CInputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInputDlg)
	DDX_Control(pDX, IDC_AVI, m_AVI);
	DDX_Control(pDX, IDCANCEL, m_Cancel);
	DDX_Control(pDX, IDC_SOURCE, m_Source);
	DDX_Control(pDX, IDC_SAVE, m_Save);
	DDX_Control(pDX, IDC_PREVIEW, m_Preview);
	DDX_Control(pDX, IDC_OVERLAY, m_Overlay);
	DDX_Control(pDX, IDC_DISPLAY, m_Display);
	DDX_Control(pDX, IDC_FORMAT, m_Format);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CInputDlg, CDialog)
	//{{AFX_MSG_MAP(CInputDlg)
	ON_BN_CLICKED(IDC_OVERLAY, OnOverlay)
	ON_BN_CLICKED(IDC_PREVIEW, OnPreview)
	ON_BN_CLICKED(IDC_DISPLAY, OnDisplay)
	ON_BN_CLICKED(IDC_FORMAT, OnFormat)
	ON_BN_CLICKED(IDC_SOURCE, OnSource)
	ON_BN_CLICKED(IDC_AVI, OnAvi)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInputDlg message handlers

BOOL CInputDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_Format.GetWindowRect(m_rcOrgButt);
	if (!m_sDrvDescr.IsEmpty())
		SetWindowText(m_sDrvDescr);
	
	CAPSTATUS CapStatus;
	CAPDRIVERCAPS CapDriverCaps;
	CSize szImage = CSize(640,480);
	m_hWndCap = capCreateCaptureWindow(NULL, WS_CHILD|WS_VISIBLE, DIST, DIST, szImage.cx, szImage.cy, m_hWnd, 1995);
	if (!capDriverConnect(m_hWndCap, m_nDrvNum))
	{
		RecalcLayout(CSize(640,480));
		AfxMessageBox( "Cannot connect to video capturing device" );
		return FALSE;
	}
	capPreviewRate (m_hWndCap, 33);     // Set the preview rate to 33 milliseconds
    if (!capDriverGetCaps(m_hWndCap, &CapDriverCaps, sizeof(CapDriverCaps)))
        memset(&CapDriverCaps, 0, sizeof(CapDriverCaps));
	if (CapDriverCaps.fHasOverlay && g_VfWProfile._GetProfileInt(_T("Settings"),_T("Overlay"),TRUE))  // Enable overlay if the card supports it
		capOverlay(m_hWndCap, TRUE);   
	else
		capPreview(m_hWndCap, TRUE);       
    if (!capDriverGetCaps(m_hWndCap, &CapDriverCaps, sizeof (CapDriverCaps)))
        memset(&CapDriverCaps, 0, sizeof (CapDriverCaps));
	capGetStatus(m_hWndCap, &CapStatus, sizeof(CAPSTATUS));
	::MoveWindow(m_hWndCap, DIST, DIST, CapStatus.uiImageWidth,	CapStatus.uiImageHeight, TRUE);
	RecalcLayout(CSize(CapStatus.uiImageWidth, CapStatus.uiImageHeight));
	::EnableWindow( ::GetDlgItem(m_hWnd, IDC_FORMAT), CapDriverCaps.fHasDlgVideoFormat );
	::EnableWindow( ::GetDlgItem(m_hWnd, IDC_SOURCE), CapDriverCaps.fHasDlgVideoSource );
	::EnableWindow( ::GetDlgItem(m_hWnd, IDC_DISPLAY), CapDriverCaps.fHasDlgVideoDisplay );
	CheckDlgButton( IDC_PREVIEW, CapStatus.fLiveWindow );
	CheckDlgButton( IDC_OVERLAY, CapStatus.fOverlayWindow );
	::EnableWindow( ::GetDlgItem(m_hWnd, IDC_OVERLAY), CapDriverCaps.fHasOverlay );


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CInputDlg::RecalcLayout(CSize szPrev)
{
	if (szPrev == CSize(0,0))
	{
		CAPSTATUS CapStatus;
		capGetStatus(m_hWndCap, &CapStatus, sizeof(CAPSTATUS));
		::MoveWindow(m_hWndCap, DIST, DIST, CapStatus.uiImageWidth,	CapStatus.uiImageHeight, TRUE);
		szPrev = CSize(CapStatus.uiImageWidth, CapStatus.uiImageHeight);
	}
	CRect rc;
	rc = CRect(DIST, szPrev.cy+2*DIST, DIST+m_rcOrgButt.Width(), szPrev.cy+2*DIST+m_rcOrgButt.Height());
	m_Format.MoveWindow(rc);
	rc = CRect(m_rcOrgButt.Width()+2*DIST, szPrev.cy+2*DIST, m_rcOrgButt.Width()+2*DIST+m_rcOrgButt.Width(),
		szPrev.cy+2*DIST+m_rcOrgButt.Height());
	m_Source.MoveWindow(rc);
	rc = CRect(2*m_rcOrgButt.Width()+3*DIST, szPrev.cy+2*DIST, 2*m_rcOrgButt.Width()+3*DIST+m_rcOrgButt.Width(),
		szPrev.cy+2*DIST+m_rcOrgButt.Height());
	m_Display.MoveWindow(rc);
	rc = CRect(3*m_rcOrgButt.Width()+4*DIST, szPrev.cy+2*DIST, 3*m_rcOrgButt.Width()+4*DIST+m_rcOrgButt.Width(),
		szPrev.cy+2*DIST+m_rcOrgButt.Height());
	m_Preview.MoveWindow(rc);
	rc = CRect(4*m_rcOrgButt.Width()+5*DIST, szPrev.cy+2*DIST, 4*m_rcOrgButt.Width()+5*DIST+m_rcOrgButt.Width(),
		szPrev.cy+2*DIST+m_rcOrgButt.Height());
	m_Overlay.MoveWindow(rc);
	if (m_DialogPurpose == ForInput)
	{
		rc = CRect(5*m_rcOrgButt.Width()+6*DIST, szPrev.cy+2*DIST, 5*m_rcOrgButt.Width()+6*DIST+m_rcOrgButt.Width(),
			szPrev.cy+2*DIST+m_rcOrgButt.Height());
		m_Save.MoveWindow(rc);
		rc = CRect(6*m_rcOrgButt.Width()+7*DIST, szPrev.cy+2*DIST, 6*m_rcOrgButt.Width()+7*DIST+m_rcOrgButt.Width(),
			szPrev.cy+2*DIST+m_rcOrgButt.Height());
		m_Cancel.MoveWindow(rc);
		m_AVI.ShowWindow(SW_HIDE);
	}
	else if (m_DialogPurpose == ForAVI)
	{
		rc = CRect(5*m_rcOrgButt.Width()+6*DIST, szPrev.cy+2*DIST, 5*m_rcOrgButt.Width()+6*DIST+m_rcOrgButt.Width(),
			szPrev.cy+2*DIST+m_rcOrgButt.Height());
		m_AVI.MoveWindow(rc);
		rc = CRect(6*m_rcOrgButt.Width()+7*DIST, szPrev.cy+2*DIST, 6*m_rcOrgButt.Width()+7*DIST+m_rcOrgButt.Width(),
			szPrev.cy+2*DIST+m_rcOrgButt.Height());
		m_Save.MoveWindow(rc);
		rc = CRect(7*m_rcOrgButt.Width()+8*DIST, szPrev.cy+2*DIST, 7*m_rcOrgButt.Width()+8*DIST+m_rcOrgButt.Width(),
			szPrev.cy+2*DIST+m_rcOrgButt.Height());
		m_Cancel.MoveWindow(rc);
	}
	else
	{
		rc = CRect(5*m_rcOrgButt.Width()+6*DIST, szPrev.cy+2*DIST, 5*m_rcOrgButt.Width()+6*DIST+m_rcOrgButt.Width(),
			szPrev.cy+2*DIST+m_rcOrgButt.Height());
		m_Cancel.MoveWindow(rc);
		m_AVI.ShowWindow(SW_HIDE);
		m_Save.ShowWindow(SW_HIDE);
	}
	rc = CRect(0, 0, max(8*m_rcOrgButt.Width()+9*DIST,szPrev.cx+2*DIST), szPrev.cy+3*DIST+m_rcOrgButt.Height());
	CalcWindowRect(rc);
	SetWindowPos(NULL, 0, 0, rc.Width(), rc.Height(), SWP_NOMOVE|SWP_NOZORDER);
}

LPBITMAPINFOHEADER CInputDlg::SaveImage()
{
	LPBITMAPINFOHEADER lpbi = m_lpbi;
	m_lpbi = NULL;
	return lpbi;
}

void CInputDlg::BeginModalState()
{
	m_bModalState = true;
	m_Save.EnableWindow(TRUE);
}

void CInputDlg::EndModalState()
{
	m_bModalState = false;
	m_Save.EnableWindow(FALSE);
}

void CInputDlg::OnOverlay() 
{
	if (IsDlgButtonChecked(IDC_PREVIEW))
	{
		capPreview(m_hWndCap, FALSE);
		CheckDlgButton( IDC_PREVIEW, FALSE);
	}
	capOverlay(m_hWndCap, IsDlgButtonChecked(IDC_OVERLAY));
	g_VfWProfile.WriteProfileInt(_T("Settings"), _T("Overlay"), IsDlgButtonChecked(IDC_OVERLAY));
}

void CInputDlg::OnPreview() 
{
	if (IsDlgButtonChecked(IDC_OVERLAY))
	{
		capPreview(m_hWndCap, FALSE);
		CheckDlgButton(IDC_OVERLAY, FALSE);
	}
	capPreview(m_hWndCap, IsDlgButtonChecked(IDC_PREVIEW));
	g_VfWProfile.WriteProfileInt(_T("Settings"), _T("Overlay"), IsDlgButtonChecked(IDC_OVERLAY));
}

void CInputDlg::OnDisplay() 
{
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_DISPLAY), FALSE);
	capDlgVideoDisplay(m_hWndCap);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_DISPLAY), TRUE);
	RecalcLayout(CSize(0,0));
}

void CInputDlg::OnFormat() 
{
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_FORMAT), FALSE);
	capDlgVideoFormat(m_hWndCap);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_FORMAT), TRUE);
	RecalcLayout(CSize(0,0));
}

void CInputDlg::OnSource() 
{
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_FORMAT), FALSE);
	capDlgVideoSource(m_hWndCap);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_FORMAT), TRUE);
	RecalcLayout(CSize(0,0));
}

#define CAP_SMALL_INDEX          (30 * 60 * 15)         // 15 minutes @ 30fps

static bool ShowPreviewOnCapture()
{
	int n = g_VfWProfile._GetProfileInt(_T("Settings"), _T("ShowPreviewOnCapture"), -1);
	if (n < 0)
	{
		n = 1;
		g_VfWProfile.WriteProfileInt(_T("Settings"), _T("ShowPreviewOnCapture"), n);
	}
	return n?true:false;
}

void CInputDlg::OnAvi() 
{
	CAPTUREPARMS cp;
	BOOL bOK = capCaptureGetSetup(m_hWndCap, &cp, sizeof(CAPTUREPARMS));
	char szFile[255];
	capFileGetCaptureFile(m_hWndCap, szFile, 255);
	CVideoDlg dlg(&g_VfWProfile, NULL, this);
	dlg.m_strFile = szFile;
	if( bOK )
	{
		dlg.m_iFPS = 1000000L/cp.dwRequestMicroSecPerFrame;
		dlg.m_iTime = cp.wTimeLimit;
	}
	else
		AfxMessageBox( "Warning: Cannot get setup information from driver, setup information will be ignored" );
	if( dlg.DoModal() == IDOK )
	{
		if( bOK )
		{
			cp.dwRequestMicroSecPerFrame = dlg.CalcFrameDelay();
			cp.wTimeLimit = dlg.CalcTime();
			cp.fLimitEnabled = TRUE;
			cp.wNumVideoRequested = 32;
			cp.dwIndexSize = CAP_SMALL_INDEX;
			cp.fCaptureAudio = FALSE;
			capCaptureSetSetup(m_hWndCap, &cp, sizeof(CAPTUREPARMS));
		}
		unlink(dlg.m_strFile);
		capFileSetCaptureFile(m_hWndCap, (LPCSTR)dlg.m_strFile);
	}
}

DWORD _CalcPlaybackScale(int nPlaybackRateScale)
{
	if (nPlaybackRateScale == 1)
		return 60; // frames per minute
	else if (nPlaybackRateScale == 2)
		return 3600; // frames per hour
	else
		return 1;
}

void ConvertAVIFile(const char *pszAVIFileName, DWORD dwNewRate, DWORD dwNewScale, int);
void CInputDlg::DoSaveAvi() 
{
	CAPTUREPARMS cp;
	BOOL bOK = capCaptureGetSetup(m_hWndCap, &cp, sizeof(CAPTUREPARMS));
	char szFile[255];
	capFileGetCaptureFile(m_hWndCap, szFile, 255);
	CVideoDlg dlg(&g_VfWProfile, NULL, this);
	dlg.m_strFile = szFile;
	if( bOK )
	{
		dlg.m_iFPS = 1000000L/cp.dwRequestMicroSecPerFrame;
		dlg.m_iTime = cp.wTimeLimit;
	}
	else
		AfxMessageBox( "Warning: Cannot get setup information from driver, setup information will be ignored" );
	dlg.ReadSettings();
	int r;
	if (dlg.m_strFile.IsEmpty())
		r = (int)dlg.DoModal();
	else r = IDOK;
	if (r == IDOK)
	{
		if( bOK )
		{
			cp.dwRequestMicroSecPerFrame = dlg.CalcFrameDelay();
			cp.wTimeLimit = dlg.CalcTime();
			cp.fLimitEnabled = TRUE;
			cp.wNumVideoRequested = 32;
			cp.dwIndexSize = CAP_SMALL_INDEX;
			cp.fCaptureAudio = FALSE;
			capCaptureSetSetup(m_hWndCap, &cp, sizeof(CAPTUREPARMS));
		}
		unlink(dlg.m_strFile);
		capFileSetCaptureFile(m_hWndCap, (LPCSTR)dlg.m_strFile);
		CAPSTATUS Status;
		if (!ShowPreviewOnCapture())
		{
			capGetStatus(m_hWndCap, &Status, sizeof(Status));
			if (Status.fLiveWindow)
				capPreview(m_hWndCap, FALSE);
			if (Status.fOverlayWindow)
				capOverlay(m_hWndCap, FALSE);
		}
		capCaptureSequence(m_hWndCap);
		if (!ShowPreviewOnCapture())
		{
			if (Status.fLiveWindow)
				capPreview(m_hWndCap, TRUE);
			if (Status.fOverlayWindow)
				capOverlay(m_hWndCap, TRUE);
		}
		AVIFileInit();
		PAVIFILE pfile;
		HRESULT hr = AVIFileOpen(&pfile, dlg.m_strFile, OF_READ|OF_SHARE_DENY_NONE , 0L);
		if (SUCCEEDED(hr))
		{
			DWORD dw = AVIFileRelease(pfile);
			if (dlg.m_iFPS != dlg.m_iPlaybackFPS || dlg.m_nRateScale != dlg.m_nPlaybackRateScale)
				ConvertAVIFile(dlg.m_strFile, dlg.CalcPlaybackRate(), dlg.CalcPlaybackScale(), IDS_ERROR_CONVERT_AVI);
			m_strAVIName = dlg.m_strFile;
			CString s;
			s.LoadString(IDS_AVIRECORDED);
			char szBuf[270];
			wsprintf( szBuf, s, dlg.m_strFile );
			AfxMessageBox( szBuf );
		}
		else
		{
			m_strAVIName.Empty();
			CString s;
			s.Format(IDS_ERROR_DURING_AVI_CAPTURE, (int)hr, (int)hr);
			AfxMessageBox(s, MB_OK|MB_ICONEXCLAMATION);
		}
		AVIFileExit();
	}
	else
		m_strAVIName.Empty();
}


void CInputDlg::PostNcDestroy() 
{
	CDialog::PostNcDestroy();
	m_pInputDlg = NULL;
	bool bModalState = m_bModalState;
	delete this;
	if (bModalState)
		PostQuitMessage(IDCANCEL);
}

void CInputDlg::OnCancel() 
{
	DestroyWindow();	
//	CDialog::OnCancel();
}

void CInputDlg::OnSave() 
{
	if (m_DialogPurpose == ForInput)
		DoSave();
	else
		DoSaveAvi();
	if (m_bModalState)
		PostQuitMessage(IDOK);
}


void CInputDlg::DoSave()
{
	if (m_lpbi)
	{
		free(m_lpbi);
		m_lpbi = NULL;
	}
	if (m_hWndCap)
	{
		BOOL bOverlay = IsDlgButtonChecked(IDC_OVERLAY);
		BOOL bPreview = IsDlgButtonChecked(IDC_PREVIEW);
		BOOL bAccum = g_VfWProfile.GetProfileInt(NULL, _T("Accumulate"), FALSE);
		if (bAccum)
		{
			int nImagesNum = g_VfWProfile.GetProfileInt(NULL, _T("AccumulateImagesNum"), FALSE);
			CAccumulate Accum;
			for (int i = 0; i < nImagesNum; i++)
			{
				if (capGrabFrame(m_hWndCap))
				{
					capEditCopy(m_hWndCap);
					if	(OpenClipboard())
					{
						try
						{
							HGLOBAL h = GetClipboardData(CF_DIB);
							if (h)
							{
								LPBITMAPINFOHEADER lpbiSrc = (LPBITMAPINFOHEADER)GlobalLock(h);
								Accum.Add(lpbiSrc);
								GlobalUnlock(h);
							}

						}
						catch (...) {};
					}
					CloseClipboard();
				}
			}
			unsigned nSize = Accum.CalcSize();
			m_lpbi = (LPBITMAPINFOHEADER)malloc(nSize);
			Accum.GetResult(m_lpbi, true);
		}
		else
		{
			if (capGrabFrame(m_hWndCap))
			{
				capEditCopy(m_hWndCap);
				if	(OpenClipboard())
				{
					try
					{
						HGLOBAL h = GetClipboardData(CF_DIB);
						if (h)
						{
							LPBITMAPINFOHEADER lpbiSrc = (LPBITMAPINFOHEADER)GlobalLock(h);
							m_lpbi = (LPBITMAPINFOHEADER)malloc(GlobalSize(h));
							memcpy(m_lpbi, lpbiSrc, GlobalSize(h));
							GlobalUnlock(h);
						}
					}
					catch (...) {};
				}
				CloseClipboard();
			}
		}
		capOverlay(m_hWndCap, bOverlay);
		capPreview(m_hWndCap, bPreview);
	}
}

void CInputDlg::OnHelp() 
{
	HelpDisplay( "InputDlg" );	
}

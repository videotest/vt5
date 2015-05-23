// PreviewDialog.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "StdProfile.h"
#include "MV500Grab.h"
#include "PreviewDialog.h"
#include "FormatDlg.h"
#include "VideoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CPreviewDialog dialog


CPreviewDialog::CPreviewDialog(int nDevice, CWnd* pParent /*=NULL*/)
	: CDialog(CPreviewDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPreviewDialog)
	//}}AFX_DATA_INIT
	m_bNotDestroy = false;
	m_WindowType = ForSettings;
	m_PreviewStatic.m_nDevice = nDevice;
}


void CPreviewDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPreviewDialog)
	DDX_Control(pDX, IDC_CAPTURE_VIDEO, m_AVISettings);
	DDX_Control(pDX, IDCANCEL, m_Cancel);
	DDX_Control(pDX, IDC_PROGRESS_CONV, m_Progress);
	DDX_Control(pDX, IDC_MESSAGE, m_Message);
	DDX_Control(pDX, IDC_FRAMES, m_Frames);
	DDX_Control(pDX, IDOK, m_OK);
	DDX_Control(pDX, IDC_SOURCE, m_Source);
	DDX_Control(pDX, IDC_FORMAT, m_Format);
	DDX_Control(pDX, IDC_PREVIEW, m_PreviewStatic);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPreviewDialog, CDialog)
	//{{AFX_MSG_MAP(CPreviewDialog)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_SOURCE, OnSource)
	ON_BN_CLICKED(IDC_FORMAT, OnFormat)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CAPTURE_VIDEO, OnCaptureVideo)
	//}}AFX_MSG_MAP
	ON_WM_SYSCOMMAND()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreviewDialog message handlers

BOOL CPreviewDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CRect rc;
	m_Source.GetWindowRect(rc);
	m_szButton = rc.Size();
	CMV500Grab::s_pGrab->InitGrab(CMV500Grab::GM_ForDraw,false);
	RecalcLayout();
	CMV500Grab::s_pGrab->StartGrab();
	int nTimeout;
	if (CMV500Grab::s_pGrab->GetCamerasNum() > 1)
		nTimeout = CStdProfileManager::m_pMgr->GetProfileInt(_T("Multy"), _T("RedrawFrequency"), 6, true);
	else
		nTimeout = CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("RedrawFrequency"), 30, true);
	SetTimer(1, nTimeout, NULL);
	CString s;
	HMENU hMenu = ::GetSystemMenu(m_hWnd,FALSE);
	::AppendMenu(hMenu,MF_SEPARATOR,0,NULL);
	s.LoadString(IDS_RESET);
	::AppendMenu(hMenu,MF_STRING,ID_RESET,s);
	s.LoadString(IDS_FRAMERATE);
	::AppendMenu(hMenu,MF_STRING,ID_RESET,s);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPreviewDialog::OnDestroy() 
{
	KillTimer(1);
	if (m_bNotDestroy)
		CMV500Grab::s_pGrab->StartGrab();
	else
		CMV500Grab::s_pGrab->DeinitGrab();
	CDialog::OnDestroy();
}

void CPreviewDialog::RecalcLayout()
{
	CSize sz = CMV500Grab::s_pGrab->GetSize();
	int nMargin = 20;
	// Dispose preview
	m_PreviewStatic.SetWindowPos(NULL, nMargin, nMargin, sz.cx, sz.cy, SWP_NOZORDER|SWP_NOACTIVATE);
	// Dispose buttons at right
	m_Source.SetWindowPos(NULL, sz.cx+2*nMargin, nMargin, m_szButton.cx, m_szButton.cy, SWP_NOZORDER|SWP_NOACTIVATE);
	m_Format.SetWindowPos(NULL, sz.cx+2*nMargin, 2*nMargin+m_szButton.cy, m_szButton.cx, m_szButton.cy, SWP_NOZORDER|SWP_NOACTIVATE);
	CRect rc;
	if (m_WindowType == ForAVICapture)
	{
		m_AVISettings.SetWindowPos(NULL, sz.cx+2*nMargin, 3*nMargin+2*m_szButton.cy, m_szButton.cx, m_szButton.cy, SWP_NOZORDER|SWP_NOACTIVATE);
		m_OK.SetWindowPos(NULL, sz.cx+2*nMargin, 4*nMargin+3*m_szButton.cy, m_szButton.cx, m_szButton.cy, SWP_NOZORDER|SWP_NOACTIVATE);
		m_Cancel.SetWindowPos(NULL, sz.cx+2*nMargin, 5*nMargin+4*m_szButton.cy, m_szButton.cx, m_szButton.cy, SWP_NOZORDER|SWP_NOACTIVATE);
		// Dispose self-made "status" bar in bottom
		m_Message.SetWindowPos(NULL, nMargin, 2*nMargin+sz.cy, m_szButton.cx, m_szButton.cy, SWP_NOZORDER|SWP_NOACTIVATE);
		m_Frames.SetWindowPos(NULL, nMargin+sz.cx-m_szButton.cx, 2*nMargin+sz.cy, m_szButton.cx, m_szButton.cy, SWP_NOZORDER|SWP_NOACTIVATE);
		m_Progress.SetWindowPos(NULL, 2*nMargin+m_szButton.cx, 2*nMargin+sz.cy, sz.cx-2*nMargin-2*m_szButton.cx, m_szButton.cy, SWP_NOZORDER|SWP_NOACTIVATE);
		// Now recalculate all window size
		rc = CRect(0,0, sz.cx+3*nMargin+m_szButton.cx, max(sz.cy+nMargin+m_szButton.cy,4*nMargin+5*m_szButton.cy)+2*nMargin);
	}
	else if (m_WindowType == ForImageCapture)
	{
		m_OK.SetWindowPos(NULL, sz.cx+2*nMargin, 3*nMargin+2*m_szButton.cy, m_szButton.cx, m_szButton.cy, SWP_NOZORDER|SWP_NOACTIVATE);
		m_Cancel.SetWindowPos(NULL, sz.cx+2*nMargin, 4*nMargin+3*m_szButton.cy, m_szButton.cx, m_szButton.cy, SWP_NOZORDER|SWP_NOACTIVATE);
		// Hide unneeded windows
		m_AVISettings.ShowWindow(SW_HIDE);
		m_Message.ShowWindow(SW_HIDE);
		m_Frames.ShowWindow(SW_HIDE);
		m_Progress.ShowWindow(SW_HIDE);
		// Now recalculate all window size
		rc = CRect(0,0, sz.cx+3*nMargin+m_szButton.cx, max(sz.cy,3*nMargin+4*m_szButton.cy)+2*nMargin);
	}
	else
	{
		m_OK.SetWindowPos(NULL, sz.cx+2*nMargin, 3*nMargin+2*m_szButton.cy, m_szButton.cx, m_szButton.cy, SWP_NOZORDER|SWP_NOACTIVATE);
		// Hide unneeded windows
		m_Cancel.ShowWindow(SW_HIDE);
		m_AVISettings.ShowWindow(SW_HIDE);
		m_Message.ShowWindow(SW_HIDE);
		m_Frames.ShowWindow(SW_HIDE);
		m_Progress.ShowWindow(SW_HIDE);
		// Now recalculate all window size
		rc = CRect(0,0, sz.cx+3*nMargin+m_szButton.cx, max(sz.cy,2*nMargin+3*m_szButton.cy)+2*nMargin);
	}
	CalcWindowRect(rc);
	SetWindowPos(NULL, 0, 0, rc.Width(), rc.Height(), SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE);
}

void CPreviewDialog::Reinit(int nMode)
{
	if (nMode == 0 || nMode == 1)
	{
		CMV500Grab::s_pGrab->DeinitGrab();
		unsigned nModeGm = nMode==0?CMV500Grab::GM_ForDraw|CMV500Grab::GM_Reinit:CMV500Grab::GM_ForDraw;
		CMV500Grab::s_pGrab->InitGrab(nModeGm,false);
		RecalcLayout();
		CMV500Grab::s_pGrab->StartGrab();
	}
	else if (nMode == 2)
	{
		CMV500Grab::s_pGrab->ReinitBrightContrast();
	}
}

void CPreviewDialog::OnSource() 
{

	int nWide = CStdProfileManager::m_pMgr->GetProfileInt(_T("Source"), _T("WideDialog"), 1, true);
	CSourceDlg dlg(m_PreviewStatic.m_nDevice,this,nWide?IDD_SOURCE1:IDD_SOURCE);
	dlg.SetSite(this);
	dlg.DoModal();
}

void CPreviewDialog::OnFormat() 
{
	CFormatDlg dlg;
	if (dlg.DoModal() == IDOK)
		Reinit(1);
}

void CPreviewDialog::OnTimer(UINT_PTR nIDEvent) 
{
//	CMV500Grab::s_pGrab->OnTimer();
	m_PreviewStatic.Invalidate();		
	CDialog::OnTimer(nIDEvent);
}

void CPreviewDialog::OnCaptureVideo() 
{
	if (CMV500Grab::s_pGrab->IsOverlay())
	{
		CMV500Grab::s_pGrab->DeinitGrab();
		CMV500Grab::s_pGrab->InitGrab(CMV500Grab::GM_RGB16,false);
		CMV500Grab::s_pGrab->StartGrab();
		CVideoDlg dlg;
		dlg.DoModal();
		CMV500Grab::s_pGrab->DeinitGrab();
		CMV500Grab::s_pGrab->InitGrab(CMV500Grab::GM_ForDraw,false);
		CMV500Grab::s_pGrab->StartGrab();
	}
	else
	{
		CVideoDlg dlg;
		dlg.DoModal();
	}
}

void CPreviewDialog::Start(const char *psz, int nMax)
{
	m_Message.ShowWindow(SW_SHOW);
	m_Message.SetWindowText(psz);
	char szBuff[100];
	wsprintf(szBuff, "0/%d", nMax);
	m_nMax = nMax;
	m_Frames.ShowWindow(SW_SHOW);
	m_Frames.SetWindowText(szBuff);
	m_Progress.ShowWindow(SW_SHOW);
	m_Progress.SetRange32(0,nMax);
	m_Progress.SetPos(0);
}

void CPreviewDialog::End()
{
	m_Message.ShowWindow(SW_HIDE);
	m_Frames.ShowWindow(SW_HIDE);
	m_Progress.ShowWindow(SW_HIDE);
}

void CPreviewDialog::Notify(int nCur)
{
	char szBuff[100];
	wsprintf(szBuff, "%d/%d", nCur, m_nMax);
	m_Progress.SetPos(nCur);
	m_Frames.SetWindowText(szBuff);
}


void CPreviewDialog::OnReset() 
{
	if (AfxMessageBox(IDS_RESET_Q, MB_YESNO|MB_ICONQUESTION) == IDYES)
	{
		CStdProfileManager::m_pMgr->ResetSettings();	
		Reinit(0);
	}
}

void CPreviewDialog::OnOK() 
{
	if (m_WindowType == ForAVICapture)
	{
		m_strAVI = CStdProfileManager::m_pMgr->GetProfileString(_T("AVI"), _T("FileName"), _T("c:\\capture.avi"));
		if (m_strAVI.IsEmpty())
		{
			OnCaptureVideo();
			m_strAVI = CStdProfileManager::m_pMgr->GetProfileString(_T("AVI"), _T("FileName"), _T("c:\\capture.avi"));
		}
		if (!m_strAVI.IsEmpty())
		{
			CMV500Grab::s_pGrab->DeinitGrab();
			if (!CMV500Grab::s_pGrab->GrabAVI(this))
				m_strAVI.Empty();
		}
	}
	CDialog::OnOK();
}

void CPreviewDialog::OnSysCommand(UINT nID, LPARAM lParam )
{
	if (nID == ID_RESET)
		OnReset();
	else if (nID == ID_FRAMERATE)
	{
	}
	else
		CDialog::OnSysCommand(nID,lParam);
}


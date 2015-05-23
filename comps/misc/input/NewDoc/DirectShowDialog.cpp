// DirectShowDialog.cpp : implementation file
//

#include "stdafx.h"
#include "newdoc.h"
#include "DirectShowDriver.h"
#include "DirectShowDialog.h"
#include "VideoDlg.h"
#include "InputUtils.h"
#include <io.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// DirectShowDialog dialog


DirectShowDialog::DirectShowDialog(IUnknown *punkDrv, int nDev, CDialogPurpose Purpose, CWnd* pParent /*=NULL*/)
	: CDialog(Purpose==ForAVI?IDD_DSHOW_VIDEO:IDD_DSHOW, pParent)
{
	//{{AFX_DATA_INIT(DirectShowDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_Purpose = Purpose;
	m_sptrIV = punkDrv;
	m_sptrDrv = punkDrv;
	m_nDev = nDev;
}


void DirectShowDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DirectShowDialog)
	DDX_Control(pDX, IDC_PROGRESS_VIDEO, m_VideoProgress);
	//}}AFX_DATA_MAP
}

void DirectShowDialog::RepositionWindow()
{
	short cx,cy;
	m_sptrIV->GetSize(m_lWnd, &cx, &cy);
	CSize szTotal;
	Reposition(CSize(cx,cy), m_rcBase, szTotal);
	CRect rcTotal(CPoint(0,0), szTotal);
	AdjustWindowRect(rcTotal,GetWindowLong(m_hWnd,GWL_STYLE),FALSE);
	SetWindowPos(NULL, 0, 0, rcTotal.Width(), rcTotal.Height(), SWP_NOZORDER|SWP_NOMOVE);
	m_sptrIV->SetInputWindowPos(m_lWnd, m_rcBase.left, m_rcBase.top, m_rcBase.Width(), m_rcBase.Height(), 0);
}


BEGIN_MESSAGE_MAP(DirectShowDialog, CDialog)
	//{{AFX_MSG_MAP(DirectShowDialog)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_SETTINGS, OnSettings)
	ON_BN_CLICKED(IDC_CROSSBAR, OnCrossbar)
	ON_BN_CLICKED(IDC_TUNER, OnTuner)
	ON_BN_CLICKED(IDC_FORMAT, OnFormat)
	ON_BN_CLICKED(IDC_VIDEO_SETTINGS, OnVideoSettings)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DirectShowDialog message handlers

BOOL DirectShowDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	try
	{
		Init(m_hWnd, IDC_DSPREVIEW, _MaxClientSize(this));
		m_sptrIV->CreateInputWindow(m_nDev, m_hWnd, &m_lWnd);
		RepositionWindow();
		BSTR bstrName = NULL;
		m_sptrDrv->GetDeviceNames(m_nDev,NULL,&bstrName,NULL);
		if (bstrName)
		{
			CString s(bstrName);
			SysFreeString(bstrName);
			SetWindowText(s);
		}
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_SETTINGS), m_sptrDrv->ExecuteDriverDialog(m_nDev,_T("?Source"))==S_OK);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_FORMAT), m_sptrDrv->ExecuteDriverDialog(m_nDev,_T("?Format"))==S_OK);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_CROSSBAR), m_sptrDrv->ExecuteDriverDialog(m_nDev,_T("?Crossbar"))==S_OK);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_TUNER), m_sptrDrv->ExecuteDriverDialog(m_nDev,_T("?Tuner"))==S_OK);
		::ShowWindow(::GetDlgItem(m_hWnd, IDC_VIDEO_SETTINGS), m_Purpose==ForAVI?SW_SHOW:SW_HIDE);
	}
	catch(CException*e)
	{
		e->Delete();
		return FALSE;
	}
	catch(...)
	{
		return FALSE;
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void DirectShowDialog::OnDestroy() 
{
	m_sptrIV->DestroyInputWindow(m_lWnd);
	CDialog::OnDestroy();
}

void DirectShowDialog::OnSettings() 
{
	m_sptrDrv->ExecuteDriverDialog(m_nDev,_T("Source"));
}

void DirectShowDialog::OnFormat() 
{
	m_sptrDrv->ExecuteDriverDialog(m_nDev,_T("Format"));
	RepositionWindow(); // sizes of window can change
}

void DirectShowDialog::OnCrossbar() 
{
	m_sptrDrv->ExecuteDriverDialog(m_nDev,_T("Crossbar"));
	RepositionWindow(); // sizes of window can change
}

void DirectShowDialog::OnTuner() 
{
	m_sptrDrv->ExecuteDriverDialog(m_nDev,_T("Tuner"));
	RepositionWindow(); // sizes of window can change
}


void DirectShowDialog::OnVideoSettings() 
{
	CVideoDlg dlg(&g_DSProfile, m_sptrDrv, this);
	dlg.DoModal();
}

void DirectShowDialog::OnOK() 
{
	if (m_Purpose == ForAVI)
	{
		CString s = g_DSProfile.GetProfileString(_T("VideoDlg"), _T("FileName"), NULL);
		if (s.IsEmpty())
		{
			CVideoDlg dlg(&g_DSProfile, m_sptrDrv, this);
			if (dlg.DoModal() != IDOK)
				return;
		}
		_tfinddata_t find;
		if (_tfindfirst(s, &find) != -1)
		{
			if (_tunlink(s) != 0)
			{
				CString sErr;
				sErr.Format(IDS_FILE_INACCESSIBLE, (LPCTSTR)s);
				AfxMessageBox(sErr, MB_OK|MB_ICONEXCLAMATION);
				return;
			}
		}
		m_nTimeLimit = g_DSProfile._GetProfileInt(_T("VideoDlg"), _T("Time"), 1);
		int nRateScale = g_DSProfile.GetProfileInt(_T("VideoDlg"), _T("RateScale"), 0);
		if (nRateScale == 1)
			m_nTimeLimit *= 60;
		else if (nRateScale == 2)
			m_nTimeLimit *= 3600;
		m_sptrDrv->InputVideoFile(NULL,m_nDev,-1);
		SetTimer(1,25,NULL);
		m_lCapStartTime = timeGetTime();
		m_sptrIV->SetInputWindowPos(m_lWnd, m_rcBase.left, m_rcBase.top, m_rcBase.Width(), m_rcBase.Height(), 0);
		m_VideoProgress.ShowWindow(SW_SHOW);
		m_VideoProgress.SetRange(0,(short)(m_nTimeLimit*10));
	}
	else
		CDialog::OnOK();
}

void DirectShowDialog::OnTimer(UINT_PTR nIDEvent) 
{
	static int nCount = 0;
	long lTime = timeGetTime()-m_lCapStartTime;
	if (lTime > m_nTimeLimit*1000)
		EndDialog(IDOK);
	if (nCount++ >=5)
	{
		nCount = 5;
		m_VideoProgress.SetPos(lTime/100);
	}
	CDialog::OnTimer(nIDEvent);
}

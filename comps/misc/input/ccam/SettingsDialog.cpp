// SettingsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "CCam.h"
#include "SettingsDialog.h"
#include "input.h"
#include "StdProfile.h"
#include "CCamUtils.h"


static CHistoIds s_HistoIDs =
{ IDD_HISTO, IDC_HISTO, IDC_GRAY, IDC_RED, IDC_GREEN, IDC_BLUE, IDC_STATIC_0, IDC_STATIC_MAX_COLOR,
IDC_STATIC_MAX_POINTS, IDC_STATIC_MSG};


// CSettingsDialog dialog

IMPLEMENT_DYNAMIC(CSettingsDialog, CBaseDialog)
CSettingsDialog::CSettingsDialog(IUnknown *punk, int nDevice, CWnd* pParent /*=NULL*/)
	: CBaseDialog(CSettingsDialog::IDD, pParent, NULL), m_HistoDlg(s_HistoIDs)
{
	m_sptrPrv = punk;
	if (m_sptrPrv == 0)
		AfxThrowNotSupportedException();
	m_sptrDrv = punk;
	m_nDev = nDevice;
	m_pDialog = NULL;
}

CSettingsDialog::~CSettingsDialog()
{
	delete m_pDialog;
}

void CSettingsDialog::AddDialog(CBaseDialog *p, int idd)
{
	if (m_pDialog)
	{
		m_pDialog->DestroyWindow();
		delete m_pDialog;
	}
	m_pDialog = p;
	m_pDialog->m_sptrDrv = m_sptrDrv;
	m_pDialog->m_sptrPrv = m_sptrPrv;
	m_pDialog->m_nDev = m_nDev;
	m_pDialog->CreatePlain(idd, &m_Tab);
	CSize sz;
	CRect rcWnd,rcDlg;
	m_pDialog->GetWindowRect(rcWnd);
	sz = rcWnd.Size();
	m_Tab.AdjustRect(TRUE, rcWnd);
	m_Tab.SetWindowPos(0, 0, 0, rcWnd.Width(), rcWnd.Height(), SWP_NOMOVE|SWP_NOZORDER);
	m_Tab.GetWindowRect(rcWnd);
	ScreenToClient(rcWnd);
	rcDlg = rcWnd;
	m_Tab.AdjustRect(FALSE, rcDlg);
	m_pDialog->SetWindowPos(0, rcDlg.left-rcWnd.left, rcDlg.top-rcWnd.top, rcDlg.Width(), rcDlg.Height(), SWP_NOZORDER);
	m_pDialog->ShowWindow(SW_SHOW);
	int n = 5;
	CRect rcTotal(0,0,max(3*m_szButton.cx+4*n,rcWnd.Width()+2*rcWnd.left), rcWnd.bottom+m_szButton.cy+2*n);
	int nButtonY = rcWnd.bottom+n;
	if (m_HistoDlg.m_hWnd != NULL)
	{
		CRect rc;
		m_HistoDlg.GetWindowRect(rc);
		rcTotal.bottom += rc.Height()+n;
		if (rc.Width() > rcTotal.Width())
			rcTotal.right = rcTotal.left+rc.Width();
		m_HistoDlg.SetWindowPos(NULL, 0, nButtonY, 0, 0, SWP_NOZORDER|SWP_NOSIZE);
		nButtonY += rc.Height()+n;
	}
	m_Ok.SetWindowPos(0, n, nButtonY, m_szButton.cx, m_szButton.cy, SWP_NOZORDER);
	m_Help.SetWindowPos(0, m_szButton.cx+2*n, nButtonY, m_szButton.cx, m_szButton.cy, SWP_NOZORDER);
	m_StaticPeriod.SetWindowPos(0, 2*m_szButton.cx+3*n, nButtonY, m_szButton.cx, m_szButton.cy, SWP_NOZORDER);
	AdjustWindowRect(rcTotal, GetWindowLong(m_hWnd, GWL_STYLE), FALSE);
	SetWindowPos(0, 0, 0, rcTotal.Width(), rcTotal.Height(), SWP_NOMOVE|SWP_NOZORDER);
}

void CSettingsDialog::AddDialog(int n)
{
	if (CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("ColorCamera"), TRUE, true, true))
	{
		if (n == 1)
		{
			if (CStdProfileManager::m_pMgr->GetProfileInt(_T("Dialogs"), _T("ColorBalance"), FALSE, true, true)) 
				AddDialog(new CBaseDialog(IDD_LEVEL_ADJUSTMENT,this,this), IDD_LEVEL_ADJUSTMENT);
			else
				AddDialog(new CBaseDialog(IDD_COLOR_BALANCE,this,this), IDD_COLOR_BALANCE);
		}
		else if (n == 2)
			AddDialog(new CBaseDialog(IDD_IMAGE,this,this), IDD_IMAGE);
		else if (n == 3)
			AddDialog(new CBaseDialog(IDD_FRAME,this,this), IDD_FRAME);
		else
		{
			if (CStdProfileManager::m_pMgr->GetProfileInt(_T("Dialogs"), _T("Camera"), FALSE, true, true))
				AddDialog(new CBaseDialog(IDD_CAMERA_EX,this,this), IDD_CAMERA_EX);
			else
				AddDialog(new CBaseDialog(IDD_CAMERA,this,this), IDD_CAMERA);
		}
	}
	else
	{
		if (n == 1)
			AddDialog(new CBaseDialog(IDD_IMAGE,this,this), IDD_IMAGE);
		else if (n == 2)
			AddDialog(new CBaseDialog(IDD_FRAME,this,this), IDD_FRAME);
		else
		{
			if (CStdProfileManager::m_pMgr->GetProfileInt(_T("Dialogs"), _T("Camera"), FALSE, true, true))
				AddDialog(new CBaseDialog(IDD_CAMERA_EX,this,this), IDD_CAMERA_EX);
			else
				AddDialog(new CBaseDialog(IDD_CAMERA,this,this), IDD_CAMERA);
		}
	}
}

void CSettingsDialog::ShowHistogram(bool bShow)
{
	if (bShow && m_HistoDlg.m_hWnd || !bShow && m_HistoDlg.m_hWnd == NULL)
		return;
	const int yButOffs = 5;
	if (bShow)
	{
		m_HistoDlg.Create(IDD_HISTO, this);
		CRect rcOk,rcHelp,rcPeriod,rcHist,rcTotal;
		m_Ok.GetWindowRect(&rcOk);
		m_Help.GetWindowRect(&rcHelp);
		m_StaticPeriod.GetWindowRect(&rcPeriod);
		ScreenToClient(rcOk);
		ScreenToClient(rcHelp);
		ScreenToClient(rcPeriod);
		m_HistoDlg.SetWindowPos(NULL, 0, rcOk.top, 0, 0, SWP_NOZORDER|SWP_NOSIZE);
		m_HistoDlg.GetWindowRect(rcHist);
		m_Ok.SetWindowPos(NULL, rcOk.left, rcOk.top+rcHist.Height()+yButOffs, 0, 0, SWP_NOZORDER|SWP_NOSIZE);
		m_Help.SetWindowPos(NULL, rcHelp.left, rcHelp.top+rcHist.Height()+yButOffs, 0, 0, SWP_NOZORDER|SWP_NOSIZE);
		m_StaticPeriod.SetWindowPos(NULL, rcPeriod.left, rcPeriod.top+rcHist.Height()+yButOffs, 0, 0, SWP_NOZORDER|SWP_NOSIZE);
		m_HistoDlg.ShowWindow(SW_SHOW);
		GetClientRect(rcTotal);
		rcTotal.bottom += rcHist.Height()+yButOffs;
		if (rcTotal.Width() < rcHist.Width()) rcTotal.right = rcTotal.left+rcHist.Width();
		AdjustWindowRect(&rcTotal, GetStyle(), FALSE);
		SetWindowPos(NULL, 0, 0, rcTotal.Width(), rcTotal.Height(), SWP_NOZORDER|SWP_NOMOVE);
	}
	else
	{
		CRect rcOk,rcHelp,rcPeriod,rcHisto,rcTotal,rcWnd;
		m_Ok.GetWindowRect(&rcOk);
		m_Help.GetWindowRect(&rcHelp);
		m_StaticPeriod.GetWindowRect(&rcPeriod);
		m_HistoDlg.GetWindowRect(rcHisto);
		GetClientRect(rcTotal);
		m_HistoDlg.DestroyWindow();
		ScreenToClient(rcOk);
		ScreenToClient(rcHelp);
		ScreenToClient(rcPeriod);
		int nButtonY = rcOk.top-rcHisto.Height()-yButOffs;
		m_Ok.SetWindowPos(NULL, rcOk.left, nButtonY, 0, 0, SWP_NOZORDER|SWP_NOSIZE);
		m_Help.SetWindowPos(NULL, rcHelp.left, nButtonY, 0, 0, SWP_NOZORDER|SWP_NOSIZE);
		m_StaticPeriod.SetWindowPos(NULL, rcPeriod.left, nButtonY, 0, 0, SWP_NOZORDER|SWP_NOSIZE);
		rcTotal.bottom -= rcHisto.Height()+yButOffs;
		m_pDialog->GetWindowRect(rcWnd);
		ScreenToClient(rcWnd);
		rcTotal.right = rcTotal.left+max(3*m_szButton.cx+4*yButOffs,rcWnd.Width()+2*rcWnd.left);
		AdjustWindowRect(rcTotal,GetStyle(),FALSE);
		SetWindowPos(NULL, 0, 0, rcTotal.Width(), rcTotal.Height(), SWP_NOZORDER|SWP_NOMOVE);
	}
}


void CSettingsDialog::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_Tab);
	DDX_Control(pDX, IDC_STATIC_PERIOD, m_StaticPeriod);
	DDX_Control(pDX, IDOK, m_Ok);
	DDX_Control(pDX, ID_HELP, m_Help);
}


BEGIN_MESSAGE_MAP(CSettingsDialog, CBaseDialog)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, OnTcnSelchangeTab1)
	ON_MESSAGE(WM_APP+1, OnDelayedRepaint)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CSettingsDialog message handlers

BOOL CSettingsDialog::OnInitDialog()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	__super::OnInitDialog();

	m_sptrPrv->BeginPreview(m_nDev, this);
	CRect rc;
	m_Ok.GetWindowRect(rc);
	m_szButton = rc.Size();
	CString s;
	s.LoadString(IDS_CAMERA);
	m_Tab.InsertItem(0, s);
	if (CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("ColorCamera"), TRUE, true, true))
	{
		s.LoadString(IDS_COLOR_BALANCE);
		m_Tab.InsertItem(1, s);
		s.LoadString(IDS_IMAGE);
		m_Tab.InsertItem(2, s);
		s.LoadString(IDS_FRAME);
		m_Tab.InsertItem(3, s);
	}
	else
	{
		s.LoadString(IDS_IMAGE);
		m_Tab.InsertItem(1, s);
		s.LoadString(IDS_FRAME);
		m_Tab.InsertItem(2, s);
	}
	AddDialog(0);
	if (m_bHistogram)
		ShowHistogram(m_bHistogram?true:false);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSettingsDialog::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	int n = m_Tab.GetCurSel();
	AddDialog(n);
	*pResult = 0;
}

void CSettingsDialog::OnOK()
{
	__super::OnOK();
}

void CSettingsDialog::OnLayoutChanged()
{
	ShowHistogram(m_bHistogram?true:false);
}

HRESULT CSettingsDialog::Invalidate()
{
	if (m_bHistogram && m_sptrDrv != 0)
	{
		LPBITMAPINFOHEADER lpbi = NULL;
		CCCamImageData *pImgData;
		DWORD dwSize;
		m_sptrDrv->GetImage(m_nDev, (void**)&pImgData, &dwSize);
		m_HistoDlg.m_HistoBox.InitHisto(pImgData->lpbi, pImgData->pData);
	}
	PostMessage(WM_APP+1);
	return S_OK;
}

HRESULT CSettingsDialog::OnChangeSize()
{
	return S_OK;
}

LRESULT CSettingsDialog::OnDelayedRepaint(WPARAM wParam, LPARAM lParam)
{
	DWORD dwPeriod;
	m_sptrPrv->GetPeriod(m_nDev,0,&dwPeriod);
	TCHAR szBuff[50];
	_ltot(dwPeriod, szBuff, 10);
	m_StaticPeriod.SetWindowText(szBuff);
	if (m_pDialog) m_pDialog->OnDelayedNewImage();
	if (m_bHistogram && m_sptrDrv != 0)
		m_HistoDlg.ResetMinMax();
	return 0;
}

void CSettingsDialog::OnDestroy()
{
	m_sptrPrv->EndPreview(m_nDev, this);
	__super::OnDestroy();
}

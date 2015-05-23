// PreviewDialog.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "PreviewDialog.h"
//#include "Settings.h"
#include "StdProfile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPreviewDialog dialog

const int Marg = 15;
const int ImgX = 640;
const int ImgY = 480;

static CHistoIds s_HistoIDs =
{ IDD_HISTO, IDC_HISTO, IDC_GRAY, IDC_RED, IDC_GREEN, IDC_BLUE, IDC_STATIC_0, IDC_STATIC_MAX_COLOR,
IDC_STATIC_MAX_POINTS, IDC_STATIC_MSG};

CPreviewDialog::CPreviewDialog(IUnknown *punk, int nDevice, bool bRight, CWnd* pParent /*=NULL*/)
	: CBaseDialog(bRight?IDD_PREVIEW1:IDD_PREVIEW, pParent, NULL), m_Image(punk, nDevice),
	m_HistoDlg(s_HistoIDs)
{
	//{{AFX_DATA_INIT(CPreviewDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_sptrPrv = punk;
	if (m_sptrPrv == 0)
		AfxThrowNotSupportedException();
	m_sptrDrv = punk;
	m_nDev = nDevice;
	m_pDialog = NULL;
	m_bDelayedInitCtrllSite = true;
	m_bDisableAllCtrls = false;
	m_bPreview = true;
	m_nDlgSize = 0;
}

CPreviewDialog::~CPreviewDialog()
{
	delete m_pDialog;
}


void CPreviewDialog::DoDataExchange(CDataExchange* pDX)
{
	CBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPreviewDialog)
	DDX_Control(pDX, IDC_STATIC_PERIOD, m_StaticPeriod);
	//}}AFX_DATA_MAP
	if (m_bPreview) DDX_Control(pDX, IDC_IMAGE, m_Image);
}


BEGIN_MESSAGE_MAP(CPreviewDialog, CBaseDialog)
	//{{AFX_MSG_MAP(CPreviewDialog)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BRIGHTNESS_CONTRAST, OnBrightnessContrast)
	ON_BN_CLICKED(IDC_COLOR_BALANCE, OnColorBalance)
	ON_BN_CLICKED(IDC_IMAGE_PAGE, OnImagePage)
	ON_BN_CLICKED(IDC_VIDEO, OnVideo)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
	ON_WM_SIZE()
	ON_WM_SIZING()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_APP+1, OnDelayedRepaint)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreviewDialog message handlers

BOOL CPreviewDialog::OnInitDialog() 
{
	CBaseDialog::OnInitDialog();
	// begin preview	
	m_sptrPrv->BeginPreview(m_nDev, this);
	m_bInitOk = true;//
	Init(m_hWnd, IDC_IMAGE, CSize(0,0));
	if (m_bPreview)
		m_Image.InitSizes(m_bInitOk);
	OnLayoutChanged();
	SetMaximalSize(m_Image.m_szPrv);
	InitControlsState();
	if (CStdProfileManager::m_pMgr->GetProfileInt(_T("General"), _T("EnableSettings"), 1, true, false) == 0)
	{
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_BRIGHTNESS_CONTRAST), FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_COLOR_BALANCE), FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_IMAGE_PAGE), FALSE);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

static void _test_size(int id, int &nMaxSize)
{
	CDialog *p = new CDialog;
	p->Create(id);
	CRect rcDlg;
	p->GetWindowRect(rcDlg);
	int nWidth = rcDlg.Width();
	if (nWidth > nMaxSize)
		nMaxSize = nWidth;
	p->DestroyWindow();
	delete p;
}

void CPreviewDialog::AddDialog(CBaseDialog *p, int idd)
{
	if (m_pDialog)
	{
		RemoveTempControl(m_pDialog->GetSafeHwnd());
		m_pDialog->DestroyWindow();
		delete m_pDialog;
	}
	m_pDialog = p;
	if (m_pDialog)
	{
		m_pDialog->m_sptrDrv = m_sptrDrv;
		m_pDialog->m_sptrPrv = m_sptrPrv;
		m_pDialog->m_nDev = m_nDev;
		if (m_nDlgSize == 0)
		{
//			_test_size(IDD_COLOR_BALANCE, m_nDlgSize);
//			_test_size(IDD_LEVEL_ADJUSTMENT, m_nDlgSize);
//			_test_size(IDD_IMAGE, m_nDlgSize);
			if (CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("UseExtendedColorBalance"), FALSE, true, true)) 
				_test_size(IDD_SETUP_EX, m_nDlgSize);
			else
				_test_size(IDD_BRIGHTNESS_CONTRAST, m_nDlgSize);
//			_test_size(IDD_VIDEO, m_nDlgSize);
		}
		m_pDialog->Create(idd, this);
		CRect rcWnd;
		m_pDialog->GetWindowRect(rcWnd);
		m_pDialog->SetWindowPos(0, 0, 0, m_nDlgSize, rcWnd.Height(), SWP_NOMOVE|SWP_NOZORDER);
		m_pDialog->ShowWindow(SW_SHOW);
		AddTempControl(m_pDialog->GetSafeHwnd(), 0, 0);
	}
	OnLayoutChanged();
}

void CPreviewDialog::CheckRadioButtons(int id)
{
	CheckRadioButton(IDC_BRIGHTNESS_CONTRAST, IDC_VIDEO, id);
}

void CPreviewDialog::OnColorBalance() 
{
	int n = IsDlgButtonChecked(IDC_COLOR_BALANCE);
	if (n)
	{
		CheckRadioButtons(-1);
		AddDialog(NULL, 0);
	}
	else
	{
		CheckRadioButtons(IDC_COLOR_BALANCE);
		if (CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("UseExtendedColorBalance"), FALSE, true, true)) 
			AddDialog(new CBaseDialog(IDD_LEVEL_ADJUSTMENT,this,this), IDD_LEVEL_ADJUSTMENT);
		else 
			AddDialog(new CBaseDialog(IDD_COLOR_BALANCE,this,this), IDD_COLOR_BALANCE);
	}
}

void CPreviewDialog::OnImagePage() 
{
	int n = IsDlgButtonChecked(IDC_IMAGE_PAGE);
	if (n)
	{
		CheckRadioButtons(-1);
		AddDialog(NULL, 0);
	}
	else
	{
		CheckRadioButtons(IDC_IMAGE_PAGE);
		AddDialog(new CBaseDialog(IDD_IMAGE,this,this), IDD_IMAGE);
	}
}

void CPreviewDialog::OnBrightnessContrast() 
{
	int n = IsDlgButtonChecked(IDC_BRIGHTNESS_CONTRAST);
	if (n)
	{
		CheckRadioButtons(-1);
		AddDialog(NULL, 0);
	}
	else
	{
		CheckRadioButtons(IDC_BRIGHTNESS_CONTRAST);
		if (CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("UseExtendedColorBalance"), FALSE, true, true))
			AddDialog(new CBaseDialog(IDD_SETUP_EX,this,this), IDD_SETUP_EX);
		else
			AddDialog(new CBaseDialog(IDD_BRIGHTNESS_CONTRAST,this,this), IDD_BRIGHTNESS_CONTRAST);
	}
}

void CPreviewDialog::OnVideo() 
{
	int n = IsDlgButtonChecked(IDC_VIDEO);
	if (n)
	{
		CheckRadioButtons(-1);
		AddDialog(NULL, 0);
	}
	else
	{
		CheckRadioButtons(IDC_VIDEO);
		AddDialog(new CBaseDialog(IDD_VIDEO,this,this), IDD_VIDEO);
	}
}

void CPreviewDialog::OnCancel() 
{
//	if (m_bInitOk && !m_bPreview) ::RestoreSettings();
	CBaseDialog::OnCancel();
}

void CPreviewDialog::OnOK() 
{
//	if (m_bInitOk && !m_bPreview) ::SaveSettings();
	CBaseDialog::OnOK();
}


void CPreviewDialog::OnDestroy() 
{
//	if (m_bInitOk && m_bPreview) ::SaveSettings();
	m_sptrPrv->EndPreview(m_nDev, this);
	CBaseDialog::OnDestroy();
}

HRESULT CPreviewDialog::Invalidate()
{
	if (m_bPreview) m_Image.Invalidate();
/*	if (m_bHistogram && m_sptrDrv != 0)
	{
		LPBITMAPINFOHEADER lpbi = NULL;
		CCCamImageData *pImgData;
		DWORD dwSize;
		m_sptrDrv->GetImage(m_nDev, (void**)&pImgData, &dwSize);
		m_HistoDlg.m_HistoBox.InitHisto(pImgData->lpbi, pImgData->pData);
	}*/
	PostMessage(WM_APP+1);
	return S_OK;
}

HRESULT CPreviewDialog::OnChangeSize()
{
	m_Image.InitSizes(true);
	OnLayoutChanged();
	return S_OK;
}

LRESULT CPreviewDialog::OnDelayedRepaint(WPARAM wParam, LPARAM lParam)
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

void CPreviewDialog::OnLayoutChanged()
{
	BOOL bHisto = CStdProfileManager::m_pMgr->GetProfileInt(_T("Histogram"), _T("Show"), FALSE);
	if (bHisto && m_HistoDlg.GetSafeHwnd()==NULL)
	{
		m_HistoDlg.Create(IDD_HISTO, this);
		AddTempControl(m_HistoDlg.GetSafeHwnd(), 0, 1);
		m_HistoDlg.ShowWindow(SW_SHOW);
		ASSERT(m_HistoDlg.GetSafeHwnd()!=NULL);
	}
	else if (!bHisto && m_HistoDlg.GetSafeHwnd()!=NULL)
	{
		m_HistoDlg.ShowWindow(SW_HIDE);
		RemoveTempControl(m_HistoDlg.GetSafeHwnd());
		m_HistoDlg.DestroyWindow();
		ASSERT(m_HistoDlg.GetSafeHwnd()==NULL);
	}
	CRect rcWnd;
	GetWindowRect(&rcWnd);
	int cxFullScreen = ::GetSystemMetrics(SM_CXFULLSCREEN);
	int cyFullScreen = ::GetSystemMetrics(SM_CYFULLSCREEN);
	CSize sz = CalcSizeByCx(m_Image.m_szPrv, min(rcWnd.Width(),cxFullScreen), true);
	if (sz.cy > cyFullScreen)
		sz = CalcSizeByCy(m_Image.m_szPrv, cyFullScreen, true);
	SetWindowPos(NULL, rcWnd.left, rcWnd.top, sz.cx, sz.cy, SWP_NOZORDER);
}

void CPreviewDialog::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_rcBase.PtInRect(point))
	{
		CPoint pt(point.x-m_rcBase.left, point.y-m_rcBase.top);
		m_Image.DoLButtonDown(nFlags, pt);
	}
	CBaseDialog::OnLButtonDown(nFlags, point);
}

void CPreviewDialog::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (m_rcBase.PtInRect(point))
	{
		CPoint pt(point.x-m_rcBase.left, point.y-m_rcBase.top);
		m_Image.DoLButtonUp(nFlags, pt);
	}
	CBaseDialog::OnLButtonUp(nFlags, point);
}

void CPreviewDialog::OnMouseMove(UINT nFlags, CPoint point) 
{
	if ((nFlags & MK_LBUTTON) && m_rcBase.PtInRect(point))
	{
		CPoint pt(point.x-m_rcBase.left, point.y-m_rcBase.top);
		m_Image.DoMouseMove(nFlags, pt);
	}
	CBaseDialog::OnMouseMove(nFlags, point);
}

BOOL CPreviewDialog::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	CPoint point;
	GetCursorPos(&point);
	ScreenToClient(&point);
	if (m_rcBase.PtInRect(point))
	{
		CPoint pt(point.x-m_rcBase.left, point.y-m_rcBase.top);
		m_Image.DoSetCursor(0, pt);
		return TRUE;
	}
	
	return CBaseDialog::OnSetCursor(pWnd, nHitTest, message);
}


void CPreviewDialog::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);
	CRect rcWnd;
	GetWindowRect(rcWnd);
	RepositionByTotal();
}

void CPreviewDialog::OnSizing(UINT fwSide, LPRECT pRect)
{
	__super::OnSizing(fwSide, pRect);
	HandleSizing(m_Image.m_szPrv, fwSide, pRect);
}

// PreviewDialog.cpp : implementation file
//

#include "stdafx.h"
#include "CCam.h"
#include "PreviewDialog.h"
#include "StdProfile.h"
#include "CCamUtils.h"
#include "CCamHelp.h"

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

static int IDDByFlags(DWORD dwFlags)
{
	BOOL bRight = CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("RightControls"), FALSE, true, true);
	BOOL bAvi = (dwFlags&CPreviewDialog::ForAvi)?TRUE:FALSE;
	if (bAvi)
		return IDD_PREVIEW_VIDEO;
	else if (bRight)
		return IDD_PREVIEW1;
	else
		return IDD_PREVIEW;
};

CPreviewDialog::CPreviewDialog(IUnknown *punk, int nDevice, DWORD dwFlags, CWnd* pParent /*=NULL*/)
	: CBaseDialog(IDDByFlags(dwFlags), pParent, NULL), m_Image(punk, nDevice),
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
	m_dwFlags = dwFlags;
	if (m_dwFlags&ForAvi)
		m_bVideo = true;
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
	ON_BN_CLICKED(IDC_COLOR_BALANCE, OnColorBalance)
	ON_BN_CLICKED(IDC_FRAME, OnFrame)
	ON_BN_CLICKED(IDC_IMAGE_PAGE, OnImagePage)
	ON_BN_CLICKED(IDC_CAMERA, OnBnClickedCamera)
	ON_BN_CLICKED(IDC_VIDEO, OnVideo)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
	ON_WM_SIZE()
	ON_WM_SIZING()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_APP+1, OnDelayedRepaint)
	ON_COMMAND(ID_HELP, OnHelp)
END_MESSAGE_MAP()

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

void CPreviewDialog::InitDlgSize()
{
	if (m_nDlgSize == 0)
	{
		_test_size(IDD_CAMERA, m_nDlgSize);
		if (CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("ColorCamera"), TRUE, true, true))
		{
			_test_size(IDD_COLOR_BALANCE, m_nDlgSize);
			_test_size(IDD_LEVEL_ADJUSTMENT, m_nDlgSize);
		}
		_test_size(IDD_IMAGE, m_nDlgSize);
		_test_size(IDD_FRAME, m_nDlgSize);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPreviewDialog message handlers

BOOL CPreviewDialog::OnInitDialog() 
{
	CBaseDialog::OnInitDialog();
	// begin preview	
	m_sptrPrv->BeginPreview(m_nDev, this);
	m_bInitOk = true;//
	CRepositionManager::Init(m_hWnd, IDC_IMAGE, CSize(0,0));
	InitDlgSize();
	CRepositionManager::m_rcMinMargins.left = m_nDlgSize+4;
	if (m_bPreview)
		m_Image.InitSizes(m_bInitOk);
	OnLayoutChanged();
	SetMaximalSize(m_Image.m_szPrv);
	InitControlsState();
	CVideoHookImpl::Init(m_hWnd, IDC_STATIC_VIDEO_STAGE, IDC_PROGRESS_VIDEO_STAGE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPreviewDialog::AddDialog(CBaseDialog *p, int idd)
{
	if (m_pDialog)
	{
		RemoveTempControl(m_pDialog->GetSafeHwnd());
		m_pDialog->DestroyWindow();
		delete m_pDialog;
		m_nIDHelp = IDD_PREVIEW;
	}
	m_pDialog = p;
	if (m_pDialog)
	{
		m_pDialog->m_sptrDrv = m_sptrDrv;
		m_pDialog->m_sptrPrv = m_sptrPrv;
		m_pDialog->m_nDev = m_nDev;
		m_pDialog->m_bVideo = m_bVideo;
		InitDlgSize();
		m_pDialog->Create(idd, this);
		CRect rcWnd;
		m_pDialog->GetWindowRect(rcWnd);
		m_pDialog->SetWindowPos(0, 0, 0, m_nDlgSize, rcWnd.Height(), SWP_NOMOVE|SWP_NOZORDER);
		m_pDialog->ShowWindow(SW_SHOW);
		AddTempControl(m_pDialog->GetSafeHwnd(), 0, 0);
		m_nIDHelp = idd;
	}
	OnLayoutChanged();
}

void CPreviewDialog::SetChoice(int id)
{
	CheckRadioButton(IDC_CAMERA, IDC_VIDEO, id);
}

void CPreviewDialog::OnBnClickedCamera()
{
	int n = IsDlgButtonChecked(IDC_CAMERA);
	if (n)
	{
		SetChoice(-1);
		AddDialog(NULL, 0);
	}
	else
	{
		SetChoice(IDC_CAMERA);
		if (CStdProfileManager::m_pMgr->GetProfileInt(_T("Dialogs"), _T("Camera"), FALSE, true, true))
			AddDialog(new CBaseDialog(IDD_CAMERA_EX,this,this), IDD_CAMERA_EX);
		else
			AddDialog(new CBaseDialog(IDD_CAMERA,this,this), IDD_CAMERA);
	}
}

void CPreviewDialog::OnColorBalance() 
{
	int n = IsDlgButtonChecked(IDC_COLOR_BALANCE);
	if (n)
	{
		SetChoice(-1);
		AddDialog(NULL, 0);
	}
	else
	{
		SetChoice(IDC_COLOR_BALANCE);
		if (CStdProfileManager::m_pMgr->GetProfileInt(_T("Dialogs"), _T("ColorBalance"), FALSE, true, true)) 
			AddDialog(new CBaseDialog(IDD_LEVEL_ADJUSTMENT,this,this), IDD_LEVEL_ADJUSTMENT);
		else 
			AddDialog(new CBaseDialog(IDD_COLOR_BALANCE,this,this), IDD_COLOR_BALANCE);
	}
}

void CPreviewDialog::OnFrame() 
{
	int n = IsDlgButtonChecked(IDC_FRAME);
	if (n)
	{
		SetChoice(-1);
		AddDialog(NULL, 0);
	}
	else
	{
		SetChoice(IDC_FRAME);
		AddDialog(new CBaseDialog(IDD_FRAME,this,this), IDD_FRAME);
	}
}

void CPreviewDialog::OnImagePage() 
{
	int n = IsDlgButtonChecked(IDC_IMAGE_PAGE);
	if (n)
	{
		SetChoice(-1);
		AddDialog(NULL, 0);
	}
	else
	{
		SetChoice(IDC_IMAGE_PAGE);
		AddDialog(new CBaseDialog(IDD_IMAGE,this,this), IDD_IMAGE);
	}
}

void CPreviewDialog::OnVideo() 
{
	int n = IsDlgButtonChecked(IDC_VIDEO);
	if (n)
	{
		SetChoice(-1);
		AddDialog(NULL, 0);
	}
	else
	{
		SetChoice(IDC_VIDEO);
		if (CStdProfileManager::m_pMgr->GetProfileInt(_T("Dialogs"), _T("Video"), FALSE, true, true)) 
			AddDialog(new CBaseDialog(IDD_VIDEO,this,this), IDD_VIDEO_EX);
		else
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
	if (m_dwFlags&ForAvi)
		m_sptrDrv->InputVideoFile2(m_nDev, 0, this);
	CBaseDialog::OnOK();
}


void CPreviewDialog::OnDestroy() 
{
//	if (m_bInitOk && m_bPreview) ::SaveSettings();
	m_sptrPrv->EndPreview(m_nDev, this);
	CVideoHookImpl::Deinit();
	CBaseDialog::OnDestroy();
}

HRESULT CPreviewDialog::Invalidate()
{
	if (m_bPreview) m_Image.Invalidate();
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

void CPreviewDialog::OnHelp() 
{
//	if (!RunHelpTopic(_T("Preview")))
		__super::OnHelp();
}




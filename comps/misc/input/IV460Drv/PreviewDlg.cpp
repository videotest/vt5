// PreviewDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "StdProfile.h"
#include "PreviewDlg.h"
//#include "SettingsSheet.h"
//#include "BaumerHelp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define g_IV460Profile (*CStdProfileManager::m_pMgr)

#define MAX_OVR_X 640
#define MAX_OVR_Y 480

class CLock
{
public:
	bool *m_pbLockVar;
	bool m_bPrev;
	CLock(bool *pbLockVar)
	{
		m_bPrev = *pbLockVar;
		m_pbLockVar = pbLockVar;
		*pbLockVar = true;
	};
	~CLock()
	{
		*m_pbLockVar = m_bPrev;
	}
};



/*
#define MAX_OVR_X GetMaxOvrX()
#define MAX_OVR_Y GetMaxOvrY()

CRITICAL_SECTION g_CritSectionInv;

int GetMaxOvrX()
{
	if (g_CxLibWork.m_nCamera==CX05||g_CxLibWork.m_nCamera==CX13||g_CxLibWork.m_nCamera==MX05||g_CxLibWork.m_nCamera==MX13)
		return 640;
	else
		return 400;
}

int GetMaxOvrY()
{
	if (g_CxLibWork.m_nCamera==CX05||g_CxLibWork.m_nCamera==CX13||g_CxLibWork.m_nCamera==MX05||g_CxLibWork.m_nCamera==MX13)
		return 510;
	else
		return 340;
}
*/

static bool s_bOnSetWindowPos = false;

static BOOL _SetWindowPos(CWnd *pSet, const CWnd *pWndInsertAfter, int x, int y, int cx, int cy, UINT nFlags)
{
	CLock lock(&s_bOnSetWindowPos);
	return pSet->SetWindowPos(pWndInsertAfter,x,y,cx,cy,nFlags);
}

static CRect CalcProportionalRect(CRect rcImage, int cxImage, int cyImage)
{
	double dXYRatio1 = double(rcImage.Width())/rcImage.Height();
	double dXYRatio2 = double(cxImage)/cyImage;
	if (dXYRatio1 > dXYRatio2)
		rcImage.right = rcImage.left+int(rcImage.Height()*dXYRatio2);
	else
		rcImage.bottom = rcImage.top+int(rcImage.Width()/dXYRatio2);
	return rcImage;
}

/////////////////////////////////////////////////////////////////////////////
// CPreviewDlg dialog


CPreviewDlg::CPreviewDlg(IUnknown *punk, CWnd* pParent /*=NULL*/)
	: CDialog(CPreviewDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPreviewDlg)
	//}}AFX_DATA_INIT
	m_pPreview = NULL;
	m_pOverlay = NULL;
	m_pDriver = NULL;
	punk->QueryInterface(IID_IInputPreview, (void **)&m_pPreview);
	punk->QueryInterface(IID_IInputWndOverlay, (void **)&m_pOverlay);
	punk->QueryInterface(IID_IDriver, (void **)&m_pDriver);
	if (!m_pPreview && !m_pOverlay || !m_pDriver)
	{
		if (m_pPreview) m_pPreview->Release();
		if (m_pOverlay) m_pOverlay->Release();
		if (m_pDriver) m_pDriver->Release();
		AfxThrowNotSupportedException();
	}
	m_bInited = false;
	m_bOvrMode = false;
	m_bOvrActive = false;
	m_bBoardOK = false;
	m_bUseOverlay = g_IV460Profile.GetProfileInt(_T("Settings"), _T("UseOverlay"), TRUE, true)?true:false;
	m_bUseStretch = g_IV460Profile.GetProfileInt(_T("Settings"), _T("UseStretch"), 1, true)?true:false;
	m_pDialogSite = NULL;
	m_pRedrawHook = NULL;
	m_bNaturalSize = g_IV460Profile.GetProfileInt(_T("PreviewDlg"), _T("NaturalSize"), FALSE, true);
	m_szNaturalSize = CSize(-1,-1);
}

CPreviewDlg::~CPreviewDlg()
{
	if (m_pPreview)
		m_pPreview->Release();
	if (m_pOverlay)
		m_pOverlay->Release();
	if (m_pDriver)
		m_pDriver->Release();
}

void CPreviewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPreviewDlg)
	DDX_Control(pDX, ID_HELP, m_Help);
	DDX_Control(pDX, IDC_REFRESH_TIME, m_Time);
	DDX_Control(pDX, IDC_IMAGE, m_Image);
	DDX_Control(pDX, IDCANCEL, m_Cancel);
	DDX_Control(pDX, IDC_SETTINGS, m_Settings);
	DDX_Control(pDX, IDOK, m_OK);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPreviewDlg, CDialog)
	//{{AFX_MSG_MAP(CPreviewDlg)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_SETTINGS, OnSettings)
	ON_WM_SIZE()
	ON_WM_MOVE()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
	ON_WM_ACTIVATE()
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZING()
	ON_MESSAGE(WM_USER+10, OnUserMsg)
//	ON_COMMAND(ID_HELP, CPreviewDlg::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreviewDlg message handlers

void CPreviewDlg::InitButtons()
{
	if (!m_pDialogSite) return;
	int nButtons = m_pDialogSite->GetButtonsNumber();
	m_arrButtons.SetSize(nButtons);
	m_arrBtnRects.SetSize(nButtons);
	for (int i = 0; i < nButtons; i++)
	{
		CButtonInfo *pButtonInfo = &m_arrButtons.ElementAt(i);
		m_pDialogSite->GetButtonInfo(i, pButtonInfo);
	}
}

BOOL CPreviewDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// 1. Obtain Buttons list
	InitButtons();
	// 2. First, obtain sizes of image
	bool bBeginPreviewCalled = false; // If board does not supports overlay at all, preview will be used.
	short cxImg = 600,cyImg = 400;
	HRESULT hr = m_pOverlay?m_pOverlay->InitOverlay():S_FALSE;
	if (hr == S_OK)
	{
		m_pOverlay->GetSize(&cxImg, &cyImg);
		m_bBoardOK = true;
	}
	else if (m_pPreview)
	{
		m_bUseOverlay = false;
		bBeginPreviewCalled = true;
		hr = m_pPreview->BeginPreview(this);
		if (hr == S_OK)
		{
			m_pPreview->GetSize(&cxImg, &cyImg);
			m_bBoardOK = true;
		}
	}
	m_Image.m_szImg = CSize(cxImg, cyImg);
	/*if (m_bUseOverlay && m_bUseStretch)
	{
		cxImg = min(cxImg, MAX_OVR_X);
		cyImg = min(cyImg, MAX_OVR_Y);
	}*/
	// 3. Second, initialize dialog and controls
	InitLayout();
	int cxScreen = ::GetSystemMetrics(SM_CXFULLSCREEN);
	int cyScreen = ::GetSystemMetrics(SM_CYFULLSCREEN);
	int nSavedX1 = g_IV460Profile.GetProfileInt(_T("PreviewDlg"), _T("SavedX1"), -1);
	int nSavedY1 = g_IV460Profile.GetProfileInt(_T("PreviewDlg"), _T("SavedY1"), -1);
	int nSavedX2 = g_IV460Profile.GetProfileInt(_T("PreviewDlg"), _T("SavedX2"), -1);
	int nSavedY2 = g_IV460Profile.GetProfileInt(_T("PreviewDlg"), _T("SavedY2"), -1);
	if (nSavedX1 != -1 && nSavedY1 != -1)
	{
		m_rcTotal = CRect(min(nSavedX1,cxScreen-20), min(nSavedY1,cyScreen-20), 0, 0);
		if (m_bNaturalSize || nSavedX2 == -1)
		{
			_SetWindowPos(this, NULL, m_rcTotal.left, m_rcTotal.top, 0, 0, SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOSIZE);
			SetLayoutByImageSize();
		}
		else
		{
			CSize sz = CalcSizesByCxWindow(nSavedX2-nSavedX1);
			_SetWindowPos(this, NULL, m_rcTotal.left, m_rcTotal.top, sz.cx, sz.cy, SWP_NOACTIVATE|SWP_NOZORDER);
			SetLayoutByClientSize();
		}
	}
	else
		SetLayoutByImageSize();
	// 4. Third, initialize preview
	if (m_bBoardOK)
	{
		if (m_bUseOverlay)
		{
			HWND hwnd = ::GetDlgItem(m_hWnd, IDC_IMAGE);
			::EnableWindow(hwnd, FALSE);
			CRect rcDst;
			::GetWindowRect(hwnd, rcDst);
			ScreenToClient(rcDst);
			CRect rcSrc;
			if (m_bUseStretch)
				rcSrc = CRect(0,0,cxImg,cyImg);
			else
				rcSrc = rcDst;
			hr = m_pOverlay->StartOverlay(m_hWnd, rcSrc, rcDst);
			if (hr == S_OK)
			{
				m_bOvrMode = true;
				m_bOvrActive = true;
			}
			else
				::EnableWindow(hwnd, TRUE);
		}
		// If not successful use preview.
		if (!m_bOvrMode)
		{
			bool bPreviewSucceeded;
			if (!bBeginPreviewCalled) // BeginPreview not called
			{
				m_pOverlay->DeinitOverlay();
				if (m_pPreview)
					bPreviewSucceeded = SUCCEEDED(m_pPreview->BeginPreview(this));
				else
					bPreviewSucceeded = false;
			}
			else
				bPreviewSucceeded = true;
			if (bPreviewSucceeded)
			{
				DWORD dwFreq;
				if (SUCCEEDED(m_pPreview->GetPreviewFreq(&dwFreq)) && dwFreq > 0)
					SetTimer(1, dwFreq, NULL);
				m_Image.SetPreview(m_pPreview);// init preview
			}
			m_bBoardOK = bPreviewSucceeded;
		}
	}
	// 5. Set dialog title
	BSTR bstrTitle;
	m_pDriver->GetDeviceNames(0, NULL, &bstrTitle, NULL);
	CString sTitle(bstrTitle);
	::SysFreeString(bstrTitle);
	SetWindowText(sTitle);
	m_bInited = true;
//	::ShowWindow(::GetDlgItem(m_hWnd, ID_HELP), SW_SHOW);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPreviewDlg::ClosePreview()
{
	if (m_bBoardOK)
	{
		m_bBoardOK = false;
		if (m_bOvrMode)
		{
			m_pOverlay->StopOverlay(NULL);
			m_pOverlay->DeinitOverlay();
		}
		else
		{
			m_pPreview->EndPreview(this);
		}
	}
}

void CPreviewDlg::OnTimer(UINT_PTR nIDEvent) 
{
	if (!m_bOvrMode) Invalidate();
	CDialog::OnTimer(nIDEvent);
}

void CPreviewDlg::InitLayout()
{
	CRect rc;
	m_Image.GetWindowRect(rc);
	ScreenToClient(rc);
	m_nGap = rc.left;
	m_OK.GetWindowRect(rc);
	m_szButton = rc.Size();
}

CSize CPreviewDlg::CalcSizesByCxWindow(int cxWindow)
{
	// Obtain system metrics values
	int cxScreen = ::GetSystemMetrics(SM_CXFULLSCREEN);
	int cyScreen = ::GetSystemMetrics(SM_CYFULLSCREEN);
	int cyCaption = ::GetSystemMetrics(SM_CYCAPTION);
	int cxDlgFrame = ::GetSystemMetrics(SM_CXDLGFRAME);
	int cyDlgFrame = ::GetSystemMetrics(SM_CYDLGFRAME);
	// obtain sizeos of image
	short cxRealImg = 0,cyRealImg = 0;
	m_pPreview->GetSize(&cxRealImg, &cyRealImg);
	// Check for cxWindow too large
	if (cxWindow > cxScreen) cxWindow = cxScreen;
	// Calc cyWindow
	int cxImage = cxWindow-2*cxDlgFrame-3*m_nGap-m_szButton.cx;
	int cyImage = cxImage*cyRealImg/cxRealImg;
	int nButtons = 4+m_arrBtnRects.GetSize();
	cyImage = max(cyImage, nButtons*m_szButton.cy+(nButtons-1)*m_nGap);
	int cyWindow = cyImage+2*m_nGap+cyCaption+2*cyDlgFrame;
	if (cyWindow > cyScreen)
		return CalcSizesByCyWindow(cyScreen);
	else return CSize(cxWindow,cyWindow);
}

CSize CPreviewDlg::CalcSizesByCyWindow(int cyWindow)
{
	// Obtain system metrics values
	int cxScreen = ::GetSystemMetrics(SM_CXFULLSCREEN);
	int cyScreen = ::GetSystemMetrics(SM_CYFULLSCREEN);
	int cyCaption = ::GetSystemMetrics(SM_CYCAPTION);
	int cxDlgFrame = ::GetSystemMetrics(SM_CXDLGFRAME);
	int cyDlgFrame = ::GetSystemMetrics(SM_CYDLGFRAME);
	// obtain sizeos of image
	short cxRealImg = 0,cyRealImg = 0;
	m_pPreview->GetSize(&cxRealImg, &cyRealImg);
	// Check for cxWindow too large
	if (cyWindow > cyScreen) cyWindow = cyScreen;
	// Calc cyWindow
	int cyImage = cyWindow-2*cxDlgFrame-cyCaption-2*m_nGap;
	int cxImage = cyImage*cxRealImg/cyRealImg;
	int cxWindow = cxImage+3*m_nGap+m_szButton.cx+2*cyDlgFrame;
	if (cxWindow > cxScreen)
		return CalcSizesByCxWindow(cxScreen);
	else return CSize(cxWindow,cyWindow);
}

void CPreviewDlg::RecalcLayoutByImageSize(int cx, int cy)
{
	CRect rcOK, rcCancel, rcSettings, rcTotal;
	m_rcImage = CRect(m_nGap, m_nGap, m_nGap+cx, m_nGap+cy);
	m_rcOK = CRect(m_rcImage.right+m_nGap, m_rcImage.top, m_rcImage.right+m_nGap+m_szButton.cx, m_rcImage.top+m_szButton.cy);
	m_rcCancel = CRect(m_rcOK.left, m_rcOK.bottom+m_nGap, m_rcOK.right, m_rcOK.bottom+m_nGap+m_szButton.cy);
	CRect rcPrev(m_rcCancel);
	for (int i = 0; i < m_arrBtnRects.GetSize(); i++)
	{
		rcPrev = CRect(rcPrev.left, rcPrev.bottom+m_nGap, rcPrev.right, rcPrev.bottom+m_nGap+m_szButton.cy);
		m_arrBtnRects[i] = rcPrev;
	}
//	m_rcSettings = CRect(m_rcCancel.left, m_rcCancel.bottom+m_nGap, m_rcCancel.right, m_rcCancel.bottom+m_nGap+m_szButton.cy);
//	m_rcHelp = CRect(m_rcSettings.left, m_rcSettings.bottom+m_nGap, m_rcSettings.right, m_rcSettings.bottom+m_nGap+m_szButton.cy);
	m_rcHelp = CRect(rcPrev.left, rcPrev.bottom+m_nGap, rcPrev.right, rcPrev.bottom+m_nGap+m_szButton.cy);
	m_rcRTime = CRect(m_rcHelp.left, m_rcHelp.bottom+m_nGap, m_rcHelp.right, m_rcHelp.bottom+m_nGap+m_szButton.cy);
	m_rcTotal = CRect(0, 0, m_rcImage.right+3*m_nGap+m_szButton.cx, max(m_rcImage.bottom+2*m_nGap,6*m_nGap+5*m_szButton.cx));
	CalcWindowRect(m_rcTotal);
}

void CPreviewDlg::RecalcLayoutByClientSize(int cxTotal, int cyTotal, int cxImage, int cyImage)
{
	CRect rcOK, rcCancel, rcSettings, rcTotal;
	m_rcOK = CRect(cxTotal-m_szButton.cx-m_nGap, m_nGap, cxTotal-m_nGap, m_nGap+m_szButton.cy);
	m_rcCancel = CRect(m_rcOK.left, m_rcOK.bottom+m_nGap, m_rcOK.right, m_rcOK.bottom+m_nGap+m_szButton.cy);
	CRect rcPrev(m_rcCancel);
	for (int i = 0; i < m_arrBtnRects.GetSize(); i++)
	{
		rcPrev = CRect(rcPrev.left, rcPrev.bottom+m_nGap, rcPrev.right, rcPrev.bottom+m_nGap+m_szButton.cy);
		m_arrBtnRects[i] = rcPrev;
	}
//	m_rcSettings = CRect(m_rcCancel.left, m_rcCancel.bottom+m_nGap, m_rcCancel.right, m_rcCancel.bottom+m_nGap+m_szButton.cy);
//	m_rcHelp = CRect(m_rcSettings.left, m_rcSettings.bottom+m_nGap, m_rcSettings.right, m_rcSettings.bottom+m_nGap+m_szButton.cy);
	m_rcHelp = CRect(rcPrev.left, rcPrev.bottom+m_nGap, rcPrev.right, rcPrev.bottom+m_nGap+m_szButton.cy);
	m_rcRTime = CRect(m_rcHelp.left, m_rcHelp.bottom+m_nGap, m_rcHelp.right, m_rcHelp.bottom+m_nGap+m_szButton.cy);
	m_rcImage = CRect(m_nGap, m_nGap, cxTotal-m_szButton.cx-2*m_nGap, cyTotal-m_nGap);
	if (!m_bUseOverlay || m_bUseStretch)
		m_rcImage = CalcProportionalRect(m_rcImage, cxImage, cyImage);
}

void CPreviewDlg::SetLayout(bool bSetTotal)
{
	m_Image.SetWindowPos(NULL, m_rcImage.left, m_rcImage.top, m_rcImage.Width(), m_rcImage.Height(),
		SWP_NOACTIVATE|SWP_NOZORDER);
	m_OK.SetWindowPos(NULL, m_rcOK.left, m_rcOK.top, m_rcOK.Width(), m_rcOK.Height(), SWP_NOACTIVATE|SWP_NOZORDER);
	m_Cancel.SetWindowPos(NULL, m_rcCancel.left, m_rcCancel.top, m_rcCancel.Width(), m_rcCancel.Height(),
		SWP_NOACTIVATE|SWP_NOZORDER);
	for (int i = 0; i < m_arrBtnRects.GetSize(); i++)
	{
		CRect rc(m_arrBtnRects[i]);
		CWnd *pWnd = GetDlgItem(m_arrButtons[i].m_nButtonId);
		if (pWnd)
			pWnd->SetWindowPos(NULL, rc.left, rc.top, rc.Width(), rc.Height(),	SWP_NOACTIVATE|SWP_NOZORDER);
	}
//	m_Settings.SetWindowPos(NULL, m_rcSettings.left, m_rcSettings.top, m_rcSettings.Width(), m_rcSettings.Height(),
//		SWP_NOACTIVATE|SWP_NOZORDER);
	m_Help.SetWindowPos(NULL, m_rcHelp.left, m_rcHelp.top, m_rcHelp.Width(), m_rcHelp.Height(),
		SWP_NOACTIVATE|SWP_NOZORDER);
	m_Time.SetWindowPos(NULL, m_rcRTime.left, m_rcRTime.top, m_rcRTime.Width(), m_rcRTime.Height(),
		SWP_NOACTIVATE|SWP_NOZORDER);
	if (bSetTotal)
		_SetWindowPos(this, NULL, 0, 0, m_rcTotal.Width(), m_rcTotal.Height(), SWP_NOMOVE|SWP_NOACTIVATE|SWP_NOZORDER);
}

void CPreviewDlg::SetLayoutByClientSize(int cx, int cy)
{
	short cxImg = 0,cyImg = 0;
	m_pPreview->GetSize(&cxImg, &cyImg);
	m_Image.m_szImg = CSize(cxImg, cyImg);
	m_rcTotal = CRect(0,0,cx,cy);
	CalcWindowRect(m_rcTotal);
	RecalcLayoutByClientSize(cx, cy, cxImg, cyImg);
	/*if (m_bUseOverlay && m_bUseStretch)
	{
		cxImg = min(cxImg, MAX_OVR_X);
		cyImg = min(cyImg, MAX_OVR_Y);
		if (m_rcImage.Width() >= cxImg || m_rcImage.Height() >= cyImg)
			RecalcLayoutByImageSize(cxImg, cyImg);
	}*/
	SetLayout(false);
}

void CPreviewDlg::SetLayoutByClientSize()
{
	CRect rc;
	GetClientRect(rc);
	SetLayoutByClientSize(rc.Width(), rc.Height());
}

void CPreviewDlg::SetLayoutByImageSize()
{
	short cxImg = 0,cyImg = 0;
	m_pPreview->GetSize(&cxImg, &cyImg);
	RecalcLayoutByImageSize(cxImg, cyImg);
	int cxScreen = ::GetSystemMetrics(SM_CXFULLSCREEN);
	int cyScreen = ::GetSystemMetrics(SM_CYFULLSCREEN);
	if (m_rcTotal.Width() > cxScreen || m_rcTotal.Height() > cyScreen)
	{
		CSize sz = CalcSizesByCxWindow(cxScreen);
		m_rcTotal = CRect(0,0,sz.cx,sz.cy);
		m_szNaturalSize = CSize(-1, -1);
		_SetWindowPos(this, NULL, 0, 0, m_rcTotal.Width(), m_rcTotal.Height(), SWP_NOMOVE|SWP_NOACTIVATE|SWP_NOZORDER);
		m_szNaturalSize = CSize(m_rcTotal.Width(), m_rcTotal.Height());
		SetLayoutByClientSize();
	}
	else
	{
		m_szNaturalSize = CSize(-1, -1);
		SetLayout(true);
		m_szNaturalSize = CSize(m_rcTotal.Width(), m_rcTotal.Height());
	}
}


HRESULT CPreviewDlg::Invalidate()
{
	if (m_bBoardOK && !m_bOvrMode)
	{
		m_Image.Invalidate();
		if (m_pRedrawHook) m_pRedrawHook->OnRedraw();
		PostMessage(WM_USER+10);
	}
	return S_OK;
}

HRESULT CPreviewDlg::OnChangeSize()
{
	if (m_bInited)
	{
		if (m_bNaturalSize)
			SetLayoutByImageSize();
		else
		{
			CRect rcWindow;
			GetWindowRect(rcWindow);
			CSize sz = CalcSizesByCxWindow(rcWindow.Width());
			_SetWindowPos(this, NULL, 0, 0, sz.cx, sz.cy, SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOMOVE);
			SetLayoutByClientSize();
		}
		if (m_bOvrMode) m_pOverlay->SetDstRect(m_hWnd, m_rcImage);
	}
	return S_OK;
}

LRESULT CPreviewDlg::OnUserMsg(WPARAM wParam, LPARAM lParam)
{
	if (wParam == 1)
	{ // For overlay mode
		if (m_bInited)
		{
			CRect rc;
			GetClientRect(rc);
			int cx = rc.Width();
			int cy = rc.Height();
			short cxImg = 0,cyImg = 0;
			m_pPreview->GetSize(&cxImg, &cyImg);
			m_rcTotal = CRect(0,0,cx,cy);
			RecalcLayoutByClientSize(cx, cy, cxImg, cyImg);
			if (m_bUseOverlay && m_bUseStretch)
			{
				cxImg = min(cxImg, MAX_OVR_X);
				cyImg = min(cyImg, MAX_OVR_Y);
			}
			if (m_rcImage.Width() >= cxImg || m_rcImage.Height() >= cyImg)
				RecalcLayoutByImageSize(cxImg, cyImg);
			SetLayout(false);
			if (m_bOvrMode)
				m_pOverlay->SetDstRect(m_hWnd, m_rcImage);
			CRect rcWnd;
			GetWindowRect(rcWnd);
			g_IV460Profile.WriteProfileInt(_T("PreviewDlg"), _T("SavedX1"), rcWnd.left);
			g_IV460Profile.WriteProfileInt(_T("PreviewDlg"), _T("SavedY1"), rcWnd.top);
			g_IV460Profile.WriteProfileInt(_T("PreviewDlg"), _T("SavedX2"), rcWnd.left+cx);
			g_IV460Profile.WriteProfileInt(_T("PreviewDlg"), _T("SavedY2"), rcWnd.top+cy);
		}
	}
	else if (wParam == 2)
		OnChangeSize();
	else
	{
		CString s;
//		s.Format(IDS_TIME_MESSAGE, g_CxLibWork.m_dwTickCount);
		m_Time.SetWindowText(s);
		if (m_pRedrawHook) m_pRedrawHook->OnDelayedRedraw();
	}
	return 0;
}

void CPreviewDlg::OnSettings() 
{
	/*
	CSettingsSheet dlg(this);
	dlg.m_bOvrMode = m_bOvrMode;
	m_pRedrawHook = &dlg;
	dlg.DoModal();
	m_pRedrawHook = NULL;
	*/
}

void CPreviewDlg::OnChangeOvrMode(bool bUseOvr)
{
	if (m_bUseOverlay != bUseOvr)
	{
		// Close previous operation
		if (m_bUseOverlay)
		{
			m_pOverlay->StopOverlay(NULL);
			m_pOverlay->DeinitOverlay();
			short cxImg = 0,cyImg = 0;
			CRect rcClient;
			GetClientRect(rcClient);
			m_pPreview->GetSize(&cxImg, &cyImg);
			m_rcTotal = CRect(0,0,rcClient.Width(),rcClient.Height());
			RecalcLayoutByClientSize(rcClient.Width(), rcClient.Height(), cxImg, cyImg);
			if (m_rcImage.Width() >= cxImg || m_rcImage.Height() >= cyImg)
				RecalcLayoutByImageSize(cxImg, cyImg);
			SetLayout(false);
		}
		else
		{
			m_pPreview->EndPreview(this);
			if (m_rcImage.Width() > MAX_OVR_X || m_rcImage.Height() > MAX_OVR_Y)
			{
				RecalcLayoutByImageSize(MAX_OVR_X, MAX_OVR_Y);
				SetLayout(true);
			}
		}
		m_bUseOverlay = bUseOvr?true:false;
		m_bOvrMode = false;
		// Initialize new
		if (m_bUseOverlay)
		{
			short cxImg = 0,cyImg = 0;
			m_pPreview->GetSize(&cxImg, &cyImg);
			m_Image.m_szImg = CSize(cxImg, cyImg);
			CRect rcDst;
			m_Image.GetWindowRect(rcDst);
			ScreenToClient(rcDst);
			CRect rcSrc;
			if (m_bUseStretch)
				rcSrc = CRect(0,0,cxImg,cyImg);
			else
				rcSrc = rcDst;
			HRESULT hr;
			hr = m_pOverlay->InitOverlay();
			if (hr == S_OK)
			{
				hr = m_pOverlay->StartOverlay(m_hWnd, rcSrc, rcDst);
				if (hr == S_OK)
				{
//					m_pOverlay->SetDstRect(m_hWnd, m_rcImage);
					PostMessage(WM_USER+10,1);
					m_bOvrMode = true;
					m_bOvrActive = true;
					m_Time.SetWindowText("");
				}
				else
					m_pOverlay->DeinitOverlay();
			}
		}
		// If not successful use preview.
		if (!m_bOvrMode)
		{
			m_pPreview->BeginPreview(this);
			m_Image.SetPreview(m_pPreview);
		}
	}
	else if (m_bOvrMode)
		m_pOverlay->SetDstRect(m_hWnd, m_rcImage);
}

void CPreviewDlg::OnChangeNaturalSize(bool bNaturalSize)
{
	m_bNaturalSize = bNaturalSize?true:false;
	if (m_bNaturalSize)
	{
		SetLayoutByImageSize();
		m_szNaturalSize = CSize(m_rcTotal.Width(), m_rcTotal.Height());
	}
	else
	{
		int nSavedX1 = g_IV460Profile.GetProfileInt(_T("PreviewDlg"), _T("SavedX1"), -1);
		int nSavedY1 = g_IV460Profile.GetProfileInt(_T("PreviewDlg"), _T("SavedY1"), -1);
		int nSavedX2 = g_IV460Profile.GetProfileInt(_T("PreviewDlg"), _T("SavedX2"), -1);
		int nSavedY2 = g_IV460Profile.GetProfileInt(_T("PreviewDlg"), _T("SavedY2"), -1);
		if (nSavedX1 != -1 && nSavedY1 != -1 && nSavedX2 != -1 && nSavedY2 != -1 )
		{
			CSize sz = CalcSizesByCxWindow(nSavedX2-nSavedX1);
			_SetWindowPos(this, NULL, 0, 0, sz.cx, sz.cy, SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOMOVE);
		}
		SetLayoutByClientSize();
	}
}


void CPreviewDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	if (m_bInited && !s_bOnSetWindowPos)
	{
		SetLayoutByClientSize(cx,cy);
		if (m_bOvrMode)
			m_pOverlay->SetDstRect(m_hWnd, m_rcImage);
		CRect rcWnd;
		GetWindowRect(rcWnd);
		CRect rc(0,0,cx,cy);
		CalcWindowRect(rc);
		g_IV460Profile.WriteProfileInt(_T("PreviewDlg"), _T("SavedX1"), rcWnd.left);
		g_IV460Profile.WriteProfileInt(_T("PreviewDlg"), _T("SavedY1"), rcWnd.top);
		g_IV460Profile.WriteProfileInt(_T("PreviewDlg"), _T("SavedX2"), rcWnd.left+rc.Width());
		g_IV460Profile.WriteProfileInt(_T("PreviewDlg"), _T("SavedY2"), rcWnd.top+rc.Height());
	}
}

void CPreviewDlg::OnSizing(UINT nSide, LPRECT lpRect)
{
	CDialog::OnSizing(nSide, lpRect);
	if (m_bInited && !s_bOnSetWindowPos)
	{
		CRect rc(*lpRect);
		short cxImg = 0,cyImg = 0;
		m_pPreview->GetSize(&cxImg, &cyImg);
		CSize sz;
		if (nSide == WMSZ_TOP || nSide == WMSZ_BOTTOM)
			sz = CalcSizesByCyWindow(rc.Height());
		else
			sz = CalcSizesByCxWindow(rc.Width());
		if (sz.cx != rc.Width())
		{
			if (nSide == WMSZ_BOTTOMLEFT || nSide == WMSZ_LEFT || nSide == WMSZ_TOPLEFT)
				lpRect->left = lpRect->right-sz.cx;
			else
				lpRect->right = lpRect->left+sz.cx;
		}
		if (sz.cy != rc.Height())
		{
			if (nSide == WMSZ_TOPRIGHT || nSide == WMSZ_TOP || nSide == WMSZ_TOPLEFT)
				lpRect->top = lpRect->bottom-sz.cy;
			else
				lpRect->bottom = lpRect->top+sz.cy;
		}
	}
}


void CPreviewDlg::OnMove(int x, int y) 
{
	CDialog::OnMove(x, y);
	if (m_bInited && !s_bOnSetWindowPos)
	{
		if (m_bOvrMode)
			m_pOverlay->SetDstRect(m_hWnd, m_rcImage);
		CRect rcWnd;
		GetWindowRect(rcWnd);
		g_IV460Profile.WriteProfileInt(_T("PreviewDlg"), _T("SavedX1"), rcWnd.left);
		g_IV460Profile.WriteProfileInt(_T("PreviewDlg"), _T("SavedY1"), rcWnd.top);
		g_IV460Profile.WriteProfileInt(_T("PreviewDlg"), _T("SavedX2"), rcWnd.right);
		g_IV460Profile.WriteProfileInt(_T("PreviewDlg"), _T("SavedY2"), rcWnd.bottom);
	}
}

void CPreviewDlg::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI)
{
	if (m_bNaturalSize && m_szNaturalSize.cx != -1 && m_szNaturalSize.cy != -1)
	{
		CRect rc;
		GetWindowRect(rc);
		lpMMI->ptMinTrackSize.x = lpMMI->ptMaxTrackSize.x = m_szNaturalSize.cx;
		lpMMI->ptMinTrackSize.y = lpMMI->ptMaxTrackSize.y = m_szNaturalSize.cy;
	}
}

void CPreviewDlg::OnActivate( UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	if (m_bOvrMode)
	{
		DWORD dwFlags;
		HRESULT hr = m_pDriver->GetFlags(&dwFlags);
		if (hr != S_OK || !(dwFlags&FG_OVERLAYCOLORKEY))
		{
			if (nState == WA_ACTIVE)
			{
				if (!m_bOvrActive)
				{
					short cxImg = 0,cyImg = 0;
					m_pPreview->GetSize(&cxImg, &cyImg);
					CRect rcSrc(0,0,cxImg,cyImg);
					m_pOverlay->StartOverlay(m_hWnd, rcSrc, m_rcImage);
					m_bOvrActive = true;
				}
			}
			else if (nState == WA_INACTIVE)
			{
				CWnd *pParent = pWndOther?pWndOther->GetParent():NULL;
				if (m_bOvrActive && pParent != this)
				{
					short cxImg = 0,cyImg = 0;
					m_pPreview->GetSize(&cxImg, &cyImg);
					CRect rcSrc(0,0,cxImg,cyImg);
					m_pOverlay->StopOverlay(m_hWnd);
					m_bOvrActive = false;
					InvalidateRect(NULL, FALSE);
				}
			}
		}
	}
	CDialog::OnActivate(nState, pWndOther, bMinimized);
}

void CPreviewDlg::OnDestroy() 
{
	ClosePreview();
	CDialog::OnDestroy();
}

void CPreviewDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_rcImage.PtInRect(point))
	{
		CPoint pt(point.x-m_rcImage.left, point.y-m_rcImage.top);
		m_Image.DoLButtonDown(nFlags, pt);
	}
	CDialog::OnLButtonDown(nFlags, point);
}

void CPreviewDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (m_rcImage.PtInRect(point))
	{
		CPoint pt(point.x-m_rcImage.left, point.y-m_rcImage.top);
		m_Image.DoLButtonUp(nFlags, pt);
	}
	CDialog::OnLButtonUp(nFlags, point);
}

void CPreviewDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	if ((nFlags & MK_LBUTTON) && m_rcImage.PtInRect(point))
	{
		CPoint pt(point.x-m_rcImage.left, point.y-m_rcImage.top);
		m_Image.DoMouseMove(nFlags, pt);
	}
	CDialog::OnMouseMove(nFlags, point);
}

BOOL CPreviewDlg::OnHelpInfo(HELPINFO* pHelpInfo) 
{
//	if (!RunContextHelp(pHelpInfo))
		return CDialog::OnHelpInfo(pHelpInfo);
//	return FALSE;
}

BOOL CPreviewDlg::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	for (int i = 0; i < m_arrButtons.GetSize(); i++)
	{
		if (nID == m_arrButtons[i].m_nButtonId)
		{
			m_pDialogSite->ButtonPress(nID, this, &m_pRedrawHook);
		}
	}
	return CDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

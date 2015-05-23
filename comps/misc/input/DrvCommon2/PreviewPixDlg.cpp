#include "StdAfx.h"
#include "previewpixdlg.h"
#include "StdProfile.h"
#include "CamValues.h"
#include "ImageUtil.h"
#include "BinResource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CCamBoolValue g_Size11(_T("PreviewDialog"), _T("Size11"), true);
CCamBoolValue g_Histo(_T("Histogram"), _T("Show"), false);
CCamBoolValue g_FPSAlways(_T("PreviewDialog"), _T("ShowFPSAlways"), false, 0, -1, CCamValue::IgnoreMethodic|CCamValue::WriteToReg);
CCamBoolValue g_FPS(_T("PreviewDialog"), _T("ShowFPSInsteadPeriod"), true, 0, -1, CCamValue::IgnoreMethodic|CCamValue::WriteToReg);

CPreviewPixDlg::CPreviewPixDlg(int idd, CPreviewIds &PreviewIds, IUnknown *punk, int nDevice,
	DWORD dwFlags, CWnd* pParent) :
	CBaseDlg(idd, pParent, NULL, punk), m_Image(punk, nDevice),
	m_HistoDlg(PreviewIds.m_HistoIds)
{
	m_sptrPrv = punk;
	if (m_sptrPrv == 0)
		AfxThrowNotSupportedException();
	m_sptrDrv = punk;
	m_nDev = nDevice;
	m_pDialog = NULL;
	m_bDelayedInitCtrllSite = true;
	m_bDisableAllCtrls = false;
	m_bPreview = true;
	m_bCaptureVideo = false;
	m_nDlgSize = 0;
	m_dwFlags = dwFlags;
	if (m_dwFlags&ForAvi)
		m_bVideo = true;
	m_PreviewIds = PreviewIds;
	m_idd = idd;
	m_bSubdlgControls = true;
	if (CStdProfileManager::m_pMgr->GetProfileInt(_T("PreviewDialog"), _T("ReserveSpace"), TRUE, true, true))
	{
		LPCTSTR lpszTemplateName = MAKEINTRESOURCE(m_PreviewIds.m_HistoIds.m_nIdDlg);
		HINSTANCE hInst = AfxFindResourceHandle(lpszTemplateName, RT_DIALOG);
		HRSRC hResource = ::FindResource(hInst, lpszTemplateName, RT_DIALOG);
		DWORD dwSize = SizeofResource(hInst, hResource);
		HGLOBAL hTemplate = LoadResource(hInst, hResource);
		LPVOID lpSrc = LockResource(hTemplate);
		_DLGTEMPLATEEX *lpDlgTemplEx = (_DLGTEMPLATEEX*)lpSrc;
		m_nDlgSize = lpDlgTemplEx->cx;
		UnlockResource(hTemplate);
		FreeResource(hTemplate);
/*		CDialog *pdlg = new CDialog;
		pdlg->Create(m_PreviewIds.m_HistoIds.m_nIdDlg);
		CRect rcWnd;
		pdlg->GetWindowRect(rcWnd);
		m_nDlgSize = rcWnd.Width();
		delete pdlg;*/
	}
	m_bProcessDelayedRepaint = false;
}

CPreviewPixDlg::~CPreviewPixDlg(void)
{
	delete m_pDialog;
}

void CPreviewPixDlg::ReserveSpaceForSubdialog(int id)
{
	CDialog *p = new CDialog;
	p->Create(id);
	CRect rcDlg;
	p->GetWindowRect(rcDlg);
	int nWidth = rcDlg.Width();
	if (nWidth > m_nDlgSize)
		m_nDlgSize = nWidth;
	p->DestroyWindow();
	delete p;
}


void CPreviewPixDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDlg::DoDataExchange(pDX);
	if (m_PreviewIds.m_idStaticPeriod != -1)
		DDX_Control(pDX, m_PreviewIds.m_idStaticPeriod, m_StaticPeriod);
	if (m_bPreview && m_PreviewIds.m_idImage != -1)
		DDX_Control(pDX, m_PreviewIds.m_idImage, m_Image);
}

BEGIN_MESSAGE_MAP(CPreviewPixDlg, CBaseDlg)
	//{{AFX_MSG_MAP(CPreviewPixDlg)
	ON_WM_DESTROY()
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

/////////////////////////////////////////////////////////////////////////////
// CPreviewDialog message handlers

BOOL CPreviewPixDlg::OnInitDialog() 
{
	m_sptrPrv->BeginPreview(m_nDev, this);
	m_bInitOk = true;//
	CBaseDlg::OnInitDialog();
	CRepositionManager::Init(m_hWnd, m_PreviewIds.m_idImage, _MaxClientSize(this));
	if (m_nDlgSize == 0 && m_sptrDDS != 0)
		m_sptrDDS->OnInitDlgSize(this);
	if (CStdProfileManager::m_pMgr->GetProfileInt(_T("PreviewDialog"), _T("ReserveSpace"), TRUE, true, true))
		CRepositionManager::m_rcMinMargins.left = m_nDlgSize+4;
	m_Image.ModifyStyle(0, WS_CLIPSIBLINGS|WS_CLIPCHILDREN);
	if (m_bPreview)
		m_Image.InitSizes(m_bInitOk);
	OnLayoutChanged();
	if ((bool)g_Size11)
		SetSizeByPreview(m_Image.m_szPrv);
	else
		SetMaximalSize(m_Image.m_szPrv);
	InitControlsState();
	if (CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("DisableHelp"), 1, true, true))
		::ShowWindow(::GetDlgItem(m_hWnd, ID_HELP), SW_HIDE);
	if (m_PreviewIds.m_idStaticVideoStage != -1 || m_PreviewIds.m_idProgressVideoStage != -1)
		CVideoHookImpl::Init(m_hWnd, m_PreviewIds.m_idStaticVideoStage, m_PreviewIds.m_idProgressVideoStage);
	if (m_dwFlags&ForSettings)
		::ShowWindow(::GetDlgItem(m_hWnd, IDCANCEL), SW_HIDE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

bool CPreviewPixDlg::CanBeZoomed()
{
	int cxFullScreen = GetSystemMetrics(SM_CXFULLSCREEN);
	int cyFullScreen = GetSystemMetrics(SM_CYFULLSCREEN);
	CSize sz = GetMinSize();
	if (sz.cx+m_Image.m_szPrv.cx*2<=cxFullScreen &&
		sz.cy+m_Image.m_szPrv.cy*2<=cyFullScreen)
		return true;
	else
		return false;
}


void CPreviewPixDlg::AddDialog(CBaseDlg *p, int idd)
{
	if (m_pDialog)
	{
		RemoveTempControl(m_pDialog->GetSafeHwnd());
		m_pDialog->DestroyWindow();
		delete m_pDialog;
		m_nIDHelp = m_idd;
	}
	m_pDialog = p;
	if (m_pDialog)
	{
		m_pDialog->m_sptrDrv = m_sptrDrv;
		m_pDialog->m_sptrPrv = m_sptrPrv;
		m_pDialog->m_sptrDDS = m_sptrDDS;
		m_pDialog->m_nDev = m_nDev;
		m_pDialog->m_bVideo = m_bVideo;
		m_pDialog->Create(idd, this);
		CRect rcWnd;
		m_pDialog->GetWindowRect(rcWnd);
		//sergey 28/04/06
		//m_pDialog->SetWindowPos(0, 0, 0, m_nDlgSize, 100, SWP_NOMOVE|SWP_NOZORDER);
		m_pDialog->SetWindowPos(0, 0, 0, m_nDlgSize, rcWnd.Height(), SWP_NOMOVE|SWP_NOZORDER);
		//end
		m_pDialog->ShowWindow(SW_SHOW);
		AddTempControl(m_pDialog->GetSafeHwnd(), 0, 0);
		m_nIDHelp = idd;
	}
	OnLayoutChanged();
}

void CPreviewPixDlg::SetChoice(int id)
{
	CheckRadioButton(m_PreviewIds.m_idFirstButton, m_PreviewIds.m_idLastButton, id);
}

void CPreviewPixDlg::OnCancel() 
{
//	if (m_bInitOk && !m_bPreview) ::RestoreSettings();
	CBaseDlg::OnCancel();
}

void CPreviewPixDlg::OnOK() 
{
//	if (m_bInitOk && !m_bPreview) ::SaveSettings();
	if (m_dwFlags&ForAvi)
	{
		CManageFlag mcv(&m_bCaptureVideo, true);
		m_sptrDrv->InputVideoFile2(m_nDev, 0, this);
	}
	
	
	CBaseDlg::OnOK();
}

void CPreviewPixDlg::OnDestroy() 
{
//	if (m_bInitOk && m_bPreview) ::SaveSettings();
	m_sptrPrv->EndPreview(m_nDev, this);
	CVideoHookImpl::Deinit();
	CBaseDlg::OnDestroy();
}

HRESULT CPreviewPixDlg::Invalidate()
{
	if (m_bPreview) m_Image.Invalidate();
	if (!m_bCaptureVideo)
	{
		bool b = m_bCaptureVideo;
		if ((bool)g_Histo && m_sptrDrv != 0)
		{
			LPBITMAPINFOHEADER lpbi = NULL;
			DWORD dwSize = 0;
			if (m_sptrDrv->GetImage(m_nDev, (void**)&lpbi, &dwSize) == S_OK)
			{
				LPVOID lpData = ((RGBQUAD*)(lpbi+1))+(lpbi->biClrUsed?lpbi->biClrUsed:lpbi->biBitCount==8?256:0);
				m_HistoDlg.m_HistoBox.InitHisto(lpbi, (LPBYTE)lpData);
			}
		}
		
		DelayedRepaint(this);
/*		if (!m_bProcessDelayedRepaint)
		{
			PostMessage(WM_APP+1);
			m_bProcessDelayedRepaint = true;
		}*/
	}
	return S_OK;
}

HRESULT CPreviewPixDlg::OnChangeSize()
{
	m_Image.InitSizes(true);
	OnLayoutChanged();
	return S_OK;
}

LRESULT CPreviewPixDlg::OnDelayedRepaint(WPARAM wParam, LPARAM lParam)
{
	static DWORD dwLastRefresh = 0;
	CString sPeriod;
	bool bRefreshPeriod = GetTickCount() > dwLastRefresh+500;
	if (bRefreshPeriod)
	{
		DWORD dwPeriod;
		m_sptrPrv->GetPeriod(m_nDev,0,&dwPeriod);
		if (g_FPS)
		{
			if (m_PreviewIds.m_idFPS > 0)
				sPeriod.Format(m_PreviewIds.m_idFPS, 1000./double(dwPeriod));
			else
				sPeriod.Format(_T("%.1f"), 1000./double(dwPeriod));
		}
		else
		{
			if (m_PreviewIds.m_idPeriod > 0)
				sPeriod.Format(m_PreviewIds.m_idPeriod, dwPeriod);
			else
				sPeriod.Format(_T("%d"), dwPeriod);
		}
		sPeriod += _T("     ");
		if (m_StaticPeriod.m_hWnd != NULL)
			m_StaticPeriod.SetWindowText(sPeriod);
		dwLastRefresh = GetTickCount();
	}
	if (m_pDialog) m_pDialog->OnDelayedNewImage();
	if ((bool)g_Histo && m_sptrDrv != 0)
	{
		m_HistoDlg.ResetMinMax();
		if (m_HistoDlg.m_Period.m_hWnd != NULL && bRefreshPeriod)
			m_HistoDlg.m_Period.SetWindowText(sPeriod);
	}
	if (m_sptrDDS != 0)
		m_sptrDDS->OnDelayedRepaint(m_hWnd, this);
	CDelayedRepaintHelper::OnDelayedRepaint();
//	m_bProcessDelayedRepaint = false;
	return 0;
}

void CPreviewPixDlg::OnLayoutChanged()
{
	if ((bool)g_Histo && m_HistoDlg.GetSafeHwnd()==NULL)
	{
		m_HistoDlg.Create(m_PreviewIds.m_HistoIds.m_nIdDlg, this);
		AddTempControl(m_HistoDlg.GetSafeHwnd(), 0, 1);
		m_HistoDlg.ShowWindow(SW_SHOW);
		if (m_sptrDDS != 0)
		{
			m_sptrDDS->OnUpdateCmdUI(::GetDlgItem(m_HistoDlg.m_hWnd, m_PreviewIds.m_HistoIds.
				m_nIdRed), m_PreviewIds.m_HistoIds.m_nIdRed);
			m_sptrDDS->OnUpdateCmdUI(::GetDlgItem(m_HistoDlg.m_hWnd, m_PreviewIds.m_HistoIds.
				m_nIdGreen), m_PreviewIds.m_HistoIds.m_nIdGreen);
			m_sptrDDS->OnUpdateCmdUI(::GetDlgItem(m_HistoDlg.m_hWnd, m_PreviewIds.m_HistoIds.
				m_nIdBlue), m_PreviewIds.m_HistoIds.m_nIdBlue);			
		}
		ASSERT(m_HistoDlg.GetSafeHwnd()!=NULL);
	}
	else if (!(bool)g_Histo && m_HistoDlg.GetSafeHwnd()!=NULL)
	{
		m_HistoDlg.ShowWindow(SW_HIDE);
		RemoveTempControl(m_HistoDlg.GetSafeHwnd());
		m_HistoDlg.DestroyWindow();
		ASSERT(m_HistoDlg.GetSafeHwnd()==NULL);
	}
	if ((bool)g_Size11)
		SetSizeByPreview(m_Image.m_szPrv, false);
	else
		SetMaximalSize(m_Image.m_szPrv, false);
	
	::ShowWindow(::GetDlgItem(m_hWnd, m_PreviewIds.m_idStaticPeriod), bool(g_FPSAlways)?SW_SHOW:SW_HIDE);
}

void CPreviewPixDlg::OnInitChild(CBaseDlg *pChild)
{
	if (m_PreviewIds.m_idHistoCheckBox > 0)
	{
		CButton *pButton = (CButton *)pChild->GetDlgItem(m_PreviewIds.m_idHistoCheckBox);
		if (pButton)
			pButton->SetCheck((bool)g_Histo?BST_CHECKED:BST_UNCHECKED);
	}
}

void CPreviewPixDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_rcBase.PtInRect(point))
	{
		CPoint pt(point.x-m_rcBase.left, point.y-m_rcBase.top);
		m_Image.DoLButtonDown(nFlags, pt);
	}
	CBaseDlg::OnLButtonDown(nFlags, point);
}

void CPreviewPixDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (m_rcBase.PtInRect(point))
	{
		CPoint pt(point.x-m_rcBase.left, point.y-m_rcBase.top);
		m_Image.DoLButtonUp(nFlags, pt);
	}
	CBaseDlg::OnLButtonUp(nFlags, point);
}

void CPreviewPixDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	if ((nFlags & MK_LBUTTON) && m_rcBase.PtInRect(point))
	{
		CPoint pt(point.x-m_rcBase.left, point.y-m_rcBase.top);
		m_Image.DoMouseMove(nFlags, pt);
	}
	CBaseDlg::OnMouseMove(nFlags, point);
}

BOOL CPreviewPixDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	CPoint point;
	GetCursorPos(&point);
	ScreenToClient(&point);
	if (m_rcBase.PtInRect(point))
	{
		CPoint pt(point.x-m_rcBase.left, point.y-m_rcBase.top);
		if (m_Image.DoSetCursor(0, pt))
			return TRUE;
	}
	
	return CBaseDlg::OnSetCursor(pWnd, nHitTest, message);
}

void CPreviewPixDlg::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);
	CRect rcWnd;
	GetWindowRect(rcWnd);
	if (!(bool)g_Size11)
		RepositionByTotal();
}

void CPreviewPixDlg::OnSizing(UINT fwSide, LPRECT pRect)
{
	__super::OnSizing(fwSide, pRect);
	HandleSizing(m_Image.m_szPrv, fwSide, pRect);
}

BOOL CPreviewPixDlg::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	for (int i = 0; i < m_arrPages.GetSize(); i++)
	{
		if (m_arrPages[i].idCmd == nID)
		{
			int n = IsDlgButtonChecked(nID);
			if (n)
			{
				SetChoice(-1);
				AddDialog(NULL, 0);
			}
			else
			{
				SetChoice(nID);
				AddDialog(new CBaseDlg(m_arrPages[i].idDialog, this, this), m_arrPages[i].idDialog);
			}
			return TRUE;
		}
	}

	return __super::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

BOOL CPreviewPixDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	UINT nID = LOWORD(wParam);
	HWND hWndCtrl = (HWND)lParam;
	int nCode = HIWORD(wParam);
	if (nID == m_PreviewIds.m_idHistoCheckBox && nCode == 0)
	{
		int n = (int)::SendMessage(hWndCtrl, BM_GETCHECK, 0, 0);
		if (n == BST_CHECKED || n == BST_UNCHECKED)
		{
			g_Histo = n==BST_CHECKED;
			OnLayoutChanged();
			return TRUE;
		}
	}
	return __super::OnCommand(wParam, lParam);
}


bool CPreviewPixDlg::IsOwnControl(HWND hwnd, int id)
{
	if (id == IDOK || id == IDCANCEL || id == IDHELP) return true;
	if (id == m_PreviewIds.m_idStaticPeriod || id == m_PreviewIds.m_idHistoCheckBox) return true;
	for (int i = 0; i < m_arrPages.GetSize(); i++)
	{
		if (id == m_arrPages[i].idCmd)
			return true;
	}
	return false;
}


void CPreviewPixDlg::AttachChildDialogToButton(int idButton, int idChildDlgRes)
{
	COptionsPageInfo page = {idButton, idChildDlgRes};
	m_arrPages.Add(page);
	ReserveSpaceForSubdialog(idChildDlgRes);
}


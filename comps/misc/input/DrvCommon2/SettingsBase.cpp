// SettingsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "SettingsBase.h"
#include "input.h"
#include "CamValues.h"
//sergey 03.05.06
#include "StdProfile.h"
//end



//sergey 03/10/06
//CCamBoolValue g_Size11(_T("PreviewDialog"), _T("Size11"), true);
//end
extern CCamBoolValue g_FPSAlways;
extern CCamBoolValue g_FPS;
extern CCamBoolValue g_Histo;


// CSettingsBase dialog

IMPLEMENT_DYNAMIC(CSettingsBase, CDialog)
CSettingsBase::CSettingsBase(IUnknown *punk, int nDevice, int idd, CSettingsIds Ids, CWnd* pParent /*=NULL*/)
: CBaseDlg(idd, pParent, NULL, punk), m_HistoDlg(Ids.m_HistoIds)//:CBaseDlg(idd, pParent, NULL, punk), m_Image(punk, nDevice),
	// m_HistoDlg(Ids.m_HistoIds)//sergey 03/10/06//
{
	m_sptrPrv = punk;
	if (m_sptrPrv == 0)
		AfxThrowNotSupportedException();
	m_sptrDrv = punk;
//	m_sptrDDS = punk;
	m_nDev = nDevice;
	m_pDialog = NULL;
	m_Ids = Ids;
	m_bSubdlgControls = true;	
}

CSettingsBase::~CSettingsBase()
{
	//sergey 01/06/06
	//m_HistoDlg.DestroyWindow();
	//delete m_HistoDlg;
	//end
	delete m_pDialog;
}

void CSettingsBase::AddDialog(CBaseDlg *p, int idd)
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
	//sergey 27/04/06??????????????!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//m_Tab.SetWindowPos(0, 0, 0, 298, 218, SWP_NOSIZE|SWP_NOMOVE|SWP_NOZORDER);
	//m_Tab.SetWindowPos(0, 0, 0, rcWnd.Width(), rcWnd.Height(), SWP_NOMOVE|SWP_NOZORDER);
	//end
	m_Tab.GetWindowRect(rcWnd);
	ScreenToClient(rcWnd);
	rcDlg = rcWnd;
	m_Tab.AdjustRect(FALSE, rcDlg);
	//sergey 27/04/06
	//m_pDialog->SetWindowPos(0, 0, 0, 193, 118, SWP_NOZORDER);
	m_pDialog->SetWindowPos(0, rcDlg.left-rcWnd.left, rcDlg.top-rcWnd.top, rcDlg.Width(), rcDlg.Height(), SWP_NOZORDER);
	//end
	m_pDialog->ShowWindow(SW_SHOW);
	int yOk = rcWnd.bottom;
	int n = 5;
	CRect rcTotal(0,0,max(3*m_szButton.cx+4*n,rcWnd.Width()+2*rcWnd.left), rcWnd.bottom+m_szButton.cy+2*n);
	if (m_HistoDlg.m_hWnd != NULL)
	{
		CRect rcHisto;
		m_HistoDlg.GetWindowRect(rcHisto);
		rcHisto = CRect(rcWnd.left, rcWnd.bottom+n, rcWnd.left+rcHisto.Width(), rcWnd.bottom+n+rcHisto.Height());
		m_HistoDlg.SetWindowPos(NULL, rcWnd.left, rcWnd.bottom+n, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
		yOk = rcWnd.bottom+2*n+rcHisto.Height();
		rcTotal.right = max(rcTotal.right, rcHisto.Width());
		rcTotal.bottom += rcHisto.Height()+n;		
	}
	//sergey 27/04/06
	m_Ok.SetWindowPos(0, n+6, yOk+6, m_szButton.cx, m_szButton.cy, SWP_NOZORDER);
	//m_Ok.SetWindowPos(0, n, yOk, m_szButton.cx, m_szButton.cy, SWP_NOZORDER);
	//end
	m_Ok.Invalidate();
	if (m_Help.m_hWnd)
		m_Help.SetWindowPos(0, m_szButton.cx+2*n, yOk, m_szButton.cx, m_szButton.cy, SWP_NOZORDER);
	if (m_StaticPeriod.m_hWnd)
		m_StaticPeriod.SetWindowPos(0, 2*m_szButton.cx+3*n, yOk, m_szButton.cx, m_szButton.cy, SWP_NOZORDER);
	AdjustWindowRect(rcTotal, GetWindowLong(m_hWnd, GWL_STYLE), FALSE);
	//sergey 27/04/06
	//SetWindowPos(0, 0, 0, 300,200, SWP_NOMOVE|SWP_NOZORDER);
	SetWindowPos(0, 0, 0, rcTotal.Width(), rcTotal.Height(), SWP_NOMOVE|SWP_NOZORDER);
	//end
}

void CSettingsBase::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
	if (m_Ids.m_idTab > 0)
		DDX_Control(pDX, m_Ids.m_idTab, m_Tab);
	if (m_Ids.m_idPeriod > 0)
		DDX_Control(pDX, m_Ids.m_idPeriod, m_StaticPeriod);
	//sergey 02/06/06
	/*if (m_Ids.m_idHistoCheckBox > 0)
		DDX_Control(pDX, m_Ids.m_idHistoCheckBox, m_HistoDlg);*/
	//end
	DDX_Control(pDX, IDOK, m_Ok);
	if (m_bUseHelp)
		DDX_Control(pDX, IDHELP, m_Help);
}


BEGIN_MESSAGE_MAP(CSettingsBase, CDialog)
	//sergey 03/10/06
	/*ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
	ON_WM_SIZE()
	ON_WM_SIZING()*/
	//end
	ON_WM_DESTROY()
	ON_MESSAGE(WM_APP+1, OnDelayedRepaint)
END_MESSAGE_MAP()


// CSettingsDialog message handlers

BOOL CSettingsBase::OnInitDialog()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	m_bUseHelp = ::GetDlgItem(m_hWnd, IDHELP)!=NULL;
	CDialog::OnInitDialog();
	m_sptrPrv->BeginPreview(m_nDev, this);
//	m_sptrDrv->StartImageAquisition(m_nDev, TRUE);
	CRect rc;
	m_Ok.GetWindowRect(rc);
	m_szButton = rc.Size();
	for (int i = 0; i < m_arrPanes.GetSize(); i++)
		m_Tab.InsertItem(i, m_arrPanes[i].m_sName);
	if (m_arrPanes.GetSize() > 0)
		AddDialog(new CBaseDlg(m_arrPanes[0].m_idd,this,NULL,m_sptrDDS), m_arrPanes[0].m_idd);
	ShowHistogram(g_Histo);
//	if (m_StaticPeriod.m_hWnd != 0)
//		m_StaticPeriod.ShowWindow((bool)g_FPSAlways?SW_SHOW:SW_HIDE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSettingsBase::OnOK()
{
	try
	{
		
	//CBaseDlg::OnOK();
	CDialog::OnOK();
	}
	catch( CException* e )
   {
      
	  e->ReportError();
   }
}

void CSettingsBase::AddPane(int idName, int idDlg)
{
	CTabPane pane;
	pane.m_idd = idDlg;
	pane.m_sName.LoadString(idName);
	m_arrPanes.Add(pane);
}

BOOL CSettingsBase::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	int idCtrl = (int)wParam;
	NMHDR *pHdr = (NMHDR *)lParam;
	if (idCtrl == m_Ids.m_idTab && pHdr->code == TCN_SELCHANGE)
	{
		int i = m_Tab.GetCurSel();
		AddDialog(new CBaseDlg(m_arrPanes[i].m_idd,this,this,m_sptrDDS), m_arrPanes[i].m_idd);
		return TRUE;
	}

	return CDialog::OnNotify(wParam, lParam, pResult);
}

void CSettingsBase::OnDestroy()
{
  try
  {
	m_sptrPrv->EndPreview(m_nDev, this);
	
	
	//CBaseDlg::OnDestroy();
	CDialog::OnDestroy();
	
//	m_sptrDrv->StartImageAquisition(m_nDev, FALSE);
  }
	catch( CException* e )
   {
      
	  e->ReportError();
   }
	
}

extern CCamBoolValue g_Histo;

HRESULT CSettingsBase::Invalidate()
{
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
//	PostMessage(WM_APP+1);
	return S_OK;
}

HRESULT CSettingsBase::OnChangeSize()
{
	//sergey 01/06/06
	//m_Image.InitSizes(true);
	//OnLayoutChanged();
	return S_OK;
	//return S_OK;
	//end
}

LRESULT CSettingsBase::OnDelayedRepaint(WPARAM wParam, LPARAM lParam)
{
		//sergey 02/05/06
	BOOL bColorCamera = CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("ColorCamera"), TRUE, true, true);	    
		if(!bColorCamera)
		{
			//::EnableWindow(hwnd, bColorCamera);
	    ::EnableWindow(::GetDlgItem(m_HistoDlg.m_hWnd, m_Ids.m_HistoIds.m_nIdRed), FALSE);	
	    ::EnableWindow(::GetDlgItem(m_HistoDlg.m_hWnd, m_Ids.m_HistoIds.m_nIdGreen), FALSE);
	    ::EnableWindow(::GetDlgItem(m_HistoDlg.m_hWnd, m_Ids.m_HistoIds.m_nIdBlue), FALSE);
			}
	
		//end
	
	DWORD dwPeriod;
	m_sptrPrv->GetPeriod(m_nDev,0,&dwPeriod);
	CString sPeriod;
	if (g_FPS)
	{
		if (m_Ids.m_idFPSStr > 0)
			sPeriod.Format(m_Ids.m_idFPSStr, 1000./double(dwPeriod));
		else
			sPeriod.Format(_T("%.1f"), 1000./double(dwPeriod));
	}
	else
	{
		if (m_Ids.m_idPeriodStr > 0)
			sPeriod.Format(m_Ids.m_idPeriodStr, dwPeriod);
		else
			sPeriod.Format(_T("%d"), dwPeriod);
	}
	if (m_StaticPeriod.m_hWnd != NULL)
		m_StaticPeriod.SetWindowText(sPeriod);
	if (m_pDialog) m_pDialog->OnDelayedNewImage();
	if ((bool)g_Histo && m_sptrDrv != 0)
	{
		m_HistoDlg.ResetMinMax();
		if (m_HistoDlg.m_Period.m_hWnd != NULL)
			m_HistoDlg.m_Period.SetWindowText(sPeriod);
	}
	if (m_sptrDDS != 0)
		m_sptrDDS->OnDelayedRepaint(m_hWnd, this);
	CDelayedRepaintHelper::OnDelayedRepaint();
	return 0;
}

void CSettingsBase::ShowHistogram(BOOL bShow)
{
	if (bShow && m_HistoDlg.m_hWnd || !bShow && m_HistoDlg.m_hWnd == NULL)
		return;
	const int yButOffs = 5;
	if (bShow)
	{
		m_HistoDlg.Create(m_Ids.m_HistoIds.m_nIdDlg, this);
		CRect rcOk,rcHelp,rcPeriod,rcHist,rcTotal;
		m_Ok.GetWindowRect(&rcOk);
		if (m_Help.m_hWnd != NULL) m_Help.GetWindowRect(&rcHelp);
		if (m_StaticPeriod.m_hWnd != NULL) m_StaticPeriod.GetWindowRect(&rcPeriod);
		ScreenToClient(rcOk);
		if (m_Help.m_hWnd != NULL) ScreenToClient(rcHelp);
		if (m_StaticPeriod.m_hWnd != NULL) ScreenToClient(rcPeriod);
		m_HistoDlg.SetWindowPos(NULL, 0, rcOk.top, 0, 0, SWP_NOZORDER|SWP_NOSIZE);
		m_HistoDlg.GetWindowRect(rcHist);
		m_Ok.SetWindowPos(NULL, rcOk.left, rcOk.top+rcHist.Height()+yButOffs, 0, 0, SWP_NOZORDER|SWP_NOSIZE);
		if (m_Help.m_hWnd != NULL) m_Help.SetWindowPos(NULL, rcHelp.left, rcHelp.top+rcHist.Height()+yButOffs, 0, 0, SWP_NOZORDER|SWP_NOSIZE);
		if (m_StaticPeriod.m_hWnd != NULL) m_StaticPeriod.SetWindowPos(NULL, rcPeriod.left, rcPeriod.top+rcHist.Height()+yButOffs, 0, 0, SWP_NOZORDER|SWP_NOSIZE);
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
		if (m_Help.m_hWnd != NULL) m_Help.GetWindowRect(&rcHelp);
		if (m_StaticPeriod.m_hWnd != NULL) m_StaticPeriod.GetWindowRect(&rcPeriod);
		m_HistoDlg.GetWindowRect(rcHisto);
		GetClientRect(rcTotal);
		m_HistoDlg.DestroyWindow();
		ScreenToClient(rcOk);
		if (m_Help.m_hWnd != NULL) ScreenToClient(rcHelp);
		if (m_StaticPeriod.m_hWnd != NULL) ScreenToClient(rcPeriod);
		int nButtonY = rcOk.top-rcHisto.Height()-yButOffs;
		m_Ok.SetWindowPos(NULL, rcOk.left, nButtonY, 0, 0, SWP_NOZORDER|SWP_NOSIZE);
		if (m_Help.m_hWnd != NULL) m_Help.SetWindowPos(NULL, rcHelp.left, nButtonY, 0, 0, SWP_NOZORDER|SWP_NOSIZE);
		if (m_StaticPeriod.m_hWnd != NULL) m_StaticPeriod.SetWindowPos(NULL, rcPeriod.left, nButtonY, 0, 0, SWP_NOZORDER|SWP_NOSIZE);
		rcTotal.bottom -= rcHisto.Height()+yButOffs;
		m_pDialog->GetWindowRect(rcWnd);
		ScreenToClient(rcWnd);
		rcTotal.right = rcTotal.left+max(3*m_szButton.cx+4*yButOffs,rcWnd.Width()+2*rcWnd.left);
		AdjustWindowRect(rcTotal,GetStyle(),FALSE);
		SetWindowPos(NULL, 0, 0, rcTotal.Width(), rcTotal.Height(), SWP_NOZORDER|SWP_NOMOVE);	
	     
	}
}

void CSettingsBase::OnInitChild(CBaseDlg *pChild)
{
	if (m_Ids.m_idHistoCheckBox > 0)
	{
		CButton *pButton = (CButton *)pChild->GetDlgItem(m_Ids.m_idHistoCheckBox);
		if (pButton)
			pButton->SetCheck((bool)g_Histo?BST_CHECKED:BST_UNCHECKED);
	}
}

BOOL CSettingsBase::OnCommand(WPARAM wParam, LPARAM lParam)
{
	UINT nID = LOWORD(wParam);
	HWND hWndCtrl = (HWND)lParam;
	int nCode = HIWORD(wParam);
	if (nID == m_Ids.m_idHistoCheckBox && nCode == 0)
	{
		int n = (int)::SendMessage(hWndCtrl, BM_GETCHECK, 0, 0);
		if (n == BST_CHECKED || n == BST_UNCHECKED)
		{
			g_Histo = n==BST_CHECKED;
			ShowHistogram(n==BST_CHECKED);
			return TRUE;
		}
	}
	return __super::OnCommand(wParam, lParam);
}

bool CSettingsBase::IsOwnControl(HWND hwnd, int id)
{
	if (id == IDOK || id == IDCANCEL || id == IDHELP) return true;
	if (id == m_Ids.m_idPeriod || id == m_Ids.m_idHistoCheckBox) return true;
	return false;
}


//sergey 02/10/06
/*void CSettingsBase::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_rcBase.PtInRect(point))
	{
		CPoint pt(point.x-m_rcBase.left, point.y-m_rcBase.top);
		m_Image.DoLButtonDown(nFlags, pt);
	}
	CBaseDlg::OnLButtonDown(nFlags, point);
}

void CSettingsBase::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (m_rcBase.PtInRect(point))
	{
		CPoint pt(point.x-m_rcBase.left, point.y-m_rcBase.top);
		m_Image.DoLButtonUp(nFlags, pt);
	}
	CBaseDlg::OnLButtonUp(nFlags, point);
}

void CSettingsBase::OnMouseMove(UINT nFlags, CPoint point) 
{
	if ((nFlags & MK_LBUTTON) && m_rcBase.PtInRect(point))
	{
		CPoint pt(point.x-m_rcBase.left, point.y-m_rcBase.top);
		m_Image.DoMouseMove(nFlags, pt);
	}
	CBaseDlg::OnMouseMove(nFlags, point);
}

BOOL CSettingsBase::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
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

void CSettingsBase::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);
	CRect rcWnd;
	GetWindowRect(rcWnd);
	if (!(bool)g_Size11)
		RepositionByTotal();
}

void CSettingsBase::OnSizing(UINT fwSide, LPRECT pRect)
{
	__super::OnSizing(fwSide, pRect);
	HandleSizing(m_Image.m_szPrv, fwSide, pRect);
}*/

//end

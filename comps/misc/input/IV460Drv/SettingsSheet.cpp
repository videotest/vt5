// SettingsSheet.cpp : implementation file
//

#include "stdafx.h"
#include "IV460Drv.h"
#include "SettingsSheet.h"
#include "StdProfile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSettingsPage

IMPLEMENT_DYNAMIC(CSettingsPage, CPropertyPage)


CSettingsSheet *CSettingsPage::GetParentSheet()
{
	CWnd *pParent = GetParent();
	return (CSettingsSheet *)pParent;
}

void CSettingsPage::SetModified(BOOL bChanged)
{
	CSettingsSheet *p = GetParentSheet();
	if (p) p->m_bModified = bChanged;
	CPropertyPage::SetModified(bChanged);
}


/////////////////////////////////////////////////////////////////////////////
// CSettingsSheet

IMPLEMENT_DYNAMIC(CSettingsSheet, CPropertySheet)

CSettingsSheet::CSettingsSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CSettingsSheet::CSettingsSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
}

CSettingsSheet::~CSettingsSheet()
{
}


BEGIN_MESSAGE_MAP(CSettingsSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CSettingsSheet)
	ON_WM_MOVE()
	ON_WM_DESTROY()
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSettingsSheet message handlers

void CSettingsSheet::UpdateControls(int nHint)
{
	int nPages = GetPageCount();
	for (int i = 0; i < nPages; i++)
	{
		CSettingsPage *p = (CSettingsPage *)GetPage(i);
		p->UpdateControls(nHint);
	}
	m_Graffic.UpdateControls(nHint);
}

BOOL CSettingsSheet::OnInitDialog() 
{
	BOOL bResult = CPropertySheet::OnInitDialog();

//	m_bOvrModeChanged = false;
	RestorePosition();
	// Restore active page
	int nActive = g_StdProf.GetProfileInt(_T("SettingsDlg"), _T("ActivePage"), 0);
	SetActivePage(nActive);
	m_bInited = true;
	// Initialize graffic mode
//	BOOL bGraffic = g_StdProf.GetProfileInt(_T("SettingsDlg"), _T("Graffic"), 0);
//	SetGrafficMode(bGraffic);

	return bResult;
}

void CSettingsSheet::OnOK()
{
	if (m_bFinished) return;
	m_bFinished = TRUE;
	g_StdProf.WriteProfileInt(_T("SettingsDlg"), _T("Graffic"), m_bGraffic);
}

void CSettingsSheet::OnCancel()
{
	if (m_bFinished) return;
	m_bFinished = TRUE;
}

void CSettingsSheet::OnApply()
{
	if (!m_bModified) return;
	m_bModified = FALSE;
}

void CSettingsSheet::RestorePosition()
{
	// Move to saved position
	/*if (m_bOvrMode)
	{
		int nSavedX1 = g_StdProf.GetProfileInt(_T("SettingsDlg"), _T("OvrSavedX1"), -1);
		int nSavedY1 = g_StdProf.GetProfileInt(_T("SettingsDlg"), _T("OvrSavedY1"), -1);
		if (nSavedX1 != -1 && nSavedY1 != -1)
			SetWindowPos(NULL, nSavedX1, nSavedY1, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
	}
	else*/
	{
		int nSavedX1 = g_StdProf.GetProfileInt(_T("SettingsDlg"), _T("NonOvrSavedX1"), -1);
		int nSavedY1 = g_StdProf.GetProfileInt(_T("SettingsDlg"), _T("NonOvrSavedY1"), -1);
		if (nSavedX1 != -1 && nSavedY1 != -1)
			SetWindowPos(NULL, nSavedX1, nSavedY1, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
	}
}

void CSettingsSheet::OnMove(int x, int y) 
{
	CPropertySheet::OnMove(x, y);
	
	if (m_bInited)
	{
		CRect rc;
		GetWindowRect(rc);
		/*if (m_bOvrMode)
		{
			g_StdProf.WriteProfileInt(_T("SettingsDlg"), _T("OvrSavedX1"), rc.left);
			g_StdProf.WriteProfileInt(_T("SettingsDlg"), _T("OvrSavedY1"), rc.top);
		}
		else*/
		{
			g_StdProf.WriteProfileInt(_T("SettingsDlg"), _T("NonOvrSavedX1"), rc.left);
			g_StdProf.WriteProfileInt(_T("SettingsDlg"), _T("NonOvrSavedY1"), rc.top);
		}
	}
	
}


void CSettingsSheet::OnDestroy() 
{
	CPropertySheet::OnDestroy();
	g_StdProf.WriteProfileInt(_T("SettingsDlg"), _T("ActivePage"), GetActiveIndex());
}

void CSettingsSheet::SetGrafficMode(BOOL bGrafficMode)
{
	if (bGrafficMode != m_bGraffic)
	{
		if (bGrafficMode)
		{
			GetWindowRect(m_rcNonGraffic);
			CRect rcClient,rc1;
			GetClientRect(rcClient);
			::GetWindowRect(::GetDlgItem(m_hWnd, IDOK), m_rcOk);
			ScreenToClient(m_rcOk);
			::GetWindowRect(::GetDlgItem(m_hWnd, IDCANCEL), m_rcCancel);
			ScreenToClient(m_rcCancel);
			::GetWindowRect(::GetDlgItem(m_hWnd, ID_APPLY_NOW), m_rcApply);
			ScreenToClient(m_rcApply);
			::GetWindowRect(::GetDlgItem(m_hWnd, IDHELP), m_rcHelp);
			ScreenToClient(m_rcHelp);
			CRect rc(0, m_rcOk.top, rcClient.Width(), m_rcOk.top);
			m_Graffic.Create(IDD_HISTO, this);
			m_Graffic.UpdateControls(ESH_Graffic_Changed);
			m_Graffic.GetWindowRect(rc1);
			rc.bottom = rc.top+rc1.Height();
			m_Graffic.MoveWindow(rc);
			SetWindowPos(NULL, 0, 0, m_rcNonGraffic.Width(), m_rcNonGraffic.Height()+rc.Height()+5, SWP_NOMOVE|SWP_NOZORDER);
			::SetWindowPos(::GetDlgItem(m_hWnd, IDOK), NULL, m_rcOk.left, m_rcOk.top+rc.Height(), 0, 0, SWP_NOSIZE|SWP_NOZORDER);
			::SetWindowPos(::GetDlgItem(m_hWnd, IDCANCEL), NULL, m_rcCancel.left, m_rcCancel.top+rc.Height(), 0, 0, SWP_NOSIZE|SWP_NOZORDER);
			::SetWindowPos(::GetDlgItem(m_hWnd, ID_APPLY_NOW), NULL, m_rcApply.left, m_rcApply.top+rc.Height(), 0, 0, SWP_NOSIZE|SWP_NOZORDER);
			::SetWindowPos(::GetDlgItem(m_hWnd, IDHELP), NULL, m_rcHelp.left, m_rcHelp.top+rc.Height(), 0, 0, SWP_NOSIZE|SWP_NOZORDER);
		}
		else
		{
			SetWindowPos(NULL, 0, 0, m_rcNonGraffic.Width(), m_rcNonGraffic.Height(), SWP_NOMOVE|SWP_NOZORDER);
			m_Graffic.DestroyWindow();
			::SetWindowPos(::GetDlgItem(m_hWnd, IDOK), NULL, m_rcOk.left, m_rcOk.top, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
			::SetWindowPos(::GetDlgItem(m_hWnd, IDCANCEL), NULL, m_rcCancel.left, m_rcCancel.top, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
			::SetWindowPos(::GetDlgItem(m_hWnd, ID_APPLY_NOW), NULL, m_rcApply.left, m_rcApply.top, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
			::SetWindowPos(::GetDlgItem(m_hWnd, IDHELP), NULL, m_rcHelp.left, m_rcHelp.top, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
		}
		m_bGraffic = bGrafficMode;
	}
}

void CSettingsSheet::OnRedraw()
{
	/*if (m_bGraffic && !m_bFreezeGraffic && m_Graffic.m_hWnd)
		m_Graffic.m_HistoBox.InitHisto(g_CxLibWork.m_lpbi);*/
}

void CSettingsSheet::OnDelayedRedraw()
{
	/*if (m_bGraffic && !m_bFreezeGraffic && m_Graffic.m_hWnd)
		m_Graffic.ResetMinMax();*/
}

BOOL CSettingsSheet::OnHelpInfo(HELPINFO* pHelpInfo) 
{
//	if (!RunContextHelp(pHelpInfo))
		return CPropertySheet::OnHelpInfo( pHelpInfo );
//	return FALSE;
}

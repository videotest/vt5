// HistoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "HistoDlg.h"
#include "StdProfile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHistoDlg dialog



CHistoDlg::CHistoDlg(CHistoIds IDs, CWnd* pParent /*=NULL*/)
	: CDialog(IDs.m_nIdDlg, pParent)
{
	m_IDs = IDs;
	//{{AFX_DATA_INIT(CHistoDlg)
	m_bGray = CStdProfileManager::m_pMgr->GetProfileInt(_T("Histogram"), _T("ShowGray"), TRUE);
	m_bRed = CStdProfileManager::m_pMgr->GetProfileInt(_T("Histogram"), _T("ShowRed"), TRUE);
	m_bGreen = CStdProfileManager::m_pMgr->GetProfileInt(_T("Histogram"), _T("ShowGreen"), TRUE);
	m_bBlue = CStdProfileManager::m_pMgr->GetProfileInt(_T("Histogram"), _T("ShowBlue"), TRUE);
	//}}AFX_DATA_INIT
	m_rcGray = m_rcRed = m_rcGreen = m_rcBlue = CRect(0,0,0,0);
}


void CHistoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHistoDlg)	
	DDX_Control(pDX, m_IDs.m_n0, m_Static0);
	DDX_Control(pDX, m_IDs.m_nMaxColor, m_MaxColor);
	DDX_Control(pDX, m_IDs.m_nMaxPoints, m_MaxPoints);
	DDX_Control(pDX, m_IDs.m_nMsg, m_Msg);
	DDX_Control(pDX, m_IDs.m_nIdBlue, m_Blue);
	DDX_Control(pDX, m_IDs.m_nIdGreen, m_Green);
	DDX_Control(pDX, m_IDs.m_nIdRed, m_Red);
	DDX_Control(pDX, m_IDs.m_nIdGray, m_Gray);
	DDX_Control(pDX, m_IDs.m_nIdHisto, m_HistoBox);
	DDX_Check(pDX, m_IDs.m_nIdGray, m_bGray);
	DDX_Check(pDX, m_IDs.m_nIdRed, m_bRed);
	DDX_Check(pDX, m_IDs.m_nIdGreen, m_bGreen);
	DDX_Check(pDX, m_IDs.m_nIdBlue, m_bBlue);
	if (m_IDs.m_nStaticPeriod > 0)
		DDX_Control(pDX, m_IDs.m_nStaticPeriod, m_Period);
	//}}AFX_DATA_MAP
}


/////////////////////////////////////////////////////////////////////////////
// CHistoDlg message handlers

void CHistoDlg::OnBlue() 
{
	InitShow();
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Histogram"), _T("ShowBlue"), m_bBlue);
}

void CHistoDlg::OnGray() 
{
	InitShow();
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Histogram"), _T("ShowGray"), m_bGray);
}

void CHistoDlg::OnGreen() 
{
	InitShow();
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Histogram"), _T("ShowGreen"), m_bGreen);
}

void CHistoDlg::OnRed() 
{
	InitShow();
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Histogram"), _T("ShowRed"), m_bRed);
}

void CHistoDlg::InitShow()
{
	UpdateData();
	unsigned nHSM = 0;
	if (m_bGray) nHSM |= CHistBox::hsmBri;
	if (m_bRed) nHSM |= CHistBox::hsmRed;
	if (m_bGreen) nHSM |= CHistBox::hsmGreen;
	if (m_bBlue) nHSM |= CHistBox::hsmBlue;
	m_HistoBox.SetHSM(nHSM);
}

BOOL CHistoDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_Gray.GetWindowRect(m_rcGray);
	ScreenToClient(m_rcGray);
	m_Red.GetWindowRect(m_rcRed);
	ScreenToClient(m_rcRed);
	m_Green.GetWindowRect(m_rcGreen);
	ScreenToClient(m_rcGreen);
	m_Blue.GetWindowRect(m_rcBlue);
	ScreenToClient(m_rcBlue);
	m_Msg.GetWindowRect(m_rcMsg);
	ScreenToClient(m_rcMsg);
	m_Msg.GetWindowText(m_sMsg);
	m_Msg.SetWindowText(_T(""));
	unsigned nHSM = 0;
	if (m_bGray) nHSM |= CHistBox::hsmBri;
	if (m_bRed) nHSM |= CHistBox::hsmRed;
	if (m_bGreen) nHSM |= CHistBox::hsmGreen;
	if (m_bBlue) nHSM |= CHistBox::hsmBlue;
	m_HistoBox.SetHSM(nHSM);
	SetMaxColor();		
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CHistoDlg::ResetMinMax()
{
	if (m_Msg.GetSafeHwnd()==0) return;
	CString s;
/*	if (m_HistoBox.m_fTrueColor)
		s.Format(_T("Min - %d(%d,%d,%d), Max - %d(%d,%d,%d), Avr - %d(%d,%d,%d)"),
			m_HistoBox.m_nMin, m_HistoBox.m_nMinR, m_HistoBox.m_nMinG, m_HistoBox.m_nMinB,
			m_HistoBox.m_nMax, m_HistoBox.m_nMaxR, m_HistoBox.m_nMaxG, m_HistoBox.m_nMaxB,
			m_HistoBox.m_nAvr, m_HistoBox.m_nAvrR, m_HistoBox.m_nAvrG, m_HistoBox.m_nAvrB);
	else*/
	CString sFmt = m_sMsg; //_T("Min - %d, Max - %d, Avr - %d")
	if (m_bGray)
		s.Format(sFmt, m_HistoBox.m_nMin, m_HistoBox.m_nMax, m_HistoBox.m_nAvr);
	else if (m_bRed)
		s.Format(sFmt, m_HistoBox.m_nMinR, m_HistoBox.m_nMaxR, m_HistoBox.m_nAvrR);
	else if (m_bGreen)
		s.Format(sFmt, m_HistoBox.m_nMinG, m_HistoBox.m_nMaxG, m_HistoBox.m_nAvrG);
	else if (m_bBlue)
		s.Format(sFmt, m_HistoBox.m_nMinB, m_HistoBox.m_nMaxB, m_HistoBox.m_nAvrB);
	m_Msg.SetWindowText(s);
	s.Format("%d", m_HistoBox.m_nMaxValue);
	m_MaxPoints.SetWindowText(s);
	
}


void CHistoDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	if (m_hWnd)
	{
		int nMargX = 5, nMargY = 5, nTxtY = 12;
		int nBtWidth = m_rcGray.Width();
		if (cx > 4*nMargX+2*nBtWidth && cy > 2*nTxtY)
		{
			CRect rcHisto, rcGray, rcRed, rcGreen, rcBlue, rcMsg;
			int nBtHeight = m_rcGray.Height();
			int nBtnLeft = cx-2*nBtWidth-2*nMargX, nBtnRight = nBtnLeft+nBtWidth;
			rcGray = CRect(nBtnLeft, nMargY, nBtnRight, nMargY+nBtHeight);
			if (m_Gray.m_hWnd) m_Gray.MoveWindow(rcGray);
			rcRed = CRect(nBtnLeft, 2*nMargY+nBtHeight, nBtnRight, 2*nMargY+2*nBtHeight);
			if (m_Red.m_hWnd) m_Red.MoveWindow(rcRed);
			rcGreen = CRect(nBtnLeft, 3*nMargY+2*nBtHeight, nBtnRight, 3*nMargY+3*nBtHeight);
			if (m_Green.m_hWnd) m_Green.MoveWindow(rcGreen);
			rcBlue = CRect(nBtnLeft, 4*nMargY+3*nBtHeight, nBtnRight, 4*nMargY+4*nBtHeight);
			if (m_Blue.m_hWnd) m_Blue.MoveWindow(rcBlue);
			rcMsg = CRect(cx-nBtWidth-nMargX, 5*nMargY+4*nBtHeight, cx-nMargX, 5*nMargY+6*nBtHeight);
			if (m_Msg.m_hWnd) m_Msg.MoveWindow(rcMsg);
			rcHisto = CRect(nMargX, nTxtY, nBtnLeft-nMargX, cy-nTxtY);
			if (m_HistoBox.m_hWnd) m_HistoBox.MoveWindow(rcHisto);
			CRect rc = CRect(0,0,rcHisto.Width()+nMargX,nTxtY);
			if (m_MaxPoints.m_hWnd) m_MaxPoints.MoveWindow(rc);
			rc = CRect(0,cy-nTxtY,nMargX+rcHisto.Width()/2,cy);
			if (m_Static0.m_hWnd) m_Static0.MoveWindow(rc);
			rc = CRect(nMargX+rcHisto.Width()/2,cy-nTxtY,nMargX+rcHisto.Width(),cy);
			if (m_MaxColor.m_hWnd) m_MaxColor.MoveWindow(rc);
			rc = CRect(nBtnRight+nMargX, 2*nMargY+2*nBtHeight, nBtnRight+nMargX+nBtWidth,
				2*nMargY+3*nBtHeight);
			if (m_Period.m_hWnd != NULL)
				m_Period.MoveWindow(rc);
		}
	}
}

void CHistoDlg::SetMaxColor()
{
	int nMaxColor = 255;
	CString s;
	s.Format("%d", nMaxColor);
	m_MaxColor.SetWindowText(s);
}

BOOL CHistoDlg::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	if (nCode == BN_CLICKED && nID > 0)
	{
		if (nID == m_IDs.m_nIdGray)
		{
			OnGray();
			return TRUE;
		}
		else if (nID == m_IDs.m_nIdRed)
		{
			OnRed();			
			return TRUE;
		}
		else if (nID == m_IDs.m_nIdGreen)
		{
			OnGreen();
			return TRUE;
		}
		else if (nID == m_IDs.m_nIdBlue)
		{
			OnBlue();
			return TRUE;
		}		

		
	}

	return __super::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

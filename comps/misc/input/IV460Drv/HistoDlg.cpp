// HistoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "HistoDlg.h"
#include "SettingsSheet.h"
#include "StdProfile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHistoDlg dialog


CHistoDlg::CHistoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHistoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHistoDlg)
	m_bGray = g_StdProf._GetProfileInt(_T("Histogram"), _T("ShowGray"), TRUE);
	m_bRed = g_StdProf._GetProfileInt(_T("Histogram"), _T("ShowRed"), TRUE);
	m_bGreen = g_StdProf._GetProfileInt(_T("Histogram"), _T("ShowGreen"), TRUE);
	m_bBlue = g_StdProf._GetProfileInt(_T("Histogram"), _T("ShowBlue"), TRUE);
	//}}AFX_DATA_INIT
	m_rcGray = m_rcRed = m_rcGreen = m_rcBlue = CRect(0,0,0,0);
}


void CHistoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHistoDlg)
	DDX_Control(pDX, IDC_STATIC_MSG, m_Msg);
	DDX_Control(pDX, IDC_BLUE, m_Blue);
	DDX_Control(pDX, IDC_GREEN, m_Green);
	DDX_Control(pDX, IDC_RED, m_Red);
	DDX_Control(pDX, IDC_GRAY, m_Gray);
	DDX_Control(pDX, IDC_HISTO, m_HistoBox);
	DDX_Check(pDX, IDC_GRAY, m_bGray);
	DDX_Check(pDX, IDC_RED, m_bRed);
	DDX_Check(pDX, IDC_GREEN, m_bGreen);
	DDX_Check(pDX, IDC_BLUE, m_bBlue);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHistoDlg, CDialog)
	//{{AFX_MSG_MAP(CHistoDlg)
	ON_BN_CLICKED(IDC_BLUE, OnBlue)
	ON_BN_CLICKED(IDC_GRAY, OnGray)
	ON_BN_CLICKED(IDC_GREEN, OnGreen)
	ON_BN_CLICKED(IDC_RED, OnRed)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHistoDlg message handlers

void CHistoDlg::OnBlue() 
{
	InitShow();
	g_StdProf.WriteProfileInt(_T("Histogram"), _T("ShowBlue"), m_bBlue);
}

void CHistoDlg::OnGray() 
{
	InitShow();
	g_StdProf.WriteProfileInt(_T("Histogram"), _T("ShowGray"), m_bGray);
}

void CHistoDlg::OnGreen() 
{
	InitShow();
	g_StdProf.WriteProfileInt(_T("Histogram"), _T("ShowGreen"), m_bGreen);
}

void CHistoDlg::OnRed() 
{
	InitShow();
	g_StdProf.WriteProfileInt(_T("Histogram"), _T("ShowRed"), m_bRed);
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
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CHistoDlg::ResetMinMax()
{
	CString s;
	if (m_HistoBox.m_nMin >= 0 && m_HistoBox.m_nMax >= 0)
		s.Format(m_sMsg, m_HistoBox.m_nMin, m_HistoBox.m_nMax, m_HistoBox.m_nAvr);
	m_Msg.SetWindowText(s);
}


void CHistoDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	if (m_hWnd)
	{
		int nMargX = 5, nMargY = 5;
		int nBtWidth = m_rcGray.Width();
		if (cx > 3*nMargX+nBtWidth && cy > 2*nMargY)
		{
			CRect rcHisto, rcGray, rcRed, rcGreen, rcBlue, rcMsg;
			int nBtHeight = m_rcGray.Height();
			rcGray = CRect(cx-nBtWidth-nMargX, nMargY, cx-nMargX, nMargY+nBtHeight);
			if (m_Gray.m_hWnd) m_Gray.MoveWindow(rcGray);
			rcRed = CRect(cx-nBtWidth-nMargX, 2*nMargY+nBtHeight, cx-nMargX, 2*nMargY+2*nBtHeight);
			if (m_Red.m_hWnd) m_Red.MoveWindow(rcRed);
			rcGreen = CRect(cx-nBtWidth-nMargX, 3*nMargY+2*nBtHeight, cx-nMargX, 3*nMargY+3*nBtHeight);
			if (m_Green.m_hWnd) m_Green.MoveWindow(rcGreen);
			rcBlue = CRect(cx-nBtWidth-nMargX, 4*nMargY+3*nBtHeight, cx-nMargX, 4*nMargY+4*nBtHeight);
			if (m_Blue.m_hWnd) m_Blue.MoveWindow(rcBlue);
			rcMsg = CRect(cx-nBtWidth-nMargX, 5*nMargY+4*nBtHeight, cx-nMargX, 5*nMargY+6*nBtHeight);
			if (m_Msg.m_hWnd) m_Msg.MoveWindow(rcMsg);
			rcHisto = CRect(nMargX, nMargY, cx-nBtWidth-2*nMargX, cy-nMargY);
			if (m_HistoBox.m_hWnd) m_HistoBox.MoveWindow(rcHisto);			
		}
	}
}

void CHistoDlg::UpdateControls(int nHint)
{
	/*if (nHint == ESH_Camera_Changed && m_hWnd)
	{
		CSettingsSheet *pSheet = (CSettingsSheet*)GetParent();
		int nCamera = pSheet->m_nCamera;
		BOOL bTrueColor = nCamera == 1 || nCamera == 3 || nCamera == 5 || nCamera == 7;
		m_Red.EnableWindow(bTrueColor);
		m_Green.EnableWindow(bTrueColor);
		m_Blue.EnableWindow(bTrueColor);
		m_HistoBox.m_fTrueColor = bTrueColor;
	}*/
}



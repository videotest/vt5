// HistoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "baumer.h"
#include "CxLibWork.h"
#include "HistoDlg.h"
#include "SettingsSheet.h"

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
	m_bGray = g_BaumerProfile._GetProfileInt(_T("Histogram"), _T("ShowGray"), TRUE);
	m_bRed = g_BaumerProfile._GetProfileInt(_T("Histogram"), _T("ShowRed"), TRUE);
	m_bGreen = g_BaumerProfile._GetProfileInt(_T("Histogram"), _T("ShowGreen"), TRUE);
	m_bBlue = g_BaumerProfile._GetProfileInt(_T("Histogram"), _T("ShowBlue"), TRUE);
	//}}AFX_DATA_INIT
	m_rcGray = m_rcRed = m_rcGreen = m_rcBlue = CRect(0,0,0,0);
}


void CHistoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHistoDlg)
	DDX_Control(pDX, IDC_STATIC_0, m_Static0);
	DDX_Control(pDX, IDC_STATIC_MAX_COLOR, m_MaxColor);
	DDX_Control(pDX, IDC_STATIC_MAX_POINTS, m_MaxPoints);
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
	g_BaumerProfile.WriteProfileInt(_T("Histogram"), _T("ShowBlue"), m_bBlue);
}

void CHistoDlg::OnGray() 
{
	InitShow();
	g_BaumerProfile.WriteProfileInt(_T("Histogram"), _T("ShowGray"), m_bGray);
}

void CHistoDlg::OnGreen() 
{
	InitShow();
	g_BaumerProfile.WriteProfileInt(_T("Histogram"), _T("ShowGreen"), m_bGreen);
}

void CHistoDlg::OnRed() 
{
	InitShow();
	g_BaumerProfile.WriteProfileInt(_T("Histogram"), _T("ShowRed"), m_bRed);
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
	CString s;
	if (m_HistoBox.m_nMin >= 0 && m_HistoBox.m_nMax >= 0)
		s.Format(m_sMsg, m_HistoBox.m_nMin, m_HistoBox.m_nMax, m_HistoBox.m_nAvr);
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
		if (cx > 3*nMargX+nBtWidth && cy > 2*nTxtY)
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
			rcMsg = CRect(cx-nBtWidth-nMargX, 5*nMargY+4*nBtHeight, cx-nMargX, 5*nMargY+4*nBtHeight+m_rcMsg.Height());
			if (m_Msg.m_hWnd) m_Msg.MoveWindow(rcMsg);
			rcHisto = CRect(nMargX, nTxtY, cx-nBtWidth-2*nMargX, cy-nTxtY);
			if (m_HistoBox.m_hWnd) m_HistoBox.MoveWindow(rcHisto);
			CRect rc = CRect(0,0,rcHisto.Width()+nMargX,nTxtY);
			if (m_MaxPoints.m_hWnd) m_MaxPoints.MoveWindow(rc);
			rc = CRect(0,cy-nTxtY,nMargX+rcHisto.Width()/2,cy);
			if (m_Static0.m_hWnd) m_Static0.MoveWindow(rc);
			rc = CRect(nMargX+rcHisto.Width()/2,cy-nTxtY,nMargX+rcHisto.Width(),cy);
			if (m_MaxColor.m_hWnd) m_MaxColor.MoveWindow(rc);
		}
	}
}

void CHistoDlg::UpdateControls(int nHint)
{
	if (m_hWnd)
	{
		if (nHint == ESH_Camera_Changed)
		{
			CSettingsSheet *pSheet = (CSettingsSheet*)GetParent();
			int nCamera = pSheet->m_nCamera;
			BOOL bTrueColor = g_CxLibWork.IsCameraColor();
			m_Red.EnableWindow(bTrueColor);
			m_Green.EnableWindow(bTrueColor);
			m_Blue.EnableWindow(bTrueColor);
			m_HistoBox.m_fTrueColor = bTrueColor;
		}
		else if (nHint == ESH_BitMode)
			SetMaxColor();
	}
}


void CHistoDlg::SetMaxColor()
{
	int nMaxColor = g_CxLibWork.m_nBitsMode==W8BIT?0x100:g_CxLibWork.m_nBitsMode==W10BIT?0x400:
		g_CxLibWork.m_nBitsMode==W12BIT?0x1000:0x10000;
	CString s;
	s.Format("%d", nMaxColor);
	m_MaxColor.SetWindowText(s);
}


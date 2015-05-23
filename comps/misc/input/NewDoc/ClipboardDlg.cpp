// ClipboardDlg.cpp : implementation file
//

#include "stdafx.h"
#include "newdoc.h"
#include "ClipboardDlg.h"
#include "DriverClipboard.h"
#include "InputUtils.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CSize CalculateStringSize(HDC hDC, LPCTSTR lpstr)
{
	CRect rc(0,0,0,0);
	::DrawText(hDC, lpstr, -1, &rc, DT_SINGLELINE|DT_CALCRECT);
	int nWidth = ((int)sqrt(double(rc.Width())*rc.Height()))*4;
	rc.right = rc.left+nWidth;
	rc.bottom = 0;
	::DrawText(hDC, lpstr, -1, &rc, DT_WORDBREAK|DT_CALCRECT);
	return rc.Size();
};

/////////////////////////////////////////////////////////////////////////////
// CClipboardDlg dialog


CClipboardDlg::CClipboardDlg(CDriverClipboard *pDrv, CWnd* pParent /*=NULL*/)
	: CDialog(CClipboardDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CClipboardDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pDrv = pDrv;
	m_rcBase = CRect(0,0,0,0);
	m_nEnableOk = -1;
	m_sz = CSize(0,0);
}

bool CClipboardDlg::RepositionWindow()
{
	CClipboardDIB Dib;
	int nEnableOk = Dib.lpbi?1:0;
	if (m_nEnableOk == -1 || m_nEnableOk != nEnableOk)
	{
		if (nEnableOk)
		{
			::EnableWindow(::GetDlgItem(m_hWnd, IDOK), TRUE);
			::SetFocus(::GetDlgItem(m_hWnd, IDOK));
		}
		else
		{
			::SetFocus(::GetDlgItem(m_hWnd, IDCANCEL));
			::EnableWindow(::GetDlgItem(m_hWnd, IDOK), FALSE);
		}
		m_nEnableOk = nEnableOk;
	}
	CSize sz;
	if (Dib.lpbi)
		sz = CSize(Dib.lpbi->biWidth, Dib.lpbi->biHeight);
	else
	{
		CString s;
		s.LoadString(IDS_CLIPBOARD_MESSAGE);
		CClientDC cdc(this);
		sz = CalculateStringSize(cdc.m_hDC, s);
	}
	if (sz != m_sz)
	{
		m_sz = sz;
		CSize szTotal;
		Reposition(sz, m_rcBase, szTotal);
		CRect rcTotal(CPoint(0,0), szTotal);
		AdjustWindowRect(rcTotal,GetWindowLong(m_hWnd,GWL_STYLE),FALSE);
		SetWindowPos(NULL, 0, 0, rcTotal.Width(), rcTotal.Height(), SWP_NOZORDER|SWP_NOMOVE);
		CenterWindow();
		return true;
	}
	return false;
}

void CClipboardDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CClipboardDlg)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CClipboardDlg, CDialog)
	//{{AFX_MSG_MAP(CClipboardDlg)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
	ON_WM_PAINT()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClipboardDlg message handlers

BOOL CClipboardDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	::GetWindowRect(::GetDlgItem(m_hWnd, IDC_CLIPBOARD_PREVIEW), &m_rcBase);
	Init(m_hWnd, IDC_CLIPBOARD_PREVIEW, _MaxClientSize(this));
	RepositionWindow();
	SetTimer(1, 200, NULL);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CClipboardDlg::OnTimer(UINT_PTR nIDEvent) 
{
	if (RepositionWindow())
		InvalidateRect(NULL, TRUE);
	else
		InvalidateRect(m_rcBase, FALSE);
	CDialog::OnTimer(nIDEvent);
}

void CClipboardDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	KillTimer(1);
}

void CClipboardDlg::OnHelp() 
{
	HelpDisplay( "ClipboardDlg" );
}

void CClipboardDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	m_pDrv->OnDrawRect(dc.m_hDC, CRect(CPoint(0,0), m_rcBase.Size()), m_rcBase);
}

// EnterNameDlg.cpp : implementation file
//

#include "stdafx.h"
#include "shell.h"
#include "EnterNameDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CEnterNameDlg dialog


CEnterNameDlg::CEnterNameDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEnterNameDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEnterNameDlg)
	m_strName = _T("New Player");
	//}}AFX_DATA_INIT
}


void CEnterNameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEnterNameDlg)
	DDX_Text(pDX, IDC_NAME, m_strName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEnterNameDlg, CDialog)
	//{{AFX_MSG_MAP(CEnterNameDlg)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEnterNameDlg message handlers

void CEnterNameDlg::OnOK() 
{
	if( !UpdateData() )
		return;
	if( m_strName.IsEmpty() )
	{
		AfxMessageBox( "Wrong name" );
		return;
	}
	CDialog::OnOK();
}

void CEnterNameDlg::OnHelp() 
{
	HelpDisplay( "EnterNameDlg" );	
}

// CWaitDlg dialog

IMPLEMENT_DYNAMIC(CWaitDlg, CDialog)
CWaitDlg::CWaitDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWaitDlg::IDD, pParent)
{
	m_hBitmap =0;
}

CWaitDlg::~CWaitDlg()
{
	if(m_hBitmap) DeleteObject(m_hBitmap);
}

void CWaitDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CWaitDlg, CDialog)
	ON_WM_TIMER( )
	ON_WM_PAINT( )
END_MESSAGE_MAP()


// CWaitDlg message handlers


BOOL CWaitDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_timerID = SetTimer(1, 30, 0);
	m_hBitmap = LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_SLIDE));
	
	GetDlgItem(IDC_PLEASE_WAIT)->GetWindowRect(m_rectAnime);
	ScreenToClient(m_rectAnime);
	m_rectAnime.top = m_rectAnime.bottom+2;
	m_rectAnime.bottom = m_rectAnime.top+8;	
	m_drawRect.SetRect(m_rectAnime.left-46, m_rectAnime.top, 0, m_rectAnime.bottom);
	m_redrawRect = m_drawRect;

	CString s;
	s.Format(IDS_LOCATING,m_path);
	SetDlgItemText(IDC_PLEASE_WAIT,s); 
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CWaitDlg::OnTimer( UINT_PTR timerId)
{
	if(IsWindowVisible())
	{
		CDC* dc = GetDC();
		CRgn rgn;
		rgn.CreateRectRgn(m_rectAnime.left, m_rectAnime.top, m_rectAnime.right, m_rectAnime.bottom);
		dc->SelectClipRgn(&rgn,RGN_AND);

		CDC memDC;
		memDC.CreateCompatibleDC(dc);
		CRect dr; 
		dr.IntersectRect(m_redrawRect,m_rectAnime);
		HBRUSH hBrush = CreateSolidBrush(RGB(0,133,206));

		FillRect(dc->m_hDC, m_rectAnime, hBrush);
		DeleteObject(hBrush);
		
		m_drawRect.OffsetRect(10,0);
		if(m_drawRect.left>=m_rectAnime.right) m_drawRect.SetRect(-46, m_rectAnime.top, 0, m_rectAnime.bottom);		
		dr.IntersectRect(m_drawRect,m_rectAnime); 
		m_redrawRect = dr;


		HBITMAP old =(HBITMAP) memDC.SelectObject(m_hBitmap);
		dc->BitBlt(dr.left, dr.top, dr.Width(),dr.Height(), &memDC, 0 ,0, SRCCOPY);
		memDC.SelectObject(old);

		ReleaseDC(dc);
	}
}

void CWaitDlg::OnPaint()
{
	CPaintDC dc(this);
	FillRect(dc.m_hDC, m_rectAnime, (HBRUSH)GetStockObject(BLACK_BRUSH));	
}

void CWaitDlg::OnOK()
{


	CDialog::OnOK();
	DestroyWindow();
}

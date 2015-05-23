// AboutDlg.cpp : implementation file
//

#include "stdafx.h"
#include "shell.h"
#include "AboutDlg.h"
#include "tetrisdlg.h"
#include "\vt5\awin\misc_string.h"

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog


CAboutDlg::CAboutDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAboutDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAboutDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg message handlers

/////////////////////////////////////////////////////////////////////////////
_string GetFileVersion( char* psz_file )
{
	if( !psz_file )
		return _T("");

	DWORD dw_size = GetFileVersionInfoSize( psz_file, 0 );
	if( !dw_size )
		return _T("");

	BYTE* pbuf = new BYTE[dw_size];
	if( !pbuf )
		return _T("");

	_char sz_ver[256];	sz_ver[0] = 0;
	if( GetFileVersionInfo( psz_file, 0, dw_size, pbuf ) ) 
	{
		VS_FIXEDFILEINFO* pffi = 0;
		UINT     uFfiLen = 0;
		if( VerQueryValue( pbuf, "\\", (void**)&pffi, &uFfiLen ) )
		{
			WORD w0 = HIWORD( pffi->dwFileVersionMS );
			WORD w1 = LOWORD( pffi->dwFileVersionMS );
			WORD w2 = HIWORD( pffi->dwFileVersionLS );
			WORD w3 = LOWORD( pffi->dwFileVersionLS );
			
			sprintf( sz_ver, "%d.%d.%d.%d", w0, w1, w2, w3 );								
		}
	}
	delete pbuf;	pbuf = 0;

	return sz_ver;
}

BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	HWND hwnd = ::GetDlgItem( GetSafeHwnd(), IDC_VERSION );
	if( hwnd )
	{
		char sz_buf[512];	sz_buf[0] = 0;
		::GetWindowText( hwnd, sz_buf, sizeof(sz_buf) );

		char sz_file_name[MAX_PATH];	sz_file_name[0] = 0;
		::GetModuleFileName( 0, sz_file_name, sizeof(sz_file_name) );
		_string str_version = GetFileVersion( sz_file_name );
		if( !str_version.length() )
			str_version = _T("5.0");

		strcat( sz_buf, (char*)str_version );
		::SetWindowText( hwnd, sz_buf );
	}


	CString	str = ::GetValueString( GetAppUnknown(), "\\General", "OEM", "" );

	CFont	*pfont = GetFont();
	LOGFONT	lf;	
	pfont->GetLogFont( &lf );

	lf.lfWeight = FW_BOLD;
	lf.lfHeight = 24;
	lf.lfWidth = 0;
	strcpy( lf.lfFaceName, "Arial" );
	
	m_fontTitle.CreateFontIndirect( &lf );
	lf.lfWeight = FW_NORMAL;
	lf.lfHeight = 12;
	lf.lfWidth = 0;

	m_fontSmall.CreateFontIndirect( &lf );

	GetDlgItem( IDC_TITLE )->SetFont( &m_fontTitle );

	GetDlgItem( IDC_TEXT1 )->SetFont( &m_fontSmall );
	GetDlgItem( IDC_TEXT2 )->SetFont( &m_fontSmall );
	GetDlgItem( IDC_TEXT3 )->SetFont( &m_fontSmall );
	GetDlgItem( IDC_TEXT4 )->SetFont( &m_fontSmall );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAboutDlg::OnOK() 
{
	if( GetAsyncKeyState( VK_CONTROL ) & 0x8000 )
	{
		CTetrisDlg	dlg( this );
		dlg.DoModal();
		return;
	}
	CDialog::OnOK();
}

void CAboutDlg::OnHelp() 
{
	HelpDisplay( "AboutDlg" );	
}

void CAboutDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	if( GetAsyncKeyState( VK_CONTROL ) & 0x8000 )
	{
		HWND hwnd = ::GetDlgItem( GetSafeHwnd(), IDC_VERSION );
		if( hwnd )
		{
			RECT rc = {0};
			::GetWindowRect( hwnd, &rc );
			::MapWindowPoints( ::GetDesktopWindow(), GetSafeHwnd(), (POINT*)&rc, 2 );

			if( ::PtInRect( &rc, point ) )
				::ExecuteAction( "ShowVersion", 0, 0 );
		}
	}
	CDialog::OnLButtonDown(nFlags, point);
}

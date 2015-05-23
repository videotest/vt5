// tetrisDlg.cpp : implementation file
//

#include "stdafx.h"
#include "shell.h"
#include "tetrisDlg.h"
#include "tetgame.h"
#include "tetwindow.h"
#include "EnterNameDlg.h"



/////////////////////////////////////////////////////////////////////////////
// CTetrisDlg dialog

CTetrisDlg::CTetrisDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTetrisDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTetrisDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_bFirstFillScopes = true;
}

CTetrisDlg::~CTetrisDlg()
{
}

void CTetrisDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTetrisDlg)
	DDX_Control(pDX, IDC_TABLE, m_list);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTetrisDlg, CDialog)
	//{{AFX_MSG_MAP(CTetrisDlg)
	ON_BN_CLICKED(IDC_NEW, OnNew)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_TET_SCOPES, OnUpdateScope)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTetrisDlg message handlers

BOOL CTetrisDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CRect	rect;
	GetDlgItem( IDC_TETPLACE )->GetWindowRect( &rect );
	GetDlgItem( IDC_TETPLACE )->ShowWindow( SW_HIDE );
	ScreenToClient( &rect );
	m_window.Create( ::AfxRegisterWndClass( 0, 0, ::GetSysColorBrush( COLOR_3DFACE ), 0 ), 0, WS_CHILD|WS_VISIBLE|WS_TABSTOP, rect, this, 1001 ); 
	//m_window.SetFocus();

	OnUpdateScope(0, 0);

	LoadScopeTable();
	
	return FALSE;  // return TRUE  unless you set the focus to a control
}

LRESULT CTetrisDlg::OnUpdateScope( WPARAM wParam, LPARAM )
{
	if( !wParam )
	{
		CString	str;
		str.Format( "%d", m_window.GetGame()->GetLines() );
		GetDlgItem( IDC_LINES )->SetWindowText( str );
		str.Format( "%d", m_window.GetGame()->GetScope() );
		GetDlgItem( IDC_SCOPE )->SetWindowText( str );
		str.Format( "%d", m_window.GetGame()->GetLevel() );
		GetDlgItem( IDC_LEVEL )->SetWindowText( str );
	}
	else
	{
		AddHiScope();
	}
	return 0;
}

void CTetrisDlg::AddHiScope()
{
	CEnterNameDlg	dlg( this );
	if( dlg.DoModal() != IDOK )
		return;

	for( int i = 0; i < SCOPE_COUNT; i++ )
		if( m_window.GetGame()->GetScope() >= m_table[i].nScope )
			break;
	if( i == SCOPE_COUNT )
		return;

	for( int j = SCOPE_COUNT-2; j >= i; j-- )
		memcpy( &m_table[j+1], &m_table[j], sizeof( m_table[0] ) );

	m_table[i].nLevel = m_window.GetGame()->GetLevel();
	m_table[i].nLines = m_window.GetGame()->GetLines();
	m_table[i].nScope = m_window.GetGame()->GetScope();
	strcpy( m_table[i].szName, dlg.m_strName );

	StoreScopeTable();
	UpdateScopeTable();
}

void CTetrisDlg::UpdateScopeTable()
{
	if( m_bFirstFillScopes )
	{
		m_list.InsertColumn( 0, "Pos", LVCFMT_RIGHT, 30 );
		m_list.InsertColumn( 1, "Playes", LVCFMT_LEFT, 100 );
		m_list.InsertColumn( 2, "Scope", LVCFMT_RIGHT, 50 );
		m_list.InsertColumn( 3, "Level", LVCFMT_RIGHT, 20 );
		m_list.InsertColumn( 4, "Lines", LVCFMT_RIGHT, 30 );
	}
	else
		m_list.DeleteAllItems();

	m_bFirstFillScopes = false;

	for( int i = 0; i < SCOPE_COUNT; i++ )
	{
		CString	str;
		str.Format( "%d", i+1 );
		m_list.InsertItem( i, str, -1 );
		m_list.SetItemText( i, 1, m_table[i].szName );
		str.Format( "%d", m_table[i].nScope );
		m_list.SetItemText( i, 2, str );
		str.Format( "%d", m_table[i].nLevel );
		m_list.SetItemText( i, 3, str );
		str.Format( "%d", m_table[i].nLines );
		m_list.SetItemText( i, 4, str );
	}
}

void CTetrisDlg::LoadScopeTable()
{
	ZeroMemory( m_table, sizeof( m_table ) );

	for( int i = 0; i < SCOPE_COUNT; i++ )
		strcpy( m_table[i].szName, "no player" );

	char	szFileName[_MAX_PATH];
	::GetModuleFileName( AfxGetApp()->m_hInstance, szFileName, _MAX_PATH );
	::GetLongPathName( szFileName, szFileName, _MAX_PATH ) ;
	char	*pfName = ::strrchr( szFileName, '\\' );
	strcpy( pfName, "\\tetris.scopes" ); 

	FILE	*pfile= ::fopen( szFileName, "rb" );
	if( pfile )
	{
		::fread( m_table, sizeof( m_table ), 1, pfile );
		::fclose( pfile );
	}

	UpdateScopeTable();
}

void CTetrisDlg::StoreScopeTable()
{
	char	szFileName[_MAX_PATH];
	::GetModuleFileName( AfxGetApp()->m_hInstance, szFileName, _MAX_PATH );
	::GetLongPathName( szFileName, szFileName, _MAX_PATH ) ;
	char	*pfName = ::strrchr( szFileName, '\\' );
	strcpy( pfName, "\\tetris.scopes" ); 

	FILE	*pfile= ::fopen( szFileName, "wb" );
	if( pfile )
	{
		::fwrite( m_table, sizeof( m_table ), 1, pfile );
		::fclose( pfile );
	}
}

void CTetrisDlg::OnNew() 
{
	m_window.GetGame()->NewGame();	
	m_window.SetFocus();
}

void CTetrisDlg::OnHelp() 
{
	HelpDisplay( "TetrisDlg" );
}

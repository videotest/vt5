// NumberCtrlDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PrintFieldAX.h"
#include "NumberCtrlDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNumberCtrlDlg dialog


CNumberCtrlDlg::CNumberCtrlDlg(CWnd* pParent, print_field_params *pparams )
	: CDialog(CNumberCtrlDlg::IDD, pParent)
{
	m_pparams = pparams;
	memcpy( &m_work, m_pparams, sizeof( m_work ) );
}


void CNumberCtrlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNumberCtrlDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNumberCtrlDlg, CDialog)
	//{{AFX_MSG_MAP(CNumberCtrlDlg)
	ON_BN_CLICKED(IDC_FLOAT, OnFloat)
	ON_BN_CLICKED(IDC_INTEGER, OnInteger)
	ON_BN_CLICKED(IDC_BOUNDS, OnChange)
	ON_BN_CLICKED(IDC_DEFAULT, OnChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNumberCtrlDlg message handlers

BOOL CNumberCtrlDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	bool	bFloat = (m_work.flags & PFF_NUMERIC_TYPE_DOUBLE)==PFF_NUMERIC_TYPE_DOUBLE;

	CheckRadioButton( IDC_INTEGER, IDC_FLOAT, bFloat?IDC_FLOAT:IDC_INTEGER );
	
	CheckDlgButton( IDC_BOUNDS, (m_work.flags&PFF_NUMERIC_STYLE_RANGES)==PFF_NUMERIC_STYLE_RANGES);
	CheckDlgButton( IDC_DEFAULT, (m_work.flags&PFF_NUMERIC_STYLE_DEFAULT)==PFF_NUMERIC_STYLE_DEFAULT);



	_load_data( bFloat );
	_update_controls();
	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CNumberCtrlDlg::OnOK() 
{
	if( !_store_data( true ) )return;

	memcpy( m_pparams, &m_work, sizeof( m_work ) );
	m_pparams->flags &= ~PFF_NUMERIC_STYLE_MASK;
	if( IsDlgButtonChecked( IDC_BOUNDS ) )
		m_pparams->flags |= PFF_NUMERIC_STYLE_RANGES;
	if( IsDlgButtonChecked( IDC_DEFAULT ) )
		m_pparams->flags |= PFF_NUMERIC_STYLE_DEFAULT;

	CDialog::OnOK();
}

void CNumberCtrlDlg::OnFloat() 
{
	_store_data( false );
	_load_data( true );
	
}

void CNumberCtrlDlg::OnInteger() 
{
	_store_data( false );
	_load_data( false );
	
}
void CNumberCtrlDlg::_load_data( bool bFloat )
{
	m_work.flags &= ~PFF_NUMERIC_TYPE_MASK;
	if( bFloat )m_work.flags |= PFF_NUMERIC_TYPE_DOUBLE;
	else m_work.flags |= PFF_NUMERIC_TYPE_INTEGER;

	CString	str_lo, str_hi, str_def;

	if( bFloat )
	{
		str_lo.Format( "%g", m_work.fmin );
		str_hi.Format( "%g", m_work.fmax );
		str_def.Format( "%g", m_work.fdef );
	}
	else
	{
		str_lo.Format( "%d", m_work.lmin );
		str_hi.Format( "%d", m_work.lmax );
		str_def.Format( "%d", m_work.ldef );
	}

	SetDlgItemText( IDC_LOW, str_lo );
	SetDlgItemText( IDC_HIGH, str_hi );
	SetDlgItemText( IDC_VALUE, str_def );
}
bool CNumberCtrlDlg::_store_data( bool btest )
{
	BOOL bBounds = IsDlgButtonChecked( IDC_BOUNDS );
	BOOL bDefault = IsDlgButtonChecked( IDC_DEFAULT );


	bool bFloat = (m_work.flags & PFF_NUMERIC_TYPE_DOUBLE) == PFF_NUMERIC_TYPE_DOUBLE;

	char	sz[100];
	GetDlgItemText( IDC_LOW, sz, sizeof( sz ) );
	if( bFloat )sscanf( sz, "%lf", &m_work.fmin );else sscanf( sz, "%d", &m_work.lmin );
	GetDlgItemText( IDC_HIGH, sz, sizeof( sz ) );
	if( bFloat )sscanf( sz, "%lf", &m_work.fmax );else sscanf( sz, "%d", &m_work.lmax );
	GetDlgItemText( IDC_VALUE, sz, sizeof( sz ) );
	if( bFloat )sscanf( sz, "%lf", &m_work.fdef );else sscanf( sz, "%d", &m_work.ldef );

	//paul 12.03.2003
	if( btest )
	{
		if( bBounds )
		{
			if( ( bFloat && m_work.fmin >= m_work.fmax ) || ( !bFloat && m_work.lmin >= m_work.lmax ) )
			{				
				AfxMessageBox( IDS_INVALIDRANGE );
				HWND hwnd = ::GetDlgItem( GetSafeHwnd(), IDC_LOW );
				::SendMessage( hwnd, EM_SETSEL, 0, -1 );
				::SetFocus( hwnd );
				return false;
			}
		}
		if( bBounds && bDefault )
		{			
			if( !(bFloat && m_work.fmin <= m_work.fdef && m_work.fmax >= m_work.fdef ) || 
				!(!bFloat && m_work.lmin <= m_work.ldef && m_work.lmax >= m_work.ldef ) )
				{						
					AfxMessageBox( IDS_INVALIDDEFVALUE );
					HWND hwnd = ::GetDlgItem( GetSafeHwnd(), IDC_VALUE );
					::SendMessage( hwnd, EM_SETSEL, 0, -1 );
					::SetFocus( hwnd );
					return false;
				}
		}
	}
	return true;
}

void CNumberCtrlDlg::_update_controls()
{
	BOOL bBounds = IsDlgButtonChecked( IDC_BOUNDS );
	BOOL bDefault = IsDlgButtonChecked( IDC_DEFAULT );

	GetDlgItem( IDC_LOW_STATIC )->EnableWindow( bBounds );
	GetDlgItem( IDC_LOW )->EnableWindow( bBounds );
	GetDlgItem( IDC_HIGH_STATIC )->EnableWindow( bBounds );
	GetDlgItem( IDC_HIGH )->EnableWindow( bBounds );

	GetDlgItem( IDC_VALUE )->EnableWindow( bDefault );
}

void CNumberCtrlDlg::OnChange() 
{
	_update_controls();	
}

// pages.cpp : implementation file
//

#include "stdafx.h"
#include "pages.h"
#include "PropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCropPageDlg dialog
IMPLEMENT_DYNCREATE(CCropPageDlg, CDialog);

// {43933FFD-8254-404e-8BEE-D8623F653B41}
GUARD_IMPLEMENT_OLECREATE(CCropPageDlg, szImageCropPropPageProgID, 
0x43933ffd, 0x8254, 0x404e, 0x8b, 0xee, 0xd8, 0x62, 0x3f, 0x65, 0x3b, 0x41);


CCropPageDlg::CCropPageDlg(): CPropertyPageBase(CCropPageDlg::IDD)
{
	//{{AFX_DATA_INIT(CCropPageDlg)
	//}}AFX_DATA_INIT
	m_nWidth = 100;
	m_nHeight = 100;
	m_sName = c_szCCropPageDlg;
	m_sUserName.LoadString( IDS_CROP_DLG );
	_OleLockApp( this );

	m_bUseCalibration = false;
	m_bDisableNotify = false;
}

CCropPageDlg::~CCropPageDlg()
{
	_OleUnlockApp( this );
}


void CCropPageDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageBase::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCropPageDlg)
	DDX_Control(pDX, IDC_WIDTH, m_editWidth);
	DDX_Control(pDX, IDC_HEIGH, m_editHeight);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCropPageDlg, CPropertyPageBase)
	//{{AFX_MSG_MAP(CCropPageDlg)
	ON_WM_VSCROLL()
	ON_EN_CHANGE(IDC_HEIGH, OnChangeSizes)
	ON_EN_CHANGE(IDC_WIDTH, OnChangeSizes)
	ON_BN_CLICKED(IDC_RADIO_CALIBR, OnRadioCalibr)
	ON_BN_CLICKED(IDC_RADIO_PIXEL, OnRadioCalibr)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCropPageDlg message handlers

void CCropPageDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CPropertyPageBase::OnVScroll(nSBCode, nPos, pScrollBar);

	_UpdateValues();

	UpdateAction();
}

void CCropPageDlg::OnChangeSizes() 
{
	static bool bRet = false;
	if( bRet )
		return;
	if( !IsInitialized() || m_bDisableNotify )
		return;

	CString	strWidth, strHeight;
	m_editWidth.GetWindowText( strWidth );
	m_editHeight.GetWindowText( strHeight );

	if( _fixx( strHeight ) )
	{
		bRet = true;
		int nS, nE;

		m_editHeight.GetSel( nS, nE );
		m_editHeight.SetWindowText( strHeight );
		m_editHeight.SetSel( nS - 1, nE - 1 );

		bRet = false;
	}
	if( _fixx( strWidth ) )
	{
		bRet = true;
		int nS, nE;
		m_editWidth.GetSel( nS, nE );
		m_editWidth.SetWindowText( strWidth );
		m_editHeight.SetSel( nS - 1, nE - 1 );

		bRet = false;
	}

    double	w, h;

	if( ::sscanf( strHeight, "%lf", &h ) == 1 ) m_nHeight = h;
	if( ::sscanf( strWidth, "%lf", &w ) == 1 )  m_nWidth  = w;

	if( m_nHeight < 0 )
	{
		bRet = true;
		m_nHeight *= -1;
		m_editHeight.SetWindowText( strHeight.Right( strHeight.GetLength() - 1 ) );
		m_editHeight.SetSel( strHeight.GetLength() - 1, strHeight.GetLength() - 1 );
		bRet = false;
	}
	else if( m_nHeight == 0 || strHeight.IsEmpty() )
	{
		bRet = true;
		m_nHeight = 100;
		strHeight.Format( "%d", (int)m_nHeight );
		m_editHeight.SetWindowText( strHeight );
		m_editHeight.SetSel( strHeight.GetLength() - 1, strHeight.GetLength() - 1 );
		bRet = false;
	}

	if( m_nWidth < 0 )
	{
		bRet = true;
		m_nWidth *= -1;
		m_editWidth.SetWindowText( strWidth.Right( strWidth.GetLength() - 1 ) );
		m_editWidth.SetSel( strWidth.GetLength() - 1, strWidth.GetLength() - 1 );
		bRet = false;
	}
	else if( m_nWidth == 0 || strWidth.IsEmpty()  )
	{
		bRet = true;
		m_nWidth = 100;
		strWidth.Format( "%d", (int)m_nHeight );
		m_editWidth.SetWindowText( strWidth );
		m_editWidth.SetSel( strWidth.GetLength() - 1, strWidth.GetLength() - 1 );
		bRet = false;
	}

	_UpdateValues();

	UpdateAction();
}

bool CCropPageDlg::_fixx( CString &str )
{
	CString ret;
	int nSz = str.GetLength();
	for( int i = 0; i < nSz; i++ )
	{
		if( str[i] >= '0' && str[i] <= '9' || str[i] == '.' )
			ret += str[i];
	}
	bool bRet = bool( ret != str ); 
	str = ret;
	return bRet;
}

BOOL CCropPageDlg::OnInitDialog() 
{
	CPropertyPageBase::OnInitDialog();

	m_nWidth = ::GetValueDouble( ::GetAppUnknown(), "\\Crop", "Width", m_nWidth );
	m_nHeight = ::GetValueDouble( ::GetAppUnknown(), "\\Crop", "Height", m_nHeight );
	m_bUseCalibration = ::GetValueInt( ::GetAppUnknown(), "Crop", "UseCalibration", 0 ) == 1;

	((CSpinButtonCtrl*)GetDlgItem( IDC_SPIN_HEIGHT ))->SetRange( 1, 1000 );
	((CSpinButtonCtrl*)GetDlgItem( IDC_SPIN_WIDTH ))->SetRange( 1, 1000 );
	
	((CButton*)GetDlgItem(m_bUseCalibration?IDC_RADIO_CALIBR:IDC_RADIO_PIXEL))->SetCheck(1);

	_UpdateEditors();
	_UpdateValues();

	UpdateAction();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCropPageDlg::_UpdateEditors()
{
	char	sz_h[20], sz_w[20];

	if( m_bUseCalibration )
	{
		GetDlgItem( IDC_SPIN_HEIGHT )->ShowWindow( false );
		GetDlgItem( IDC_SPIN_WIDTH )->ShowWindow( false );
		sprintf( sz_w, "%g", m_nWidth );
		sprintf( sz_h, "%g", m_nHeight );
	}
	else
	{
		GetDlgItem( IDC_SPIN_HEIGHT )->ShowWindow( true );
		GetDlgItem( IDC_SPIN_WIDTH )->ShowWindow( true );
		m_bDisableNotify = true;
		((CSpinButtonCtrl*)GetDlgItem( IDC_SPIN_HEIGHT ))->SetPos( (int)m_nHeight );
		((CSpinButtonCtrl*)GetDlgItem( IDC_SPIN_WIDTH ))->SetPos( (int)m_nWidth );
		m_bDisableNotify = false;
		sprintf( sz_w, "%d", (int)m_nWidth );
		sprintf( sz_h, "%d", (int)m_nHeight );
	}

	
	
	m_editWidth.SetWindowText( sz_w );
	m_editHeight.SetWindowText( sz_h );
}

void CCropPageDlg::_UpdateValues()
{
	::SetValue( ::GetAppUnknown(), "Crop", "Width", m_nWidth );
	::SetValue( ::GetAppUnknown(), "Crop", "Height", m_nHeight );
}


void CCropPageDlg::OnRadioCalibr() 
{
	// TODO: Add your control notification handler code here
	m_bUseCalibration = ((CButton*)GetDlgItem(IDC_RADIO_CALIBR))->GetCheck() == 1;

	_UpdateEditors();
	::SetValue(::GetAppUnknown(), "Crop", "UseCalibration", _variant_t((long)(m_bUseCalibration?1:0)));
	UpdateAction();
}

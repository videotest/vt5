// ComparePage.cpp : implementation file
//

#include "stdafx.h"
#include "Compare.h"
#include "ComparePage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "PropPage.h"

/////////////////////////////////////////////////////////////////////////////
// CComparePage property page

IMPLEMENT_DYNCREATE(CComparePage, CDialog)

// {2D7CDC79-F1AB-4837-A5F6-D361FE245EE3}
GUARD_IMPLEMENT_OLECREATE(CComparePage, szComparePropPageProgID, 
0x2d7cdc79, 0xf1ab, 0x4837, 0xa5, 0xf6, 0xd3, 0x61, 0xfe, 0x24, 0x5e, 0xe3);

CComparePage::CComparePage() : CPropertyPageBase(CComparePage::IDD)
{
	//{{AFX_DATA_INIT(CComparePage)
	m_nAngle = 0;
	m_nWidth = 100;
	m_nXPos = 0;
	m_nYPos = 0;
	m_nHeight = 100;
	//}}AFX_DATA_INIT
	m_sName = c_szCCmpPage;
	m_sUserName.LoadString( IDS_PROPPAGE_COMPARE );
}

CComparePage::~CComparePage()
{
}

long GetSetLong( CWnd *pwnd, long &nVal, long nMin, long nMax, bool bSet )
{
	long	n = nVal;
	if( !bSet )
	{
		CString	str;
		pwnd->GetWindowText( str );

		if( !strlen( str ) || !strcmp( str, "-" ) )
			return max( nMin, min( nMax, 0 ) );

		if( !::sscanf( str, "%d", &n ) )
			n =  max( nMin, min( nMax, 0 ) );



		nVal = n;
	}

	nVal = max( nMin, min( nMax, nVal ) );

	if( bSet || n != nVal )
	{
		CString	str;
		str.Format( "%d", nVal );
		pwnd->SetWindowText( str );
	}

	return nVal;
}
long SafeStringToLong( const char *psz )
{
	if( !strlen( psz ) )return 0;
	if( !strcmp( psz, "-" ) )return 0;
	long	n;

	::sscanf( psz, "%d", &n );
	return n;
}

void CComparePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageBase::DoDataExchange(pDX);
	/*
	//{{AFX_DATA_MAP(CComparePage)
	DDX_Text(pDX, IDC_ANGLE, m_nAngle);
	DDX_Text(pDX, IDC_WIDTH_VAL, m_nWidth);
	DDX_Text(pDX, IDC_XPOS, m_nXPos);
	DDX_Text(pDX, IDC_YPOS, m_nYPos);
	DDX_Text(pDX, IDC_HEIGHT_VAL, m_nHeight);
	//}}AFX_DATA_MAP*/

	GetSetLong( GetDlgItem( IDC_ANGLE ), m_nAngle, -180, 180, pDX->m_bSaveAndValidate == false );
	GetSetLong( GetDlgItem( IDC_WIDTH_VAL ), m_nWidth, 1, 200, pDX->m_bSaveAndValidate == false );
	GetSetLong( GetDlgItem( IDC_HEIGHT_VAL ), m_nHeight, 1, 200, pDX->m_bSaveAndValidate == false );
	GetSetLong( GetDlgItem( IDC_XPOS ), m_nXPos, -1000, 1000, pDX->m_bSaveAndValidate == false );
	GetSetLong( GetDlgItem( IDC_YPOS ), m_nYPos, -1000, 1000, pDX->m_bSaveAndValidate == false );
}


BEGIN_MESSAGE_MAP(CComparePage, CPropertyPageBase)
	//{{AFX_MSG_MAP(CComparePage)
	ON_BN_CLICKED(IDC_DEFAULT, OnDefault)
	ON_EN_CHANGE(IDC_ANGLE, OnChangeEdit)
	ON_EN_CHANGE(IDC_HEIGHT_VAL, OnChangeEdit)
	ON_EN_CHANGE(IDC_WIDTH_VAL, OnChangeEdit)
	ON_EN_CHANGE(IDC_YPOS, OnChangeEdit)
	ON_EN_CHANGE(IDC_XPOS, OnChangeEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CComparePage message handlers

BOOL CComparePage::OnInitDialog() 
{
	CPropertyPageBase::OnInitDialog();
	
	DoLoadSettings();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CComparePage::DoLoadSettings()
{
	m_nAngle = ::GetValueInt( GetAppUnknown(), "Compare", "Angle", m_nAngle );
	m_nWidth = ::GetValueInt( GetAppUnknown(), "Compare", "Width", m_nWidth );
	m_nHeight = ::GetValueInt( GetAppUnknown(), "Compare", "Height", m_nHeight );
	m_nXPos = ::GetValueInt( GetAppUnknown(), "Compare", "XPos", m_nXPos );
	m_nYPos = ::GetValueInt( GetAppUnknown(), "Compare", "YPos", m_nYPos );

	UpdateData( false );
}

void CComparePage::DoStoreSettings()
{
	if( !UpdateData() )
		return;

	::SetValue( GetAppUnknown(), "Compare", "Angle", m_nAngle );
	::SetValue( GetAppUnknown(), "Compare", "Width", m_nWidth );
	::SetValue( GetAppUnknown(), "Compare", "Height", m_nHeight );
	::SetValue( GetAppUnknown(), "Compare", "XPos", m_nXPos );
	::SetValue( GetAppUnknown(), "Compare", "YPos", m_nYPos );
}

BEGIN_EVENTSINK_MAP(CComparePage, CPropertyPageBase)
    //{{AFX_EVENTSINK_MAP(CComparePage)
	ON_EVENT(CComparePage, IDC_DOWN, -600 /* Click */, OnClickDown, VTS_NONE)
	ON_EVENT(CComparePage, IDC_HEIGHT, -600 /* Click */, OnClickHeight, VTS_NONE)
	ON_EVENT(CComparePage, IDC_HEIGHT_DOWN, -600 /* Click */, OnClickHeightDown, VTS_NONE)
	ON_EVENT(CComparePage, IDC_LEFT, -600 /* Click */, OnClickLeft, VTS_NONE)
	ON_EVENT(CComparePage, IDC_RIGHT, -600 /* Click */, OnClickRight, VTS_NONE)
	ON_EVENT(CComparePage, IDC_ROTATE_LEFT, -600 /* Click */, OnClickRotateLeft, VTS_NONE)
	ON_EVENT(CComparePage, IDC_ROTATE_RIGHT, -600 /* Click */, OnClickRotateRight, VTS_NONE)
	ON_EVENT(CComparePage, IDC_UP, -600 /* Click */, OnClickUp, VTS_NONE)
	ON_EVENT(CComparePage, IDC_WIDTH, -600 /* Click */, OnClickWidth, VTS_NONE)
	ON_EVENT(CComparePage, IDC_WIDTH_DOWN, -600 /* Click */, OnClickWidthDown, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()


void CComparePage::OnClickUp() 
{
	UpdateData();
	m_nYPos = max( m_nYPos-1, -1000 );
	UpdateData( false );
	DoStoreSettings();
	UpdateAction();
}


void CComparePage::OnClickDown() 
{
	UpdateData();
	m_nYPos = min( m_nYPos+1, 1000 );
	UpdateData( false );
	DoStoreSettings();
	UpdateAction();
}

void CComparePage::OnClickHeight() 
{
	UpdateData();
	m_nHeight = min( m_nHeight+1, 200 );
	UpdateData( false );
	DoStoreSettings();
	UpdateAction();
}

void CComparePage::OnClickHeightDown() 
{
	UpdateData();
	m_nHeight = max( m_nHeight-1, 1 );
	UpdateData( false );
	DoStoreSettings();
	UpdateAction();
}

void CComparePage::OnClickLeft() 
{
	UpdateData();
	m_nXPos = max( m_nXPos-1, -1000 );
	UpdateData( false );
	DoStoreSettings();
	UpdateAction();
}

void CComparePage::OnClickRight() 
{
	UpdateData();
	m_nXPos = min( m_nXPos+1, 1000 );
	UpdateData( false );
	DoStoreSettings();
	UpdateAction();
}

void CComparePage::OnClickRotateLeft() 
{
	UpdateData();
	m_nAngle = min( m_nAngle+1, 180 );
	UpdateData( false );
	DoStoreSettings();
	UpdateAction();
}

void CComparePage::OnClickRotateRight() 
{
	UpdateData();
	m_nAngle = max( m_nAngle-1, -180 );
	UpdateData( false );
	DoStoreSettings();
	UpdateAction();
}


void CComparePage::OnClickWidth() 
{
	UpdateData();
	m_nWidth = min( m_nWidth+1, 200 );
	UpdateData( false );
	DoStoreSettings();
	UpdateAction();
}

void CComparePage::OnClickWidthDown() 
{
	UpdateData();
	m_nWidth = max( m_nWidth-1, 1 );
	UpdateData( false );
	DoStoreSettings();
	UpdateAction();
}

void CComparePage::OnDefault() 
{
	m_nAngle = 0;
	m_nWidth = 100;
	m_nXPos = 0;
	m_nYPos = 0;
	m_nHeight = 100;
	UpdateData( false );
	DoStoreSettings();
	UpdateAction();
}

void CComparePage::OnChangeEdit() 
{
	if( !UpdateData() )
		return;

	DoStoreSettings();
	UpdateAction();

	return;
}


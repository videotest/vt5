// ArrowPage.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "ArrowPage.h"
#include "editorint.h"

#include "PropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CArrowPage property page

IMPLEMENT_DYNCREATE(CArrowPage, CDialog)

// {2755A321-2CCD-423e-A6E4-17474A2E1763}
GUARD_IMPLEMENT_OLECREATE(CArrowPage, szEditorArrowPropPageProgID, 
0x2755a321, 0x2ccd, 0x423e, 0xa6, 0xe4, 0x17, 0x47, 0x4a, 0x2e, 0x17, 0x63);

CArrowPage::CArrowPage() : CPropertyPageBase(CArrowPage::IDD)
{
	//{{AFX_DATA_INIT(CArrowPage)
	m_nSize = 0;
	m_nWidth = 0;
	//}}AFX_DATA_INIT
	m_sName = c_szCArrowPage;
	m_sUserName.LoadString( IDS_PROPPAGE_ARROW );
	m_bInit = false;
}

CArrowPage::~CArrowPage()
{
}

void CArrowPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageBase::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CArrowPage)
	DDX_Text(pDX, IDC_ARROW_SIZE, m_nSize);
	DDX_Text(pDX, IDC_WIDTH, m_nWidth);
	DDX_Control(pDX, IDC_COLOR, m_clr);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CArrowPage, CPropertyPageBase)
	//{{AFX_MSG_MAP(CArrowPage)
	ON_BN_CLICKED(IDC_FIRST_ARROW, OnChange)
	ON_BN_CLICKED(IDC_FIRST_CIRCLE, OnChange)
	ON_BN_CLICKED(IDC_FIRST_NOTHING, OnChange)
	ON_BN_CLICKED(IDC_LAST_ARROW, OnChange)
	ON_BN_CLICKED(IDC_LAST_CIRCLE, OnChange)
	ON_BN_CLICKED(IDC_LAST_NOTHING, OnChange)
	ON_EN_CHANGE(IDC_ARROW_SIZE, OnChange)
	ON_EN_CHANGE(IDC_WIDTH, OnChange)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CArrowPage message handlers

BOOL CArrowPage::OnInitDialog() 
{
	CPropertyPageBase::OnInitDialog();
	m_bInit = true;
	
	_UpdateSettings( false );	

	((CSpinButtonCtrl*)GetDlgItem( IDC_SIZE_SPIN ))->SetRange( 5, 400 );
	((CSpinButtonCtrl*)GetDlgItem( IDC_WIDTH_SPIN ))->SetRange( 0, 20 );

	m_clr.SetColor(::GetValueColor(GetAppUnknown(), "Editor", "Fore", RGB(255,255,255)));

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CArrowPage::_UpdateSettings( bool bRead )
{
	if( !m_bInit )return;
	if( bRead )
	{
		UpdateData();

		DWORD	dwNewFlags = 0;
		if( IsDlgButtonChecked( IDC_FIRST_ARROW ) )
			dwNewFlags |= afFirstArrow;
		else if( IsDlgButtonChecked( IDC_FIRST_CIRCLE ) )
			dwNewFlags |= afFirstCircle;
		if( IsDlgButtonChecked( IDC_LAST_ARROW ) )
			dwNewFlags |= afLastArrow;
		else if( IsDlgButtonChecked( IDC_LAST_CIRCLE ) )
			dwNewFlags |= afLastCircle;

		
		::SetValue( ::GetAppUnknown(), "\\Editor\\Arrows", "Style", (long)dwNewFlags );
		::SetValue( ::GetAppUnknown(), "\\Editor\\Arrows", "Width", (long)m_nWidth );
		::SetValue( ::GetAppUnknown(), "\\Editor\\Arrows", "Size", (long)m_nSize );

		UpdateAction();
	}
	else
	{
		DWORD	dwArrowFlags = ::GetValueInt( ::GetAppUnknown(), "\\Editor\\Arrows", "Style", 0 );
		m_nWidth = ::GetValueInt( ::GetAppUnknown(), "\\Editor\\Arrows", "Width", 3 );
		m_nSize = ::GetValueInt( ::GetAppUnknown(), "\\Editor\\Arrows", "Size", 20 );

		if( dwArrowFlags & afFirstArrow )
			CheckRadioButton( IDC_FIRST_NOTHING, IDC_FIRST_CIRCLE, IDC_FIRST_ARROW );
		else if( dwArrowFlags & afFirstCircle )
			CheckRadioButton( IDC_FIRST_NOTHING, IDC_FIRST_CIRCLE, IDC_FIRST_CIRCLE );
		else
			CheckRadioButton( IDC_FIRST_NOTHING, IDC_FIRST_CIRCLE, IDC_FIRST_NOTHING );

		if( dwArrowFlags & afLastArrow )
			CheckRadioButton( IDC_LAST_NOTHING, IDC_LAST_CIRCLE, IDC_LAST_ARROW );
		else if( dwArrowFlags & afLastCircle )
			CheckRadioButton( IDC_LAST_NOTHING, IDC_LAST_CIRCLE, IDC_LAST_CIRCLE);
		else
			CheckRadioButton( IDC_LAST_NOTHING, IDC_LAST_CIRCLE, IDC_LAST_NOTHING );

		UpdateData( false );


	}
}

void CArrowPage::OnChange() 
{
	_UpdateSettings( true );
}

void CArrowPage::OnDestroy() 
{
	CPropertyPageBase::OnDestroy();
	

	m_bInit = false;
}

BEGIN_EVENTSINK_MAP(CArrowPage, CPropertyPageBase)
    //{{AFX_EVENTSINK_MAP(CArrowPage)
	ON_EVENT(CArrowPage, IDC_COLOR, 1 /* OnChange */, OnOnChangeColor, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CArrowPage::OnOnChangeColor() 
{
	_SetColor( m_clr.GetColor() );
	UpdateAction();
}

void CArrowPage::DoLoadSettings()
{
	if(!m_hWnd)
		return;
	m_clr.SetColor(::GetValueColor(GetAppUnknown(), "Editor", "Fore", RGB(255,255,255)));
}

void CArrowPage::_SetColor(COLORREF color)
{
	::SetValueColor(GetAppUnknown(), "Editor", "Fore", color);

	IUnknown* punkChooser = ::GetColorChooser();
	sptrIChooseColor	sptrChooser(punkChooser);
	if(punkChooser)
		punkChooser->Release();
	if(sptrChooser != 0)
		sptrChooser->SetLButtonColor((OLE_COLOR)color);
}

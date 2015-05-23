// ObjectEditorPage.cpp : implementation file
//

#include "stdafx.h"
#include "objects.h"
#include "ObjectEditorPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CObjectEditorPage property page
IMPLEMENT_DYNCREATE(CObjectEditorPage, CDialog)

// {64E42DC6-DE76-463a-9C3E-AA6B2EA29294}
GUARD_IMPLEMENT_OLECREATE(CObjectEditorPage, "Objects.ObjectEditorPage", 
0x64e42dc6, 0xde76, 0x463a, 0x9c, 0x3e, 0xaa, 0x6b, 0x2e, 0xa2, 0x92, 0x94);


CObjectEditorPage::CObjectEditorPage() :
	CPropertyPageBase(
		::GetValueInt(::GetAppUnknown(), "ObjectEditor", "UseRButton", 1) != 0 ?
		CObjectEditorPage::IDD:
		IDD_OBJECT_EDITOR2)
{
	//{{AFX_DATA_INIT(CObjectEditorPage)
	m_nObjectMin = 0;
	//}}AFX_DATA_INIT
	_OleLockApp( this );
	m_sName = c_szCObjectEditorPage;
	m_sUserName.LoadString( IDS_OBJECTEDITOR_TITLE );
}

CObjectEditorPage::~CObjectEditorPage()
{
	_OleUnlockApp( this );
}

void CObjectEditorPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageBase::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CObjectEditorPage)
	DDX_Control(pDX, IDC_BACK1, m_back1);
	DDX_Control(pDX, IDC_BACK2, m_back2);
	DDX_Control(pDX, IDC_FORE1, m_fore1);
	DDX_Control(pDX, IDC_FORE2, m_fore2);
	DDX_Text(pDX, IDC_EDIT_MIN_OBJECT, m_nObjectMin);
	DDV_MinMaxLong(pDX, m_nObjectMin, 0, 255);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CObjectEditorPage, CPropertyPageBase)
	//{{AFX_MSG_MAP(CObjectEditorPage)
	ON_EN_CHANGE(IDC_EDIT_MIN_OBJECT, OnChangeEditMinObject)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CObjectEditorPage message handlers

BEGIN_EVENTSINK_MAP(CObjectEditorPage, CPropertyPageBase)
    //{{AFX_EVENTSINK_MAP(CObjectEditorPage)
	ON_EVENT(CObjectEditorPage, IDC_BACK1, -600 /* Click */, OnClickBack1, VTS_NONE)
	ON_EVENT(CObjectEditorPage, IDC_BACK2, -600 /* Click */, OnClickBack2, VTS_NONE)
	ON_EVENT(CObjectEditorPage, IDC_FORE1, -600 /* Click */, OnClickFore1, VTS_NONE)
	ON_EVENT(CObjectEditorPage, IDC_FORE2, -600 /* Click */, OnClickFore2, VTS_NONE)
	ON_EVENT(CObjectEditorPage, IDC_BACK1, 1 /* OnChange */, OnChangeWidth, VTS_NONE)
	ON_EVENT(CObjectEditorPage, IDC_BACK2, 1 /* OnChange */, OnChangeWidth, VTS_NONE)
	ON_EVENT(CObjectEditorPage, IDC_FORE1, 1 /* OnChange */, OnChangeWidth, VTS_NONE)
	ON_EVENT(CObjectEditorPage, IDC_FORE2, 1 /* OnChange */, OnChangeWidth, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CObjectEditorPage::OnClickBack1() 
{
	m_back2.SetChecked( 0 );
	DoStoreSettings();
}

void CObjectEditorPage::OnClickBack2() 
{
	m_back1.SetChecked( 0 );
	DoStoreSettings();
}

void CObjectEditorPage::OnClickFore1() 
{
	m_fore2.SetChecked( 0 );
	DoStoreSettings();
}

void CObjectEditorPage::OnClickFore2() 
{
	m_fore1.SetChecked( 0 );
	DoStoreSettings();
}

void CObjectEditorPage::OnChangeWidth() 
{
	DoStoreSettings();
}

void CObjectEditorPage::DoLoadSettings()
{
	long	w1, w2, w3, w4;
	long	c1, c2;

	w1 = ::GetValueInt( GetAppUnknown(), "\\ObjectEditor\\Page", "Fore1", m_fore1.GetWidth() );
	w2 = ::GetValueInt( GetAppUnknown(), "\\ObjectEditor\\Page", "Fore2", m_fore2.GetWidth() );
	w3 = ::GetValueInt( GetAppUnknown(), "\\ObjectEditor\\Page", "Back1", m_back1.GetWidth() );
	w4 = ::GetValueInt( GetAppUnknown(), "\\ObjectEditor\\Page", "Back2", m_back2.GetWidth() );
	c1 = ::GetValueInt( GetAppUnknown(), "\\ObjectEditor\\Page", "ForeIdx", 0 );
	c2 = ::GetValueInt( GetAppUnknown(), "\\ObjectEditor\\Page", "BackIdx", 0 );

	m_fore1.SetChecked( c1==0 );
	m_fore2.SetChecked( c1==1 );
	m_back1.SetChecked( c2==0 );
	m_back2.SetChecked( c2==1 );

/******* Maxim [16.05.02] **************************************************************************/
	if( m_fore1.GetChecked() )
		w1 = ::GetValueInt( GetAppUnknown(), "\\ObjectEditor", "MergePenWidth", m_fore1.GetWidth() );
	if( m_fore2.GetChecked() )
		w2 = ::GetValueInt( GetAppUnknown(), "\\ObjectEditor", "MergePenWidth", m_fore2.GetWidth() );
	if( m_back1.GetChecked() )
		w3 = ::GetValueInt(  GetAppUnknown(), "\\ObjectEditor", "SeparatePenWidth", m_back1.GetWidth() );
	if( m_back2.GetChecked() )
		w4 = ::GetValueInt( GetAppUnknown(), "\\ObjectEditor", "SeparatePenWidth", m_back2.GetWidth() );
/******* Maxim [16.05.02] **************************************************************************/

	m_fore1.SetWidth( w1 );
	m_fore2.SetWidth( w2 );
	m_back1.SetWidth( w3 );
	m_back2.SetWidth( w4 );


//	DoStoreSettings();
}

void CObjectEditorPage::DoStoreSettings()
{

	::SetValue( GetAppUnknown(), "\\ObjectEditor\\Page", "Fore1", (long)m_fore1.GetWidth() );
	::SetValue( GetAppUnknown(), "\\ObjectEditor\\Page", "Fore2", (long)m_fore2.GetWidth() );
	::SetValue( GetAppUnknown(), "\\ObjectEditor\\Page", "Back1", (long)m_back1.GetWidth() );
	::SetValue( GetAppUnknown(), "\\ObjectEditor\\Page", "Back2", (long)m_back2.GetWidth() );

	if( m_fore1.GetChecked() )
	{
		::SetValue( GetAppUnknown(), "\\ObjectEditor", "MergePenWidth", m_fore1.GetWidth() );
		::SetValue( GetAppUnknown(), "\\ObjectEditor\\Page", "ForeIdx", 0l );
	}

	if( m_fore2.GetChecked() )
	{
		::SetValue( GetAppUnknown(), "\\ObjectEditor", "MergePenWidth", m_fore2.GetWidth() );
		::SetValue( GetAppUnknown(), "\\ObjectEditor\\Page", "ForeIdx", 1l );
	}
	if( m_back1.GetChecked() )
	{
		::SetValue( GetAppUnknown(), "\\ObjectEditor", "SeparatePenWidth", m_back1.GetWidth() );
		::SetValue( GetAppUnknown(), "\\ObjectEditor\\Page", "BackIdx", 0l );
	}
	if( m_back2.GetChecked() )
	{
		::SetValue( GetAppUnknown(), "\\ObjectEditor", "SeparatePenWidth", m_back2.GetWidth() );
		::SetValue( GetAppUnknown(), "\\ObjectEditor\\Page", "BackIdx", 1l );
	}


	CWnd* pWnd = GetDlgItem(IDC_EDIT_MIN_OBJECT);
	if(pWnd)
	{
		CString str;
		str.Format("%d", max(0, min(255, ::GetValueInt( GetAppUnknown(), "ObjectEditor", "MinSize", (long)0))));	
		pWnd->SetWindowText(str);
	}

	UpdateAction();
}

BOOL CObjectEditorPage::OnInitDialog() 
{
	CPropertyPageBase::OnInitDialog();

	((CSpinButtonCtrl*)GetDlgItem( IDC_MIN_SPIN ))->SetRange( 0, 255 );
	
	DoLoadSettings();	

	bool bUseRButton = ::GetValueInt(::GetAppUnknown(), "ObjectEditor", "UseRButton", 1) != 0;
	HWND hWndContr = ::GetDlgItem(m_hWnd, IDC_VTSTATICTEXT1);
	::ShowWindow(hWndContr, bUseRButton?SW_SHOW:SW_HIDE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CObjectEditorPage::OnChangeEditMinObject() 
{
	if( !IsInitialized() )
		return;
	CWnd* pWnd = GetDlgItem(IDC_EDIT_MIN_OBJECT);
	if(pWnd)
	{
		CString str;
		pWnd->GetWindowText(str);
		::SetValue( GetAppUnknown(), "ObjectEditor", "MinSize", (long)atoi(str));	
		UpdateAction();
	}
}

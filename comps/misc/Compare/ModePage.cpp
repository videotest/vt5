// ModePage.cpp : implementation file
//

#include "stdafx.h"
#include "Compare.h"
#include "ModePage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#include "PropPage.h"

/////////////////////////////////////////////////////////////////////////////
// CModePage property page

IMPLEMENT_DYNCREATE(CModePage, CDialog)

// {B49BD123-698E-4fbe-A920-61644A926E93}
GUARD_IMPLEMENT_OLECREATE(CModePage, szModePropPageProgID, 
0xb49bd123, 0x698e, 0x4fbe, 0xa9, 0x20, 0x61, 0x64, 0x4a, 0x92, 0x6e, 0x93);


CModePage::CModePage() : CPropertyPageBase(CModePage::IDD)
{
	//{{AFX_DATA_INIT(CModePage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_sName = c_szCModePage;
	m_sUserName.LoadString( IDS_PROPPAGE_MODE );
	m_bAddMode = false;
	m_nTransparency = 50;
}

CModePage::~CModePage()
{
}

void CModePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageBase::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModePage)
	DDX_Control(pDX, IDC_TRANSPARENCY, m_slider);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CModePage, CPropertyPageBase)
	//{{AFX_MSG_MAP(CModePage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModePage message handlers

BEGIN_EVENTSINK_MAP(CModePage, CPropertyPageBase)
    //{{AFX_EVENTSINK_MAP(CModePage)
	ON_EVENT(CModePage, IDC_ADD, -600 /* Click */, OnClickAdd, VTS_NONE)
	ON_EVENT(CModePage, IDC_SUB, -600 /* Click */, OnClickSub, VTS_NONE)
	ON_EVENT(CModePage, IDC_TRANSPARENCY, 1 /* PosChanged */, OnPosChangedTransparency, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

BOOL CModePage::OnInitDialog() 
{
	CPropertyPageBase::OnInitDialog();
	
	DoLoadSettings();
	_UpdateControls();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CModePage::OnClickAdd() 
{
	m_bAddMode = true;

	DoStoreSettings();
	_UpdateControls();
	UpdateAction();
}

void CModePage::OnClickSub() 
{
	m_bAddMode = false;

	DoStoreSettings();
	_UpdateControls();
	UpdateAction();
	
}

void CModePage::OnPosChangedTransparency() 
{
	m_nTransparency = m_slider.GetPosLo(  );

	DoStoreSettings();
	UpdateAction();
}


void CModePage::DoStoreSettings()
{
	::SetValue( GetAppUnknown(), "Compare", "Mode", (long)m_bAddMode );
	::SetValue( GetAppUnknown(), "Compare", "Transparency", (long)m_nTransparency );
}
void CModePage::DoLoadSettings()
{
	m_bAddMode = ::GetValueInt( GetAppUnknown(), "Compare", "Mode", m_bAddMode ) !=0;
	m_nTransparency = ::GetValueInt( GetAppUnknown(), "Compare", "Transparency", m_nTransparency );

	m_slider.SetPosLo( m_nTransparency );

}

void CModePage::_UpdateControls()
{
	((CButton*)GetDlgItem( IDC_SUB ) )->SetCheck( !m_bAddMode );
	((CButton*)GetDlgItem( IDC_ADD ) )->SetCheck( m_bAddMode );
}
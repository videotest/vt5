// FramesPropPage.cpp : implementation file
//

#include "stdafx.h"
#include "VTControls.h"
#include "FramesPropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFramesPropPage dialog

IMPLEMENT_DYNCREATE(CFramesPropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CFramesPropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CFramesPropPage)
	ON_CBN_SELCHANGE(IDC_COMBO_TEMPLATE, OnTemplateChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

// {BE0B02A7-A0AE-11D3-A538-0000B493A187}
IMPLEMENT_OLECREATE_EX(CFramesPropPage, "PushButton.CFramesPropPage",
	0xbe0b02a7, 0xa0ae, 0x11d3, 0xa5, 0x38, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87)


/////////////////////////////////////////////////////////////////////////////
// CFramesPropPage::CFramesPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CFramesPropPage

BOOL CFramesPropPage::CFramesPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: Define string resource for page type; replace '0' below with ID.

	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_PUSHBUTTON_FAMESPPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CFramesPropPage::CFramesPropPage - Constructor

// TODO: Define string resource for page caption; replace '0' below with ID.

CFramesPropPage::CFramesPropPage() :
	COlePropertyPage(IDD, IDS_PUSHBUTTON_FAMESPPG_CAPTION)
{
	//{{AFX_DATA_INIT(CFramesPropPage)
	m_bInnerRaised = FALSE;
	m_bInnerSunken = FALSE;
	m_bOuterRaised = FALSE;
	m_bOuterSunken = FALSE;
	m_nTemplateNum = -1;
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CFramesPropPage::DoDataExchange - Moves data between page and properties

void CFramesPropPage::DoDataExchange(CDataExchange* pDX)
{
	// NOTE: ClassWizard will add DDP, DDX, and DDV calls here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//{{AFX_DATA_MAP(CFramesPropPage)
	DDP_Check(pDX, IDC_CHECK1, m_bInnerRaised, _T("InnerRaised") );
	DDX_Check(pDX, IDC_CHECK1, m_bInnerRaised);
	DDP_Check(pDX, IDC_CHECK2, m_bInnerSunken, _T("InnerSunken") );
	DDX_Check(pDX, IDC_CHECK2, m_bInnerSunken);
	DDP_Check(pDX, IDC_CHECK4, m_bOuterRaised, _T("OuterRaised") );
	DDX_Check(pDX, IDC_CHECK4, m_bOuterRaised);
	DDP_Check(pDX, IDC_CHECK5, m_bOuterSunken, _T("OuterSunken") );
	DDX_Check(pDX, IDC_CHECK5, m_bOuterSunken);
	DDX_CBIndex(pDX, IDC_COMBO_TEMPLATE, m_nTemplateNum);
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CFramesPropPage message handlers

void CFramesPropPage::OnTemplateChange() 
{

	

	UpdateData( TRUE );
	

	if( m_nTemplateNum == 0 )
	{		
		((CButton*)GetDlgItem(IDC_CHECK1))->SetCheck(TRUE);
		((CButton*)GetDlgItem(IDC_CHECK2))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_CHECK4))->SetCheck(TRUE);
		((CButton*)GetDlgItem(IDC_CHECK5))->SetCheck(FALSE);				
	}

	if( m_nTemplateNum == 1 )
	{
		((CButton*)GetDlgItem(IDC_CHECK1))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_CHECK2))->SetCheck(TRUE);
		((CButton*)GetDlgItem(IDC_CHECK4))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_CHECK5))->SetCheck(TRUE);				
	}

	if( m_nTemplateNum == 2 )
	{
		((CButton*)GetDlgItem(IDC_CHECK1))->SetCheck(TRUE);
		((CButton*)GetDlgItem(IDC_CHECK2))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_CHECK4))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_CHECK5))->SetCheck(TRUE);				
	}

	if( m_nTemplateNum == 3 )
	{
		((CButton*)GetDlgItem(IDC_CHECK1))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_CHECK2))->SetCheck(TRUE);
		((CButton*)GetDlgItem(IDC_CHECK4))->SetCheck(TRUE);
		((CButton*)GetDlgItem(IDC_CHECK5))->SetCheck(FALSE);				
	}

	if( m_nTemplateNum == 4 )
	{
		((CButton*)GetDlgItem(IDC_CHECK1))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_CHECK2))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_CHECK4))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_CHECK5))->SetCheck(FALSE);				
	}



	SetControlStatus(IDC_CHECK1, TRUE );
	SetControlStatus(IDC_CHECK2, TRUE );
	SetControlStatus(IDC_CHECK4, TRUE );
	SetControlStatus(IDC_CHECK5, TRUE );
	
	UpdateData( TRUE );
	
	
}

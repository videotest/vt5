// DBNumberPpg.cpp : Implementation of the CDBNumberPropPage property page class.

#include "stdafx.h"
#include "DBControls.h"
#include "DBNumberPpg.h"
#include "ax_ctrl_misc.h"

IMPLEMENT_DYNCREATE(CDBNumberPropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CDBNumberPropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CDBNumberPropPage)
	ON_EN_SETFOCUS(IDC_EDIT1, OnSetfocusEdit1)
	ON_EN_KILLFOCUS(IDC_EDIT1, OnKillfocusEdit1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CDBNumberPropPage, "DBCONTROLS.DBNumberPropPage.1",
	0x5023e32a, 0x2dae, 0x495b, 0xb5, 0x79, 0x30, 0x11, 0x5b, 0x3d, 0x1f, 0xc2)


/////////////////////////////////////////////////////////////////////////////
// CDBNumberPropPage::CDBNumberPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CDBNumberPropPage

BOOL CDBNumberPropPage::CDBNumberPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_DBNUMBER_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CDBNumberPropPage::CDBNumberPropPage - Constructor

CDBNumberPropPage::CDBNumberPropPage() :
	COlePropertyPage(IDD, IDS_DBNUMBER_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CDBNumberPropPage)
	// NOTE: ClassWizard will add member initialization here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CDBNumberPropPage::DoDataExchange - Moves data between page and properties

void CDBNumberPropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CDBNumberPropPage)
	// NOTE: ClassWizard will add DDP, DDX, and DDV calls here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);

	if( pDX->m_bSaveAndValidate )
		FirePropExchange( this );

}


/////////////////////////////////////////////////////////////////////////////
// CDBNumberPropPage message handlers

void CDBNumberPropPage::OnSetfocusEdit1() 
{
	// TODO: Add your control notification handler code here
	
}

void CDBNumberPropPage::OnKillfocusEdit1() 
{
	
}

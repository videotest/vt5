// DBTextPpg.cpp : Implementation of the CDBTextPropPage property page class.

#include "stdafx.h"
#include "DBControls.h"
#include "DBTextPpg.h"
#include "ax_ctrl_misc.h"


IMPLEMENT_DYNCREATE(CDBTextPropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CDBTextPropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CDBTextPropPage)
	// NOTE - ClassWizard will add and remove message map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CDBTextPropPage, "DBCONTROLS.DBTextPropPage.1",
	0xb3aeb122, 0x8131, 0x4c85, 0x95, 0x30, 0xf4, 0xda, 0xdb, 0x94, 0xc3, 0xd1)


/////////////////////////////////////////////////////////////////////////////
// CDBTextPropPage::CDBTextPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CDBTextPropPage

BOOL CDBTextPropPage::CDBTextPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_DBTEXT_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CDBTextPropPage::CDBTextPropPage - Constructor

CDBTextPropPage::CDBTextPropPage() :
	COlePropertyPage(IDD, IDS_DBTEXT_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CDBTextPropPage)
	// NOTE: ClassWizard will add member initialization here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CDBTextPropPage::DoDataExchange - Moves data between page and properties

void CDBTextPropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CDBTextPropPage)
	// NOTE: ClassWizard will add DDP, DDX, and DDV calls here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);

	if( pDX->m_bSaveAndValidate )
		FirePropExchange( this );

}


/////////////////////////////////////////////////////////////////////////////
// CDBTextPropPage message handlers

// DBDateTimePpg.cpp : Implementation of the CDBDateTimePropPage property page class.

#include "stdafx.h"
#include "DBControls.h"
#include "DBDateTimePpg.h"
#include "ax_ctrl_misc.h"

IMPLEMENT_DYNCREATE(CDBDateTimePropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CDBDateTimePropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CDBDateTimePropPage)
	// NOTE - ClassWizard will add and remove message map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CDBDateTimePropPage, "DBCONTROLS.DBDateTimePropPage.1",
	0xae949ed6, 0x38c1, 0x484c, 0x98, 0xcc, 0x4f, 0xbf, 0xee, 0xf2, 0xf7, 0xbe)


/////////////////////////////////////////////////////////////////////////////
// CDBDateTimePropPage::CDBDateTimePropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CDBDateTimePropPage

BOOL CDBDateTimePropPage::CDBDateTimePropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_DBDATETIME_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CDBDateTimePropPage::CDBDateTimePropPage - Constructor

CDBDateTimePropPage::CDBDateTimePropPage() :
	COlePropertyPage(IDD, IDS_DBDATETIME_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CDBDateTimePropPage)
	// NOTE: ClassWizard will add member initialization here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CDBDateTimePropPage::DoDataExchange - Moves data between page and properties

void CDBDateTimePropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CDBDateTimePropPage)
	// NOTE: ClassWizard will add DDP, DDX, and DDV calls here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);

	if( pDX->m_bSaveAndValidate )
		FirePropExchange( this );

}


/////////////////////////////////////////////////////////////////////////////
// CDBDateTimePropPage message handlers

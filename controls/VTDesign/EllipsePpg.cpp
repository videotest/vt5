// EllipsePpg.cpp : Implementation of the CVTEllipsePropPage property page class.

#include "stdafx.h"
#include "VTDesign.h"
#include "EllipsePpg.h"
#include "ax_ctrl_misc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CVTEllipsePropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CVTEllipsePropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CVTEllipsePropPage)
	// NOTE - ClassWizard will add and remove message map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CVTEllipsePropPage, "VTDESIGN.VTDEllipsePropPage.1",
	0xbc127b14, 0x26c, 0x11d4, 0x81, 0x24, 0, 0, 0xe8, 0xdf, 0x68, 0xc3)


/////////////////////////////////////////////////////////////////////////////
// CVTEllipsePropPage::CVTEllipsePropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CVTEllipsePropPage

BOOL CVTEllipsePropPage::CVTEllipsePropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_VTDELLIPSE_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CVTEllipsePropPage::CVTEllipsePropPage - Constructor

CVTEllipsePropPage::CVTEllipsePropPage() :
	COlePropertyPage(IDD, IDS_VTDELLIPSE_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CVTEllipsePropPage)
	// NOTE: ClassWizard will add member initialization here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CVTEllipsePropPage::DoDataExchange - Moves data between page and properties

void CVTEllipsePropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CVTEllipsePropPage)
	// NOTE: ClassWizard will add DDP, DDX, and DDV calls here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);

	if( pDX->m_bSaveAndValidate )
		FirePropExchange( this );

}


/////////////////////////////////////////////////////////////////////////////
// CVTEllipsePropPage message handlers

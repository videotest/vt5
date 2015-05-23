// ColorManAXPpg.cpp : Implementation of the CColorManAXPropPage property page class.

#include "stdafx.h"
#include "ColorManAX.h"
#include "ColorManAXPpg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CColorManAXPropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CColorManAXPropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CColorManAXPropPage)
	// NOTE - ClassWizard will add and remove message map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

GUARD_IMPLEMENT_OLECREATE_CTRL(CColorManAXPropPage, "COLORMANAX.ColorManAXPropPage.1",
	0x5db73bb7, 0xf590, 0x11d3, 0xa0, 0xbc, 0, 0, 0xb4, 0x93, 0xa1, 0x87)


/////////////////////////////////////////////////////////////////////////////
// CColorManAXPropPage::CColorManAXPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CColorManAXPropPage

BOOL CColorManAXPropPage::CColorManAXPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	return UpdateRegistryPage(bRegister, AfxGetInstanceHandle(), IDS_COLORMANAX_PPG);
}


/////////////////////////////////////////////////////////////////////////////
// CColorManAXPropPage::CColorManAXPropPage - Constructor

CColorManAXPropPage::CColorManAXPropPage() :
	COlePropertyPage(IDD, IDS_COLORMANAX_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CColorManAXPropPage)
	// NOTE: ClassWizard will add member initialization here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CColorManAXPropPage::DoDataExchange - Moves data between page and properties

void CColorManAXPropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CColorManAXPropPage)
	// NOTE: ClassWizard will add DDP, DDX, and DDV calls here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CColorManAXPropPage message handlers

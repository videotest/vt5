// LookupHelperAXPpg.cpp : Implementation of the CLookupHelperAXPropPage property page class.

#include "stdafx.h"
#include "LookupHelperAX.h"
#include "LookupHelperAXPpg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CLookupHelperAXPropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CLookupHelperAXPropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CLookupHelperAXPropPage)
	// NOTE - ClassWizard will add and remove message map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

GUARD_IMPLEMENT_OLECREATE_CTRL(CLookupHelperAXPropPage, "LOOKUPHELPERAX.LookupHelperAXPropPage.1",
	0x29e2e5ad, 0x553f, 0x46ff, 0xa5, 0xf0, 0xe6, 0x2, 0xcc, 0xb5, 0x9e, 0x26);


/////////////////////////////////////////////////////////////////////////////
// CLookupHelperAXPropPage::CLookupHelperAXPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CLookupHelperAXPropPage

BOOL CLookupHelperAXPropPage::CLookupHelperAXPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	return UpdateRegistryPage(bRegister, AfxGetInstanceHandle(), IDS_LOOKUPHELPERAX_PPG);
}


/////////////////////////////////////////////////////////////////////////////
// CLookupHelperAXPropPage::CLookupHelperAXPropPage - Constructor

CLookupHelperAXPropPage::CLookupHelperAXPropPage() :
	COlePropertyPage(IDD, IDS_LOOKUPHELPERAX_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CLookupHelperAXPropPage)
	// NOTE: ClassWizard will add member initialization here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CLookupHelperAXPropPage::DoDataExchange - Moves data between page and properties

void CLookupHelperAXPropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CLookupHelperAXPropPage)
	// NOTE: ClassWizard will add DDP, DDX, and DDV calls here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CLookupHelperAXPropPage message handlers

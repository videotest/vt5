// HistAXPpg.cpp : Implementation of the CHistAXPropPage property page class.

#include "stdafx.h"
#include "HistAX.h"
#include "HistAXPpg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CHistAXPropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CHistAXPropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CHistAXPropPage)
	// NOTE - ClassWizard will add and remove message map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

GUARD_IMPLEMENT_OLECREATE_CTRL(CHistAXPropPage, "HISTAX.HistAXPropPage.1",
	0x6fdde59, 0xf981, 0x11d3, 0xa0, 0xc1, 0, 0, 0xb4, 0x93, 0xa1, 0x87)


/////////////////////////////////////////////////////////////////////////////
// CHistAXPropPage::CHistAXPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CHistAXPropPage

BOOL CHistAXPropPage::CHistAXPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	return UpdateRegistryPage(bRegister, AfxGetInstanceHandle(), IDS_HISTAX_PPG);
}


/////////////////////////////////////////////////////////////////////////////
// CHistAXPropPage::CHistAXPropPage - Constructor

CHistAXPropPage::CHistAXPropPage() :
	COlePropertyPage(IDD, IDS_HISTAX_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CHistAXPropPage)
	// NOTE: ClassWizard will add member initialization here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CHistAXPropPage::DoDataExchange - Moves data between page and properties

void CHistAXPropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CHistAXPropPage)
	// NOTE: ClassWizard will add DDP, DDX, and DDV calls here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CHistAXPropPage message handlers

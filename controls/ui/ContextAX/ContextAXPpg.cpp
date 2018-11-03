// ContextAXPpg.cpp : Implementation of the CContextAXPropPage property page class.

#include "stdafx.h"
#include "ContextAX.h"
#include "ContextAXPpg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CContextAXPropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CContextAXPropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CContextAXPropPage)
	// NOTE - ClassWizard will add and remove message map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

GUARD_IMPLEMENT_OLECREATE_CTRL(CContextAXPropPage, "CONTEXTAX.ContextAXPropPage.1",
	0x5b33bc9, 0x4adb, 0x42ac, 0x8a, 0x62, 0xa7, 0x9, 0xf3, 0xf8, 0x11, 0x2a)


/////////////////////////////////////////////////////////////////////////////
// CContextAXPropPage::CContextAXPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CContextAXPropPage

BOOL CContextAXPropPage::CContextAXPropPageFactory::UpdateRegistry(BOOL bRegister)
{
#if defined(NOGUARD)
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_CONTEXTAX_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
#else
	return UpdateRegistryPage(bRegister, AfxGetInstanceHandle(), IDS_CONTEXTAX_PPG);
#endif
}


/////////////////////////////////////////////////////////////////////////////
// CContextAXPropPage::CContextAXPropPage - Constructor

CContextAXPropPage::CContextAXPropPage() :
	COlePropertyPage(IDD, IDS_CONTEXTAX_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CContextAXPropPage)
	// NOTE: ClassWizard will add member initialization here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CContextAXPropPage::DoDataExchange - Moves data between page and properties

void CContextAXPropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CContextAXPropPage)
	// NOTE: ClassWizard will add DDP, DDX, and DDV calls here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CContextAXPropPage message handlers

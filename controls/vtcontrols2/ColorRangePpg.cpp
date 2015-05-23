// ColorRangeCtrlPpg.cpp : Implementation of the CColorRangePropPage property page class.

#include "stdafx.h"
#include "vtcontrols2.h"
#include "ColorRangePpg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CColorRangePropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CColorRangePropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CColorRangePropPage)
	// NOTE - ClassWizard will add and remove message map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CColorRangePropPage, "VTCONTROLS2.ColorRangePropPage.1",
	0xbddb4d87, 0x8852, 0x40f5, 0x82, 0xdd, 0x43, 0x1d, 0x41, 0x41, 0x79, 0xcd)


/////////////////////////////////////////////////////////////////////////////
// CColorRangePropPage::CColorRangePropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CColorRangePropPage

BOOL CColorRangePropPage::CColorRangePropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_COLORRANGE_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CColorRangePropPage::CColorRangePropPage - Constructor

CColorRangePropPage::CColorRangePropPage() :
	COlePropertyPage(IDD, IDS_COLORRANGE_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CColorRangePropPage)
	// NOTE: ClassWizard will add member initialization here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CColorRangePropPage::DoDataExchange - Moves data between page and properties

void CColorRangePropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CColorRangePropPage)
	// NOTE: ClassWizard will add DDP, DDX, and DDV calls here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CColorRangePropPage message handlers

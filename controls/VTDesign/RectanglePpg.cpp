// RectanglePpg.cpp : Implementation of the CVTRectanglePropPage property page class.

#include "stdafx.h"
#include "VTDesign.h"
#include "RectanglePpg.h"
#include "ax_ctrl_misc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CVTRectanglePropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CVTRectanglePropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CVTRectanglePropPage)
	// NOTE - ClassWizard will add and remove message map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CVTRectanglePropPage, "VTDESIGN.VTDRectanglePropPage.1",
	0xbc127b10, 0x26c, 0x11d4, 0x81, 0x24, 0, 0, 0xe8, 0xdf, 0x68, 0xc3)


/////////////////////////////////////////////////////////////////////////////
// CVTRectanglePropPage::CVTRectanglePropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CVTRectanglePropPage

BOOL CVTRectanglePropPage::CVTRectanglePropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_VTDRECTANGLE_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CVTRectanglePropPage::CVTRectanglePropPage - Constructor

CVTRectanglePropPage::CVTRectanglePropPage() :
	COlePropertyPage(IDD, IDS_VTDRECTANGLE_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CVTRectanglePropPage)
	// NOTE: ClassWizard will add member initialization here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CVTRectanglePropPage::DoDataExchange - Moves data between page and properties

void CVTRectanglePropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CVTRectanglePropPage)
	// NOTE: ClassWizard will add DDP, DDX, and DDV calls here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);

	if( pDX->m_bSaveAndValidate )
		FirePropExchange( this );

}


/////////////////////////////////////////////////////////////////////////////
// CVTRectanglePropPage message handlers

// WorksModePropPage.cpp : implementation file
//

#include "stdafx.h"
#include "MeasCtrls.h"
#include "WorksModePropPage.h"
#include "works_modes.h"


// CWorksModePropPage dialog

IMPLEMENT_DYNCREATE(CWorksModePropPage, COlePropertyPage)



// Message map

BEGIN_MESSAGE_MAP(CWorksModePropPage, COlePropertyPage)
END_MESSAGE_MAP()



// Initialize class factory and guid

// {9A35486F-AA85-449D-9FBF-0ECC6B880905}
IMPLEMENT_OLECREATE_EX(CWorksModePropPage, "MeasCtrls.WorksModePropPage",
	0x9a35486f, 0xaa85, 0x449d, 0x9f, 0xbf, 0xe, 0xcc, 0x6b, 0x88, 0x9, 0x5)



// CWorksModePropPage::CWorksModePropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CWorksModePropPage

BOOL CWorksModePropPage::CWorksModePropPageFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: Define string resource for page type; replace '0' below with ID.

	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_WORKSMODE_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}



// CWorksModePropPage::CWorksModePropPage - Constructor

// TODO: Define string resource for page caption; replace '0' below with ID.

CWorksModePropPage::CWorksModePropPage() :
	COlePropertyPage(IDD, IDS_WORKSMODE_PPG_CAPTION)
		, m_BWorksMode(FALSE)
	{
}



// CWorksModePropPage::DoDataExchange - Moves data between page and properties

void CWorksModePropPage::DoDataExchange(CDataExchange* pDX)	  
{														   
	DDX_Radio(pDX, IDC_RADIO_MEASUSERS, m_BWorksMode);
	DDP_Radio(pDX, IDC_RADIO_MEASUSERS, m_BWorksMode, _T("WorksMode") );
	

	DDP_PostProcessing(pDX);
}



// CWorksModePropPage message handlers

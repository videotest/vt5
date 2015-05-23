// ChoiceGreekSymbolPropPage.cpp : Implementation of the CChoiceGreekSymbolPropPage property page class.

#include "stdafx.h"
#include "MeasCtrls.h"
#include "ChoiceGreekSymbolPropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CChoiceGreekSymbolPropPage, COlePropertyPage)



// Message map

BEGIN_MESSAGE_MAP(CChoiceGreekSymbolPropPage, COlePropertyPage)
END_MESSAGE_MAP()



// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CChoiceGreekSymbolPropPage, "CHOICEGREEKSYM.ChoiceGreekSymPropPage.1",
	0x8a736917, 0x947f, 0x4cb0, 0xb2, 0xd7, 0xdc, 0xc9, 0xc8, 0x71, 0x1, 0xd3)



// CChoiceGreekSymbolPropPage::CChoiceGreekSymbolPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CChoiceGreekSymbolPropPage

BOOL CChoiceGreekSymbolPropPage::CChoiceGreekSymbolPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_CHOICEGREEKSYMBOL_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}



// CChoiceGreekSymbolPropPage::CChoiceGreekSymbolPropPage - Constructor

CChoiceGreekSymbolPropPage::CChoiceGreekSymbolPropPage() :
	COlePropertyPage(IDD, IDS_CHOICEGREEKSYMBOL_PPG_CAPTION)
{
}



// CChoiceGreekSymbolPropPage::DoDataExchange - Moves data between page and properties

void CChoiceGreekSymbolPropPage::DoDataExchange(CDataExchange* pDX)
{
	DDP_PostProcessing(pDX);
}



// CChoiceGreekSymbolPropPage message handlers

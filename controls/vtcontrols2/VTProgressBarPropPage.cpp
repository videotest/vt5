// VTProgressBarPropPage.cpp : Implementation of the CVTProgressBarPropPage property page class.

#include "stdafx.h"
#include "VTProgressBarPropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CVTProgressBarPropPage, COlePropertyPage)



// Message map

BEGIN_MESSAGE_MAP(CVTProgressBarPropPage, COlePropertyPage)
END_MESSAGE_MAP()



// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CVTProgressBarPropPage, "VTPROGRESSBAR.VTProgressBarPropPage.1",
	0xe7410cf8, 0xe917, 0x44a1, 0x9e, 0x69, 0x95, 0xc2, 0xff, 0xa0, 0xfc, 0x26)



// CVTProgressBarPropPage::CVTProgressBarPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CVTProgressBarPropPage

BOOL CVTProgressBarPropPage::CVTProgressBarPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_VTPROGRESSBAR_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}



// CVTProgressBarPropPage::CVTProgressBarPropPage - Constructor

CVTProgressBarPropPage::CVTProgressBarPropPage() :
	COlePropertyPage(IDD, IDS_VTPROGRESSBAR_PPG_CAPTION)
		, m_lMin(0)
		, m_lMax(0)
		, m_lPos(0)
		, m_bSmooth(FALSE)
		, m_bShowPercent(FALSE)
	{
}



// CVTProgressBarPropPage::DoDataExchange - Moves data between page and properties

void CVTProgressBarPropPage::DoDataExchange(CDataExchange* pDX)
{
	DDP_Text(pDX, IDC_EDIT_MIN, m_lMin, _T("Min") );
	DDX_Text(pDX, IDC_EDIT_MIN, m_lMin);
	DDP_Text(pDX, IDC_EDIT_MAX, m_lMax, _T("Max") );
	DDX_Text(pDX, IDC_EDIT_MAX, m_lMax);
	DDP_Text(pDX, IDC_EDIT_POS, m_lPos, _T("Pos") );
	DDX_Text(pDX, IDC_EDIT_POS, m_lPos);
	DDP_Check(pDX, IDC_SMOOTH, m_bSmooth, _T("Smooth") );
	DDX_Check(pDX, IDC_SMOOTH, m_bSmooth);
	DDP_Check(pDX, IDC_SHOW_PERCENT, m_bShowPercent, _T("ShowPercent") );
	DDX_Check(pDX, IDC_SHOW_PERCENT, m_bShowPercent);
	
	DDP_PostProcessing(pDX);	
	
	if( pDX->m_bSaveAndValidate )
	{
		DDP_Text(pDX, IDC_EDIT_MIN, m_lMin, _T("Min") );
		DDX_Text(pDX, IDC_EDIT_MIN, m_lMin);
		DDP_Text(pDX, IDC_EDIT_MAX, m_lMax, _T("Max") );
		DDX_Text(pDX, IDC_EDIT_MAX, m_lMax);
		DDP_Text(pDX, IDC_EDIT_POS, m_lPos, _T("Pos") );
		DDX_Text(pDX, IDC_EDIT_POS, m_lPos);

		pDX->m_bSaveAndValidate = FALSE;
		DDP_Text(pDX, IDC_EDIT_MIN, m_lMin, _T("Min") );
		DDX_Text(pDX, IDC_EDIT_MIN, m_lMin);
		DDP_Text(pDX, IDC_EDIT_MAX, m_lMax, _T("Max") );
		DDX_Text(pDX, IDC_EDIT_MAX, m_lMax);
		DDP_Text(pDX, IDC_EDIT_POS, m_lPos, _T("Pos") );
		DDX_Text(pDX, IDC_EDIT_POS, m_lPos);
		pDX->m_bSaveAndValidate = TRUE;
	}  
}



// CVTProgressBarPropPage message handlers

// VTSliderPpg.cpp : Implementation of the CVTSliderPropPage property page class.

#include "stdafx.h"
#include "VTControls.h"
#include "VTSliderPpg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CVTSliderPropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CVTSliderPropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CVTSliderPropPage)
	// NOTE - ClassWizard will add and remove message map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CVTSliderPropPage, "VTCONTROLS.VTSliderPropPage.1",
	0xeb1fe3de, 0x54e4, 0x4a13, 0xa3, 0xd9, 0x78, 0xf4, 0xbc, 0x75, 0x4e, 0xad)


/////////////////////////////////////////////////////////////////////////////
// CVTSliderPropPage::CVTSliderPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CVTSliderPropPage

BOOL CVTSliderPropPage::CVTSliderPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_VTSLIDER_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CVTSliderPropPage::CVTSliderPropPage - Constructor

CVTSliderPropPage::CVTSliderPropPage() :
	COlePropertyPage(IDD, IDS_VTSLIDER_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CVTSliderPropPage)
	m_bCycled = FALSE;
	m_bShowNum = FALSE;
	m_bTick = FALSE;
	m_strHighKeyValue = _T("");
	m_bLowKeyValue = _T("");
	m_nMax = 0;
	m_nMin = 0;
	m_strName = _T("");
	m_bAutoUpdate = FALSE;
	m_nTickFreq = 0;
	m_bSingleSlide = FALSE;
	m_bUseSystemFont = FALSE;
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CVTSliderPropPage::DoDataExchange - Moves data between page and properties

void CVTSliderPropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CVTSliderPropPage)
	DDP_Check(pDX, IDC_CHECK_CYCLED, m_bCycled, _T("Cycled") );
	DDX_Check(pDX, IDC_CHECK_CYCLED, m_bCycled);
	DDP_Check(pDX, IDC_CHECK_SHOW_NUM, m_bShowNum, _T("ShowNums") );
	DDX_Check(pDX, IDC_CHECK_SHOW_NUM, m_bShowNum);
	DDP_Check(pDX, IDC_CHECK_TICK, m_bTick, _T("ShowTick") );
	DDX_Check(pDX, IDC_CHECK_TICK, m_bTick);
	DDP_Text(pDX, IDC_EDIT_HIGH_KEY, m_strHighKeyValue, _T("HighKeyValue") );
	DDX_Text(pDX, IDC_EDIT_HIGH_KEY, m_strHighKeyValue);
	DDP_Text(pDX, IDC_EDIT_LOW_KEY, m_bLowKeyValue, _T("LowKeyValue") );
	DDX_Text(pDX, IDC_EDIT_LOW_KEY, m_bLowKeyValue);
	DDP_Text(pDX, IDC_EDIT_MAX, m_nMax, _T("Max") );
	DDX_Text(pDX, IDC_EDIT_MAX, m_nMax);
	DDP_Text(pDX, IDC_EDIT_MIN, m_nMin, _T("Min") );
	DDX_Text(pDX, IDC_EDIT_MIN, m_nMin);
	DDP_Text(pDX, IDC_EDIT_NAME, m_strName, _T("Name") );
	DDX_Text(pDX, IDC_EDIT_NAME, m_strName);
	DDP_Check(pDX, IDC_CHECK_AUTO_UPDATE, m_bAutoUpdate, _T("AutoUpdate") );
	DDX_Check(pDX, IDC_CHECK_AUTO_UPDATE, m_bAutoUpdate);
	DDP_Text(pDX, IDC_EDIT_TICK_FREQ, m_nTickFreq, _T("TickFreq") );
	DDX_Text(pDX, IDC_EDIT_TICK_FREQ, m_nTickFreq);
	DDP_Check(pDX, IDC_CHECKSINGLE_SLIDE, m_bSingleSlide, _T("SingleSlide") );
	DDX_Check(pDX, IDC_CHECKSINGLE_SLIDE, m_bSingleSlide);
	DDP_Check(pDX, IDC_USE_SYSTEM_FONT, m_bUseSystemFont, _T("UseSystemFont") );
	DDX_Check(pDX, IDC_USE_SYSTEM_FONT, m_bUseSystemFont);
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CVTSliderPropPage message handlers

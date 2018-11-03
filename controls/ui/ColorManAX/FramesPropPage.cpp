// FramesPropPage.cpp : implementation file
//

#include "stdafx.h"
#include "ColorManAX.h"
#include "FramesPropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFramesPropPage dialog

IMPLEMENT_DYNCREATE(CFramesPropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CFramesPropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CFramesPropPage)
	// NOTE - ClassWizard will add and remove message map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

// {DC439341-F5D3-11d3-A0BC-0000B493A187}
GUARD_IMPLEMENT_OLECREATE_CTRL(CFramesPropPage, "ColorManAX.CFramesPropPage",
0xdc439341, 0xf5d3, 0x11d3, 0xa0, 0xbc, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87);


/////////////////////////////////////////////////////////////////////////////
// CFramesPropPage::CFramesPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CFramesPropPage

BOOL CFramesPropPage::CFramesPropPageFactory::UpdateRegistry(BOOL bRegister)
{
#if defined(NOGUARD)
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_COLORMANAX_FRAMESPPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
#else
	return UpdateRegistryPage(bRegister, AfxGetInstanceHandle(), IDS_COLORMANAX_FRAMESPPG);
#endif
}


/////////////////////////////////////////////////////////////////////////////
// CFramesPropPage::CFramesPropPage - Constructor

// TODO: Define string resource for page caption; replace '0' below with ID.

CFramesPropPage::CFramesPropPage() :
	COlePropertyPage(IDD, IDS_COLORMANAX_FRAMESPPG_CAPTION)
{
	//{{AFX_DATA_INIT(CFramesPropPage)
	m_bInnerSunken = FALSE;
	m_bOuterRaised = FALSE;
	m_bOuterSunken = FALSE;
	m_bInnerRaised = FALSE;
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CFramesPropPage::DoDataExchange - Moves data between page and properties

void CFramesPropPage::DoDataExchange(CDataExchange* pDX)
{
	// NOTE: ClassWizard will add DDP, DDX, and DDV calls here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//{{AFX_DATA_MAP(CFramesPropPage)
	DDP_Check(pDX, IDC_CHECK2, m_bInnerSunken, _T("InnerSunken") );
	DDX_Check(pDX, IDC_CHECK2, m_bInnerSunken);
	DDP_Check(pDX, IDC_CHECK4, m_bOuterRaised, _T("OuterRaised") );
	DDX_Check(pDX, IDC_CHECK4, m_bOuterRaised);
	DDP_Check(pDX, IDC_CHECK5, m_bOuterSunken, _T("OuterSunken") );
	DDX_Check(pDX, IDC_CHECK5, m_bOuterSunken);
	DDP_Check(pDX, IDC_CHECK1, m_bInnerRaised, _T("InnerRaised") );
	DDX_Check(pDX, IDC_CHECK1, m_bInnerRaised);
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CFramesPropPage message handlers

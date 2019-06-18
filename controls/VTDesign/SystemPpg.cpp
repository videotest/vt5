// SystemPpg.cpp : Implementation of the CVTSystemPropPage property page class.

#include "stdafx.h"
#include "VTDesign.h"
#include "SystemPpg.h"
#include "ax_ctrl_misc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CVTSystemPropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CVTSystemPropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CVTSystemPropPage)
	ON_MESSAGE(CPN_SELENDOK, OnColorChange)	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CVTSystemPropPage, "VTDESIGN.VTDSystemPropPage.1",
	0xbc127b24, 0x26c, 0x11d4, 0x81, 0x24, 0, 0, 0xe8, 0xdf, 0x68, 0xc3)


/////////////////////////////////////////////////////////////////////////////
// CVTSystemPropPage::CVTSystemPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CVTSystemPropPage

BOOL CVTSystemPropPage::CVTSystemPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_VTDSYSTEM_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CVTSystemPropPage::CVTSystemPropPage - Constructor

CVTSystemPropPage::CVTSystemPropPage() :
	COlePropertyPage(IDD, IDS_VTDSYSTEM_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CVTSystemPropPage)	
	m_nDataType = -1;
	m_vertAlign = -1;
	m_horzAlign = -1;
	//}}AFX_DATA_INIT
	m_bTransparentBk = FALSE;	
	m_backColor = m_fontColor = RGB(0,0,0);
}


/////////////////////////////////////////////////////////////////////////////
// CVTSystemPropPage::DoDataExchange - Moves data between page and properties

void CVTSystemPropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CVT3DBarPropPage)
	DDP_CBIndex(pDX, IDC_DATA_TYPE, m_nDataType, _T("DataType") );
	DDX_CBIndex(pDX, IDC_DATA_TYPE, m_nDataType);
	DDX_Control(pDX, IDC_COLOR_FONT, m_fontColorPicker);
	DDX_Control(pDX, IDC_COLOR_BK, m_backColorPicker);	
	DDP_CBIndex(pDX, IDC_VERT, m_vertAlign, _T("VertAlign") );
	DDX_CBIndex(pDX, IDC_VERT, m_vertAlign);
	DDP_CBIndex(pDX, IDC_HORZ, m_horzAlign, _T("HorzAlign") );
	DDX_CBIndex(pDX, IDC_HORZ, m_horzAlign);
	//}}AFX_DATA_MAP


	DDP_Text(pDX, IDC_COLOR_FONT, m_fontColor, _T("FontColor") );
	DDX_ColourPicker(pDX, IDC_COLOR_FONT, m_fontColor);	

	DDP_Text(pDX, IDC_COLOR_BK, m_backColor, _T("BackColor") );
	DDX_ColourPicker(pDX, IDC_COLOR_BK, m_backColor);


	DDP_Text(pDX, IDC_COLOR_BK, m_bTransparentBk, _T("TransparentBack") );
	DDX_ColourPicker(pDX, IDC_COLOR_BK, m_bTransparentBk);


	DDP_PostProcessing(pDX);

	if( pDX->m_bSaveAndValidate )
		FirePropExchange( this );

}


/////////////////////////////////////////////////////////////////////////////
// CVTSystemPropPage message handlers
LONG CVTSystemPropPage::OnColorChange(UINT lParam, LONG wParam)
{
	SetModifiedFlag( TRUE );		
	return 1;
}

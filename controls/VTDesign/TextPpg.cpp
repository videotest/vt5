// TextPpg.cpp : Implementation of the CVTTextPropPage property page class.

#include "stdafx.h"
#include "VTDesign.h"
#include "TextPpg.h"
#include "ax_ctrl_misc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CVTTextPropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CVTTextPropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CVTTextPropPage)
	ON_MESSAGE(CPN_SELENDOK, OnColorChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CVTTextPropPage, "VTDESIGN.VTDTextPropPage.1",
	0xbc127b18, 0x26c, 0x11d4, 0x81, 0x24, 0, 0, 0xe8, 0xdf, 0x68, 0xc3)


/////////////////////////////////////////////////////////////////////////////
// CVTTextPropPage::CVTTextPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CVTTextPropPage

BOOL CVTTextPropPage::CVTTextPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_VTDTEXT_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CVTTextPropPage::CVTTextPropPage - Constructor

CVTTextPropPage::CVTTextPropPage() :
	COlePropertyPage(IDD, IDS_VTDTEXT_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CVTTextPropPage)
	m_strText = _T("");			
	m_nDegree = 0;
	m_vertAlign = -1;
	m_horzAlign = -1;
	m_strKey = _T("");
	//}}AFX_DATA_INIT
	m_bTransparentBk = FALSE;	
	m_backColor = m_fontColor = RGB(0,0,0);
}


/////////////////////////////////////////////////////////////////////////////
// CVTTextPropPage::DoDataExchange - Moves data between page and properties

void CVTTextPropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CVTTextPropPage)
	DDX_Control(pDX, IDC_COLOR_FONT, m_fontColorPicker);
	DDX_Control(pDX, IDC_COLOR_BK, m_backColorPicker);	
	DDP_Text(pDX, IDC_TEXT, m_strText, _T("Text") );
	DDX_Text(pDX, IDC_TEXT, m_strText);
	DDP_Text(pDX, IDC_DEGREE, m_nDegree, _T("Degree") );
	DDX_Text(pDX, IDC_DEGREE, m_nDegree);
	DDP_CBIndex(pDX, IDC_VERT, m_vertAlign, _T("VertAlign") );
	DDX_CBIndex(pDX, IDC_VERT, m_vertAlign);
	DDP_CBIndex(pDX, IDC_HORZ, m_horzAlign, _T("HorzAlign") );
	DDX_CBIndex(pDX, IDC_HORZ, m_horzAlign);
	DDP_Text(pDX, IDC_KEY, m_strKey, _T("Key") );
	DDX_Text(pDX, IDC_KEY, m_strKey);
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
// CVTTextPropPage message handlers
LONG CVTTextPropPage::OnColorChange(UINT lParam, LONG wParam)
{
	SetModifiedFlag( TRUE );		
	return 1;
}

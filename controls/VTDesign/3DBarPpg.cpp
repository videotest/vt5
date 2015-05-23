// 3DBarPpg.cpp : Implementation of the CVT3DBarPropPage property page class.

#include "stdafx.h"
#include "VTDesign.h"
#include "3DBarPpg.h"
#include "ax_ctrl_misc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CVT3DBarPropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CVT3DBarPropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CVT3DBarPropPage)
	ON_MESSAGE(CPN_SELENDOK, OnColorChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CVT3DBarPropPage, "VTDESIGN.VTD3DBarPropPage.1",
	0xbc127b20, 0x26c, 0x11d4, 0x81, 0x24, 0, 0, 0xe8, 0xdf, 0x68, 0xc3)


/////////////////////////////////////////////////////////////////////////////
// CVT3DBarPropPage::CVT3DBarPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CVT3DBarPropPage

BOOL CVT3DBarPropPage::CVT3DBarPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_VTD3DBAR_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CVT3DBarPropPage::CVT3DBarPropPage - Constructor

CVT3DBarPropPage::CVT3DBarPropPage() :
	COlePropertyPage(IDD, IDS_VTD3DBAR_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CVT3DBarPropPage)
	m_strText = _T("");
	m_nTextAlign = -1;
	m_nBorderType = -1;
	m_nTextPosition = -1;
	m_fLineWidth = 0.0;
	//}}AFX_DATA_INIT
	m_bTransparentBk = FALSE;	
	m_backColor = m_fontColor = RGB(0,0,0);

}


/////////////////////////////////////////////////////////////////////////////
// CVT3DBarPropPage::DoDataExchange - Moves data between page and properties

void CVT3DBarPropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CVT3DBarPropPage)
	DDX_Control(pDX, IDC_COLOR_FONT, m_fontColorPicker);
	DDX_Control(pDX, IDC_COLOR_BK, m_backColorPicker);	
	DDP_Text(pDX, IDC_TEXT, m_strText, _T("Text") );
	DDX_Text(pDX, IDC_TEXT, m_strText);
	DDP_CBIndex(pDX, IDC_TEXT_ALIGN, m_nTextAlign, _T("TextAlign") );
	DDX_CBIndex(pDX, IDC_TEXT_ALIGN, m_nTextAlign);
	DDP_CBIndex(pDX, IDC_BORDER_STYLE, m_nBorderType, _T("BorderType") );
	DDX_CBIndex(pDX, IDC_BORDER_STYLE, m_nBorderType);
	DDP_CBIndex(pDX, IDC_TEXT_POSITION, m_nTextPosition, _T("TextPosition") );
	DDX_CBIndex(pDX, IDC_TEXT_POSITION, m_nTextPosition);
	DDP_Text(pDX, IDC_WIDTH, m_fLineWidth, _T("LineWidth") );
	DDX_Text(pDX, IDC_WIDTH, m_fLineWidth);
	DDV_MinMaxDouble(pDX, m_fLineWidth, 0.01, 10000.);
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
// CVT3DBarPropPage message handlers
LRESULT CVT3DBarPropPage::OnColorChange(WPARAM lParam, LPARAM wParam)
{
	SetModifiedFlag( TRUE );		
	return 1;
}

BOOL CVT3DBarPropPage::OnInitDialog() 
{
	COlePropertyPage::OnInitDialog();

	((CSpinButtonCtrl*)GetDlgItem(IDC_WIDTH_SPIN))->SetRange(1, 10000);
	
	
	return FALSE;
	             
}

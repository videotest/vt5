// VTCheckBoxPpg.cpp : Implementation of the CVTCheckBoxPropPage property page class.

#include "stdafx.h"
#include "VTControls.h"
#include "VTCheckBoxPpg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CVTCheckBoxPropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CVTCheckBoxPropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CVTCheckBoxPropPage)
	ON_BN_CLICKED(IDC_NEW_DRAWING_MODE, OnNewDrawingMode)
	ON_BN_CLICKED(IDC_3DAPPEARANCE, On3dappearance)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CVTCheckBoxPropPage, "VTCHECKBOX.VTCheckBoxPropPage.1",
	0x30274983, 0x521c, 0x489f, 0x98, 0x29, 0xb4, 0x52, 0x77, 0x96, 0x90, 0x87)


/////////////////////////////////////////////////////////////////////////////
// CVTCheckBoxPropPage::CVTCheckBoxPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CVTCheckBoxPropPage

BOOL CVTCheckBoxPropPage::CVTCheckBoxPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_VTCHECKBOX_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CVTCheckBoxPropPage::CVTCheckBoxPropPage - Constructor

CVTCheckBoxPropPage::CVTCheckBoxPropPage() :
	COlePropertyPage(IDD, IDS_VTCHECKBOX_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CVTCheckBoxPropPage)
	m_strCaption = _T("");
	m_bNewDrawMode = FALSE;
	m_bAppearance3D = FALSE;
	m_bUseSystemFont = FALSE;
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CVTCheckBoxPropPage::DoDataExchange - Moves data between page and properties

void CVTCheckBoxPropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CVTCheckBoxPropPage)
	DDP_Text(pDX, IDC_CAPTION, m_strCaption, _T("Caption") );
	DDX_Text(pDX, IDC_CAPTION, m_strCaption);
	DDP_Check(pDX, IDC_NEW_DRAWING_MODE, m_bNewDrawMode, _T("NewDrawMode") );
	DDX_Check(pDX, IDC_NEW_DRAWING_MODE, m_bNewDrawMode);
	DDP_Check(pDX, IDC_3DAPPEARANCE, m_bAppearance3D, _T("Appearance3D") );
	DDX_Check(pDX, IDC_3DAPPEARANCE, m_bAppearance3D);
	DDP_Check(pDX, IDC_USE_SYSTEM_FONT, m_bUseSystemFont, _T("UseSystemFont") );
	DDX_Check(pDX, IDC_USE_SYSTEM_FONT, m_bUseSystemFont);
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);


	

		
}


/////////////////////////////////////////////////////////////////////////////
// CVTCheckBoxPropPage message handlers
void CVTCheckBoxPropPage::OnNewDrawingMode() 
{	
	SetButtonsState();
}

/////////////////////////////////////////////////////////////////////////////
void CVTCheckBoxPropPage::On3dappearance() 
{
	SetButtonsState();		
}

/////////////////////////////////////////////////////////////////////////////
void CVTCheckBoxPropPage::SetButtonsState()
{
	CButton* pButtonDrawMode	= (CButton*)GetDlgItem(IDC_NEW_DRAWING_MODE);
	CButton* pButtonAppearance	= (CButton*)GetDlgItem(IDC_3DAPPEARANCE);

	if( !pButtonDrawMode || !pButtonDrawMode->GetSafeHwnd() ||
		!pButtonAppearance || !pButtonAppearance->GetSafeHwnd() )
		return;

	if( pButtonDrawMode->GetCheck() )
		pButtonAppearance->EnableWindow( TRUE );
	else
		pButtonAppearance->EnableWindow( FALSE );
	
}

/////////////////////////////////////////////////////////////////////////////
BOOL CVTCheckBoxPropPage::OnInitDialog() 
{
	COlePropertyPage::OnInitDialog();
	
	SetButtonsState();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

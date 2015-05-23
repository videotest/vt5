// VTStaticTextPpg.cpp : Implementation of the CVTStaticTextPropPage property page class.

#include "stdafx.h"
#include "VTControls.h"
#include "VTStaticTextPpg.h"
#include "Utils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CVTStaticTextPropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CVTStaticTextPropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CVTStaticTextPropPage)
	// NOTE - ClassWizard will add and remove message map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CVTStaticTextPropPage, "VTCONTROLS.VTStaticTextPropPage.1",
	0xeb99fe93, 0xcac0, 0x4b66, 0x8f, 0xca, 0x6c, 0x1a, 0xc4, 0xfb, 0x3c, 0xf9)


/////////////////////////////////////////////////////////////////////////////
// CVTStaticTextPropPage::CVTStaticTextPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CVTStaticTextPropPage

BOOL CVTStaticTextPropPage::CVTStaticTextPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_VTSTATICTEXT_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CVTStaticTextPropPage::CVTStaticTextPropPage - Constructor

CVTStaticTextPropPage::CVTStaticTextPropPage() :
	COlePropertyPage(IDD, IDS_VTSTATICTEXT_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CVTStaticTextPropPage)
	m_bUseSystemFont = FALSE;
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CVTStaticTextPropPage::DoDataExchange - Moves data between page and properties

void CVTStaticTextPropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CVTStaticTextPropPage)
	DDP_Check(pDX, IDC_USE_SYSTEM_FONT, m_bUseSystemFont, _T("UseSystemFont") );
	DDX_Check(pDX, IDC_USE_SYSTEM_FONT, m_bUseSystemFont);
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CVTStaticTextPropPage message handlers
/////////////////////////////////////////////////////////////////////////////
// CVTStaticTextPropPageExt dialog

IMPLEMENT_DYNCREATE(CVTStaticTextPropPageExt, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CVTStaticTextPropPageExt, COlePropertyPage)
	//{{AFX_MSG_MAP(CVTStaticTextPropPageExt)
	ON_BN_CLICKED(IDC_BUTTON_FILEOPEN, OnFileopen)
	ON_BN_CLICKED(IDC_BUTTON_RELOAD, OnReloadBitmap)
	ON_BN_CLICKED(IDC_BUTTON_AUTO_SIZE, OnAutoSize)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

// {ADB351C5-EE34-4E5D-9748-7A6F6E5B9239}		  
IMPLEMENT_OLECREATE_EX(CVTStaticTextPropPageExt, "VTCONTROLS.VTStaticTextPropPageExt.1",
	0xadb351c5, 0xee34, 0x4e5d, 0x97, 0x48, 0x7a, 0x6f, 0x6e, 0x5b, 0x92, 0x39)


/////////////////////////////////////////////////////////////////////////////
// CVTStaticTextPropPageExt::CVTStaticTextPropPageExtFactory::UpdateRegistry -
// Adds or removes system registry entries for CVTStaticTextPropPageExt

BOOL CVTStaticTextPropPageExt::CVTStaticTextPropPageExtFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: Define string resource for page type; replace '0' below with ID.

	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_VTSTATICTEXT_PPG_EXT);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CVTStaticTextPropPageExt::CVTStaticTextPropPageExt - Constructor

// TODO: Define string resource for page caption; replace '0' below with ID.

CVTStaticTextPropPageExt::CVTStaticTextPropPageExt() :
	COlePropertyPage(IDD, IDS_VTSTATICTEXT_PPG_EXT_CAPTION)
{
	//{{AFX_DATA_INIT(CVTStaticTextPropPageExt)
	m_bTextAlignTop = FALSE;
	m_bTextAlignRght = FALSE;
	m_bTextAlignLeft = FALSE;
	m_bTextAlignBottom = FALSE;
	m_bTextAlignCenter = FALSE;
	m_bTextAlignMiddle = FALSE;
	m_bPictAlignRight = FALSE;
	m_bPictAlignLeft = FALSE;
	m_bPictAlignTop = FALSE;
	m_bPictAlignBottom = FALSE;
	m_bText2PictBottom = FALSE;
	m_bText2PictLeft = FALSE;
	m_bText2PictRight = FALSE;
	m_bText2PictTop = FALSE;
	m_strPictureFileName = _T("");
	m_strText = _T("");
	m_bCenterImage = FALSE;
	m_bTransparencyText = FALSE;
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CVTStaticTextPropPageExt::DoDataExchange - Moves data between page and properties

void CVTStaticTextPropPageExt::DoDataExchange(CDataExchange* pDX)
{
	// NOTE: ClassWizard will add DDP, DDX, and DDV calls here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//{{AFX_DATA_MAP(CVTStaticTextPropPageExt)
	DDP_Check(pDX, IDC_CHECK_TOP_TEXT, m_bTextAlignTop, _T("TextAlignTop") );
	DDX_Check(pDX, IDC_CHECK_TOP_TEXT, m_bTextAlignTop);
	DDP_Check(pDX, IDC_CHECK_RIGHT_TEXT, m_bTextAlignRght, _T("TextAlignRight") );
	DDX_Check(pDX, IDC_CHECK_RIGHT_TEXT, m_bTextAlignRght);
	DDP_Check(pDX, IDC_CHECK_LEFT_TEXT, m_bTextAlignLeft, _T("TextAlignLeft") );
	DDX_Check(pDX, IDC_CHECK_LEFT_TEXT, m_bTextAlignLeft);
	DDP_Check(pDX, IDC_CHECK_BOTTOM_TEXT, m_bTextAlignBottom, _T("TextAlignBottom") );
	DDX_Check(pDX, IDC_CHECK_BOTTOM_TEXT, m_bTextAlignBottom);
	DDP_Check(pDX, IDC_CHECK_CENTER_TEXT, m_bTextAlignCenter, _T("TextAlignCenter") );
	DDX_Check(pDX, IDC_CHECK_CENTER_TEXT, m_bTextAlignCenter);
	DDP_Check(pDX, IDC_CHECK_MIDDLE_TEXT, m_bTextAlignMiddle, _T("TextAlignMiddle") );
	DDX_Check(pDX, IDC_CHECK_MIDDLE_TEXT, m_bTextAlignMiddle);
	DDP_Check(pDX, IDC_CHECK_RIGHT_PIC, m_bPictAlignRight, _T("PictAlignRight") );
	DDX_Check(pDX, IDC_CHECK_RIGHT_PIC, m_bPictAlignRight);
	DDP_Check(pDX, IDC_CHECK_LEFT_PIC, m_bPictAlignLeft, _T("PictAlignLeft") );
	DDX_Check(pDX, IDC_CHECK_LEFT_PIC, m_bPictAlignLeft);
	DDP_Check(pDX, IDC_CHECK_TOP_PIC, m_bPictAlignTop, _T("PictAlignTop") );
	DDX_Check(pDX, IDC_CHECK_TOP_PIC, m_bPictAlignTop);
	DDP_Check(pDX, IDC_CHECK_BOTTOM_PIC, m_bPictAlignBottom, _T("PictAlignBottom") );
	DDX_Check(pDX, IDC_CHECK_BOTTOM_PIC, m_bPictAlignBottom);
	DDP_Check(pDX, IDC_CHECK_BOTTOM_TEXT_TO_PIC, m_bText2PictBottom, _T("Text2PictAlignBottom") );
	DDX_Check(pDX, IDC_CHECK_BOTTOM_TEXT_TO_PIC, m_bText2PictBottom);
	DDP_Check(pDX, IDC_CHECK_LEFT_TEXT_TO_PIC, m_bText2PictLeft, _T("Text2PictAlignLeft") );
	DDX_Check(pDX, IDC_CHECK_LEFT_TEXT_TO_PIC, m_bText2PictLeft);
	DDP_Check(pDX, IDC_CHECK_RIGHT_TEXT_TO_PIC, m_bText2PictRight, _T("Text2PictAlignRight") );
	DDX_Check(pDX, IDC_CHECK_RIGHT_TEXT_TO_PIC, m_bText2PictRight);
	DDP_Check(pDX, IDC_CHECK_TOP_TEXT_TO_PIC, m_bText2PictTop, _T("Text2PictAlignTop") );
	DDX_Check(pDX, IDC_CHECK_TOP_TEXT_TO_PIC, m_bText2PictTop);
	DDP_Text(pDX, IDC_EDIT_PIC_FILENAME, m_strPictureFileName, _T("PictureFileName") );
	DDX_Text(pDX, IDC_EDIT_PIC_FILENAME, m_strPictureFileName);
	DDP_Text(pDX, IDC_EDIT_TEXT, m_strText, _T("Text") );
	DDX_Text(pDX, IDC_EDIT_TEXT, m_strText);
	DDP_Check(pDX, IDC_CHECK_CENTER_IMAGE, m_bCenterImage, _T("CenterImage") );
	DDX_Check(pDX, IDC_CHECK_CENTER_IMAGE, m_bCenterImage);
	DDP_Check(pDX, IDC_CHECK_TRANSPARENCY, m_bTransparencyText, _T("TransparencyText") );
	DDX_Check(pDX, IDC_CHECK_TRANSPARENCY, m_bTransparencyText);
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CVTStaticTextPropPageExt message handlers

BOOL CVTStaticTextPropPageExt::OnInitDialog() 
{
	COlePropertyPage::OnInitDialog();

	((CButton*)GetDlgItem(IDC_BUTTON_FILEOPEN))->
		SetIcon(LoadIcon(AfxGetInstanceHandle( ), "IDI_ICON"));	

	((CButton*)GetDlgItem(IDC_BUTTON_RELOAD))->
		SetIcon(AfxGetApp()->LoadIcon( IDI_ICON_RELOAD ) );

	
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CVTStaticTextPropPageExt::OnFileopen() 
{
	//UpdateData(TRUE);	

	CString strFileName;

	GetDlgItem(IDC_EDIT_PIC_FILENAME)->
				GetWindowText( strFileName );

	CFileDialog dlg(TRUE, 0, 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 0, 0, _FILE_OPEN_SIZE_ );
	
	strcpy(dlg.m_ofn.lpstrFile, strFileName);
	dlg.m_ofn.lpstrFilter = "Bitmap files(*.bmp)\0*.bmp\0\0";
	if(dlg.DoModal() == IDOK)
	{
		m_strPictureFileName = dlg.GetPathName();
		GetDlgItem(IDC_EDIT_PIC_FILENAME)->
				SetWindowText( m_strPictureFileName );
		UpdateData(TRUE);	
	}

	
	
}


void CVTStaticTextPropPageExt::OnReloadBitmap() 
{	
	ULONG Ulong;
	LPDISPATCH *lpDispatch	= NULL;
	LPDISPATCH pDispatch	= NULL;

	//Find object dispatch
	lpDispatch = GetObjectArray(&Ulong);	
	
	if( (!lpDispatch) || (!lpDispatch[0]) )
	{									   
		AfxMessageBox( "Can't find object dispatch interface" , 
			MB_OK | MB_ICONSTOP );
		return;
	}

	pDispatch = lpDispatch[0];

	::InvokeByName( pDispatch, "ReloadBitmap" );	
	UpdateData( TRUE );
}


void CVTStaticTextPropPageExt::OnAutoSize() 
{
	ULONG Ulong;
	LPDISPATCH *lpDispatch	= NULL;
	LPDISPATCH pDispatch	= NULL;

	//Find object dispatch
	lpDispatch = GetObjectArray(&Ulong);	
	
	if( (!lpDispatch) || (!lpDispatch[0]) )
	{									   
		AfxMessageBox( "Can't find object dispatch interface" , 
			MB_OK | MB_ICONSTOP );
		return;
	}

	pDispatch = lpDispatch[0];

	UpdateData( TRUE );	
	InvokeByName( pDispatch, "SetAutoSize" );	
	
}

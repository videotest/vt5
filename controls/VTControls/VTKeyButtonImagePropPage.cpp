// VTKeyButtonImagePropPage.cpp : implementation file
//

#include "stdafx.h"
#include "vtcontrols.h"
#include "VTKeyButtonImagePropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVTKeyButtonImagePropPage dialog

IMPLEMENT_DYNCREATE(CVTKeyButtonImagePropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CVTKeyButtonImagePropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CVTKeyButtonImagePropPage)
	ON_BN_CLICKED(IDC_BUTTON_FILEOPEN, OnButtonFileopen)
	ON_BN_CLICKED(IDC_BUTTON_RELOAD, OnButtonReload)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

// {E53DC803-C0E2-449B-88C5-E1EE3A5D0D7D}
IMPLEMENT_OLECREATE_EX(CVTKeyButtonImagePropPage, "VTCONTROLS.CVTKeyButtonImagePropPage.1",
	0xe53dc803, 0xc0e2, 0x449b, 0x88, 0xc5, 0xe1, 0xee, 0x3a, 0x5d, 0xd, 0x7d)


/////////////////////////////////////////////////////////////////////////////
// CVTKeyButtonImagePropPage::CVTKeyButtonImagePropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CVTKeyButtonImagePropPage

BOOL CVTKeyButtonImagePropPage::CVTKeyButtonImagePropPageFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: Define string resource for page type; replace '0' below with ID.

	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_VTKEYIMAGEBUTTON_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CVTKeyButtonImagePropPage::CVTKeyButtonImagePropPage - Constructor
CVTKeyButtonImagePropPage::CVTKeyButtonImagePropPage() :
	COlePropertyPage(IDD, IDS_VTKEYIMAGEBUTTON_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CVTKeyButtonImagePropPage)
	m_strFileName = _T("");
	m_nImageIndex = 0;
	m_nImageHeight = 0;
	m_nImageWidth = 0;
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CVTKeyButtonImagePropPage::DoDataExchange - Moves data between page and properties

void CVTKeyButtonImagePropPage::DoDataExchange(CDataExchange* pDX)
{
	// NOTE: ClassWizard will add DDP, DDX, and DDV calls here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//{{AFX_DATA_MAP(CVTKeyButtonImagePropPage)
	DDP_Text(pDX, IDC_EDIT_FILENAME, m_strFileName, _T("BmpFileName") );
	DDX_Text(pDX, IDC_EDIT_FILENAME, m_strFileName);
	DDP_Text(pDX, IDC_IMAGE_INDEX, m_nImageIndex, _T("ImageIndex") );
	DDX_Text(pDX, IDC_IMAGE_INDEX, m_nImageIndex);
	DDP_Text(pDX, IDC_ITEM_HEIGHT, m_nImageHeight, _T("ImageHeight") );
	DDX_Text(pDX, IDC_ITEM_HEIGHT, m_nImageHeight);
	DDP_Text(pDX, IDC_ITEM_WIDTH, m_nImageWidth, _T("ImageWidth") );
	DDX_Text(pDX, IDC_ITEM_WIDTH, m_nImageWidth);
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CVTKeyButtonImagePropPage message handlers
BOOL CVTKeyButtonImagePropPage::OnInitDialog() 
{
	COlePropertyPage::OnInitDialog();

	((CButton*)GetDlgItem(IDC_BUTTON_FILEOPEN))->
		SetIcon( LoadIcon(AfxGetInstanceHandle( ), "IDI_ICON") );	
	
	((CButton*)GetDlgItem(IDC_BUTTON_RELOAD))->
		SetIcon(AfxGetApp()->LoadIcon( IDI_ICON_RELOAD ) );


	((CSpinButtonCtrl*)(GetDlgItem(IDC_SPIN1)))->SetRange( 1, 100 );
	((CSpinButtonCtrl*)(GetDlgItem(IDC_SPIN2)))->SetRange( 1, 100 );
	((CSpinButtonCtrl*)(GetDlgItem(IDC_SPIN3)))->SetRange( -1, 100 );

	return TRUE;  
	              
}

/////////////////////////////////////////////////////////////////////////////
void CVTKeyButtonImagePropPage::OnButtonFileopen() 
{
	CFileDialog dlg(TRUE, 0, 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 0, 0, _FILE_OPEN_SIZE_ );
	
	CString strFileName;
	strcpy(dlg.m_ofn.lpstrFile, strFileName);
	dlg.m_ofn.lpstrFilter = "Bitmap files(*.bmp)\0*.bmp\0\0";
	if(dlg.DoModal() == IDOK)
	{		
		strFileName = dlg.GetPathName();
		GetDlgItem(IDC_EDIT_FILENAME)->SetWindowText(strFileName);
	}
	
}

/////////////////////////////////////////////////////////////////////////////
void CVTKeyButtonImagePropPage::OnButtonReload() 
{

	ULONG Ulong;
	LPDISPATCH *lpDispatch = NULL;

	lpDispatch = GetObjectArray(&Ulong);
	
	if( !lpDispatch[0] )
	{		
		return;	
	}

	
	DISPID dispID = 12;//ReloadImageList	

	DISPPARAMS dispparams;	

	VARIANT vt={VT_EMPTY};
	
	ZeroMemory( &dispparams, sizeof(dispparams) );	

	dispparams.rgvarg			= NULL;
	dispparams.cArgs			= 0;
	dispparams.cNamedArgs		= 0;		


	HRESULT hr = lpDispatch[0]->Invoke( 
					dispID, 
					IID_NULL, 
					LOCALE_SYSTEM_DEFAULT,
					DISPATCH_METHOD,
					&dispparams,
					&vt,
					NULL,
					NULL
					);


	ASSERT(hr == S_OK);	
	
}

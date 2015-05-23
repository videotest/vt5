// ImagePpg.cpp : Implementation of the CVTImagePropPage property page class.

#include "stdafx.h"
#include "VTDesign.h"
#include "ImagePpg.h"
#include "ax_ctrl_misc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CVTImagePropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CVTImagePropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CVTImagePropPage)
	ON_MESSAGE(CPN_SELENDOK, OnColorChange)
	ON_BN_CLICKED(IDC_LOAD, OnLoadPermanent)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowseFile)
	ON_BN_CLICKED(IDC_PERMANENT_LINK, OnPermanent)
	ON_BN_CLICKED(IDC_RADIO1, OnLinkWithFile)
	ON_BN_CLICKED(IDC_RADIO2, OnRadio2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CVTImagePropPage, "VTDESIGN.VTDImagePropPage.1",
	0xbc127b1c, 0x26c, 0x11d4, 0x81, 0x24, 0, 0, 0xe8, 0xdf, 0x68, 0xc3)


/////////////////////////////////////////////////////////////////////////////
// CVTImagePropPage::CVTImagePropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CVTImagePropPage

BOOL CVTImagePropPage::CVTImagePropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_VTDIMAGE_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CVTImagePropPage::CVTImagePropPage - Constructor

CVTImagePropPage::CVTImagePropPage() :
	COlePropertyPage(IDD, IDS_VTDIMAGE_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CVTImagePropPage)
	m_strFileName = _T("");	
	m_bPermanentImage = -1;
	//}}AFX_DATA_INIT

	m_bLockUpdateControl = FALSE;

	m_transparentColor = m_bkColor = RGB(0,0,0);
}


/////////////////////////////////////////////////////////////////////////////
// CVTImagePropPage::DoDataExchange - Moves data between page and properties

void CVTImagePropPage::DoDataExchange(CDataExchange* pDX)
{

	//{{AFX_DATA_MAP(CVTImagePropPage)	
	DDX_Control(pDX, IDC_COLOR_TRANSPARENT, m_transparentColorPicker);
	DDX_Control(pDX, IDC_COLOR_BK, m_bkColorPicker);
	if( !pDX->m_bSaveAndValidate || !m_bLockUpdateControl )
		DDP_Text(pDX, IDC_FILENAME, m_strFileName, _T("FileName") );
	DDX_Text(pDX, IDC_FILENAME, m_strFileName);
	if( !pDX->m_bSaveAndValidate || !m_bLockUpdateControl )
		DDP_Radio(pDX, IDC_PERMANENT_LINK, m_bPermanentImage, _T("PermanentImage") );
	DDX_Radio(pDX, IDC_PERMANENT_LINK, m_bPermanentImage);
	//}}AFX_DATA_MAP

	
	DDP_Text(pDX, IDC_COLOR_TRANSPARENT, m_transparentColor, _T("TransparentColor") );
	DDX_ColourPicker(pDX, IDC_COLOR_TRANSPARENT, m_transparentColor);

	DDP_Text(pDX, IDC_COLOR_BK, m_bkColor, _T("BackgroundColor") );
	DDX_ColourPicker(pDX, IDC_COLOR_BK, m_bkColor);


	DDP_PostProcessing(pDX);

	if( pDX->m_bSaveAndValidate )
		FirePropExchange( this );


}


/////////////////////////////////////////////////////////////////////////////
// CVTImagePropPage message handlers

BOOL CVTImagePropPage::OnInitDialog() 
{
	COlePropertyPage::OnInitDialog();

	((CButton*)GetDlgItem(IDC_LOAD))->SetIcon(LoadIcon(AfxGetInstanceHandle( ), "IDI_ICON_LOAD" ));	
	((CButton*)GetDlgItem(IDC_BROWSE))->SetIcon(LoadIcon(AfxGetInstanceHandle( ), "IDI_ICON_LOAD"));			

	UpdateCtrl();
	
	return FALSE;
	             
}

void CVTImagePropPage::UpdateCtrl()
{
	//UpdateData( TRUE );
	UpdateDataWithOutUpdateAX( );
	
	if( m_bPermanentImage != 0)
	{
		GetDlgItem(IDC_LOAD)->EnableWindow( TRUE );
		GetDlgItem(IDC_BROWSE)->EnableWindow( FALSE );
		GetDlgItem(IDC_FILENAME)->EnableWindow( FALSE );
	}
	else
	{
		GetDlgItem(IDC_LOAD)->EnableWindow( FALSE );
		GetDlgItem(IDC_BROWSE)->EnableWindow( TRUE );
		GetDlgItem(IDC_FILENAME)->EnableWindow( TRUE );
	}
}

void CVTImagePropPage::UpdateDataWithOutUpdateAX()
{
	m_bLockUpdateControl = TRUE;
	UpdateData( TRUE );
	m_bLockUpdateControl = FALSE;
}

LRESULT CVTImagePropPage::OnColorChange(WPARAM lParam, LPARAM wParam)
{
	SetModifiedFlag( TRUE );		
	return 1;
}

void CVTImagePropPage::OnLoadPermanent() 
{
	CString strFileName;

	GetDlgItem(IDC_FILENAME)->
				GetWindowText( strFileName );

	CFileDialog dlg(TRUE, 0, 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 0, 0, _FILE_OPEN_SIZE_ );
	
	strcpy(dlg.m_ofn.lpstrFile, strFileName);
	dlg.m_ofn.lpstrFilter = "Bitmap files(*.bmp)\0*.bmp\0\0";
	if(dlg.DoModal() == IDOK)
	{
		m_strFileName = dlg.GetPathName();
		GetDlgItem(IDC_FILENAME)->
				SetWindowText( m_strFileName );
		
		//UpdateData( TRUE );
	}
							   
	//UpdateData( TRUE );
}

void CVTImagePropPage::OnBrowseFile() 
{
	OnLoadPermanent();
	//UpdateData( TRUE );
	UpdateDataWithOutUpdateAX( );
}

void CVTImagePropPage::OnPermanent() 
{
	UpdateCtrl();
}

void CVTImagePropPage::OnLinkWithFile() 
{
	UpdateCtrl();
}

void CVTImagePropPage::OnRadio2() 
{
	UpdateCtrl();	
}

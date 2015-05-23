// AciveColorsViewPpg.cpp : Implementation of the CAciveColorsViewPropPage property page class.

#include "stdafx.h"
#include "AciveColorsView.h"
#include "AciveColorsViewPpg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CAciveColorsViewPropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CAciveColorsViewPropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CAciveColorsViewPropPage)
	ON_BN_CLICKED(IDC_BUTTON_FILEOPEN, OnButtonFileopen)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CAciveColorsViewPropPage, "ACIVECOLORSVIEW.AciveColorsViewPropPage.1",
	0xc1f6a8c, 0x9db3, 0x11d3, 0xa5, 0x36, 0, 0, 0xb4, 0x93, 0xa1, 0x87)


/////////////////////////////////////////////////////////////////////////////
// CAciveColorsViewPropPage::CAciveColorsViewPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CAciveColorsViewPropPage

BOOL CAciveColorsViewPropPage::CAciveColorsViewPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_ACIVECOLORSVIEW_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CAciveColorsViewPropPage::CAciveColorsViewPropPage - Constructor

CAciveColorsViewPropPage::CAciveColorsViewPropPage() :
	COlePropertyPage(IDD, IDS_ACIVECOLORSVIEW_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CAciveColorsViewPropPage)
	m_strFileName = _T("");
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CAciveColorsViewPropPage::DoDataExchange - Moves data between page and properties

void CAciveColorsViewPropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CAciveColorsViewPropPage)
	DDX_Control(pDX, IDC_EDIT_FILENAME, m_editFileName);
	DDX_Control(pDX, IDC_BUTTON_FILEOPEN, m_btnBrowse);
	DDP_Text(pDX, IDC_EDIT_FILENAME, m_strFileName, _T("BMPFileName") );
	DDX_Text(pDX, IDC_EDIT_FILENAME, m_strFileName);
	DDV_MaxChars(pDX, m_strFileName, 255);
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CAciveColorsViewPropPage message handlers

void CAciveColorsViewPropPage::OnButtonFileopen() 
{
	// TODO: Add your control notification handler code here
	CFileDialog dlg(TRUE, 0, 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 0, 0, _FILE_OPEN_SIZE_ );
	UpdateData(TRUE);
	strcpy(dlg.m_ofn.lpstrFile, m_strFileName);
	dlg.m_ofn.lpstrFilter = "Bitmap files(*.bmp)\0*.bmp\0\0";
	if(dlg.DoModal() == IDOK)
	{
		m_strFileName = dlg.GetPathName();
	}
	m_editFileName.SetWindowText(m_strFileName);
	
}

BOOL CAciveColorsViewPropPage::OnInitDialog() 
{
	COlePropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_btnBrowse.SetIcon(LoadIcon(AfxGetInstanceHandle(), "IDI_ICON"));
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

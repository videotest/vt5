// PalettePpg.cpp : Implementation of the CPalettePropPage property page class.

#include "stdafx.h"
#include "Palette.h"
#include "PalettePpg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CPalettePropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CPalettePropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CPalettePropPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CPalettePropPage, "PALETTE.PalettePropPage.1",
	0x57c4cb49, 0x9e87, 0x11d3, 0xa5, 0x37, 0, 0, 0xb4, 0x93, 0xa1, 0x87)


/////////////////////////////////////////////////////////////////////////////
// CPalettePropPage::CPalettePropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CPalettePropPage

BOOL CPalettePropPage::CPalettePropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_PALETTE_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CPalettePropPage::CPalettePropPage - Constructor

CPalettePropPage::CPalettePropPage() :
	COlePropertyPage(IDD, IDS_PALETTE_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CPalettePropPage)
	m_nCols = 0;
	m_nRows = 0;
	m_nIntercellSpace = 0;
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CPalettePropPage::DoDataExchange - Moves data between page and properties

void CPalettePropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CPalettePropPage)
	DDP_Text(pDX, IDC_EDIT_COLS, m_nCols, _T("Cols") );
	DDX_Text(pDX, IDC_EDIT_COLS, m_nCols);
	DDV_MinMaxInt(pDX, m_nCols, 2, 20);
	DDP_Text(pDX, IDC_EDIT_ROWS, m_nRows, _T("Rows") );
	DDX_Text(pDX, IDC_EDIT_ROWS, m_nRows);
	DDV_MinMaxInt(pDX, m_nRows, 2, 20);
	DDP_Text(pDX, IDC_EDIT_INTERCELLSPACE, m_nIntercellSpace, _T("IntercellSpace") );
	DDX_Text(pDX, IDC_EDIT_INTERCELLSPACE, m_nIntercellSpace);
	DDV_MinMaxInt(pDX, m_nIntercellSpace, 0, 10);
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CPalettePropPage message handlers

/*void CPalettePropPage::OnButtonFileopen() 
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
}*/

BOOL CPalettePropPage::OnInitDialog() 
{
	COlePropertyPage::OnInitDialog();
	// TODO: Add extra initialization here
	//m_btnBrowse.SetIcon(LoadIcon(AfxGetInstanceHandle( ), "IDI_ICON"));
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


// ViewLoaderPropPage.cpp : Implementation of the CViewLoaderPropPage property page class.

#include "stdafx.h"
#include "resource.h"
#include "ViewLoaderPropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CViewLoaderPropPage, COlePropertyPage)



// Message map

BEGIN_MESSAGE_MAP(CViewLoaderPropPage, COlePropertyPage)
END_MESSAGE_MAP()



// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CViewLoaderPropPage, "VIEWLOADER.ViewLoaderPropPage.1",
	0xbccabf37, 0xa2c2, 0x4636, 0xb9, 0x7, 0xb7, 0xe8, 0x75, 0x8d, 0x8e, 0xe6)



// CViewLoaderPropPage::CViewLoaderPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CViewLoaderPropPage

BOOL CViewLoaderPropPage::CViewLoaderPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_VIEWLOADER_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}



// CViewLoaderPropPage::CViewLoaderPropPage - Constructor

CViewLoaderPropPage::CViewLoaderPropPage() :
	COlePropertyPage(IDD, IDS_VIEWLOADER_PPG_CAPTION)
		, m_tAlgorithm(0)
	{
}



// CViewLoaderPropPage::DoDataExchange - Moves data between page and properties

void CViewLoaderPropPage::DoDataExchange(CDataExchange* pDX)
{
	DDP_CBIndex(pDX, IDC_TEMPLATE_ALGORITHM, (int&)m_tAlgorithm, _T("TemplateAlgorithm"));
	DDX_CBIndex(pDX, IDC_TEMPLATE_ALGORITHM, (int&)m_tAlgorithm);
	DDP_PostProcessing(pDX);
}


//enum AutoTemplateAlghoritm
//{
//	ataActiveView = 0,
//	ataOneObjectPerPage = 1,
//	ataVerticalAsIs = 2,
//	ataGallery = 3,
//	ataFromSplitter = 4,
//};
// CViewLoaderPropPage message handlers

BOOL CViewLoaderPropPage::OnInitDialog()
{
	COlePropertyPage::OnInitDialog();

	CComboBox* cb = (CComboBox*)GetDlgItem(IDC_TEMPLATE_ALGORITHM);
	cb->InsertString(0,_T("ActiveView"));
	cb->InsertString(1,_T("OneObjectPerPage"));
	cb->InsertString(2,_T("VerticalAsIs"));
	cb->InsertString(3,_T("Gallery"));
	cb->InsertString(4,_T("FromSplitter"));
	return FALSE;
}

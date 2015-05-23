// VTTablePropPage.cpp : Implementation of the CVTTablePropPage property page class.

#include "stdafx.h"
#include "VTTable.h"
#include "VTTableCtrl.h"
#include "VTTablePropPage.h"
#include <comdef.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CVTTablePropPage, COlePropertyPage)



// Message map

BEGIN_MESSAGE_MAP(CVTTablePropPage, COlePropertyPage)
	ON_BN_CLICKED(IDC_PASTE, OnBnClickedPaste)
	ON_BN_CLICKED(IDC_COPY, OnBnClickedCopy)
END_MESSAGE_MAP()



// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CVTTablePropPage, "VTTABLE.VTTablePropPage.1",
	0xd01d160b, 0xd12d, 0x4693, 0x8d, 0x2d, 0xc8, 0x52, 0x9e, 0x32, 0x3e, 0xad)



// CVTTablePropPage::CVTTablePropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CVTTablePropPage

BOOL CVTTablePropPage::CVTTablePropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_VTTABLE_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}



// CVTTablePropPage::CVTTablePropPage - Constructor

CVTTablePropPage::CVTTablePropPage() :
	COlePropertyPage(IDD, IDS_VTTABLE_PPG_CAPTION)
{
}



// CVTTablePropPage::DoDataExchange - Moves data between page and properties

void CVTTablePropPage::DoDataExchange(CDataExchange* pDX)
{
	DDP_Text(pDX, IDC_ROOTSECTION, m_sRootSection, _T("RootSection") );
	DDX_Text(pDX, IDC_ROOTSECTION, m_sRootSection);

	DDP_PostProcessing(pDX);
}

//////////////////////////////////////////////////////////
IDispatch* CVTTablePropPage::GetAXInstance()
{
	unsigned long lCount = 0;
	IDispatch **ppdisp = GetObjectArray( &lCount );

	if( lCount == 0 )
		return 0;
	
	return ppdisp[0];
}



// CVTTablePropPage message handlers

void CVTTablePropPage::OnBnClickedPaste()
{
	// TODO: Add your control notification handler code here
	IDispatch* pDisp = GetAXInstance( );
	if( pDisp )
	{
		// paste
		HRESULT hRes = _com_dispatch_method( pDisp, CVTTableCtrl::dispidPasteFromClipboard, 
			DISPATCH_METHOD, VT_EMPTY, NULL, NULL );
	}
}

void CVTTablePropPage::OnBnClickedCopy()
{
	// TODO: Add your control notification handler code here
	IDispatch* pDisp = GetAXInstance( );
	if( pDisp )
	{
		// paste
		HRESULT hRes = _com_dispatch_method( pDisp, CVTTableCtrl::dispidCopyToClipboard, 
			DISPATCH_METHOD, VT_EMPTY, NULL, NULL );
	}
}

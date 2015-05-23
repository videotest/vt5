#include "stdafx.h"
#include "afxpriv2.h"

#include "print.h"
#include "PrintActions.h"
#include "resource.h"
//#include "TemplateSetup.h"
#include "PrinterSetup.h"
#include "PageSetup.h"
#include "PrintView.h"
#include "PrintPreview.h"
#include "PrintProgress.h"
#include "TemplateDeleteDlg.h"
#include "TemplateDlg.h"
#include "ReportConstructor.h"



extern CReportConstructor g_ReportConstructor;


HANDLE CopyHandle(HANDLE h); //In printDlg

//[ag]1. dyncreate

IMPLEMENT_DYNCREATE(CActionShowPreviewIndirect, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionPrintTemplateDelete, CCmdTargetEx);
//IMPLEMENT_DYNCREATE(CActionLeavePrintPreview, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionTemplateSetup, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionFitToPageWidth, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionFitToPage, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionPrintSetup, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionPageSetup, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionShowPrintDesignMode, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionPrint, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionShowPrintPreview, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionReportEditText, CCmdTargetEx);

// olecreate 

// {E9AB9A83-FE02-4ebd-9D46-E4256BB11181}
GUARD_IMPLEMENT_OLECREATE(CActionShowPreviewIndirect, "Print.ShowPreviewIndirect",
0xe9ab9a83, 0xfe02, 0x4ebd, 0x9d, 0x46, 0xe4, 0x25, 0x6b, 0xb1, 0x11, 0x81);
// {F199658A-BF10-4395-A63A-286FFF4A8328}
GUARD_IMPLEMENT_OLECREATE(CActionPrintTemplateDelete, "Print.PrintTemplateDelete",
0xf199658a, 0xbf10, 0x4395, 0xa6, 0x3a, 0x28, 0x6f, 0xff, 0x4a, 0x83, 0x28);
/*
// {DDA43EA0-2505-4f08-B29C-61DA4D27A348}
GUARD_IMPLEMENT_OLECREATE(CActionLeavePrintPreview, "Print.LeavePrintPreview",
0xdda43ea0, 0x2505, 0x4f08, 0xb2, 0x9c, 0x61, 0xda, 0x4d, 0x27, 0xa3, 0x48);
*/
// {78528574-930E-4984-A8BA-296E0087FA1C}
GUARD_IMPLEMENT_OLECREATE(CActionTemplateSetup, "Print.TemplateSetup",
0x78528574, 0x930e, 0x4984, 0xa8, 0xba, 0x29, 0x6e, 0x0, 0x87, 0xfa, 0x1c);
// {C8FFEBCA-0E68-4880-9DA3-F9245E5F5B4F}
GUARD_IMPLEMENT_OLECREATE(CActionFitToPageWidth, "Print.FitToPageWidth",
0xc8ffebca, 0xe68, 0x4880, 0x9d, 0xa3, 0xf9, 0x24, 0x5e, 0x5f, 0x5b, 0x4f);
// {81703A82-5D51-464c-973C-02BD825AAAB0}
GUARD_IMPLEMENT_OLECREATE(CActionFitToPage, "Print.FitToPage",
0x81703a82, 0x5d51, 0x464c, 0x97, 0x3c, 0x2, 0xbd, 0x82, 0x5a, 0xaa, 0xb0);
// {A578B577-0AA9-4abe-ACB9-8E8EAFD9103D}
GUARD_IMPLEMENT_OLECREATE(CActionPrintSetup, "Print.PrintSetup",
0xa578b577, 0xaa9, 0x4abe, 0xac, 0xb9, 0x8e, 0x8e, 0xaf, 0xd9, 0x10, 0x3d);
// {E5788774-2552-4c4e-B089-5B032BB7A78D}
GUARD_IMPLEMENT_OLECREATE(CActionPageSetup, "Print.PageSetup",
0xe5788774, 0x2552, 0x4c4e, 0xb0, 0x89, 0x5b, 0x3, 0x2b, 0xb7, 0xa7, 0x8d);
// {6F19C7A3-0681-47ae-BF55-A5EDB89A77F9}
GUARD_IMPLEMENT_OLECREATE(CActionShowPrintDesignMode, "Print.ShowPrintDesignMode",
0x6f19c7a3, 0x681, 0x47ae, 0xbf, 0x55, 0xa5, 0xed, 0xb8, 0x9a, 0x77, 0xf9);
// {A88EE732-DB96-4999-93A3-785B8278811B}
GUARD_IMPLEMENT_OLECREATE(CActionPrint, "Print.Print",
0xa88ee732, 0xdb96, 0x4999, 0x93, 0xa3, 0x78, 0x5b, 0x82, 0x78, 0x81, 0x1b);
// {48F5E456-E89B-4e05-B6CE-7613F6A93F88}
GUARD_IMPLEMENT_OLECREATE(CActionShowPrintPreview, "Print.ShowPrintPreview",
0x48f5e456, 0xe89b, 0x4e05, 0xb6, 0xce, 0x76, 0x13, 0xf6, 0xa9, 0x3f, 0x88);
// {A8A5C578-300E-49f5-B3A5-38B027C499DC}
GUARD_IMPLEMENT_OLECREATE(CActionReportEditText, "Print.ReportEditText", 
0xa8a5c578, 0x300e, 0x49f5, 0xb3, 0xa5, 0x38, 0xb0, 0x27, 0xc4, 0x99, 0xdc);

// guidinfo 

// {53B3221B-3796-4514-949D-0916278236EE}
static const GUID guidShowPreviewIndirect =
{ 0x53b3221b, 0x3796, 0x4514, { 0x94, 0x9d, 0x9, 0x16, 0x27, 0x82, 0x36, 0xee } };
// {AC0A004F-7F2E-4860-9A67-D46B785E5C18}
static const GUID guidPrintTemplateDelete =
{ 0xac0a004f, 0x7f2e, 0x4860, { 0x9a, 0x67, 0xd4, 0x6b, 0x78, 0x5e, 0x5c, 0x18 } };
// {C95269CB-C926-4ffb-A260-1398383F0D79}
static const GUID guidLeavePrintPreview =
{ 0xc95269cb, 0xc926, 0x4ffb, { 0xa2, 0x60, 0x13, 0x98, 0x38, 0x3f, 0xd, 0x79 } };
// {6650407B-AD83-47e1-A496-AB53F3042BEF}
static const GUID guidTemplateSetup =
{ 0x6650407b, 0xad83, 0x47e1, { 0xa4, 0x96, 0xab, 0x53, 0xf3, 0x4, 0x2b, 0xef } };
// {81D5CD12-CCF0-491e-B34A-641523BCBF1A}
static const GUID guidFitToPageWidth =
{ 0x81d5cd12, 0xccf0, 0x491e, { 0xb3, 0x4a, 0x64, 0x15, 0x23, 0xbc, 0xbf, 0x1a } };
// {17B7D4F7-6451-4572-A4D3-EAD92EDEDDAA}
static const GUID guidFitToPage =
{ 0x17b7d4f7, 0x6451, 0x4572, { 0xa4, 0xd3, 0xea, 0xd9, 0x2e, 0xde, 0xdd, 0xaa } };
// {5F9CD474-E419-47ed-886F-9CEF298272C7}
static const GUID guidPrintSetup =
{ 0x5f9cd474, 0xe419, 0x47ed, { 0x88, 0x6f, 0x9c, 0xef, 0x29, 0x82, 0x72, 0xc7 } };
// {E1F0FD1D-978E-4683-9544-0A7FD6A964B2}
static const GUID guidPageSetup =
{ 0xe1f0fd1d, 0x978e, 0x4683, { 0x95, 0x44, 0xa, 0x7f, 0xd6, 0xa9, 0x64, 0xb2 } };
// {76B5C534-0F91-40bd-9B76-7EC2AA7ADE5A}
static const GUID guidShowPrintDesignMode =
{ 0x76b5c534, 0xf91, 0x40bd, { 0x9b, 0x76, 0x7e, 0xc2, 0xaa, 0x7a, 0xde, 0x5a } };
// {AB06F57B-5999-43b5-84D7-84B10D261236}
static const GUID guidPrint =
{ 0xab06f57b, 0x5999, 0x43b5, { 0x84, 0xd7, 0x84, 0xb1, 0xd, 0x26, 0x12, 0x36 } };
// {B65C576F-D787-4903-9AD2-41590D2D8100}
static const GUID guidShowPrintPreview =
{ 0xb65c576f, 0xd787, 0x4903, { 0x9a, 0xd2, 0x41, 0x59, 0xd, 0x2d, 0x81, 0x0 } };
// {62666A4F-86B0-4dc8-888F-7304463EA489}
static const GUID guidReportEditText = 
{ 0x62666a4f, 0x86b0, 0x4dc8, { 0x88, 0x8f, 0x73, 0x4, 0x46, 0x3e, 0xa4, 0x89 } };
// action info


ACTION_INFO_FULL(CActionShowPreviewIndirect, IDS_ACTION_PRINT_SHOW_INDIRECT, -1, -1, guidShowPreviewIndirect);
ACTION_INFO_FULL(CActionPrintTemplateDelete, IDS_ACTION_PRINT_TEMPLATE_DELETE, -1, -1, guidPrintTemplateDelete);
//ACTION_INFO_FULL(CActionLeavePrintPreview, IDS_ACTION_LEAVE_PRINTPREVIEW, -1, -1, guidLeavePrintPreview);
ACTION_INFO_FULL(CActionTemplateSetup, IDS_ACTION_TEMPLATE_SETUP, -1, -1, guidTemplateSetup);
ACTION_INFO_FULL(CActionFitToPageWidth, IDS_ACTION_FIT_TO_PAGE_WIDTH, -1, -1, guidFitToPageWidth);
ACTION_INFO_FULL(CActionFitToPage, IDS_ACTION_FIT_TO_PAGE, -1, -1, guidFitToPage);
ACTION_INFO_FULL(CActionPrintSetup, IDS_ACTION_PRINT_SETUP, -1, -1, guidPrintSetup);
ACTION_INFO_FULL(CActionPageSetup, IDS_ACTION_PAGE_SETUP, -1, -1, guidPageSetup);
ACTION_INFO_FULL(CActionShowPrintDesignMode, IDS_ACTION_SHOW_PRINT_DESIGN_MODE, -1, -1, guidShowPrintDesignMode);
ACTION_INFO_FULL(CActionShowPrintPreview, IDS_ACTION_SHOW_PRINTPREVIEW, -1, -1, guidShowPrintPreview);
ACTION_INFO_FULL(CActionPrint, IDS_ACTION_PRINT, -1, -1, guidPrint);
ACTION_INFO_FULL(CActionReportEditText, IDS_ACTION_REPORTEDITTEXT, -1, -1, guidReportEditText);

//[ag]7. targets

ACTION_TARGET(CActionShowPreviewIndirect, szTargetFrame);
ACTION_TARGET(CActionPrintTemplateDelete, szTargetApplication);
//ACTION_TARGET(CActionLeavePrintPreview, szTargetFrame);
ACTION_TARGET(CActionTemplateSetup, "anydoc");
ACTION_TARGET(CActionFitToPageWidth, szTargetViewSite);
ACTION_TARGET(CActionFitToPage, szTargetViewSite);
ACTION_TARGET(CActionPrintSetup, szTargetApplication);
ACTION_TARGET(CActionPageSetup, "anydoc");
ACTION_TARGET(CActionShowPrintDesignMode, szTargetFrame);
ACTION_TARGET(CActionShowPrintPreview, szTargetFrame);
ACTION_TARGET(CActionPrint, szTargetFrame);
ACTION_TARGET(CActionReportEditText, szTargetViewSite);

ACTION_ARG_LIST(CActionShowPreviewIndirect)
	ARG_INT("ReadOnly", 1)
END_ACTION_ARG_LIST()


//[ag]9. implementation

//////////////////////////////////////////////////////////////////////
//CActionShowPrintDesignMode implementation
CString CActionShowPreviewIndirect::GetViewProgID()
{
	return szPrintViewProgID;	
}	


bool CActionShowPreviewIndirect::Invoke()
{
	sptrIFrameWindow	sptrF( m_punkTarget );
	
	if( sptrF == NULL )
		return false;

	IUnknown	*punkDoc;
	sptrF->GetDocument( &punkDoc );

	if( punkDoc == NULL )
		return false;	


	sptrIDocumentSite	sptrDocSite( punkDoc );
	punkDoc->Release();

	
	if( !CActionShowViewBase::Invoke() )
		return false;


	IUnknown	*punkV = 0;
	sptrDocSite->GetActiveView( &punkV );

	if( !punkV )
		return false;

	sptrIReportView	sptrV(punkV);
	punkV->Release();

	if( sptrV == NULL )
		return false;

	bool bReadOnly = ( GetArgumentInt( "ReadOnly" ) == 1 );

	sptrV->EnterPreviewMode( bReadOnly );

	return true;

}

//extended UI
bool CActionShowPreviewIndirect::IsAvaible()
{
	return true;
}

bool CActionShowPreviewIndirect::IsChecked()
{

	if( !CActionShowViewBase::IsChecked() )
		return false;

	sptrIFrameWindow	sptrF( m_punkTarget );

	if( sptrF == NULL )
		return false;

	IUnknown	*punkDoc;
	sptrF->GetDocument( &punkDoc );

	if( punkDoc == NULL )
		return false;	


	sptrIDocumentSite	sptrDocSite( punkDoc );
	punkDoc->Release();

	IUnknown	*punkV = 0;
	sptrDocSite->GetActiveView( &punkV );

	if( !punkV )
		return false;

	sptrIReportView	sptrV(punkV);
	punkV->Release();

	if( sptrV == NULL )
		return false;

	int nMode;
	sptrV->GetMode( &nMode );
	if( (ViewMode)nMode == vmPreview )
		return true;
	else
		return false;
}




CWnd* _GetActiveView()
{	
	sptrIApplication sptrA( GetAppUnknown( ) );
	if( sptrA == NULL )
		return NULL;

	/*
	HWND hwnd = NULL;
	ISOK( sptrA->GetMainWindowHandle( &hwnd ) );
	if( hwnd )
	{
		return CWnd::FromHandle( hwnd );
	}

	return NULL;
	*/
		
	
	IUnknown* punk = NULL;
	ISOK( sptrA->GetActiveDocument( &punk ) );
	if( punk == NULL )
		return NULL;

	sptrIDocumentSite sptrDS( punk );
	punk->Release();

	if( sptrDS == NULL )
		return NULL;

	ISOK( sptrDS->GetActiveView( &punk ) );

	if( punk == NULL )
		return NULL;

	sptrIView sptrV( punk );
	punk->Release();

	if( sptrV == NULL )
		return NULL;


	CWnd* pWndView = NULL;

	pWndView = GetWindowFromUnknown( sptrV );

	return pWndView;
	

}


//////////////////////////////////////////////////////////////////////
//CActionPrintTemplateDelete implementation
CActionPrintTemplateDelete::CActionPrintTemplateDelete()
{
}

CActionPrintTemplateDelete::~CActionPrintTemplateDelete()
{
}

bool CActionPrintTemplateDelete::Invoke()
{
	CTemplateDeleteDlg dlg( _GetActiveView() );
	dlg.DoModal( );

	return true;
}

//extended UI
bool CActionPrintTemplateDelete::IsAvaible()
{
	return true;
}

bool CActionPrintTemplateDelete::IsChecked()
{
	return false;
}

/*
//////////////////////////////////////////////////////////////////////
//CActionLeavePrintPreview implementation
CActionLeavePrintPreview::CActionLeavePrintPreview()
{
}

CActionLeavePrintPreview::~CActionLeavePrintPreview()
{
}

bool CActionLeavePrintPreview::Invoke()
{
	return true;
}

//extended UI
bool CActionLeavePrintPreview::IsAvaible()
{
	return false;
}

bool CActionLeavePrintPreview::IsChecked()
{
	return false;
}
*/

//////////////////////////////////////////////////////////////////////
//CActionTemplateSetup implementation
CActionTemplateSetup::CActionTemplateSetup()
{
}

CActionTemplateSetup::~CActionTemplateSetup()
{
}

bool CActionTemplateSetup::Invoke()
{	
	g_ReportConstructor.AttachDocument( m_punkTarget );

	CTemplateDlg dlg( _GetActiveView() );
	dlg.SetDocSite( m_punkTarget );	
	dlg.DoModal();

	return true;
}

//extended UI
bool CActionTemplateSetup::IsAvaible()
{
	return true;
}

bool CActionTemplateSetup::IsChecked()
{
	return false;
}

//////////////////////////////////////////////////////////////////////
//CActionFitToPageWidth implementation
CActionFitToPageWidth::CActionFitToPageWidth()
{
}

CActionFitToPageWidth::~CActionFitToPageWidth()
{
}

bool CActionFitToPageWidth::Invoke()
{
	sptrIReportView	sptrV( m_punkTarget );
	if( sptrV == NULL )
		return false;

	sptrV->FitToPageWidth();

	return true;
}

//extended UI
bool CActionFitToPageWidth::IsAvaible()
{
	sptrIReportView	sptrV( m_punkTarget );
	if( sptrV == NULL )
		return false;

	return true;
}

bool CActionFitToPageWidth::IsChecked()
{
	return false;
}

//////////////////////////////////////////////////////////////////////
//CActionFitToPage implementation
CActionFitToPage::CActionFitToPage()
{
}

CActionFitToPage::~CActionFitToPage()
{
}

bool CActionFitToPage::Invoke()
{
	sptrIReportView	sptrV( m_punkTarget );
	if( sptrV == NULL )
		return false;

	sptrV->FitToPage();

	return true;
}

//extended UI
bool CActionFitToPage::IsAvaible()
{
	sptrIReportView	sptrV( m_punkTarget );
	if( sptrV == NULL )
		return false;
	return true;
}

bool CActionFitToPage::IsChecked()
{
	return false;
}

//////////////////////////////////////////////////////////////////////
//CActionPrintSetup implementation
CActionPrintSetup::CActionPrintSetup()
{
}

CActionPrintSetup::~CActionPrintSetup()
{
}



bool CActionPrintSetup::Invoke()
{
	BOOL bRes = FALSE;
	try{

	
	bRes = g_prnSetup.ChooseAnother();
	g_prnSetup.Write();

	IApplicationPtr appPtr( m_punkTarget );
	if( appPtr == NULL )
		return false;

	IUnknown* punk = NULL;
	ISOK( appPtr->GetActiveDocument( &punk ));
	if( punk == NULL )
		return false;

	sptrIDocumentSite	sptrDocSite( punk );

	punk->Release();

	if( sptrDocSite == NULL )
		return false;

	ISOK( sptrDocSite->GetActiveView( &punk) );

	if( punk == NULL )
		return false;		

	IReportViewPtr reportView( punk );	

	punk->Release();

	if( reportView != NULL )
		ISOK( reportView->OnPrinterSettingsChange() );
	
	}
	catch(...)
	{
		AfxMessageBox( "catch..." );
	}

	return bRes?true:false;
}

//extended UI
bool CActionPrintSetup::IsAvaible()
{
	return true;
}

bool CActionPrintSetup::IsChecked()
{
	return false;
}

//////////////////////////////////////////////////////////////////////
//CActionPageSetup implementation
CActionPageSetup::CActionPageSetup()
{
}

CActionPageSetup::~CActionPageSetup()
{
}

bool CActionPageSetup::Invoke()
{
	sptrIDocumentSite	sptrDocSite( m_punkTarget );

	IUnknown	*punkForm = ::GetActiveObjectFromDocument( sptrDocSite, szArgumenReportForm );	

	if( !punkForm )
		return false;

	sptrIReportForm form( punkForm );
	punkForm->Release();

	if( form == NULL )
		return false;

	int nOrientation;
	CSize size;
	CRect rect;

	CPageSetup dlg( _GetActiveView() );

	ISOK( form->GetPaperSize( &size ) );
	ISOK( form->GetPaperField( &rect ) );
	ISOK( form->GetPaperOrientation( &nOrientation ) );
	


	dlg.SetData( FROM_VTPIXEL_TO_DOUBLE(size.cx), FROM_VTPIXEL_TO_DOUBLE(size.cy),
					FROM_VTPIXEL_TO_DOUBLE(rect.left), FROM_VTPIXEL_TO_DOUBLE(rect.top), 
					FROM_VTPIXEL_TO_DOUBLE(rect.right), FROM_VTPIXEL_TO_DOUBLE(rect.bottom),
					nOrientation
					);


	ISOK( form->GetSimpleMode( &dlg.m_bUseSimpleMode ) );		
	ISOK( form->GetBuildMode( &dlg.m_nBuildMode ) );		

	

	

	if( dlg.DoModal() == IDOK )
	{
		ISOK( form->SetPaperSize( CSize(
			FROM_DOUBLE_TO_VTPIXEL(dlg.m_fWidth), 
			FROM_DOUBLE_TO_VTPIXEL(dlg.m_fHeight)
			) ) );
		ISOK( form->SetPaperField( CRect(
			FROM_DOUBLE_TO_VTPIXEL(dlg.m_fLeft), 
			FROM_DOUBLE_TO_VTPIXEL(dlg.m_fTop), 
			FROM_DOUBLE_TO_VTPIXEL(dlg.m_fRight), 
			FROM_DOUBLE_TO_VTPIXEL(dlg.m_fBottom)
			) ) );
		ISOK( form->SetPaperOrientation( dlg.m_nOrientation ) );		


		ISOK( form->SetSimpleMode( dlg.m_bUseSimpleMode ) );	

		ISOK( form->SetBuildMode( dlg.m_nBuildMode ) );
	}


	IUnknown	*punkV = 0;
	sptrDocSite->GetActiveView( &punkV );

	if( !punkV )
		return false;

	sptrIReportView	sptrV(punkV);
	punkV->Release();

	if( sptrV == NULL )
		return false;

	sptrV->UpdateView();

	return true;
}

//extended UI
bool CActionPageSetup::IsAvaible()
{
	sptrIDocumentSite	sptrDocSite( m_punkTarget );

	IUnknown	*punkForm = ::GetActiveObjectFromDocument( sptrDocSite, szArgumenReportForm );	

	if( !punkForm )
		return false;

	sptrIReportForm form( punkForm );
	punkForm->Release();

	if( form == NULL )
		return false;	

	/////////////////////////
	IUnknown	*punkV = 0;
	sptrDocSite->GetActiveView( &punkV );

	if( !punkV )
		return false;

	sptrIReportView	sptrV(punkV);
	punkV->Release();

	if( sptrV == NULL )
		return false;

	return true;
}

bool CActionPageSetup::IsChecked()
{
	return false;
}

//////////////////////////////////////////////////////////////////////
//CActionShowPrintDesignMode implementation
CString CActionShowPrintDesignMode::GetViewProgID()
{
	return szPrintViewProgID;	
}	

bool CActionShowPrintDesignMode::Invoke()
{
	IUnknown *punkData = NULL;
	sptrIFrameWindow	sptrF( m_punkTarget );
	if( sptrF == NULL )
		return false;

	IUnknown	*punkDoc;
	sptrF->GetDocument( &punkDoc );
	if( punkDoc == NULL )
		return false;	

	sptrIDocumentSite	sptrDocSite( punkDoc );
	punkDoc->Release();


	g_ReportConstructor.AttachDocument( sptrDocSite );

	
	BOOL bFirtstCreate = FALSE;
	{
		IUnknown	*punkV = 0;
		sptrDocSite->GetActiveView( &punkV );
		if( punkV )
		{
			IDataContext2Ptr ptrC( punkV );
			punkV->Release();
			if( ptrC )
			{
				LONG_PTR lPos = 0;
				ptrC->GetFirstObjectPos( _bstr_t(szTypeReportForm), &lPos );
				ptrC->GetNextObject( _bstr_t(szTypeReportForm), &lPos, &punkData );
			}			
		}

		// A.M. fix, BT 4296
		if( punkData == NULL )
		{
			punkData = ::CreateTypedObject( szTypeReportForm );
			if( punkData != NULL )
			{
				::SetValue( sptrDocSite, 0, 0, _variant_t(punkData) );
				bFirtstCreate = TRUE;
			}
		}
	}
	

	if( !CActionShowViewBase::Invoke() )
		return false;

	IUnknown	*punkV = 0;
	sptrDocSite->GetActiveView( &punkV );
	if( !punkV )
		return false;

	sptrIReportView	sptrV(punkV);
	punkV->Release();
	if( sptrV == NULL )
		return false;

	// [vanek] BT2000:3985 activate object - 07.09.2004
	{
		IDataContext3Ptr ptr_dc = sptrV;
		if( ptr_dc == 0)
			ptr_dc = punkDoc;

		if( ptr_dc != 0 )
			ptr_dc->SetActiveObject( _bstr_t( szTypeReportForm ), punkData, 1 );
	}

	sptrV->EnterDesignMode();
	
	if( bFirtstCreate ) 
		sptrV->OnPrinterSettingsChange();
		

	if( bFirtstCreate )
	{
		CWnd* pWnd = NULL;
		pWnd = GetWindowFromUnknown( sptrV );
		if( pWnd && pWnd->GetSafeHwnd() )
			pWnd->UpdateWindow();
	}

	if( punkData )
		punkData->Release(); punkData = 0;

	return true;
}

bool CActionShowPrintDesignMode::IsAvaible()
{	
	return true;
}

bool CActionShowPrintDesignMode::IsChecked()
{
	if( !CActionShowViewBase::IsChecked() )
		return false;

	sptrIFrameWindow	sptrF( m_punkTarget );

	if( sptrF == NULL )
		return false;

	IUnknown	*punkDoc;
	sptrF->GetDocument( &punkDoc );

	if( punkDoc == NULL )
		return false;	


	sptrIDocumentSite	sptrDocSite( punkDoc );
	punkDoc->Release();

	IUnknown	*punkV = 0;
	sptrDocSite->GetActiveView( &punkV );

	if( !punkV )
		return false;

	sptrIReportView	sptrV(punkV);
	punkV->Release();

	if( sptrV == NULL )
		return false;

	int nMode;
	sptrV->GetMode( &nMode );
	if( (ViewMode)nMode == vmDesign)
		return true;
	else
		return false;

	
}


bool WriteActiveReportToDoc( IUnknown* punk_doc )
{
	IDocumentSitePtr ptr_ds( punk_doc );
	if( ptr_ds == 0 )
		return false;

	IUnknown* punk_view = 0;
	ptr_ds->GetActiveView( &punk_view );
	if( !punk_view )
		return false;

	IUnknown* punk_report = ::GetActiveObjectFromContext( punk_view, szTypeReportForm );
	punk_view->Release();	punk_view = 0;

	if( !punk_report )
		return false;

	CString str_name = ::GetObjectName( punk_report );
	punk_report->Release();	punk_report = 0;


	::SetValue( ptr_ds, REPORT_TEMPLATE_SECTION, REPORT_TEMPLATE_SOURCE, (long)tsUseExist );
	::SetValue( ptr_ds, REPORT_TEMPLATE_SECTION, REPORT_EXIST_TEMPLATE_NAME, (LPCSTR)str_name );	


	return true;
}



class CLockView
{
	sptrIFrameWindow2	m_sptrF;
	BOOL m_bPrevLocked;
public:
	CLockView(IFrameWindow2 *pFrameWindow2)
	{
		m_sptrF = pFrameWindow2;
		pFrameWindow2->GetLockedFlag(&m_bPrevLocked);
		pFrameWindow2->SetLockedFlag(TRUE);
	}
	~CLockView()
	{
		m_sptrF->SetLockedFlag(m_bPrevLocked);
	}
};


//////////////////////////////////////////////////////////////////////
//CActionShowPrintPreview implementation
bool CActionShowPrintPreview::Invoke()
{				
	CWaitCursor wait;
	

	sptrIFrameWindow2	sptrF( m_punkTarget );

	if( sptrF == NULL )
		return false;

	CLockView LockViews(sptrF);

	IUnknown	*punkDoc;
	sptrF->GetDocument( &punkDoc );

	if( punkDoc == NULL )
		return false;	

	WriteActiveReportToDoc( punkDoc );

	g_ReportConstructor.AttachDocument( punkDoc );
	
	CPrintPreview dlg( _GetActiveView() );
	dlg.SetActiveDocument( punkDoc );
	dlg.DoModal( );


	sptrIApplication sptrA( GetAppUnknown( ) );
	if( sptrA != NULL )
	{
		HWND hwnd = NULL;
		ISOK( sptrA->GetMainWindowHandle( &hwnd ) );
		if( hwnd )
		{
			CWnd* pWndView = CWnd::FromHandle( hwnd );
			pWndView->SendMessage( WM_NCACTIVATE, TRUE, 0 );
		}
	}
		

	punkDoc->Release();
	

	return true;
}

bool CActionShowPrintPreview::IsAvaible()
{
	return true;
}

bool CActionShowPrintPreview::IsChecked()
{
		return false;

}

//////////////////////////////////////////////////////////////////////
//CActionPrint implementation
CActionPrint::CActionPrint()
{
	m_bPrintDlgOK = true;
}

CActionPrint::~CActionPrint()
{
}


bool CActionPrint::ExecuteSettings( CWnd *pwndParent )
{
	if( ForceShowDialog() )
	{	/*
		CPrintDlg dlg( FALSE );
		if( !dlg.DoModal() )
			m_bPrintDlgOK = false;
			*/
	}

	return true;
}

bool CActionPrint::Invoke()
{
	
	//WriteActiveObject();
	

	if( !m_bPrintDlgOK )
		return false;

	sptrIFrameWindow	sptrF( m_punkTarget );

	if( sptrF == NULL )
		return false;

	IUnknown	*punkDoc;
	sptrF->GetDocument( &punkDoc );
	
	if( punkDoc == NULL )
		return false;	
	punkDoc->Release();

	sptrIDocumentSite	sptrDocSite( punkDoc );
	if( sptrDocSite == NULL )
		return false;

	WriteActiveReportToDoc( punkDoc );


	AutoTemplateAlghoritm ataOld = (AutoTemplateAlghoritm)::GetValueInt( sptrDocSite, 
								REPORT_TEMPLATE_SECTION, REPORT_AUTO_TEMPLATE_ALGHORITM, ataActiveView );



	::SetValue( sptrDocSite, 
								REPORT_TEMPLATE_SECTION, REPORT_AUTO_TEMPLATE_ALGHORITM, (long)ataActiveView );


	g_ReportConstructor.AttachDocument( sptrDocSite );

	CReportConstructor rptConstructor( sptrDocSite );	

	IUnknown* punkReport = NULL;
	punkReport = rptConstructor.GenerateReport();
	if( punkReport == NULL )
	{
		::SetValue( sptrDocSite, 
								REPORT_TEMPLATE_SECTION, REPORT_AUTO_TEMPLATE_ALGHORITM, (long)ataOld );

		return false;
	}
	

	IUnknownPtr ptrReport = punkReport;
	punkReport->Release();

	//Set name
	INamedObject2Ptr sptrNO2( ptrReport );
	if( sptrNO2 )	
		sptrNO2->SetName( _bstr_t((LPCSTR)g_szPrintPreviewObject) );
	

	::SetValue( sptrDocSite, 0, 0, _variant_t( (IUnknown*)ptrReport ) );

	
	

	sptrIReportView	sptrV = 0;

	{
	
		IUnknown	*punkV = 0;
		sptrDocSite->GetActiveView( &punkV );

		if( punkV )
		{
			sptrV = punkV;
			punkV->Release();
			if( sptrV )
			{			
				ISOK( sptrV->DisconnectNotifyController( TRUE ) );
			}
		}	
	}
	
	

	
	{
		CPrintProgress dlg;
		dlg.SetActiveDocument( sptrDocSite );
		dlg.SetReportForm( ptrReport );
		dlg.DoModal( );	
	}
	  

	
	if( sptrV )
		ISOK( sptrV->DisconnectNotifyController( FALSE ) );
		

	::DeleteObject( sptrDocSite, ptrReport );

	::SetValue( sptrDocSite, 
								REPORT_TEMPLATE_SECTION, REPORT_AUTO_TEMPLATE_ALGHORITM, (long)ataOld );
	

	return true;
}

//extended UI
bool CActionPrint::IsAvaible()
{
	return true;
}

bool CActionPrint::IsChecked()
{
	return false;
}


//////////////////////////////////////////////////////////////////////
//CActionReportEditText implementation
CActionReportEditText::CActionReportEditText()
{
}

CActionReportEditText::~CActionReportEditText()
{
}

bool CActionReportEditText::Invoke()
{
	IWindowPtr sptrWnd(m_punkTarget);
	ILayoutViewPtr sptrLayout(m_punkTarget);
	if (sptrWnd != 0 && sptrLayout != 0)
	{
		HWND hWnd;
		sptrWnd->GetHandle((HANDLE*)&hWnd);
		PostMessage(hWnd, WM_KEYDOWN, VK_F2, 0);
		PostMessage(hWnd, WM_KEYUP, VK_F2, 0);
	}
	return true;
}

const CLSID clsidTextCtrl =
{0xbc127b17, 0x26c, 0x11d4, { 0x81, 0x24, 0, 0, 0xe8, 0xdf, 0x68, 0xc3 } };

//extended UI
bool CActionReportEditText::IsAvaible()
{
	ILayoutViewPtr sptrLayout(m_punkTarget);
	IViewPtr sptrView(m_punkTarget);
	if (sptrLayout != 0 && sptrView != 0)
	{
		IUnknownPtr punkFrame;
		sptrView->GetMultiFrame(&punkFrame, false);
		IMultiSelectionPtr sptrMF(punkFrame);
		if (sptrMF != 0)
		{
			DWORD dwCount;
			sptrMF->GetObjectsCount(&dwCount);
			if (dwCount<=0)
				return false;
			IUnknownPtr punkAX,punkC;
			sptrMF->GetObjectByIdx(0, &punkAX);
			sptrLayout->GetControlObject(punkAX, &punkC);
			IOleObjectPtr sptrOO(punkC);
			if (sptrOO == 0)
				return false;
			CLSID clsidObj;
			sptrOO->GetUserClassID(&clsidObj);
			if (clsidTextCtrl == clsidObj)
				return true;
		}
	}
	return false;
}


#include "stdafx.h"
#include "print.h"
#include "resource.h"
#include "printview.h"
#include "exporttoJPEG.h"
#include "compressint.h"
#include "ReportConstructor.h"

extern CReportConstructor g_ReportConstructor;

IMPLEMENT_DYNCREATE(CActionExportRptToJPEG, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionBeforePrintView, CCmdTargetEx);


///GUARD_IMPLEMENT_OLECREATE----------------------------------------------------
// {719DA1D9-51BD-4646-9FA3-9AF259C101C1}
GUARD_IMPLEMENT_OLECREATE
(CActionExportRptToJPEG, "Print.ExportToJPEG", 
0x719da1d9, 0x51bd, 0x4646, 0x9f, 0xa3, 0x9a, 0xf2, 0x59, 0xc1, 0x1, 0xc1);

// {DD073AB2-1946-456f-B5A7-A288663E3A57}
GUARD_IMPLEMENT_OLECREATE
(CActionBeforePrintView, "Print.BeforePrintView", 
0xdd073ab2, 0x1946, 0x456f, 0xb5, 0xa7, 0xa2, 0x88, 0x66, 0x3e, 0x3a, 0x57);

//------------------------------------------------------------------------------


//GUIDs
// {DDAE8D98-61FB-45d6-B805-5C39C61D79E2}
static const GUID guidExportToJPEG = 
{ 0xddae8d98, 0x61fb, 0x45d6, { 0xb8, 0x5, 0x5c, 0x39, 0xc6, 0x1d, 0x79, 0xe2 } };
// {6575F7D8-4C94-4111-BC2E-EB96F3C4BB00}
static const GUID guidBeforePrintView = 
{ 0x6575f7d8, 0x4c94, 0x4111, { 0xbc, 0x2e, 0xeb, 0x96, 0xf3, 0xc4, 0xbb, 0x0 } };

//------------------------------------------------------------------------------

//ACTION_INFO
ACTION_INFO_FULL(CActionExportRptToJPEG,IDS_ACTION_EXPORT_TO_JPEG, -1, -1, guidExportToJPEG);
ACTION_INFO_FULL(CActionBeforePrintView,IDS_ACTION_BEFORE_PRINT, -1, -1, guidBeforePrintView);
//------------------------------------------------------------------------------
//ACTION_TARGET
ACTION_TARGET(CActionExportRptToJPEG, szTargetApplication);
ACTION_TARGET(CActionBeforePrintView, szTargetApplication);
//------------------------------------------------------------------------------

//ACTION_ARG
ACTION_ARG_LIST(CActionExportRptToJPEG)
	ARG_INT("VSize", 0)
END_ACTION_ARG_LIST()
//------------------------------------------------------------------------------
CActionExportRptToJPEG::CActionExportRptToJPEG(void)
{
	rptView=0;
	rpageView=0;
}

CActionExportRptToJPEG::~CActionExportRptToJPEG(void)
{
	if(rptView) rptView->Release();
	if(rpageView) rpageView->Release();
}

bool CActionExportRptToJPEG::Invoke()
{
	
	int vertSize = GetArgumentInt("VSize");//desired vertical dimension
	
	
	IUnknown* pUnk =GetAppUnknown();
	if(!pUnk) return false;
	
	IApplication* pApp;
	IDocument* doc;
	HRESULT hr =  pUnk->QueryInterface(&pApp);

	if(SUCCEEDED(hr))
	{
		hr = pApp->GetActiveDocument(&pUnk);
		//HWND hMainWindow;
		//pApp->GetMainWindowHandle(&hMainWindow);
		pApp->Release();
		if(FAILED(hr)) return false;

		hr = pUnk->QueryInterface(&doc);
		pUnk->Release();
		if(FAILED(hr))return false; 

		IDocumentSitePtr ptr_ds( doc );
		doc->Release();
		if( ptr_ds == 0 )
		return false;

		IUnknown* punk_view = 0;
		ptr_ds->GetActiveView( &punk_view );
		if( !punk_view )
			return false;
		
		punk_view->QueryInterface(&rptView);
		if(!rptView) {punk_view->Release(); return false;}
		punk_view->QueryInterface(&rpageView);
		if(!rpageView){punk_view->Release(); return false;}
		punk_view->Release();	punk_view = 0;

		CString filter;
		_variant_t v_name;
		v_name = ::GetValue(GetAppUnknown(),  "ExportToJPEG", "SaveFileDirectory", COleVariant("")); 
		filter.LoadString(IDS_FILE_FILTERS_JPEG);
		saveFileName="Report_page";

		CFileDialog fd(FALSE,_T("jpg"), saveFileName, OFN_OVERWRITEPROMPT, filter, 0);
		saveFileName = v_name.bstrVal;
		if(saveFileName!="")
		{
		fd.GetOFN().lpstrInitialDir = saveFileName;
		}
			
		if(fd.DoModal()==IDCANCEL) return false;
		saveFileName = fd.GetPathName();

		HCURSOR hCur = SetCursor(LoadCursor(NULL, IDC_WAIT));
	

		rptView->GetPageCount(&pageCount);

		HDC hDC = GetDC(0);
		rptView->GetPageRect(hDC, &pageRect);
		ReleaseDC(0,hDC);

		if(vertSize==0) zoom=0.f;
		else zoom = (float)vertSize/pageRect.Height(); 

		print_to_file();



		::SetValue( GetAppUnknown(), "ExportToJPEG", "SaveFileDirectory", saveFileName);
		SetCursor(hCur); 

	}	

	return true;
}

bool CActionExportRptToJPEG::print_to_file()
{
	
		if(!rptView) return false;
		if(!rpageView)return false;

		
		ICompressionManagerPtr	ptrCompressor( GetAppUnknown() );
		if( ptrCompressor == 0 )return false;
	HDC	hdc_screen = ::GetDC( 0 );

		CRect rect;		
		rptView->GetPageRect(hdc_screen,&rect);
		if(zoom==0) rpageView->GetZoom(&zoom);
		rect.left*=zoom;
		rect.right*=zoom;
		rect.top*=zoom;
		rect.bottom*=zoom;
		
		int pageCount;
		rptView->GetPageCount(&pageCount);
		/*rptView->ScrollToPage(*/

	HDC	hdc = ::CreateCompatibleDC( hdc_screen );

	BITMAPINFOHEADER	bmih;
	ZeroMemory( &bmih, sizeof( bmih ) );
	bmih.biSize = sizeof( bmih );
	bmih.biWidth = rect.Width();
	bmih.biHeight = rect.Height();
	bmih.biPlanes = 1;
	bmih.biBitCount = 24;

	byte	*pdib = 0;
	HBITMAP hbmp = ::CreateDIBSection( hdc_screen, (BITMAPINFO*)&bmih, DIB_RGB_COLORS, (void**)&pdib, 0, 0 ); 
	

	::SelectObject( hdc, hbmp );

	::SetROP2( hdc, R2_COPYPEN );

	for(int i=0;i<pageCount;i++)
	{
		::FillRect( hdc, &rect, (HBRUSH)::GetStockObject( WHITE_BRUSH ) );

		CString str = saveFileName;
		CString ext;
		if(pageCount>1)
		{
			TCHAR suffix[33];
			ZeroMemory(suffix,33*sizeof(TCHAR));
			int dot = str.ReverseFind('.');
			ext = str.Right(str.GetLength()-dot);
			str.Truncate(dot);
			_itot(i+1, suffix, 10);
			if(i<10)
			{
				str+=_T("0");
			}
			str+=suffix;
		}
		rpageView->DrawPage(hdc,i ,zoom);
		ptrCompressor->WriteDIB24ToFile( &bmih, pdib, _bstr_t( str + ext) );
	}
	::ReleaseDC( 0, hdc_screen );
	::DeleteDC( hdc );
	::DeleteObject( hbmp );
	return true;
}
///CActionBeforePrintView-------------------------------------------------------
bool CActionBeforePrintView::Invoke()
{
	IUnknown* appUnk = GetAppUnknown();
	if(!appUnk) return false;
	IApplication* app;
	if(appUnk) appUnk->QueryInterface(&app);

	if(!app) return false;
	IUnknown* docUnk;
	app->GetActiveDocument(&docUnk);
	app->Release();
	if(!docUnk) return false;

	g_ReportConstructor.AttachDocument(docUnk);

	docUnk->Release();
	return true;

}

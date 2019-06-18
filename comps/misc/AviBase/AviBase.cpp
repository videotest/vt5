// AviBase.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "AviBase.h"
#include "AviGallery.h"
#include "GalleryAviPage.h"

#include "core5.h"
#include "misc_utils.h"
#include "data5.h"
#include "docview5.h"
#include "AviFillter.h"


#pragma comment (lib, "common2.lib")
#pragma comment (lib, "vfw32.lib")
#pragma comment (lib, "Winmm.lib.")
#pragma comment(lib, "amstrmid.lib" )


#include "resource.h"
#include "actions.h"
#include "AviObj.h"

#include "PlayAviPage.h"

#include "misc_const.h"

#include "AviParams.h"

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	if( ul_reason_for_call == DLL_PROCESS_ATTACH )
	{
		new App;
		App::instance()->Init( (HINSTANCE)hModule );		

		//Register actions
		App::instance()->ObjectInfo( _dyncreate_t<CActionInsertAviObjectInfo>::CreateObject, clsidInsertAviObjectInfo, szInsertAviObjectProgID, pszActionSection );
		
		App::instance()->ObjectInfo( _dyncreate_t<CActionAviMoveFirstInfo>::CreateObject, clsidAviMoveFirstInfo, szAviMoveFirstProgID, pszActionSection );
		
		App::instance()->ObjectInfo( _dyncreate_t<CActionAviMoveNextInfo>::CreateObject, clsidAviMoveNextInfo, szAviMoveNextProgID, pszActionSection );
		
		App::instance()->ObjectInfo( _dyncreate_t<CActionAviMoveLastInfo>::CreateObject, clsidAviMoveLastInfo, szAviMoveLastProgID, pszActionSection );
		
		App::instance()->ObjectInfo( _dyncreate_t<CActionAviMovePrevInfo>::CreateObject, clsidAviMovePrevInfo, szAviMovePrevProgID, pszActionSection );
		
		App::instance()->ObjectInfo( _dyncreate_t<CActionAviMoveToInfo>::CreateObject, clsidAviMoveToInfo, szAviMoveToProgID, pszActionSection );
		
		App::instance()->ObjectInfo( _dyncreate_t<CActionAviPlayInfo>::CreateObject, clsidAviPlay, szAviPlayProgID, pszActionSection );
		
		App::instance()->ObjectInfo( _dyncreate_t<CActionAviStopInfo>::CreateObject, clsidAviStop, szAviStopProgID, pszActionSection );

		App::instance()->ObjectInfo( _dyncreate_t<CShowAviGalleryInfo>::CreateObject, clsidShowAviGallery, szShowAviGalleryProgID, pszActionSection );

		App::instance()->ObjectInfo( _dyncreate_t<CAviSignatureInfo>::CreateObject, clsidAviSignature, szAviSignatureProgID, pszActionSection );


		//register prop pages
		App::instance()->ObjectInfo( _dyncreate_t<CPlayAviPage>::CreateObject, clsidAviPlayPage, szAviPlayPageProgID, pszPageSection );
		App::instance()->ObjectInfo( _dyncreate_t<CGalleryAviPage>::CreateObject, clsidAviGalleryPage, szAviGalleryPageProgID, pszPageSection );
		
		//register gallery view
		App::instance()->ObjectInfo( _dyncreate_t<CAviGallery>::CreateObject, clsidAviGalleryView, szAviGalleryViewProgID, "\\Image\\Views" );
		
		//Register AVI object
		App::instance()->ObjectInfo( _dyncreate_t<CAviObj>::CreateObject, clsidAviObj, szTypeAviObjProgID );
		//App::instance()->ObjectInfo( _dyncreate_t<CAviObjInfo>::CreateObject, clsidAviObjInfo, szTypeAviObjInfoProgID, "named data objects" );


		//Register avi file filter
		App::instance()->ObjectInfo( _dyncreate_t<CAviFillter>::CreateObject, clsidAviFileFilter, szAviFileFilterProgID, "Image\\export-import" );

		// AVI measure parameters
		App::instance()->ObjectInfo(_dyncreate_t<CAviParams>::CreateObject, clsidAviParams, pszAviParamsProgID, szPluginMeasurement);


		ClassFactory * pfactory = (ClassFactory*)ClassFactory::CreateObject();
		pfactory->LoadObjectInfo();
		pfactory->Release();
	}
	else if( ul_reason_for_call == DLL_PROCESS_DETACH )
	{
		App::instance()->Deinit();
		App::instance()->Release();
	}
    return TRUE;
}

implement_com_exports();


IAviImagePtr GetActiveAviFromDoc( IUnknown* punkDoc )
{
	if( punkDoc == 0 )
		return 0;

	IDocumentSitePtr ptrDS( punkDoc );
	if( ptrDS == 0 )
		return 0;

	IUnknown* punk = 0;
	ptrDS->GetActiveView( &punk );

	if( !punk )
		return 0;

	IDataContext3Ptr ptrDC( punk );
	punk->Release();	punk = 0;

	if( ptrDC == 0 )
		return 0;

	_bstr_t bstrImage = szTypeImage;
	long lPos = 0;
	ptrDC->GetFirstSelectedPos( bstrImage, &lPos );

	while( lPos )
	{
		IUnknown* punkAvi = 0;
		ptrDC->GetNextSelected( bstrImage, &lPos, &punkAvi );
		if( !punkAvi )
			continue;

		IAviImagePtr ptrAvi( punkAvi );
		punkAvi->Release();	punkAvi = 0;

		if( ptrAvi )
			return ptrAvi;		
	}	

	return 0;
}

IAviImagePtr GetActiveAviFromContext( IUnknown* punkContext )
{
	IDataContext3Ptr ptrDC( punkContext );	

	if( ptrDC == 0 )
		return 0;

	_bstr_t bstrImage = szTypeImage;
	long lPos = 0;
	ptrDC->GetFirstSelectedPos( bstrImage, &lPos );

	while( lPos )
	{
		IUnknown* punkAvi = 0;
		ptrDC->GetNextSelected( bstrImage, &lPos, &punkAvi );
		if( !punkAvi )
			continue;

		IAviImagePtr ptrAvi( punkAvi );
		punkAvi->Release();	punkAvi = 0;
		if( ptrAvi )
			return ptrAvi;

		
	}	

	return 0;
}


//////////////////////////////////////////////////////////////////////
bool GetPropPageStuff( IPropertySheetPtr& ptrSheet, IOptionsPagePtr& ptrPage, int& nPageNum, CLSID clsid )
{	
		
	IPropertySheet	*pSheet = ::FindPropertySheet();
	if( !pSheet )
		return false;

	ptrSheet = pSheet;
	pSheet->Release();	pSheet = 0;

	if( ptrSheet == 0 )
		return false;

	IOptionsPage	*pPage = ::FindPage( ptrSheet, clsid, &nPageNum );
	if( !pPage )
		return false;

	ptrPage = pPage;
	pPage->Release();	pPage = 0;

	if( ptrPage == 0 )
		return false;
	
	if( nPageNum == -1 )
		return false;

	return true;
}

//////////////////////////////////////////////////////////////////////
IUnknown* CreateAviObject()
{
	GUID guidInternal = clsidAviObj;
	GUID guidExternal = INVALID_KEY;
	IVTApplicationPtr ptrApp( ::GetAppUnknown() );
	if( ptrApp )
	{						
		if( S_OK != ptrApp->GetEntry( 1, (DWORD*)&guidInternal, (DWORD*)&guidExternal, 0 ) )
			guidExternal = INVALID_KEY;
	}

	if( guidExternal == INVALID_KEY )
		return 0; 

	IUnknown* punk = 0;
	HRESULT hres = ::CoCreateInstance( guidExternal, 0,  1, IID_IUnknown, (void**)&punk );
	if( hres != S_OK )
		return 0; 

	return punk;
	
}
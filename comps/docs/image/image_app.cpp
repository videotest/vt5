#include "stdafx.h"
#include "image.h"
#include "image_app.h"
#include "bin_view.h"
#include "actions.h"
#include "misc_const.h"
#include "morph1.h"
#include "DilateObjects.h"


#pragma comment (lib, "common2.lib")


BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	if( ul_reason_for_call == DLL_PROCESS_ATTACH )
	{
		new App;
		App::instance()->Init( (HINSTANCE)hModule );

		App::instance()->ObjectInfo( _dyncreate_t<CBinaryImage>::CreateObject, clsidBinImage, pszImageProgID );
		App::instance()->ObjectInfo( _dyncreate_t<CBinCleanV4>::CreateObject, clsidBinCleanV4, pszBinCleanV4ProgID );
		App::instance()->ObjectInfo( _dyncreate_t<CBinMedian>::CreateObject, clsidBinMedian, pszBinMedianProgID );
		App::instance()->ObjectInfo( _dyncreate_t<CBinErode>::CreateObject, clsidBinErode, pszBinErodeProgID );
		App::instance()->ObjectInfo( _dyncreate_t<CBinDilate>::CreateObject, clsidBinDilate, pszBinDilateProgID );
		App::instance()->ObjectInfo( _dyncreate_t<CBinOpen>::CreateObject, clsidBinOpen, pszBinOpenProgID );
		App::instance()->ObjectInfo( _dyncreate_t<CBinClose>::CreateObject, clsidBinClose, pszBinCloseProgID );
		App::instance()->ObjectInfo( _dyncreate_t<CBinNot>::CreateObject, clsidBinNot, pszBinNotProgID );
		App::instance()->ObjectInfo( _dyncreate_t<CBinOr>::CreateObject, clsidBinOr, pszBinOrProgID );
		App::instance()->ObjectInfo( _dyncreate_t<CBinAnd>::CreateObject, clsidBinAnd, pszBinAndProgID );
		App::instance()->ObjectInfo( _dyncreate_t<CBinXor>::CreateObject, clsidBinXor, pszBinXorProgID );
		App::instance()->ObjectInfo( _dyncreate_t<CBinCustomLogic>::CreateObject, clsidBinCustomLogic, pszBinCustomLogicProgID );
		App::instance()->ObjectInfo( _dyncreate_t<CMorphSmooth>::CreateObject, clsidMorphSmooth, pszMorphSmoothID );
		App::instance()->ObjectInfo( _dyncreate_t<CBinDilateObjects>::CreateObject, clsidBinDilateObjects, pszBinDilateObjectsProgID );
		App::instance()->ObjectInfo( _dyncreate_t<CBinReconstructFore>::CreateObject, clsidBinReconstructFore, pszBinReconstructForeProgID );
		App::instance()->ObjectInfo( _dyncreate_t<CBinReconstructBack>::CreateObject, clsidBinReconstructBack, pszBinReconstructBackProgID );
		App::instance()->ObjectInfo( _dyncreate_t<CBinDeleteSmallObjects>::CreateObject, clsidBinDeleteSmallObjects, pszBinDeleteSmallObjectsProgID );
		App::instance()->ObjectInfo( _dyncreate_t<CSmoothContour>::CreateObject, clsidSmoothContour, pszSmoothContourProgID );
		//App::instance()->ObjectInfo( _dyncreate_t<CResizeBin>::CreateObject, clsidResizeBin, pszResizeBinProgID );


		App::instance()->ObjectInfo( _dyncreate_t<CBinaryImageInfo>::CreateObject, clsidBinImageInfo, pszImageInfoProgID, "named data objects" );
		//App::instance()->ObjectInfo( _dyncreate_t<CBinaryView>::CreateObject, clsidBinView, pszBinViewProgID, "SOFTWARE\\VideoTest\\vt5\\add-ins\\Image\\Views" );
		App::instance()->ObjectInfo( _dyncreate_t<CBinCleanV4Info>::CreateObject, clsidBinCleanV4Info, pszBinCleanV4InfoProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CBinMedianInfo>::CreateObject, clsidBinMedianInfo, pszBinMedianInfoProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CBinErodeInfo>::CreateObject, clsidBinErodeInfo, pszBinErodeInfoProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CBinDilateInfo>::CreateObject, clsidBinDilateInfo, pszBinDilateInfoProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CBinOpenInfo>::CreateObject, clsidBinOpenInfo, pszBinOpenInfoProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CBinCloseInfo>::CreateObject, clsidBinCloseInfo, pszBinCloseInfoProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CBinNotInfo>::CreateObject, clsidBinNotInfo, pszBinNotInfoProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CBinOrInfo>::CreateObject, clsidBinOrInfo, pszBinOrInfoProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CBinAndInfo>::CreateObject, clsidBinAndInfo, pszBinAndInfoProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CBinXorInfo>::CreateObject, clsidBinXorInfo, pszBinXorInfoProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CBinCustomLogicInfo>::CreateObject, clsidBinCustomLogicInfo, pszBinCustomLogicInfoProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CMorphSmoothInfo>::CreateObject, clsidMorphSmoothInfo, pszMorphSmoothInfoID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CBinDilateObjectsInfo>::CreateObject, clsidBinDilateObjectsInfo, pszBinDilateObjectsInfoProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CBinReconstructForeInfo>::CreateObject, clsidBinReconstructForeInfo, pszBinReconstructForeInfoProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CBinReconstructBackInfo>::CreateObject, clsidBinReconstructBackInfo, pszBinReconstructBackInfoProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CBinDeleteSmallObjectsInfo>::CreateObject, clsidBinDeleteSmallObjectsInfo, pszBinDeleteSmallObjectsInfoProgID, pszActionSection );
		//MorphDivision
		App::instance()->ObjectInfo( _dyncreate_t<CBinMorphDivisionInfo>::CreateObject, clsidBinMorphDivisionInfo, pszBinMorphDivisionInfoProgID, pszActionSection );
		//MorphDivision2
		App::instance()->ObjectInfo( _dyncreate_t<CBinMorphDivision2Info>::CreateObject, clsidBinMorphDivision2Info, pszBinMorphDivision2InfoProgID, pszActionSection );

		//App::instance()->ObjectInfo( _dyncreate_t<CResizeBinInfo>::CreateObject, clsidResizeBinInfo, pszResizeBinInfoProgID, pszActionSection );
		//
		App::instance()->ObjectInfo( _dyncreate_t<CSmoothContourInfo>::CreateObject, clsidSmoothContourInfo, pszSmoothContourInfoProgID, pszActionSection );

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
/*
STDAPI DllCanUnloadNow()
{
	return App::instance() != 0;
}

STDAPI DllGetClassObject( const CLSID &rclsid, const IID &iid, void **ppClassFactory )
{
	ClassFactory	*pfactory = (ClassFactory*)ClassFactory::CreateObject();
	pfactory->SetCLSID( rclsid );

	HRESULT hResult = 
	pfactory->QueryInterface( iid, ppClassFactory );
	pfactory->Release();

	return hResult;
}

STDAPI DllRegisterServer()
{
	ClassFactory	*pfactory = (ClassFactory*)ClassFactory::CreateObject();
	pfactory->RegisterAll( true );
	pfactory->Release();
	return 0;
}

STDAPI DllUnregisterServer()
{
	ClassFactory	*pfactory = (ClassFactory*)ClassFactory::CreateObject();
	pfactory->RegisterAll( false );
	pfactory->Release();
	return 0;
}
*/
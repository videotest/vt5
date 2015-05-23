// ImageFilters.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "ImageFilters.h"
#include "actions.h"
#include "misc_const.h"
#include "arithmetic.h"





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
		App::instance()->ObjectInfo( _dyncreate_t<CAutomaticBCInfo>::CreateObject, clsidAutoBCInfo, pszAutoBCProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CApplyLookupInfo>::CreateObject, clsidApplyLookupInfo, pszApplyLookupProgID, pszActionSection );

		App::instance()->ObjectInfo( _dyncreate_t<CCombineFilterInfo>::CreateObject, clsidCombineInfo, pszCombineProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CAddFilterInfo>::CreateObject, clsidAddInfo, pszAddProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CSubFilterInfo>::CreateObject, clsidSubInfo, pszSubProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CDiffFilterInfo>::CreateObject, clsidDiffInfo, pszDiffProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CSubLightFilterInfo>::CreateObject, clsidSubLightInfo, pszSubLightProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CDivFilterInfo>::CreateObject, clsidDivInfo, pszDivProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CMulFilterInfo>::CreateObject, clsidMulInfo, pszMulProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CSubFilterExInfo>::CreateObject, clsidSubInfoEx, pszSubExProgID, pszActionSection );

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
	return App::instance() == 0 ? S_OK : S_FALSE;
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
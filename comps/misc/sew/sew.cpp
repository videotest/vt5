// sew.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "com_main.h"
#include "sew.h"
#include "sew_actions.h"
#include "sew_actions2.h"
#include "misc_const.h"
#include "sew_page.h"

#pragma comment (lib, "common2.lib")


BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	if( ul_reason_for_call == DLL_PROCESS_ATTACH )
	{
		_dyncreate_t<App>::CreateObject();
		App::instance()->Init( (HINSTANCE)hModule );
		App::instance()->ObjectInfo( _dyncreate_t<CAutoSewHorzInfo>::CreateObject, clsidAutoSewHorzInfo, pszAutoSewHorzProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CAutoSewVertInfo>::CreateObject, clsidAutoSewVertInfo, pszAutoSewVertProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CAutoSew2FilterInfo>::CreateObject, clsidAutoSew2Info, pszAutoSew2ProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CSewManualInfo>::CreateObject, clsidSewManual, pszSewProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CSewPropertyPage>::CreateObject, clsidSewPage, pszSewPageProgID, pszPageSection );

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

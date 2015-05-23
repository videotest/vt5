// FileFilters2.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "JPGFilter.h"
#include "TIFFilter.h"
#include "JPGcompressor.h"
#include "misc_const.h"

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
		App::instance()->ObjectInfo( _dyncreate_t<CJPGFilter>::CreateObject, clsidJPGFilter, pszJPGFilterProgID, "Image\\export-import" );
		App::instance()->ObjectInfo( _dyncreate_t<CTIFFilter>::CreateObject, clsidTIFFilter, pszTIFFilterProgID, "Image\\export-import" );
		App::instance()->ObjectInfo( _dyncreate_t<CJPGcompressor>::CreateObject, clsidJPGcompressor, pszJPGcompressorProgID, pszAppAggrSection );

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
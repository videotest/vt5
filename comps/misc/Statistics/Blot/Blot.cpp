// Blot.cpp : Implementation of DLL Exports.

#include "stdafx.h"
#include "resource.h"
#include "\vt5\awin\misc_module.h"
#include <com_main.h>
#include "Blot_h.h"

class CBlotModule : public CAtlDllModuleT< CBlotModule >
{
public :
	DECLARE_LIBID(LIBID_BlotLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_BLOT, "{CC7C10E5-C7CF-4497-B73A-8820EB5926EB}")
};

CBlotModule _AtlModule;

// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	hInstance;

	if( dwReason == DLL_PROCESS_ATTACH )
	{
		static module	_module( 0 );
		_module.init( hInstance );

		new App;
		App::instance()->Init( hInstance );
	}
	else if( dwReason == DLL_PROCESS_DETACH )
	{
		App::instance()->Deinit();
		App::instance()->Release();
	}

	return _AtlModule.DllMain(dwReason, lpReserved); 
}


// Used to determine whether the DLL can be unloaded by OLE
STDAPI DllCanUnloadNow(void)
{
    return _AtlModule.DllCanUnloadNow();
}


// Returns a class factory to create an object of the requested type
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
}


// DllRegisterServer - Adds entries to the system registry
STDAPI DllRegisterServer(void)
{
    // registers object, typelib and all interfaces in typelib
    HRESULT hr = _AtlModule.DllRegisterServer();
	return hr;
}


// DllUnregisterServer - Removes entries from the system registry
STDAPI DllUnregisterServer(void)
{
	HRESULT hr = _AtlModule.DllUnregisterServer();
	return hr;
}

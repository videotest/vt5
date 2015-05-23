// MacroRecorder.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "MacroMan.h"
#include "MacroHelper.h"
#include "misc_const.h"

#pragma comment (lib, "common2.lib")

static LRESULT CALLBACK msghook(UINT nCode, WPARAM wParam, LPARAM lParam);

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    if( ul_reason_for_call == DLL_PROCESS_ATTACH )
	{
		static module	_module( 0 );
		_module.init( (HINSTANCE)hModule );

		new App;
		App::instance()->Init( (HINSTANCE)hModule );
		
		App::instance()->ObjectInfo( _dyncreate_t<CMacroManager>::CreateObject, CLSID_CoMacroManager, 
			szMacroManagerProgID, pszScriptNamespace );

		App::instance()->ObjectInfo( _dyncreate_t<CMacroHelper>::CreateObject, CLSID_CoMacroHelper, 
			szMacroHelperProgID, pszScriptNamespace );

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


// idio.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "nameconsts.h"
#include "idio_object.h"
#include "idio_actions.h"

/////////////////////////////////////////////////////////////////////////////////////////
//main module

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	if( ul_reason_for_call == DLL_PROCESS_ATTACH )
	{
		new App;
		App::instance()->Init( (HINSTANCE)hModule );
		App::instance()->ObjectInfo( _dyncreate_t<idio_info>::CreateObject, clsid_idio_info, "idio.IdioInfo", szPluginObject );
		App::instance()->ObjectInfo( _dyncreate_t<idio>::CreateObject, clsid_idio, "idio.Idio" );
		App::instance()->ObjectInfo( _dyncreate_t<action_insert_info>::CreateObject, clsis_action_insert, "idio.ActionInsertIdio", szPluginAction );
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
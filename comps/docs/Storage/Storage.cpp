// Storage.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "Storage.h"
#include "storage_obj.h"
#include "storage_actions.h"
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
		App::instance()->ObjectInfo( _dyncreate_t<storage_obj>::CreateObject, CLSID_StorageObject, szStorageObjectProgID );
		App::instance()->ObjectInfo( _dyncreate_t<storage_obj_info>::CreateObject, clsidStorageObjectInfo, pszStorageObjectInfoProgID, "named data objects" );

		//Register Action CreateStatTable
		App::instance()->ObjectInfo( _dyncreate_t<CCreateStorageObjInfo>::CreateObject, clsidCreateStorageObjInfo, pszCreateStorageObjProgID, pszActionSection );

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


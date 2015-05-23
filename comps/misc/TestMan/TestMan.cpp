// TestMan.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "misc_const.h"
#include "\vt5\awin\misc_module.h"

#include "testitem.h"
#include "TestDataArray.h"
#include "testmanager.h"

#include "test_defs.h"

#include "Snapshot.h"

#pragma comment (lib, "common2.lib")


static const GUID clsidSnapshot = 
{ 0x6c62262b, 0x158e, 0x4e62, { 0xb9, 0xb8, 0xb1, 0x35, 0x5, 0x1d, 0x8a, 0xee } };
#define pszActionSnapshotProgID "TestMan.Snapshot"


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
		
		App::instance()->ObjectInfo( _dyncreate_t<CTestManager>::CreateObject, clsidTestManager, pszTestManagerProgID, szPluginApplication);
		App::instance()->ObjectInfo( _dyncreate_t<CTestItem>::CreateObject, clsidTestItem, pszTestItemProgID );
		App::instance()->ObjectInfo( _dyncreate_t<CTestDataArray>::CreateObject, clsidTestDataArray, pszTestDataArrayProgID );
		App::instance()->ObjectInfo( _dyncreate_t<CTestDataArrayInfo>::CreateObject, clsidTestDataArrayInfo, pszTestDataArrayInfoProgID, "named data objects" );

		App::instance()->ObjectInfo(_dyncreate_t<CSnapshotInfo>::CreateObject, clsidSnapshot, pszActionSnapshotProgID, pszActionSection);

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



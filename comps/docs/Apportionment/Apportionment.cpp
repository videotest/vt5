// Apportionment.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "Apportionment.h"
#include "view.h"
#include "actions.h"
#include "misc_const.h"
#include "MenuRegistrator.h"
#include "\vt5\awin\misc_module.h"

#pragma comment (lib, "common2.lib")

const char* szApportionmentSectName = "\\Apportionment";

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

		//actions
		App::instance()->ObjectInfo( _dyncreate_t<CActionShowApportionmentInfo>::CreateObject, clsidActionShowApportionment, szActionShowApportionmentProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CActionClassesArrangeInfo>::CreateObject, clsidActionClassesArrange, szActionClassesArrangeProgID, pszActionSection );

		CMenuRegistrator rcm;
		rcm.RegisterMenu( szApportionmentViewNameMenu, 0, szApportionmentViewNameMenuUI );

		//view
		App::instance()->ObjectInfo( _dyncreate_t<CAView>::CreateObject, clsidAView, szAViewProgID, "\\Image\\Views" );
				
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

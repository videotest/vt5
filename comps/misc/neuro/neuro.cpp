// neuro.cpp : Defines the entry point for the DLL application.
//


#include "stdafx.h"
#include "neuro.h"
#include "misc_const.h"
#include "ClassifyNeuro.h"
#include "AdjustParams.h"

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
		App::instance()->ObjectInfo( _dyncreate_t<CClassifyNeuroInfo>::CreateObject, clsidClassifyNeuroInfo, pszClassifyNeuroProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CTeachNeuroInfo>::CreateObject, clsidTeachNeuroInfo, pszTeachNeuroProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CAdjustParamsInfo>::CreateObject, clsidAdjustParamsInfo, pszAdjustParamsProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CNeuroClassifier>::CreateObject, clsidNeuroClassifier, pszNeuroClassifierProgID );

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

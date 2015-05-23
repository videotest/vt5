#include "stdafx.h"
#include "ClassBase.h"
#include "misc_const.h"
#include "Actions.h"
#include "classifyobject.h"
#include "classifier.h"
#include "Teach.h"

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
		App::instance()->ObjectInfo( _dyncreate_t<CObjectSetClassInfo>::CreateObject, clsidObjectSetClassInfo, pszObjectSetClassProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CManualClassifier>::CreateObject, clsidManualClassifier, pszManualClassifierProgID );
		App::instance()->ObjectInfo( _dyncreate_t<CAutomaticClassifier>::CreateObject, clsidAutomaticClassifier, pszAutomaticClassifierProgID );
		App::instance()->ObjectInfo( _dyncreate_t<CActionClassifyObjectAllInfo>::CreateObject, clsidActionClassifyObjectAllInfo, pszActionClassifyObjectAllProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CActionTeachObjectAllInfo>::CreateObject, clsidActionTeachObjectAllInfo, pszActionTeachObjectAllProgID, pszActionSection );
		//App::instance()->ObjectInfo( _dyncreate_t<CTeachNeuroInfo>::CreateObject, clsidTeachNeuroInfo, pszTeachNeuroProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CActionGetObjectDescriptionInfo>::CreateObject, clsidActionGetObjectDescriptionInfo, pszActionGetObjectDescriptionInfoProgID, pszActionSection );

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

// TestFreeze.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "misc_const.h"
#include "Freeze.h"
#pragma comment (lib, "common2.lib")

// {5FDBC762-A2B6-481a-818A-103D9218354A}
static const GUID clsidFreezeInfo = 
{ 0x5fdbc762, 0xa2b6, 0x481a, { 0x81, 0x8a, 0x10, 0x3d, 0x92, 0x18, 0x35, 0x4a } };

#define pszActionFreezeInfoProgID "Freeze.FreezeInfo"

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	if( ul_reason_for_call == DLL_PROCESS_ATTACH )
	{
		new App;
		App::instance()->Init((HINSTANCE)hModule);
//		App::instance()->ObjectInfo(_dyncreate_t<CBinThresholdAllPhases>::CreateObject, clsidThreshAllPhases, pszActionThresholdAllPhasesProgID);
		
		App::instance()->ObjectInfo(_dyncreate_t<CFreezeInfo>::CreateObject, clsidFreezeInfo, pszActionFreezeInfoProgID, pszActionSection);

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


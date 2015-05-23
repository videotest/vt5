// CommWork.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "misc_const.h"
#include "InfoField.h"
#include "InfoFieldObj.h"

#pragma comment (lib, "common2.lib")


// {EFDCE27C-2FED-470F-A54D-071C0F771F00}
static const GUID clsidInfoField =
{ 0xefdce27c, 0x2fed, 0x470f, { 0xa5, 0x4d, 0x7, 0x1c, 0xf, 0x77, 0x1f, 0x0 } };

// {265BE30E-F760-4953-B1B4-FCBF744177E1}
static const GUID clsidInfoFieldInfo = 
{ 0x265be30e, 0xf760, 0x4953, { 0xb1, 0xb4, 0xfc, 0xbf, 0x74, 0x41, 0x77, 0xe1 } };


#define pszInfoFieldProgID      "CommWork.ActionInfoField"
#define pszInfoFieldInfoProgID  "CommWork.ActionInfoFieldInfo"


BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	if( ul_reason_for_call == DLL_PROCESS_ATTACH )
	{
		new App;
		App::instance()->Init( (HINSTANCE)hModule );
		App::instance()->ObjectInfo(_dyncreate_t<CActionInfoField>::CreateObject, clsidInfoField, pszInfoFieldProgID);
		App::instance()->ObjectInfo( _dyncreate_t<CActionInfoFieldInfo>::CreateObject, clsidInfoFieldInfo, pszInfoFieldInfoProgID, pszActionSection);
		App::instance()->ObjectInfo( _dyncreate_t<CInfoFieldObject>::CreateObject, clsidInfoFieldObject, szInfoFieldObjectProgId);
		App::instance()->ObjectInfo( _dyncreate_t<CInfoFieldObjectInfo>::CreateObject, clsidInfoFieldObjectInfo, szInfoFieldObjectInfoProgId, "named data objects");
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

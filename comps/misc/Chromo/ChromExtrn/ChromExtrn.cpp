// ChromExtrn.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "misc_const.h"
#include "RectFilter.h"
#include "WoolzFilter.h"
#include "WoolzObject.h"
#include "Compare.h"

#pragma comment (lib, "common2.lib")

// {78B43CD0-C032-410B-91D8-0A2574A287AA}
static const GUID clsidActionCompare = 
{ 0x78b43cd0, 0xc032, 0x410b, { 0x91, 0xd8, 0xa, 0x25, 0x74, 0xa2, 0x87, 0xaa } };

// {B115947F-D348-45D9-8E2F-D4DFB4FF2949}
static const GUID clsidActionCompareInfo = 
{ 0xb115947f, 0xd348, 0x45d9, { 0x8e, 0x2f, 0xd4, 0xdf, 0xb4, 0xff, 0x29, 0x49 } };

#define pszCompareActionProgID		"ChromExtrn.ActionCompare"
#define pszCompareActionInfoProgID	"ChromExtrn.ActionCompareInfo"



BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	if( ul_reason_for_call == DLL_PROCESS_ATTACH )
	{
		new App;
		App::instance()->Init( (HINSTANCE)hModule );
		App::instance()->ObjectInfo(_dyncreate_t<CRectFilter>::CreateObject, clsidRectFilter, pszRectFilterProgID, "Image\\export-import" );
		App::instance()->ObjectInfo(_dyncreate_t<CWoolzFilter>::CreateObject, clsidWoolzFilter, pszWoolzFilterProgID, "Image\\export-import" );
		App::instance()->ObjectInfo( _dyncreate_t<CWoolzObject>::CreateObject, clsidWoolzObject, szWoolzObjectProgId );
		App::instance()->ObjectInfo( _dyncreate_t<CWoolzObjectInfo>::CreateObject, clsidWoolzObjectInfo, szWoolzObjectInfoProgId, "named data objects" );

		App::instance()->ObjectInfo(_dyncreate_t<CActionCompare>::CreateObject, clsidActionCompare, pszCompareActionProgID);
		App::instance()->ObjectInfo( _dyncreate_t<CActionCompareInfo>::CreateObject, clsidActionCompareInfo, pszCompareActionInfoProgID, pszActionSection );


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
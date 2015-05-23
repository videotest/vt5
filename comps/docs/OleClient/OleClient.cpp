// OleClient.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "misc_const.h"

#include "nameconsts.h"

#include "OleItem.h"
#include "actions.h"
#include "OleView.h"
#include "Render.h"
#include "Ole_Page.h"
#include "MenuRegistrator.h"



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

		
		//Register OLE object
		App::instance()->ObjectInfo( _dyncreate_t<COleItemData>::CreateObject, clsidOleItemData, szTypeOleItemProgID );
		App::instance()->ObjectInfo( _dyncreate_t<COleItemDataInfo>::CreateObject, clsidOleItemDataInfo, szTypeOleItemInfoProgID, "named data objects" );
		

		//Register Insert Ole object Action  
		App::instance()->ObjectInfo( _dyncreate_t<CInsertOleObjectInfo>::CreateObject, clsidInsertOleObjectInfo, szInsertOleObjectProgID, pszActionSection );

		//Register Insert Ole object Action to Exist
		App::instance()->ObjectInfo( _dyncreate_t<CInsertOleObjectToExistInfo>::CreateObject, clsidInsertOleObjectToExistInfo, szInsertOleObjectToExistProgID, pszActionSection );

		//Register Insert Ole object from Clipboard Action  
		App::instance()->ObjectInfo( _dyncreate_t<CInsertOleObjectFromClipboardInfo>::CreateObject, clsidInsertOleObjectFromClipboardInfo, szInsertOleObjectFromClipboardProgID, pszActionSection );

		//Register Insert Ole object from Clipboard Action to Exist
		App::instance()->ObjectInfo( _dyncreate_t<CInsertOleObjectFromClipboardToExistInfo>::CreateObject, clsidInsertOleObjectFromClipboardToExistInfo, szInsertOleObjectFromClipboardToExistProgID, pszActionSection );

		//Register Activate Ole object Action  
		App::instance()->ObjectInfo( _dyncreate_t<CActivateOleObjectInfo>::CreateObject, clsidActivateOleObjectInfo, szActivateOleObjectProgID, pszActionSection );

		//Register Deactivate Ole object Action  
		App::instance()->ObjectInfo( _dyncreate_t<CDeactivateOleObjectInfo>::CreateObject, clsidDeactivateOleObjectInfo, szDeactivateOleObjectProgID, pszActionSection );

		//Register Show Ole object Action  
		App::instance()->ObjectInfo( _dyncreate_t<CShowOleObjectInfo>::CreateObject, clsidShowOleObjectInfo, szShowOleObjectProgID, pszActionSection );

		//Register Show Ole object content Action  
		App::instance()->ObjectInfo( _dyncreate_t<CShowOleObjectContentInfo>::CreateObject, clsidShowOleObjectContentInfo, szShowOleObjectContentProgID, pszActionSection );

		//Register Update Ole object Action  
		App::instance()->ObjectInfo( _dyncreate_t<CUpdateOleObjectInfo>::CreateObject, clsidUpdateOleObject, szUpdateOleObjectProgID, pszActionSection );

		//Register OleView View
		App::instance()->ObjectInfo( _dyncreate_t<COleView>::CreateObject, clsidOleObjectView, szOleClientView, "Image\\Views" );

		//Register Ole Render
		App::instance()->ObjectInfo( _dyncreate_t<COleRender>::CreateObject, clsidOleRender, szTypeOleRenderProgID, szPluginRender );

		//Register Ole Prop Page
		App::instance()->ObjectInfo( _dyncreate_t<COlePropertyPage>::CreateObject, clsidOlePropPage, szOlePropPage, pszPageSection );
		

		CMenuRegistrator rcm;
		rcm.RegisterMenu( szOleViewNameMenu, 0, szOleViewNameMenuUI );
		

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

// sewLI.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "misc_const.h"

#include "sewLI.h"
#include "sewLI_action.h"
#include "sewLI_page.h"
#include "sewLI_object.h"
#include "sewLIView.h"
#include "SewRender.h"

#pragma comment (lib, "common2.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const char* szSewLISectName = "\\SewLI";

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	if( ul_reason_for_call == DLL_PROCESS_ATTACH )
	{
		_dyncreate_t<App>::CreateObject();
		App::instance()->Init( (HINSTANCE)hModule );
		App::instance()->ObjectInfo( _dyncreate_t<CSewLIBase>::CreateObject, clsidSewLIBase, pszSewLIBaseProgID);
		//App::instance()->ObjectInfo( _dyncreate_t<CSewLIObjectAction>::CreateObject, clsidSewLIObjectAction, pszSewLIObjectActionProgID);
		App::instance()->ObjectInfo( _dyncreate_t<CSewLIInfo>::CreateObject, clsidSewLI, pszSewLIProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CSewLIPropertyPage>::CreateObject, clsidSewLIPage, pszSewLIPageProgID, pszPageSection );
		App::instance()->ObjectInfo( _dyncreate_t<CSewLIBaseInfo>::CreateObject, clsidSewLIBaseInfo, pszSewLIBaseInfoProgID, "named data objects" );
//		App::instance()->ObjectInfo( _dyncreate_t<CSewLIObjectActionInfo>::CreateObject, clsidSewLIObjectActionInfo, pszSewLIObjectActionInfoProgID, pszActionSection );

		App::instance()->ObjectInfo( _dyncreate_t<CActionSewImportInfo>::CreateObject, clsidSewImportAction, pszSewImportActionProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CActionSewExportInfo>::CreateObject, clsidSewExportAction, pszSewExportActionProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CActionSewMoveFragmentInfo>::CreateObject, clsidSewMove, pszSewMoveActionProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CActionSewDeleteFragmentInfo>::CreateObject, clsidSewDelete, pszSewDeleteActionProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CActionSewSaveImageInfo>::CreateObject, clsidSewSaveImage, pszSewLISewSaveImage, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CShowSewLIInfo>::CreateObject, clsidShowSewLI, pszSewShowSewLIProgID, pszActionSection );

		//view
		App::instance()->ObjectInfo( _dyncreate_t<CSewLIView>::CreateObject, clsidSewLIView, pszSewLIViewProgID, "\\Image\\Views" );

		// renderer
		App::instance()->ObjectInfo( _dyncreate_t<CSewRender>::CreateObject, clsidSewLIRender, pszSewLIRenderProgID, szPluginRender );

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
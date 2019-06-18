// StatData.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "misc_const.h"

#include "stat_consts.h"
#include "stat_object.h"
#include "stat_filter.h"
#include "stat_actions.h"
#include "stat_ui_actions.h"
#include "stat_view.h"
#include "stat_dependence.h"
#include "stat_depend_actions.h"

#include "\vt5\awin\misc_module.h"
#include "MenuRegistrator.h"


#ifdef _DEBUG
#pragma comment (lib, "\\vt5\\common2\\debug\\common2.lib")
#pragma comment (lib, "\\vt5\\Controls2\\debug\\controls2.lib")
#else
#pragma comment (lib, "\\vt5\\common2\\release\\common2.lib")
#pragma comment (lib, "\\vt5\\Controls2\\release\\controls2.lib")
#endif//_DEBUG

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

		//register menu
		CMenuRegistrator rcm;
		rcm.RegisterMenu( szStatTabletViewNameMenu, 0, szStatTableViewNameMenuUI );

		CMenuRegistrator rcm2;
		rcm2.RegisterMenu( szStatTableDependenceViewNameMenu, 0, szStatTableDependenceViewNameMenuUI );
		
		//Register StatTable object		
		App::instance()->ObjectInfo( _dyncreate_t<CStatTableObjectInfo>::CreateObject, clsidStatTableObjectInfo, szStatTableObjectInfoProgID, "named data objects" );		
		App::instance()->ObjectInfo( _dyncreate_t<CStatTableObject>::CreateObject, clsidStatTableObject, szStatTableObjectProgID );				

		//Register StatTable File Filter
		App::instance()->ObjectInfo( _dyncreate_t<CStatTableFilter>::CreateObject, clsidStatTableFileFilter, szStatTableFileFilterProgID, "Image\\export-import" );					

		//Register StatView		
		App::instance()->ObjectInfo( _dyncreate_t<STAT_DEPENDENCE::stat_dependence_view>::CreateObject, clsidStatTableDependenceView, szStatTableDependenceViewProgID, "Image\\Views" );
		App::instance()->ObjectInfo( _dyncreate_t<CStatView>::CreateObject, clsidStatTableView, szStatTableViewProgID, "Image\\Views" );

		//Register Action CreateStatTable
		App::instance()->ObjectInfo( _dyncreate_t<CCreateStatTableInfo>::CreateObject, clsidCreateStatTable, szCreateStatTableProgID, pszActionSection );
		//Register Action ShowStatTable
		App::instance()->ObjectInfo( _dyncreate_t<CShowStatTableInfo>::CreateObject, clsidShowStatTable, szShowStatTableProgID, pszActionSection );
		//Register Action ExportStatTable
		App::instance()->ObjectInfo( _dyncreate_t<CExportStatTableInfo>::CreateObject, clsidExportStatTable, szExportStatTableProgID, pszActionSection );
		//Register Action DeleteObjectFromStatTable
		App::instance()->ObjectInfo( _dyncreate_t<CDeleteObjectFromStatTableInfo>::CreateObject, clsidDeleteObjectFromStatTable, szDeleteObjectFromStatTableProgID, pszActionSection );
		//Register Action FilterStatTable
		App::instance()->ObjectInfo( _dyncreate_t<CFilterStatTableInfo>::CreateObject, clsidFilterStatTable, szFilterStatTableProgID, pszActionSection );
		//Register Action AddStatTable
		App::instance()->ObjectInfo( _dyncreate_t<CSewStatTableInfo>::CreateObject, clsidSewStatTable, szSewStatTableProgID, pszActionSection );
		//Register Action HideStatTableColumn
		App::instance()->ObjectInfo( _dyncreate_t<CHideStatTableColumnInfo>::CreateObject, clsidHideStatTableColumn, szHideStatTableColumnProgID, pszActionSection );
		//Register Action ShowAllStatTableColumns
		App::instance()->ObjectInfo( _dyncreate_t<CShowAllStatTableColumnsInfo>::CreateObject, clsidShowAllStatTableColumns, szShowAllStatTableColumnsProgID, pszActionSection );
		//Register Action CustomizeStatTableColumns
		App::instance()->ObjectInfo( _dyncreate_t<CCustomizeStatTableColumnsInfo>::CreateObject, clsidCustomizeStatTableColumns, szCustomizeStatTableColumnsProgID, pszActionSection );

		//Register Action show_stat_dependence
		App::instance()->ObjectInfo( _dyncreate_t<STAT_DEPENDENCE::STAT_DEPENDENCE_ACTIONS::show_stat_dependence_info>::CreateObject, clsidShowStatTableDependence, szShowStatTableDependenceProgID, pszActionSection );

		ClassFactory* pfactory = (ClassFactory*)ClassFactory::CreateObject();
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

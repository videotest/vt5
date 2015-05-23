// Blood.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "misc_const.h"
#include "Blood.h"
#include "FindCells.h"
#include "BubbleSelect.h"
#include "Cell2Bin.h"
#include "ClassifyAam.h"
#include "SplitCircles.h"
#include "CutFrame.h"
#include "FindCircles.h"
#include "ResegmentCells.h"

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
		App::instance()->ObjectInfo( _dyncreate_t<CFindCellsInfo>::CreateObject, clsidFindCellsInfo, pszFindCellsProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CBubbleSelectInfo>::CreateObject, clsidBubbleSelectInfo, pszBubbleSelectProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CCell2BinInfo>::CreateObject, clsidCell2BinInfo, pszCell2BinProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CClassifyAamInfo>::CreateObject, clsidClassifyAamInfo, pszClassifyAamProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CLearnAamInfo>::CreateObject, clsidLearnAamInfo, pszLearnAamProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CLearnLinkAamInfo>::CreateObject, clsidLearnLinkAamInfo, pszLearnLinkAamProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CSplitCirclesInfo>::CreateObject, clsidSplitCirclesInfo, pszSplitCirclesProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CCutFrameInfo>::CreateObject, clsidCutFrameInfo, pszCutFrameProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CFindCirclesInfo>::CreateObject, clsidFindCirclesInfo, pszFindCirclesProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CResegmentCellsInfo>::CreateObject, clsidResegmentCellsInfo, pszResegmentCellsProgID, pszActionSection );

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

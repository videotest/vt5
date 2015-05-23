// Particles.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "misc_const.h"
#include "Particles.h"
#include "Segmentation.h"
#include "AddFrame.h"
#include "trajectory.h"
#include "SpermSegm.h"
#include "TailEdit.h"
#include "TakeObjectsSquares.h"

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
		App::instance()->ObjectInfo( _dyncreate_t<CSegmSmallObjectsInfo>::CreateObject, clsidSegmSmallObjects, pszSegmSmallObjectsProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CSpermSegmInfo>::CreateObject, clsidSpermSegm, pszSpermSegmProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CParticlesAddFrameInfo>::CreateObject, clsidParticlesAddFrame, pszParticlesAddFrameProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CSegmObjectsInfo>::CreateObject, clsidSegmObjects, pszSegmObjectsProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CSegmLinesInfo>::CreateObject, clsidSegmLines, pszSegmLinesProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CTailEditInfo>::CreateObject, clsidTailEdit, pszTailEditProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CTakeObjectsRectsInfo>::CreateObject, clsidTakeObjectsRects, pszTakeObjectsRectsProgID, pszActionSection );

		App::instance()->ObjectInfo( _dyncreate_t<trajectory_info>::CreateObject, clsid_trajectory_info, "trajectory.TrajectoryInfo", szPluginObject );
		App::instance()->ObjectInfo( _dyncreate_t<trajectory>::CreateObject, clsid_trajectory, "trajectory.Trajectory" );

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

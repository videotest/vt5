// Chrom_aam.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "misc_const.h"
#include "Chrom_aam.h"
#include "SelectByAxis.h"
#include "ChromSegm.h"
#include "CrossDiv.h"
#include "CrossDiv1.h"
#include "AutoDiv.h"
#include "GlueObj.h"
#include "ErectChrom.h"
#include "AverageObjects.h"
#include "FillHoles.h"
#include "ErectCC.h"

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
		App::instance()->ObjectInfo( _dyncreate_t<CSelectByAxisInfo>::CreateObject, clsidSelectByAxis, pszSelectByAxisProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CChromSegmInfo>::CreateObject, clsidChromSegm, pszChromSegmProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CChromSegmSecondInfo>::CreateObject, clsidChromSegmSecond, pszChromSegmSecondProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CStrangeAverageInfo>::CreateObject, clsidStrangeAverage, pszStrangeAverageProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CBoundDistanceInfo>::CreateObject, clsidBoundDistance, pszBoundDistanceProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CAutoAdjustBinInfo>::CreateObject, clsidAutoAdjustBin, pszAutoAdjustBinProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CCrossDivInfo>::CreateObject, clsidCrossDiv, pszCrossDivProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CCrossDiv1Info>::CreateObject, clsidCrossDiv1, pszCrossDiv1ProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CAutoDivInfo>::CreateObject, clsidAutoDiv, pszAutoDivProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CGlueObjInfo>::CreateObject, clsidGlueObj, pszGlueObjProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CErectChromInfo>::CreateObject, clsidErectChrom, pszErectChromProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CAverageObjectsInfo>::CreateObject, clsidAverageObjects, pszAverageObjectsProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CFillHolesInfo>::CreateObject, clsidFillHoles, pszFillHolesProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CErectChromOnInfo>::CreateObject, clsidErectChromOn, pszErectChromOnProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CErectOneChromOnOffInfo>::CreateObject, clsidErectOneChromOnOff, pszErectOneChromOnOffProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<IErectCC>::CreateObject, clsidErectCC, pszErectCCProgID );

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

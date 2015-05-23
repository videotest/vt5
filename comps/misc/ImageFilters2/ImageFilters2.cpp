// ImageFilters2.cpp : Defines the entry point for the DLL application.
//
#include "stdafx.h"
#include "misc_const.h"
#include "OpenByReconstructionFilter.h"
#include "OpenByReconstructionInfo.h"
#include "CloseByReconstruction.h"
#include "ImageFilters2.h"
#include "ColorBalance.h"
#include "AutoContrast.h"
#include "MeanEVFilter.h"
#include "MeanEVV.h"
#include "MeanKNV.h"
#include "Gradient.h"
#include "Variance.h"
#include "DoubleEdge.h"
#include "AAM_OBR.h"
#include "AAM_CBR.h"
#include "Even.h"
#include "EvenV.h"
#include "ColorSeg1.h"
#include "AreaSeg.h"
#include "FilterNucleus.h"
#include "TopHat.h"
#include "watermarkfilter.h"
#include "Signature.h"
#include "FMM.h"
#include "Sharpness.h"
#include "CleanFrame.h"

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
		App::instance()->ObjectInfo( _dyncreate_t<COpenByReconstructionInfo>::CreateObject, clsidOBRInfo, pszOBRProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CCloseByReconstructionInfo>::CreateObject, clsidCBRInfo, pszCBRProgID, pszActionSection );
		//App::instance()->ObjectInfo( _dyncreate_t<CCloseByReconstructionInfo>::CreateObject, clsidCBRInfo, pszCBRProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CColorBalanceInfo>::CreateObject, clsidColorBalanceInfo, pszColorBalanceProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CAutoContrastInfo>::CreateObject, clsidAutoContrastInfo, pszAutoContrastProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CMeanEVInfo>::CreateObject, clsidMeanEVInfo, pszMeanEVProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CMeanKNVInfo>::CreateObject, clsidMeanKNVInfo, pszMeanKNVProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CGradientInfo>::CreateObject, clsidGradientInfo, pszGradientProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CVarianceInfo>::CreateObject, clsidVarianceInfo, pszVarianceProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CDoubleEdgeInfo>::CreateObject, clsidDoubleEdgeInfo, pszDoubleEdgeProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CAamObrInfo>::CreateObject, clsidAamObrInfo, pszAamObrProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CAamCbrInfo>::CreateObject, clsidAamCbrInfo, pszAamCbrProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CEvenInfo>::CreateObject, clsidEvenInfo, pszEvenProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CColorSegInfo>::CreateObject, clsidColorSegInfo, pszColorSegProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CAreaSegInfo>::CreateObject, clsidAreaSegInfo, pszAreaSegProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CFilterNucleusInfo>::CreateObject, clsidFilterNucleusInfo, pszFilterNucleusProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CTopHatInfo>::CreateObject, clsidTopHatInfo, pszTopHatProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CWatermarkFilterInfo>::CreateObject, clsidWatermarkInfo, pszWatermarkProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CSignatureFilterInfo>::CreateObject, clsidSignatureInfo, pszSignatureProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CDilateOnImageInfo>::CreateObject, clsidFMM, pszFMMProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CEvenVInfo>::CreateObject, clsidEvenVInfo, pszEvenVProgID, pszActionSection );
		//App::instance()->ObjectInfo( _dyncreate_t<CBckgrndVInfo>::CreateObject, clsidBckgrndVInfo, pszBckgrndVProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CMeanEVVInfo>::CreateObject, clsidMeanEVVInfo, pszMeanEVVProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CSharpnessInfo>::CreateObject, clsidSharpnessInfo, pszSharpnessProgID, pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CCleanFrameInfo>::CreateObject, clsidCleanFrame, pszCleanFrameProgID, pszActionSection );

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

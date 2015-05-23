// thresholding.cpp : Defines the entry point for the DLL application.
//
#include "stdafx.h"
#include "misc_const.h"
#include "IterationEdge.h"
#include "FuzzySetsEdge.h"
#include "EntropyEdge.h"
#include "PeaksEdge.h"
#include "BoundsEdge.h"
#include "BinaryMinimum.h"
#include "thresholding.h"
#include "PyramidSegmentation.h"
#include "Relaxation.h"
#include "ChowKaneko.h"
#include "LocalMax.h"
#include "LocalMin.h"
#include "FindPhasesGray.h"
#include "FindPhasesColor.h"
#include "SegmColorPhases.h"
#include "TransformColor1.h"
#include "FindCircles2.h"
#include "ProcessPairs.h"
#include "FindCircles3.h"
#include "Canny.h"
#include "FindCurves.h"
#include "ThresholdByEdges.h"
#include "ContinueCurves.h"
#include "LinesThreshold.h"
#include "EvenByEdge.h"
#include "SegmSerial.h"
#include "SegmSerial3.h"

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

		App::instance()->ObjectInfo( _dyncreate_t<CIterationEdgeInfo>::CreateObject, clsidIterationInfo, pszIterationProgID , pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CFuzzySetsEdgeInfo>::CreateObject, clsidFuzzySetsInfo, pszFuzzySetsProgID , pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CEntropyEdgeInfo>::CreateObject, clsidEntropyInfo, pszEntropyProgID , pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CPeaksEdgeInfo>::CreateObject, clsidPeaksInfo, pszPeaksProgID , pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CBoundsEdgeInfo>::CreateObject, clsidBoundsInfo, pszBoundsProgID , pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CBinaryMinimumEdgeInfo>::CreateObject, clsidMinBinaryInfo, pszMinBinaryProgID , pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CPyramidInfo>::CreateObject, clsidPyramidInfo, pszPyramidProgID , pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CRelaxationEdgeInfo>::CreateObject, clsidRelaxationInfo, pszRelaxationProgID , pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CChowKanekoInfo>::CreateObject, clsidChowKanekoInfo, pszChowKanekoProgID , pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CLocalMaxInfo>::CreateObject, clsidLocalMaxInfo, pszLocalMaxProgID , pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CLocalMinInfo>::CreateObject, clsidLocalMinInfo, pszLocalMinProgID , pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CFindPhasesGrayInfo>::CreateObject, clsidFindPhasesGrayInfo, pszFindPhasesGrayProgID , pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CFindPhasesColorInfo>::CreateObject, clsidFindPhasesColorInfo, pszFindPhasesColorProgID , pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CSegmColorPhasesInfo>::CreateObject, clsidSegmColorPhasesInfo, pszSegmColorPhasesProgID , pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CTransformColor1Info>::CreateObject, clsidTransformColor1Info, pszTransformColor1ProgID , pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CFindCircles2Info>::CreateObject, clsidFindCircles2Info, pszFindCircles2ProgID , pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CProcessPairsInfo>::CreateObject, clsidProcessPairsInfo, pszProcessPairsProgID , pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CFindCircles3Info>::CreateObject, clsidFindCircles3Info, pszFindCircles3ProgID , pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CCannyThresholdInfo>::CreateObject, clsidCannyThresholdInfo, pszCannyThresholdProgID , pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CFindCurvesInfo>::CreateObject, clsidFindCurvesInfo, pszFindCurvesProgID , pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CThresholdByEdgesInfo>::CreateObject, clsidThresholdByEdgesInfo, pszThresholdByEdgesProgID , pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CContinueCurvesInfo>::CreateObject, clsidContinueCurvesInfo, pszContinueCurvesProgID , pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CLinesThresholdInfo>::CreateObject, clsidLinesThresholdInfo, pszLinesThresholdProgID , pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CEvenByEdgeInfo>::CreateObject, clsidEvenByEdgeInfo, pszEvenByEdgeProgID , pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CSegmSerialInfo>::CreateObject, clsidSegmSerialInfo, pszSegmSerialProgID , pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CSegmSerial2Info>::CreateObject, clsidSegmSerial2Info, pszSegmSerial2ProgID , pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CSegmSerial3Info>::CreateObject, clsidSegmSerial3Info, pszSegmSerial3ProgID , pszActionSection );
		App::instance()->ObjectInfo( _dyncreate_t<CFastFindPhasesColorInfo>::CreateObject, clsidFastFindPhasesColorInfo, pszFastFindPhasesColorProgID , pszActionSection );

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



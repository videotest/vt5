// Metall.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "PhaseActions.h"
#include "SegmSO.h"
#include "MeasureChord.h"
#include "JoinPhases.h"
#include "misc_const.h"
#include "DelLine.h"
#include "ActionStrip.h"
#include "ActionStripEx.h"
#include "Stringer.h"
#include "MetallParams.h"
#include "Border.h"
#include "CompareStatSelections.h"
#include "Watershed.h"
#include "MergeObjects.h"
#include "DirDilate.h"
#include "PolyEdit.h"
#include "FindStripes.h"
#include "BinaryFromObjects.h"
#pragma comment (lib, "common2.lib")

static const GUID clsidSegmSOInfo = 
{ 0x10d8291d, 0xbba6, 0x427c, { 0xb9, 0xe4, 0x6b, 0xa9, 0x4, 0xe3, 0xef, 0x95 } };

static const GUID clsidMeasureChordInfo = 
{ 0x53e93313, 0x3294, 0x4983, { 0xad, 0x4d, 0x12, 0x47, 0x4d, 0xbf, 0x46, 0xf4 } };

static const GUID clsidJoinPhasesInfo = 
{ 0xdee56e2d, 0xee87, 0x46e9, { 0x91, 0xeb, 0x78, 0x8, 0x39, 0x30, 0x6b, 0x21 } };

// {AF2BCD73-F7C3-4365-A59E-50EA1372D77D}
static const GUID clsidDelLinesInfo = 
{ 0xaf2bcd73, 0xf7c3, 0x4365, { 0xa5, 0x9e, 0x50, 0xea, 0x13, 0x72, 0xd7, 0x7d } };

// {2EAD31F0-3B5C-4a6d-B4B1-423AE03CCFB7}
static const GUID clsidDelLinesBinInfo = 
{ 0x2ead31f0, 0x3b5c, 0x4a6d, { 0xb4, 0xb1, 0x42, 0x3a, 0xe0, 0x3c, 0xcf, 0xb7 } };

static const GUID clsidActionStripInfo = 
{ 0x8ae7bdb1, 0x348f, 0x4ee9, { 0xa6, 0x65, 0xe0, 0xd2, 0xc, 0xb8, 0x4a, 0x2b } };

// {7D07D3E3-F871-453e-82C1-B370AD7CA183}
static const GUID clsidStringerInfo = 
{ 0x7d07d3e3, 0xf871, 0x453e, { 0x82, 0xc1, 0xb3, 0x70, 0xad, 0x7c, 0xa1, 0x83 } };

// {8DDCFCA6-1CC5-45cb-A812-237360031CDF}
static const GUID clsidFindBorderInfo = 
{ 0x8ddcfca6, 0x1cc5, 0x45cb, { 0xa8, 0x12, 0x23, 0x73, 0x60, 0x3, 0x1c, 0xdf } };


// {8D6F845C-0167-4e05-AA96-DD4CE4C146C9}
static const GUID clsidCompareStatSelections = 
{ 0x8d6f845c, 0x167, 0x4e05, { 0xaa, 0x96, 0xdd, 0x4c, 0xe4, 0xc1, 0x46, 0xc9 } };

// {3CD8BEB5-1AEE-47fd-8136-519DBED30524}
static const GUID clsidWatershedInfo = 
{ 0x3cd8beb5, 0x1aee, 0x47fd, { 0x81, 0x36, 0x51, 0x9d, 0xbe, 0xd3, 0x5, 0x24 } };

// {9AD2CF7A-DE38-4266-B439-C76CEF658D71}
static const GUID clsidMergeObjects = 
{ 0x9ad2cf7a, 0xde38, 0x4266, { 0xb4, 0x39, 0xc7, 0x6c, 0xef, 0x65, 0x8d, 0x71 } };

// {746E842A-87B3-4edf-A03E-FE378F8C962A}
static const GUID clsidDirDilate = 
{ 0x746e842a, 0x87b3, 0x4edf, { 0xa0, 0x3e, 0xfe, 0x37, 0x8f, 0x8c, 0x96, 0x2a } };

static const GUID clsidActionStripExInfo = 
{ 0x980e9a5e, 0x7385, 0x4915, { 0x97, 0x69, 0xff, 0xa2, 0xb9, 0x15, 0xdc, 0x75 } };

static const GUID clsidActionPolyEditInfo = 
{ 0xfd13fabf, 0x9f6, 0x4aae, { 0x93, 0x9c, 0xde, 0xc2, 0xef, 0xf2, 0x88, 0xc6 } };

static const GUID clsidFindStripes = 
{ 0x5bd63384, 0x2bf5, 0x4c89, { 0x80, 0x4c, 0x6d, 0x80, 0xc0, 0xdb, 0xcc, 0xde } };

// {E32F7F9C-AF42-4125-99A2-D2185C78B2DA}
static const GUID clsidBinaryFromObjectsInfo = 
{ 0xe32f7f9c, 0xaf42, 0x4125, { 0x99, 0xa2, 0xd2, 0x18, 0x5c, 0x78, 0xb2, 0xda } };

#define pszActionSegmSOProgID     "Metall.SegmSO"
#define pszActionMeasureChordProgID     "Metall.MeasureChord"
#define pszActionJoinPhasesProgID     "Metall.JoinPhases"
#define pszActionDelLinesProgID     "Metall.DelLines"
#define pszActionDelLinesBinProgID     "Metall.DelLinesBin"
#define pszActionStripProgID     "Metall.StripTop"
#define pszActionStringerProgID  "Metall.Stringer"
#define pszActionFindBorderProgID  "Metall.FindBorder"
#define pszCompareStatSelectionProgId  "Metall.CompareStatSelections"
#define pszActionWatershedProgID  "Metall.Watershed"
#define pszActionMergeObjectsProgID  "Metall.MergeObjects"
#define pszActionDirDilateProgID  "Metall.DirDilate"
#define pszActionStripExProgID     "Metall.StripTopEx"
#define pszActionPolyEditProgID     "Metall.PolyEdit"
#define pszActionFindStripesProgID     "Metall.FindStripes"
#define pszActionBinaryFromObjectsProgID     "Metall.BinFromObjects"


BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	if( ul_reason_for_call == DLL_PROCESS_ATTACH )
	{
		new App;
		App::instance()->Init((HINSTANCE)hModule);
		App::instance()->ObjectInfo(_dyncreate_t<CSegmSOInfo>::CreateObject, clsidSegmSOInfo, pszActionSegmSOProgID, pszActionSection);
		App::instance()->ObjectInfo(_dyncreate_t<CMeasureChordInfo>::CreateObject, clsidMeasureChordInfo, pszActionMeasureChordProgID, pszActionSection);
		App::instance()->ObjectInfo(_dyncreate_t<CJoinPhasesInfo>::CreateObject, clsidJoinPhasesInfo, pszActionJoinPhasesProgID, pszActionSection);
		App::instance()->ObjectInfo(_dyncreate_t<CDelLineInfo>::CreateObject, clsidDelLinesInfo, pszActionDelLinesProgID, pszActionSection);
		App::instance()->ObjectInfo(_dyncreate_t<CDelLineBinInfo>::CreateObject, clsidDelLinesBinInfo, pszActionDelLinesBinProgID, pszActionSection);
		App::instance()->ObjectInfo(_dyncreate_t<CActionStripInfo>::CreateObject, clsidActionStripInfo, pszActionStripProgID, pszActionSection);
		App::instance()->ObjectInfo(_dyncreate_t<CStringerInfo>::CreateObject, clsidStringerInfo, pszActionStringerProgID, pszActionSection);
		App::instance()->ObjectInfo(_dyncreate_t<CStringerAxis>::CreateObject, clsidStringerAxis, szStringerAxisProgId);
		App::instance()->ObjectInfo(_dyncreate_t<CStringerAxisInfo>::CreateObject, clsidStringerAxisInfo, szStringerAxisInfoProgId, "named data objects");
		App::instance()->ObjectInfo(_dyncreate_t<CFindBorderInfo>::CreateObject, clsidFindBorderInfo, pszActionFindBorderProgID, pszActionSection);
		App::instance()->ObjectInfo(_dyncreate_t<CCompareStatSelectionsInfo>::CreateObject, clsidCompareStatSelections, pszCompareStatSelectionProgId, pszActionSection);
		App::instance()->ObjectInfo(_dyncreate_t<CWatershedInfo>::CreateObject, clsidWatershedInfo, pszActionWatershedProgID, pszActionSection);
		App::instance()->ObjectInfo(_dyncreate_t<CMergeObjectsInfo>::CreateObject, clsidMergeObjects, pszActionMergeObjectsProgID, pszActionSection);
		App::instance()->ObjectInfo(_dyncreate_t<CDirDilateInfo>::CreateObject, clsidDirDilate, pszActionDirDilateProgID, pszActionSection);
		App::instance()->ObjectInfo(_dyncreate_t<CActionStripExInfo>::CreateObject, clsidActionStripExInfo, pszActionStripExProgID, pszActionSection);
		App::instance()->ObjectInfo(_dyncreate_t<CPolyEditInfo>::CreateObject, clsidActionPolyEditInfo, pszActionPolyEditProgID, pszActionSection);
		App::instance()->ObjectInfo(_dyncreate_t<CFindStripesInfo>::CreateObject, clsidFindStripes, pszActionFindStripesProgID, pszActionSection);
		App::instance()->ObjectInfo(_dyncreate_t<CBinaryFromObjectsInfo>::CreateObject, clsidBinaryFromObjectsInfo, pszActionBinaryFromObjectsProgID, pszActionSection);

		App::instance()->ObjectInfo(_dyncreate_t<CMetallParams>::CreateObject, clsidMetallParams, pszMetallParamsProgID, szPluginMeasurement);

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


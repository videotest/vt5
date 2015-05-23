// PhaseAnalysis.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "PhaseActions.h"
#include "misc_const.h"
#pragma comment (lib, "common2.lib")

// {AF7E058F-144B-44c0-8E1C-706AEEB45854}
//static const GUID clsidThreshAllPhases = 
//{ 0xaf7e058f, 0x144b, 0x44c0, { 0x8e, 0x1c, 0x70, 0x6a, 0xee, 0xb4, 0x58, 0x54 } };

// {9ADCB68F-887F-41c7-A50F-22724B5D0A5C}
static const GUID clsidThreshAllPhasesInfo = 
{ 0x9adcb68f, 0x887f, 0x41c7, { 0xa5, 0xf, 0x22, 0x72, 0x4b, 0x5d, 0xa, 0x5c } };

// {38376AAE-1F56-4d7f-AEEC-7691290E157D}
//static const GUID clsidBinThresholdPhase = 
//{ 0x38376aae, 0x1f56, 0x4d7f, { 0xae, 0xec, 0x76, 0x91, 0x29, 0xe, 0x15, 0x7d } };

// {756A9287-D2F7-4273-AFC1-BE14CD5F5967}
static const GUID clsidBinThresholdPhaseInfo = 
{ 0x756a9287, 0xd2f7, 0x4273, { 0xaf, 0xc1, 0xbe, 0x14, 0xcd, 0x5f, 0x59, 0x67 } };

// {15A5CA60-F968-4246-AB1C-9C805DA338C8}
//static const GUID clsidSwitchPhase = 
//{ 0x15a5ca60, 0xf968, 0x4246, { 0xab, 0x1c, 0x9c, 0x80, 0x5d, 0xa3, 0x38, 0xc8 } };

// {700E19F5-E9FD-4d05-BA73-2646CF522B8D}
static const GUID clsidSwitchPhaseInfo = 
{ 0x700e19f5, 0xe9fd, 0x4d05, { 0xba, 0x73, 0x26, 0x46, 0xcf, 0x52, 0x2b, 0x8d } };

// {788F249D-AE8F-4dad-9166-2507D7079CC1}
//static const GUID clsidNewPhase = 
//{ 0x788f249d, 0xae8f, 0x4dad, { 0x91, 0x66, 0x25, 0x7, 0xd7, 0x7, 0x9c, 0xc1 } };

// {39532B1A-A4B7-4f49-91B1-7B799626FEFF}
static const GUID clsidNewPhaseInfo = 
{ 0x39532b1a, 0xa4b7, 0x4f49, { 0x91, 0xb1, 0x7b, 0x79, 0x96, 0x26, 0xfe, 0xff } };

// {06400C36-E9AD-4e17-8A56-AA0EDEAD5F89}
//static const GUID clsidNewPhasePack = 
//{ 0x6400c36, 0xe9ad, 0x4e17, { 0x8a, 0x56, 0xaa, 0xe, 0xde, 0xad, 0x5f, 0x89 } };

// {A2C8F9A1-89CA-4b56-8AD0-74157CA672C6}
static const GUID clsidNewPhasePackInfo = 
{ 0xa2c8f9a1, 0x89ca, 0x4b56, { 0x8a, 0xd0, 0x74, 0x15, 0x7c, 0xa6, 0x72, 0xc6 } };

// {39C3F9DD-BE26-427f-8F19-3F43B7566C7E}
static const GUID clsidPackPhaseInfo = 
{ 0x39c3f9dd, 0xbe26, 0x427f, { 0x8f, 0x19, 0x3f, 0x43, 0xb7, 0x56, 0x6c, 0x7e } };

// {34A1DD92-DDF3-4e65-B8CF-33D5D2F91A60}
static const GUID clsidUnpackPhaseInfo = 
{ 0x34a1dd92, 0xddf3, 0x4e65, { 0xb8, 0xcf, 0x33, 0xd5, 0xd2, 0xf9, 0x1a, 0x60 } };

// {F26F67A5-7B84-42a2-A8D4-B905E2FC8740}
//static const GUID clsidSleep = 
//{ 0xf26f67a5, 0x7b84, 0x42a2, { 0xa8, 0xd4, 0xb9, 0x5, 0xe2, 0xfc, 0x87, 0x40 } };

// {7DEE9A9C-841E-4b99-AC30-DEBECCA9B9AB}
static const GUID clsidSleepInfo = 
{ 0x7dee9a9c, 0x841e, 0x4b99, { 0xac, 0x30, 0xde, 0xbe, 0xcc, 0xa9, 0xb9, 0xab } };

// {5522A1B6-8C9F-4bef-997F-2893A2081E4A}
//static const GUID clsidUpdateWindow = 
//{ 0x5522a1b6, 0x8c9f, 0x4bef, { 0x99, 0x7f, 0x28, 0x93, 0xa2, 0x8, 0x1e, 0x4a } };

// {8AA72974-509D-40eb-93E9-D2892DE3E44F}
static const GUID clsidUpdateWindowInfo = 
{ 0x8aa72974, 0x509d, 0x40eb, { 0x93, 0xe9, 0xd2, 0x89, 0x2d, 0xe3, 0xe4, 0x4f } };


//#define pszActionThresholdAllPhasesProgID     "PhaseAnalysis.ThresholdAllPhases"
#define pszActionThresholdAllPhasesInfoProgID "PhaseAnalysis.ThresholdAllPhasesInfo"
//#define pszActionBinThresholdPhaseProgID     "Threshold.ThresholdPhaseBin"
#define pszActionBinThresholdPhaseInfoProgID "Threshold.ThresholdPhaseBinInfo"
//#define pszActionSwitchPhaseProgID     "PhaseAnalysis.SwitchPhase"
#define pszActionSwitchPhaseInfoProgID "PhaseAnalysis.SwitchPhaseInfo"
//#define pszActionNewPhaseProgID     "PhaseAnalysis.NewPhase"
#define pszActionNewPhaseInfoProgID "PhaseAnalysis.NewPhaseInfo"
//#define pszActionNewPhasePackProgID     "PhaseAnalysis.NewPhasePack"
#define pszActionNewPhasePackInfoProgID "PhaseAnalysis.NewPhasePackInfo"
//#define pszActionSleepProgID     "MiscActions.Sleep"
#define pszActionSleepInfoProgID "MiscActions.SleepInfo"
//#define pszActionUpdateWindowProgID     "MiscActions.UpdateWindow"
#define pszActionUpdateWindowInfoProgID "MiscActions.UpdateWindowInfo"
#define pszActionPackPhaseInfoProgID "PhaseAnalysis.PackPhaseInfo"
#define pszActionUnpackPhaseInfoProgID "PhaseAnalysis.UnpackPhaseInfo"

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	if( ul_reason_for_call == DLL_PROCESS_ATTACH )
	{
		new App;
		App::instance()->Init((HINSTANCE)hModule);
//		App::instance()->ObjectInfo(_dyncreate_t<CBinThresholdAllPhases>::CreateObject, clsidThreshAllPhases, pszActionThresholdAllPhasesProgID);
		App::instance()->ObjectInfo(_dyncreate_t<CBinThresholdAllPhasesInfo>::CreateObject, clsidThreshAllPhasesInfo, pszActionThresholdAllPhasesInfoProgID, pszActionSection);
//		App::instance()->ObjectInfo(_dyncreate_t<CBinThresholdPhase>::CreateObject, clsidBinThresholdPhase, pszActionBinThresholdPhaseProgID);
		App::instance()->ObjectInfo(_dyncreate_t<CBinThresholdPhaseInfo>::CreateObject, clsidBinThresholdPhaseInfo, pszActionBinThresholdPhaseInfoProgID, pszActionSection);
//		App::instance()->ObjectInfo(_dyncreate_t<CSwitchPhase>::CreateObject, clsidSwitchPhase, pszActionSwitchPhaseProgID);
		App::instance()->ObjectInfo(_dyncreate_t<CSwitchPhaseInfo>::CreateObject, clsidSwitchPhaseInfo, pszActionSwitchPhaseInfoProgID, pszActionSection);
//		App::instance()->ObjectInfo(_dyncreate_t<CNewPhase>::CreateObject, clsidNewPhase, pszActionNewPhaseProgID);
		App::instance()->ObjectInfo(_dyncreate_t<CNewPhaseInfo>::CreateObject, clsidNewPhaseInfo, pszActionNewPhaseInfoProgID, pszActionSection);
//		App::instance()->ObjectInfo(_dyncreate_t<CNewPhasePacked>::CreateObject, clsidNewPhasePack, pszActionNewPhasePackProgID);
		App::instance()->ObjectInfo(_dyncreate_t<CNewPhasePackedInfo>::CreateObject, clsidNewPhasePackInfo, pszActionNewPhasePackInfoProgID, pszActionSection);
//		App::instance()->ObjectInfo(_dyncreate_t<CSleep>::CreateObject, clsidSleep, pszActionSleepProgID);
		App::instance()->ObjectInfo(_dyncreate_t<CSleepInfo>::CreateObject, clsidSleepInfo, pszActionSleepInfoProgID, pszActionSection);
//		App::instance()->ObjectInfo(_dyncreate_t<CUpdateWindow>::CreateObject, clsidUpdateWindow, pszActionUpdateWindowProgID);
		App::instance()->ObjectInfo(_dyncreate_t<CUpdateWindowInfo>::CreateObject, clsidUpdateWindowInfo, pszActionUpdateWindowInfoProgID, pszActionSection);
		App::instance()->ObjectInfo(_dyncreate_t<CPackPhaseInfo>::CreateObject, clsidPackPhaseInfo, pszActionPackPhaseInfoProgID, pszActionSection);
		App::instance()->ObjectInfo(_dyncreate_t<CUnpackPhaseInfo>::CreateObject, clsidUnpackPhaseInfo, pszActionUnpackPhaseInfoProgID, pszActionSection);
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



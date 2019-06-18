// Stage.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "Stage.h"
#include "resource.h"
#include "misc_const.h"
#include "nameconsts.h"
#include "StageObject.h"
#include "StageActions.h"
#include "LStep4X.h"
#pragma comment (lib, "common2.lib")

// {C9857836-7159-4336-BC82-65C6A88FCA9A}
static const GUID clsidStage = 
//{ 0xd8cb5640, 0x7c9d, 0x4a33, { 0xb7, 0x4a, 0x31, 0xa6, 0x8a, 0xf1, 0x20, 0x1c } };

{ 0xc9857836, 0x7159, 0x4336, { 0xbc, 0x82, 0x65, 0xc6, 0xa8, 0x8f, 0xca, 0x9a } };

#define pszStageProgId "Stage.StageObject"

// {61B28DD1-C091-4ee7-91C2-0096E27806EC}
static const GUID clsidMove = 
//{ 0xc7e237, 0xf771, 0x4b24, { 0xb3, 0x7d, 0x15, 0x35, 0x7e, 0x87, 0xc9, 0x45 } };

{ 0x61b28dd1, 0xc091, 0x4ee7, { 0x91, 0xc2, 0x0, 0x96, 0xe2, 0x78, 0x6, 0xec } };

// {44DC0068-F269-4118-81E6-0BFC253DA960}
static const GUID clsidMoveInfo = 
{ 0x44dc0068, 0xf269, 0x4118, { 0x81, 0xe6, 0xb, 0xfc, 0x25, 0x3d, 0xa9, 0x60 } };

#define pszMoveProgId "Stage.MoveStage"
#define pszMoveInfoProgId "Stage.MoveStageInfo"

// {C89A9A6A-E301-43e0-98FC-E48D88A8368A}
static const GUID clsidAutofocus = 
//{ 0xaf75f4d8, 0xad83, 0x4555, { 0x82, 0x8a, 0xf4, 0x58, 0x47, 0xfb, 0xa6, 0xc5 } };

{ 0xc89a9a6a, 0xe301, 0x43e0, { 0x98, 0xfc, 0xe4, 0x8d, 0x88, 0xa8, 0x36, 0x8a } };

// {EF9E6411-6F1C-4afa-BA7A-3CAB6294D568}
static const GUID clsidAutofocusInfo =
//{ 0xa22734df, 0x4c19, 0x4b1a, { 0xa5, 0xfb, 0x7a, 0xab, 0xc5, 0x2a, 0x6a, 0xe2 } };

{ 0xef9e6411, 0x6f1c, 0x4afa, { 0xba, 0x7a, 0x3c, 0xab, 0x62, 0x94, 0xd5, 0x68 } };

#define pszAutofusProgId "Stage.StageAutoFocus"
#define pszAutofusInfoProgId "Stage.StageAutoFocusInfo"

// {23D2231A-9839-4e36-AAE4-39B1292EA677}
static const GUID clsidUndoMove = 
//{ 0x8f84f919, 0x3219, 0x4b7d, { 0x97, 0xbe, 0x86, 0x2b, 0x4c, 0x2f, 0xb9, 0xa5 } };

{ 0x23d2231a, 0x9839, 0x4e36, { 0xaa, 0xe4, 0x39, 0xb1, 0x29, 0x2e, 0xa6, 0x77 } };

// {94EDECAC-F809-47ce-BA85-C0F7BB900F57}
static const GUID clsidUndoMoveInfo = 
//{ 0x8d210712, 0x16df, 0x4168, { 0xbe, 0x85, 0xd1, 0x4c, 0x20, 0xc1, 0xb3, 0x6a } };

{ 0x94edecac, 0xf809, 0x47ce, { 0xba, 0x85, 0xc0, 0xf7, 0xbb, 0x90, 0xf, 0x57 } };

#define pszUndoMoveProgId "Stage.StageUndoMove"
#define pszUndoMoveInfoProgId "Stage.StageUndoMoveInfo"

// {A3A659A6-839A-485a-A9AD-8BA9B12FB4CD}
static const GUID clsidFocus = 
//{ 0x3ea39131, 0xe792, 0x436a, { 0x9d, 0xb1, 0x8d, 0x1b, 0x7d, 0x8, 0xda, 0xa6 } };

{ 0xa3a659a6, 0x839a, 0x485a, { 0xa9, 0xad, 0x8b, 0xa9, 0xb1, 0x2f, 0xb4, 0xcd } };

// {0351A281-8E87-460e-AD5A-8D2BE6FEA08A}
static const GUID clsidFocusInfo = 
//{ 0xde95a0f2, 0x9765, 0x4849, { 0x8d, 0xef, 0x17, 0x33, 0xbc, 0x3f, 0xf2, 0x11 } };

{ 0x351a281, 0x8e87, 0x460e, { 0xad, 0x5a, 0x8d, 0x2b, 0xe6, 0xfe, 0xa0, 0x8a } };

#define pszFocusProgId "Stage.Focus"
#define pszFocusInfoProgId "Stage.FocusInfo"

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	ClassFactory * pfactory;
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		new App;
		App::instance()->Init((HINSTANCE)hModule);
		App::instance()->ObjectInfo(_dyncreate_t<CStageObject>::CreateObject, clsidStage,
			pszStageProgId, szPluginApplication);
		App::instance()->ObjectInfo(_dyncreate_t<CMoveStage>::CreateObject, clsidMove, pszMoveProgId);
		App::instance()->ObjectInfo(_dyncreate_t<CMoveStageInfo>::CreateObject, clsidMoveInfo,
			pszMoveInfoProgId, pszActionSection);
		App::instance()->ObjectInfo(_dyncreate_t<CAutofocus>::CreateObject, clsidAutofocus, pszAutofusProgId);
		App::instance()->ObjectInfo(_dyncreate_t<CAutofocusInfo>::CreateObject, clsidAutofocusInfo,
			pszAutofusInfoProgId, pszActionSection);
		App::instance()->ObjectInfo(_dyncreate_t<CFocus>::CreateObject, clsidFocus, pszFocusProgId);
		App::instance()->ObjectInfo(_dyncreate_t<CFocusInfo>::CreateObject, clsidFocusInfo,
			pszFocusInfoProgId, pszActionSection);
		App::instance()->ObjectInfo(_dyncreate_t<CUndoMove>::CreateObject, clsidUndoMove, pszUndoMoveProgId);
		App::instance()->ObjectInfo(_dyncreate_t<CUndoMoveInfo>::CreateObject, clsidUndoMoveInfo,
			pszUndoMoveInfoProgId, pszActionSection);
		pfactory = (ClassFactory*)ClassFactory::CreateObject();
		pfactory->LoadObjectInfo();
		pfactory->Release();
		break;
	case DLL_PROCESS_DETACH:
		App::instance()->Deinit();
		App::instance()->Release();
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
    return TRUE;
}

implement_com_exports();


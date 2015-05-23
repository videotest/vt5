// Classify.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "Actions.h"
#include "CopyImgToObj.h"
#include "Equalize.h"
#include "Manual.h"
#include "misc_const.h"
#include "StatClassifier.h"
#include "Ver30.h"


#pragma comment (lib, "common2.lib")


// {F71A726D-E236-4EF6-B307-0392D83D2E59}
static const GUID clsidActionChromoEqualize = 
{ 0xf71a726d, 0xe236, 0x4ef6, { 0xb3, 0x7, 0x3, 0x92, 0xd8, 0x3d, 0x2e, 0x59 } };

// {3420778C-A6B3-4229-9F80-8BA8F98B5E92}
static const GUID clsidActionChromoEqualizeInfo = 
{ 0x3420778c, 0xa6b3, 0x4229, { 0x9f, 0x80, 0x8b, 0xa8, 0xf9, 0x8b, 0x5e, 0x92 } };

// {DCD7F9C0-0BC3-453E-9281-E2BB105BCB7E}
static const GUID clsidActionChromoEqualizeObj = 
{ 0xdcd7f9c0, 0xbc3, 0x453e, { 0x92, 0x81, 0xe2, 0xbb, 0x10, 0x5b, 0xcb, 0x7e } };

// {A66CDC32-CFCD-47F8-AF95-B47F59C190FA}
static const GUID clsidActionChromoEqualizeObjInfo = 
{ 0xa66cdc32, 0xcfcd, 0x47f8, { 0xaf, 0x95, 0xb4, 0x7f, 0x59, 0xc1, 0x90, 0xfa } };

// {99FDB5C9-02B7-4C6B-BBA2-C625295C9401}
static const GUID clsidActionChromoUnequalizeObj = 
{ 0x99fdb5c9, 0x2b7, 0x4c6b, { 0xbb, 0xa2, 0xc6, 0x25, 0x29, 0x5c, 0x94, 0x1 } };

// {40AFB2C6-F4B7-4579-916D-42F6CE866162}
static const GUID clsidActionChromoUnequalizeObjInfo = 
{ 0x40afb2c6, 0xf4b7, 0x4579, { 0x91, 0x6d, 0x42, 0xf6, 0xce, 0x86, 0x61, 0x62 } };

// {453D2A00-05EE-11D6-BA8D-0002B38ACE2C}
static const GUID clsidActionCopyImageToObjects = 
{ 0x453d2a00, 0x5ee, 0x11d6, { 0xba, 0x8d, 0x0, 0x2, 0xb3, 0x8a, 0xce, 0x2c } };

// {453D2A01-05EE-11D6-BA8D-0002B38ACE2C}
static const GUID clsidActionCopyImageToObjectsInfo = 
{ 0x453d2a01, 0x5ee, 0x11d6, { 0xba, 0x8d, 0x0, 0x2, 0xb3, 0x8a, 0xce, 0x2c } };

// {70F6287B-6596-4B85-849E-E7FF26F1EDE5}
static const GUID clsidActionClassify = 
{ 0x70f6287b, 0x6596, 0x4b85, { 0x84, 0x9e, 0xe7, 0xff, 0x26, 0xf1, 0xed, 0xe5 } };

// {441436E7-98F4-42A2-919B-CAE7681EA7F6}
static const GUID clsidActionClassifyInfo = 
{ 0x441436e7, 0x98f4, 0x42a2, { 0x91, 0x9b, 0xca, 0xe7, 0x68, 0x1e, 0xa7, 0xf6 } };

// {C4723342-FB7D-11D5-BA8D-0002B38ACE2C}
static const GUID clsidActionManClassify = 
{ 0xc4723342, 0xfb7d, 0x11d5, { 0xba, 0x8d, 0x0, 0x2, 0xb3, 0x8a, 0xce, 0x2c } };

// {C4723343-FB7D-11D5-BA8D-0002B38ACE2C}
static const GUID clsidActionManClassifyInfo = 
{ 0xc4723343, 0xfb7d, 0x11d5, { 0xba, 0x8d, 0x0, 0x2, 0xb3, 0x8a, 0xce, 0x2c } };

// {85D0AC8B-15A7-468E-9297-290C80F5E7B2}
static const GUID clsidActionTeach = 
{ 0x85d0ac8b, 0x15a7, 0x468e, { 0x92, 0x97, 0x29, 0xc, 0x80, 0xf5, 0xe7, 0xb2 } };

// {C5549F17-F598-4F0D-BFE0-772CAEF73EFF}
static const GUID clsidActionTeachInfo = 
{ 0xc5549f17, 0xf598, 0x4f0d, { 0xbf, 0xe0, 0x77, 0x2c, 0xae, 0xf7, 0x3e, 0xff } };

// {470FB65B-9B03-4239-A101-C28DB78505D7}
static const GUID clsidActionEmpty = 
{ 0x470fb65b, 0x9b03, 0x4239, { 0xa1, 0x1, 0xc2, 0x8d, 0xb7, 0x85, 0x5, 0xd7 } };

// {7DB01880-4BF2-492C-B9B7-9FEB42986133}
static const GUID clsidActionEmptyInfo = 
{ 0x7db01880, 0x4bf2, 0x492c, { 0xb9, 0xb7, 0x9f, 0xeb, 0x42, 0x98, 0x61, 0x33 } };

// {65232100-FAC5-11D5-BA8D-0002B38ACE2C}
static const GUID clsidActionAddObject = 
{ 0x65232100, 0xfac5, 0x11d5, { 0xba, 0x8d, 0x0, 0x2, 0xb3, 0x8a, 0xce, 0x2c } };

// {65232102-FAC5-11D5-BA8D-0002B38ACE2C}
static const GUID clsidActionAddObjectInfo = 
{ 0x65232102, 0xfac5, 0x11d5, { 0xba, 0x8d, 0x0, 0x2, 0xb3, 0x8a, 0xce, 0x2c } };

// {65232103-FAC5-11D5-BA8D-0002B38ACE2C}
static const GUID clsidActionAddAllObjects = 
{ 0x65232103, 0xfac5, 0x11d5, { 0xba, 0x8d, 0x0, 0x2, 0xb3, 0x8a, 0xce, 0x2c } };

// {65232104-FAC5-11D5-BA8D-0002B38ACE2C}
static const GUID clsidActionAddAllObjectsInfo = 
{ 0x65232104, 0xfac5, 0x11d5, { 0xba, 0x8d, 0x0, 0x2, 0xb3, 0x8a, 0xce, 0x2c } };

// {E0F4BC3D-7F60-4d90-8EEA-03C4CAC736F6}
static const GUID clsidStatClassifier = 
{ 0xe0f4bc3d, 0x7f60, 0x4d90, { 0x8e, 0xea, 0x3, 0xc4, 0xca, 0xc7, 0x36, 0xf6 } };


#define pszActionChromoEqualizeProgID     "Edit.ChromoEqualize"
#define pszActionChromoEqualizeInfoProgID "Edit.ChromoEqualizeInfo"
#define pszActionChromoEqualizeObjProgID     "Edit.ChromoEqualizeObj"
#define pszActionChromoEqualizeObjInfoProgID "Edit.ChromoEqualizeObjInfo"
#define pszActionChromoUnequalizeObjProgID     "Edit.ChromoUnequalizeObj"
#define pszActionChromoUnequalizeObjInfoProgID "Edit.ChromoUnequalizeObjInfo"
#define pszActionCopyImageToObjectsProgID "Edit.CopyImageToObjects"
#define pszActionCopyImageToObjectsInfoProgID "Edit.CopyImageToObjectsInfo"
#define pszClassifyActionProgID		"Classifiers.ActionClassify"
#define pszClassifyActionInfoProgID	"Classifiers.ActionClassifyInfo"
#define pszManClassifyActionProgID		"Classifiers.ActionManualClassify"
#define pszManClassifyActionInfoProgID	"Classifiers.ActionManualClassifyInfo"
#define pszTeachActionProgID		"Classifiers.ActionTeach"
#define pszTeachActionInfoProgID	"Classifiers.ActionTeachInfo"
#define pszEmptyActionProgID		"Classifiers.ActionEmpty"
#define pszEmptyActionInfoProgID	"Classifiers.ActionEmptyInfo"
#define pszAddObjectActionProgID		"Classifiers.ActionAddObject"
#define pszAddObjectActionInfoProgID	"Classifiers.ActionAddObjectInfo"
#define pszAddAllObjectsActionProgID		"Classifiers.ActionAddAllObjects"
#define pszAddAllObjectsActionInfoProgID	"Classifiers.ActionAddAllObjectsInfo"
#define pszStatClassifierProgID 	"Classifiers.StatClassifier"


BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	if( ul_reason_for_call == DLL_PROCESS_ATTACH )
	{
		new App;
		App::instance()->Init((HINSTANCE)hModule);

		App::instance()->ObjectInfo(_dyncreate_t<CActionChromoEqualizeObj>::CreateObject, clsidActionChromoEqualizeObj, pszActionChromoEqualizeObjProgID);
		App::instance()->ObjectInfo(_dyncreate_t<CActionChromoEqualizeObjInfo>::CreateObject, clsidActionChromoEqualizeObjInfo, pszActionChromoEqualizeObjInfoProgID, pszActionSection );
		App::instance()->ObjectInfo(_dyncreate_t<CActionChromoUnEqualizeObj>::CreateObject, clsidActionChromoUnequalizeObj, pszActionChromoEqualizeObjProgID);
		App::instance()->ObjectInfo(_dyncreate_t<CActionChromoUnEqualizeObjInfo>::CreateObject, clsidActionChromoUnequalizeObjInfo, pszActionChromoUnequalizeObjInfoProgID, pszActionSection );
		App::instance()->ObjectInfo(_dyncreate_t<CActionChromoEqualize>::CreateObject, clsidActionChromoEqualize, pszActionChromoEqualizeProgID);
		App::instance()->ObjectInfo(_dyncreate_t<CActionChromoEqualizeInfo>::CreateObject, clsidActionChromoEqualizeInfo, pszActionChromoEqualizeInfoProgID, pszActionSection );
		App::instance()->ObjectInfo(_dyncreate_t<CActionCopyImageToObjects>::CreateObject, clsidActionCopyImageToObjects, pszActionCopyImageToObjectsProgID);
		App::instance()->ObjectInfo( _dyncreate_t<CActionCopyImageToObjectsInfo>::CreateObject, clsidActionCopyImageToObjectsInfo, pszActionCopyImageToObjectsInfoProgID, pszActionSection );
		App::instance()->ObjectInfo(_dyncreate_t<CActionClassify>::CreateObject, clsidActionClassify, pszClassifyActionProgID);
		App::instance()->ObjectInfo(_dyncreate_t<CActionClassifyInfo>::CreateObject, clsidActionClassifyInfo, pszClassifyActionInfoProgID, pszActionSection );
		App::instance()->ObjectInfo(_dyncreate_t<CActionManClassify>::CreateObject, clsidActionManClassify, pszManClassifyActionProgID);
		App::instance()->ObjectInfo(_dyncreate_t<CActionManClassifyInfo>::CreateObject, clsidActionManClassifyInfo, pszManClassifyActionInfoProgID, pszActionSection );
		App::instance()->ObjectInfo(_dyncreate_t<CActionTeach>::CreateObject, clsidActionTeach, pszTeachActionProgID);
		App::instance()->ObjectInfo(_dyncreate_t<CActionTeachInfo>::CreateObject, clsidActionTeachInfo, pszTeachActionInfoProgID, pszActionSection );
		App::instance()->ObjectInfo(_dyncreate_t<CActionEmptyClassifier>::CreateObject, clsidActionEmpty, pszEmptyActionProgID);
		App::instance()->ObjectInfo(_dyncreate_t<CActionEmptyClassifierInfo>::CreateObject, clsidActionEmptyInfo, pszEmptyActionInfoProgID, pszActionSection );
		App::instance()->ObjectInfo(_dyncreate_t<CActionAddObject>::CreateObject, clsidActionAddObject, pszAddObjectActionProgID);
		App::instance()->ObjectInfo(_dyncreate_t<CActionAddObjectInfo>::CreateObject, clsidActionAddObjectInfo, pszAddObjectActionInfoProgID, pszActionSection );
		App::instance()->ObjectInfo(_dyncreate_t<CActionAddAllObjects>::CreateObject, clsidActionAddAllObjects, pszAddAllObjectsActionProgID);
		App::instance()->ObjectInfo(_dyncreate_t<CActionAddAllObjectsInfo>::CreateObject, clsidActionAddAllObjectsInfo, pszAddAllObjectsActionInfoProgID, pszActionSection );
		App::instance()->ObjectInfo(_dyncreate_t<CVersion3xObject>::CreateObject, clsidVersion3x, szVer3xObjectProgId);
		App::instance()->ObjectInfo(_dyncreate_t<CStatClassifier>::CreateObject, clsidStatClassifier, pszStatClassifierProgID);
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

#ifndef __oleclient_constant_h__
#define __oleclient_constant_h__



#define szDllName "OleClient.dll"

//#############################################################################

#define szTypeOleItemProgID "OleClient.OleObject"
#define szTypeOleItemInfoProgID "OleClient.OleItemInfo"



// {3F5798F7-1C0B-4684-BB28-419EC111AE5F}
static const GUID clsidOleItemData = 
{ 0x3f5798f7, 0x1c0b, 0x4684, { 0xbb, 0x28, 0x41, 0x9e, 0xc1, 0x11, 0xae, 0x5f } };

// {1A035164-23A3-47bf-9483-3A2A5A1D0990}
static const GUID clsidOleItemDataInfo = 
{ 0x1a035164, 0x23a3, 0x47bf, { 0x94, 0x83, 0x3a, 0x2a, 0x5a, 0x1d, 0x9, 0x90 } };




//#############################################################################
// {19A2772D-7A80-4f1e-B5B2-2948F69A0304}
static const GUID clsidInsertOleObjectInfo = 
{ 0x19a2772d, 0x7a80, 0x4f1e, { 0xb5, 0xb2, 0x29, 0x48, 0xf6, 0x9a, 0x3, 0x4 } };

#define szInsertOleObjectProgID "OleClient.InsertOleObject"

//#############################################################################
// {3E9F573C-7C0B-4d5d-BD0F-550AC0747A7C}
static const GUID clsidInsertOleObjectToExistInfo = 
{ 0x3e9f573c, 0x7c0b, 0x4d5d, { 0xbd, 0xf, 0x55, 0xa, 0xc0, 0x74, 0x7a, 0x7c } };
#define szInsertOleObjectToExistProgID "OleClient.InsertOleObjectToExist"

//#############################################################################
// {78B4AD96-7F4D-4201-BCCB-AC0C9944A28A}
static const GUID clsidInsertOleObjectFromClipboardInfo = 
{ 0x78b4ad96, 0x7f4d, 0x4201, { 0xbc, 0xcb, 0xac, 0xc, 0x99, 0x44, 0xa2, 0x8a } };

#define szInsertOleObjectFromClipboardProgID "OleClient.InsertOleObjectFromClipboard"

//#############################################################################
// {7F78C3FF-EE23-44df-9037-BC303CF56B13}
static const GUID clsidInsertOleObjectFromClipboardToExistInfo = 
{ 0x7f78c3ff, 0xee23, 0x44df, { 0x90, 0x37, 0xbc, 0x30, 0x3c, 0xf5, 0x6b, 0x13 } };
#define szInsertOleObjectFromClipboardToExistProgID "OleClient.InsertOleObjectFromClipboardToExist"

//#############################################################################
// {363CA0CA-29C5-4d59-8558-14CB8DB2C555}
static const GUID clsidShowOleObjectInfo = 
{ 0x363ca0ca, 0x29c5, 0x4d59, { 0x85, 0x58, 0x14, 0xcb, 0x8d, 0xb2, 0xc5, 0x55 } };
#define szShowOleObjectProgID "OleClient.ShowOleObject"


//#############################################################################
#define szOleClientView "OleClient.OleObjectView"

// {65B8C254-8AB5-4685-A06C-273443C10F91}
static const GUID clsidOleObjectView = 
{ 0x65b8c254, 0x8ab5, 0x4685, { 0xa0, 0x6c, 0x27, 0x34, 0x43, 0xc1, 0xf, 0x91 } };


//#############################################################################
// {5EF31714-377E-47c1-877D-F6DC385B66E6}
static const GUID clsidActivateOleObjectInfo = 
{ 0x5ef31714, 0x377e, 0x47c1, { 0x87, 0x7d, 0xf6, 0xdc, 0x38, 0x5b, 0x66, 0xe6 } };

#define szActivateOleObjectProgID "OleClient.ActivateOleObject"

//#############################################################################
// {E65500D8-791B-4f52-B138-A2226D7629B7}
static const GUID clsidDeactivateOleObjectInfo = 
{ 0xe65500d8, 0x791b, 0x4f52, { 0xb1, 0x38, 0xa2, 0x22, 0x6d, 0x76, 0x29, 0xb7 } };


#define szDeactivateOleObjectProgID "OleClient.DeactivateOleObject"

//#############################################################################
// {54F29462-16D0-485c-B00A-47F64105A402}
static const GUID clsidShowOleObjectContentInfo = 
{ 0x54f29462, 0x16d0, 0x485c, { 0xb0, 0xa, 0x47, 0xf6, 0x41, 0x5, 0xa4, 0x2 } };

#define szShowOleObjectContentProgID "OleClient.ShowOleObjectContent"

//#############################################################################
#define szTypeOleRenderProgID	"OleClient.OleRender"
// {8D1C4613-8AF4-4553-98BD-A8F8FB83469A}
static const GUID clsidOleRender = 
{ 0x8d1c4613, 0x8af4, 0x4553, { 0x98, 0xbd, 0xa8, 0xf8, 0xfb, 0x83, 0x46, 0x9a } };


//#############################################################################
#define szOlePropPage	"OleClient.OlePropertyPage"

// {4EAF2E48-7F6E-4e5e-B164-8B1F9B39D0ED}
static const GUID clsidOlePropPage = 
{ 0x4eaf2e48, 0x7f6e, 0x4e5e, { 0xb1, 0x64, 0x8b, 0x1f, 0x9b, 0x39, 0xd0, 0xed } };



#define szUpdateOleObjectProgID	"OleClient.UpdateOleObject"
// {9805FC5C-BFBD-4032-89E9-595B47EC6801}
static const GUID clsidUpdateOleObject = 
{ 0x9805fc5c, 0xbfbd, 0x4032, { 0x89, 0xe9, 0x59, 0x5b, 0x47, 0xec, 0x68, 0x1 } };



#endif //__oleclient_constant_h__
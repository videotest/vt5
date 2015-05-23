#include "stdafx.h"
#include "resource.h"
#include "imgactions.h"
#include "imageSourceview.h"
#include "aphine.h"
#include "ResizeDlg.h"
#include "RotateDlg.h"
#include "math.h"
#include "calibrint.h"
#include "\vt5\common2\misc_calibr.h"
#include "misc_utils.h"
#include "CalibrMarker.h"



//dyncreate
IMPLEMENT_DYNCREATE(CActionShowView, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionShowBinaryContour, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionShowBinary, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionShowBinaryBack, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionShowBinaryFore, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionShowPseudo, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionShowSource, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionShowPhase, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionCrop, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionBorders, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionClone, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionMerge, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionExpansion, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionRotate270, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionRotate180, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionRotate90, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionMirrorVert, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionMirrorHorz, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionRotate, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionResize, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionShowMasks, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionShowObjects, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionImageTransform, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionShowImage, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionShowCalibr, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionCipher, CCmdTargetEx);


//olecreate
// {7EF8F55E-5C7B-4b90-B435-FF56CFD7D045}
GUARD_IMPLEMENT_OLECREATE(CActionShowView, "ImageDoc.ShowView",
0x7ef8f55e, 0x5c7b, 0x4b90, 0xb4, 0x35, 0xff, 0x56, 0xcf, 0xd7, 0xd0, 0x45);
// {6B7EB936-6ECB-47a6-918A-D7CD5A41A6E4}
GUARD_IMPLEMENT_OLECREATE(CActionShowBinaryContour, "ImageDoc.ShowBinaryContour",
0x6b7eb936, 0x6ecb, 0x47a6, 0x91, 0x8a, 0xd7, 0xcd, 0x5a, 0x41, 0xa6, 0xe4);
// {63551DC8-6DF0-42ee-9C55-488B2BC6E163}
GUARD_IMPLEMENT_OLECREATE(CActionShowBinary, "ImageDoc.ShowBinary",
0x63551dc8, 0x6df0, 0x42ee, 0x9c, 0x55, 0x48, 0x8b, 0x2b, 0xc6, 0xe1, 0x63);
// {E7752AD0-DF77-424e-8719-2215EA28ADC4}
GUARD_IMPLEMENT_OLECREATE(CActionShowBinaryBack, "ImageDoc.ShowBinaryBack",
0xe7752ad0, 0xdf77, 0x424e, 0x87, 0x19, 0x22, 0x15, 0xea, 0x28, 0xad, 0xc4);
// {D2FE0ED5-5483-4fa8-892D-D507742B17E5}
GUARD_IMPLEMENT_OLECREATE(CActionShowBinaryFore, "ImageDoc.ShowBinaryFore",
0xd2fe0ed5, 0x5483, 0x4fa8, 0x89, 0x2d, 0xd5, 0x7, 0x74, 0x2b, 0x17, 0xe5);
// {D3317133-CDFB-4da8-9405-99C12B18F7D8}
GUARD_IMPLEMENT_OLECREATE(CActionShowPseudo, "ImageDoc.ShowPseudo",
0xd3317133, 0xcdfb, 0x4da8, 0x94, 0x5, 0x99, 0xc1, 0x2b, 0x18, 0xf7, 0xd8);
// {F03E05B6-8D41-40c9-9131-876432E85659}
GUARD_IMPLEMENT_OLECREATE(CActionShowSource, "ImageDoc.ShowSource",
0xf03e05b6, 0x8d41, 0x40c9, 0x91, 0x31, 0x87, 0x64, 0x32, 0xe8, 0x56, 0x59);
// {B4EA15AF-B7E6-4868-A353-3ECAB06FE600}
GUARD_IMPLEMENT_OLECREATE(CActionShowPhase, "ImageDoc.ShowPhase",
0xb4ea15af, 0xb7e6, 0x4868, 0xa3, 0x53, 0x3e, 0xca, 0xb0, 0x6f, 0xe6, 0x0);
// {6F3F7653-1E93-46fc-AAE9-BA1AE783EB81}
GUARD_IMPLEMENT_OLECREATE(CActionCrop, "ImageDoc.Crop",
0x6f3f7653, 0x1e93, 0x46fc, 0xaa, 0xe9, 0xba, 0x1a, 0xe7, 0x83, 0xeb, 0x81);
// {77748337-0C8F-11d4-A0CA-0000B493A187}
GUARD_IMPLEMENT_OLECREATE(CActionBorders, "ImageDoc.Borders",
0x77748337, 0xc8f, 0x11d4, 0xa0, 0xca, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87);
// {77748333-0C8F-11d4-A0CA-0000B493A187}
GUARD_IMPLEMENT_OLECREATE(CActionClone, "ImageDoc.Clone",
0x77748333, 0xc8f, 0x11d4, 0xa0, 0xca, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87);
// {F7317F93-0BBE-11d4-A0CA-0000B493A187}
GUARD_IMPLEMENT_OLECREATE(CActionMerge, "ImageDoc.Merge",
0xf7317f93, 0xbbe, 0x11d4, 0xa0, 0xca, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87);
// {CFDDFDE3-0ADC-11d4-A0CA-0000B493A187}
GUARD_IMPLEMENT_OLECREATE(CActionExpansion, "ImageDoc.Expansion",
0xcfddfde3, 0xadc, 0x11d4, 0xa0, 0xca, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87);
// {B556C3DB-0A12-11d4-A0CA-0000B493A187}
GUARD_IMPLEMENT_OLECREATE(CActionRotate270, "ImageDoc.Rotate270",
0xb556c3db, 0xa12, 0x11d4, 0xa0, 0xca, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87);
// {B556C3D7-0A12-11d4-A0CA-0000B493A187}
GUARD_IMPLEMENT_OLECREATE(CActionRotate180, "ImageDoc.Rotate180",
0xb556c3d7, 0xa12, 0x11d4, 0xa0, 0xca, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87);
// {B556C3D3-0A12-11d4-A0CA-0000B493A187}
GUARD_IMPLEMENT_OLECREATE(CActionRotate90, "ImageDoc.Rotate90",
0xb556c3d3, 0xa12, 0x11d4, 0xa0, 0xca, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87);
// {EE86BD47-0A0E-11d4-A0CA-0000B493A187}
GUARD_IMPLEMENT_OLECREATE(CActionMirrorVert, "ImageDoc.MirrorVert",
0xee86bd47, 0xa0e, 0x11d4, 0xa0, 0xca, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87);
// {EE86BD43-0A0E-11d4-A0CA-0000B493A187}
GUARD_IMPLEMENT_OLECREATE(CActionMirrorHorz, "ImageDoc.MirrorHorz",
0xee86bd43, 0xa0e, 0x11d4, 0xa0, 0xca, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87);
// {546E2A47-0955-11d4-A0CA-0000B493A187}
GUARD_IMPLEMENT_OLECREATE(CActionRotate, "ImageDoc.Rotate",
0x546e2a47, 0x955, 0x11d4, 0xa0, 0xca, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87);
// {546E2A43-0955-11d4-A0CA-0000B493A187}
GUARD_IMPLEMENT_OLECREATE(CActionResize, "ImageDoc.Resize",
0x546e2a43, 0x955, 0x11d4, 0xa0, 0xca, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87);
// {1288D79B-9B39-11d3-A69B-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CActionShowMasks, "ImageDoc.ShowMasks",
0x1288d79b, 0x9b39, 0x11d3, 0xa6, 0x9b, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);
// {1288D797-9B39-11d3-A69B-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CActionShowObjects, "ImageDoc.ShowObjects",
0x1288d797, 0x9b39, 0x11d3, 0xa6, 0x9b, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);
// {700D32A3-87B6-11d3-A67E-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CActionImageTransform, "ImageDoc.ImageTransform",
0x700d32a3, 0x87b6, 0x11d3, 0xa6, 0x7e, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);
// {9FFC0183-4F1A-11d3-A61F-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CActionShowImage, "ShowImage.ShowImage",
0x9ffc0183, 0x4f1a, 0x11d3, 0xa6, 0x1f, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);
// {E4C388B3-3FD9-444f-9741-4F6FB604378C}
GUARD_IMPLEMENT_OLECREATE(CActionShowCalibr, "ImageDoc.ShowCalibr", 
0xe4c388b3, 0x3fd9, 0x444f, 0x97, 0x41, 0x4f, 0x6f, 0xb6, 0x4, 0x37, 0x8c);
// {EC48C748-9DE4-4877-9C75-7C3E184A3BA3}
GUARD_IMPLEMENT_OLECREATE(CActionCipher, "ImageDoc.Cipher",
0xec48c748, 0x9de4, 0x4877, 0x9c, 0x75, 0x7c, 0x3e, 0x18, 0x4a, 0x3b, 0xa3);


//action info release
// {1CD458EE-E230-4660-B8C0-60D8FEB126C0}
static const GUID guidShowView = 
{ 0x1cd458ee, 0xe230, 0x4660, { 0xb8, 0xc0, 0x60, 0xd8, 0xfe, 0xb1, 0x26, 0xc0 } };
// {75D9994E-5D2F-4a2c-B08B-353808CB5873}
static const GUID guidShowBinaryContour = 
{ 0x75d9994e, 0x5d2f, 0x4a2c, { 0xb0, 0x8b, 0x35, 0x38, 0x8, 0xcb, 0x58, 0x73 } };
// {BE02E6F4-C1BC-452a-B52F-7212ABC93EC5}
static const GUID guidShowBinary = 
{ 0xbe02e6f4, 0xc1bc, 0x452a, { 0xb5, 0x2f, 0x72, 0x12, 0xab, 0xc9, 0x3e, 0xc5 } };
// {81A27A1C-E42F-44d4-9A2E-59D9943A97EB}
static const GUID guidShowBinaryBack = 
{ 0x81a27a1c, 0xe42f, 0x44d4, { 0x9a, 0x2e, 0x59, 0xd9, 0x94, 0x3a, 0x97, 0xeb } };
// {CFB3C4CC-3429-49a0-9642-E6743F513136}
static const GUID guidShowBinaryFore = 
{ 0xcfb3c4cc, 0x3429, 0x49a0, { 0x96, 0x42, 0xe6, 0x74, 0x3f, 0x51, 0x31, 0x36 } };
// {AC8FE502-1C48-4fda-B74A-F9B2116E161D}
static const GUID guidEnableBinarySelection =
{ 0xac8fe502, 0x1c48, 0x4fda, { 0xb7, 0x4a, 0xf9, 0xb2, 0x11, 0x6e, 0x16, 0x1d } };
// {E35BAC81-2DBA-40b0-8832-7795A29D86B9}
static const GUID guidShowPseudo =
{ 0xe35bac81, 0x2dba, 0x40b0, { 0x88, 0x32, 0x77, 0x95, 0xa2, 0x9d, 0x86, 0xb9 } };
// {D8B7F00D-303F-4aea-9DA0-1A0A6F45E730}
static const GUID guidShowSource =
{ 0xd8b7f00d, 0x303f, 0x4aea, { 0x9d, 0xa0, 0x1a, 0xa, 0x6f, 0x45, 0xe7, 0x30 } };
// {026897C4-6069-4d0b-A198-346FCAFBFE87}
static const GUID guidCrop =
{ 0x26897c4, 0x6069, 0x4d0b, { 0xa1, 0x98, 0x34, 0x6f, 0xca, 0xfb, 0xfe, 0x87 } };
// {77748335-0C8F-11d4-A0CA-0000B493A187}
static const GUID guidBorders =
{ 0x77748335, 0xc8f, 0x11d4, { 0xa0, 0xca, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87 } };
// {77748331-0C8F-11d4-A0CA-0000B493A187}
static const GUID guidClone =
{ 0x77748331, 0xc8f, 0x11d4, { 0xa0, 0xca, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87 } };
// {F7317F91-0BBE-11d4-A0CA-0000B493A187}
static const GUID guidMerge =
{ 0xf7317f91, 0xbbe, 0x11d4, { 0xa0, 0xca, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87 } };
// {CFDDFDE1-0ADC-11d4-A0CA-0000B493A187}
static const GUID guidExpansion =
{ 0xcfddfde1, 0xadc, 0x11d4, { 0xa0, 0xca, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87 } };
// {B556C3D9-0A12-11d4-A0CA-0000B493A187}
static const GUID guidRotate270 =
{ 0xb556c3d9, 0xa12, 0x11d4, { 0xa0, 0xca, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87 } };
// {B556C3D5-0A12-11d4-A0CA-0000B493A187}
static const GUID guidRotate180 =
{ 0xb556c3d5, 0xa12, 0x11d4, { 0xa0, 0xca, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87 } };
// {B556C3D1-0A12-11d4-A0CA-0000B493A187}
static const GUID guidRotate90 =
{ 0xb556c3d1, 0xa12, 0x11d4, { 0xa0, 0xca, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87 } };
// {EE86BD45-0A0E-11d4-A0CA-0000B493A187}
static const GUID guidMirrorVert =
{ 0xee86bd45, 0xa0e, 0x11d4, { 0xa0, 0xca, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87 } };
// {EE86BD41-0A0E-11d4-A0CA-0000B493A187}
static const GUID guidMirrorHorz =
{ 0xee86bd41, 0xa0e, 0x11d4, { 0xa0, 0xca, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87 } };
// {546E2A45-0955-11d4-A0CA-0000B493A187}
static const GUID guidRotate =
{ 0x546e2a45, 0x955, 0x11d4, { 0xa0, 0xca, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87 } };
// {546E2A41-0955-11d4-A0CA-0000B493A187}
static const GUID guidResize =
{ 0x546e2a41, 0x955, 0x11d4, { 0xa0, 0xca, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87 } };
// {F0EB9A63-C437-467b-93E2-51EC4D15DF83}
static const GUID guidEnableObjectSelection =
{ 0xf0eb9a63, 0xc437, 0x467b, { 0x93, 0xe2, 0x51, 0xec, 0x4d, 0x15, 0xdf, 0x83 } };
// {7AB2AB9E-E55D-412f-A56E-70C7D7D55D50}
static const GUID guidEnableFrameSelection =
{ 0x7ab2ab9e, 0xe55d, 0x412f, { 0xa5, 0x6e, 0x70, 0xc7, 0xd7, 0xd5, 0x5d, 0x50 } };
// {1288D799-9B39-11d3-A69B-0090275995FE}
static const GUID guidShowMasks =
{ 0x1288d799, 0x9b39, 0x11d3, { 0xa6, 0x9b, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };
// {1288D795-9B39-11d3-A69B-0090275995FE}
static const GUID guidShowObjects =
{ 0x1288d795, 0x9b39, 0x11d3, { 0xa6, 0x9b, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };
// {700D32A1-87B6-11d3-A67E-0090275995FE}
static const GUID guidImageTransform =
{ 0x700d32a1, 0x87b6, 0x11d3, { 0xa6, 0x7e, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };
// {9FFC0181-4F1A-11d3-A61F-0090275995FE}
static const GUID guidShowImage =
{ 0x9ffc0181, 0x4f1a, 0x11d3, { 0xa6, 0x1f, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };
// {925C7123-86D2-4ca3-9096-5650E270421D}
static const GUID guidShowPhase = 
{ 0x925c7123, 0x86d2, 0x4ca3, { 0x90, 0x96, 0x56, 0x50, 0xe2, 0x70, 0x42, 0x1d } };
// {1899F252-CE73-466d-B4DD-1CD7A44CF15C}
static const GUID guidShowCalibr = 
{ 0x1899f252, 0xce73, 0x466d, { 0xb4, 0xdd, 0x1c, 0xd7, 0xa4, 0x4c, 0xf1, 0x5c } };
// {24AAA163-AC6E-4950-9ED1-1ABD06EA585E}
static const GUID guidCipher = 
{ 0x24aaa163, 0xac6e, 0x4950, { 0x9e, 0xd1, 0x1a, 0xbd, 0x6, 0xea, 0x58, 0x5e } };

//action info
ACTION_INFO_FULL(CActionShowView, IDS_ACTION_SHOW_VIEW, IDS_MENU_VIEW, -1, guidShowView);
ACTION_INFO_FULL(CActionShowBinaryContour, IDS_ACTION_SHOW_BINARYCONTOUR, IDS_MENU_VIEW, -1, guidShowBinaryContour);
ACTION_INFO_FULL(CActionShowBinary, IDS_ACTION_SHOW_BINARY, IDS_MENU_VIEW, -1, guidShowBinary);
ACTION_INFO_FULL(CActionShowBinaryBack, IDS_ACTION_SHOW_BINARYBACK, IDS_MENU_VIEW, -1, guidShowBinaryBack);
ACTION_INFO_FULL(CActionShowBinaryFore, IDS_ACTION_SHOW_BINARYFORE, IDS_MENU_VIEW, -1, guidShowBinaryFore);
ACTION_INFO_FULL(CActionShowPhase, IDS_ACTION_SHOW_PHASE, IDS_MENU_VIEW, -1, guidShowPhase);
ACTION_INFO_FULL(CActionShowPseudo, IDS_ACTION_SHOW_PSEUDO, IDS_MENU_VIEW, -1, guidShowPseudo);
ACTION_INFO_FULL(CActionShowSource, IDS_ACTION_SHOW_SOURCE, IDS_MENU_VIEW, -1, guidShowSource);
ACTION_INFO_FULL(CActionCrop, IDS_ACTION_CROP, -1, -1, guidCrop);
ACTION_INFO_FULL(CActionBorders, IDS_ACTION_BORDERS, -1, -1, guidBorders);
ACTION_INFO_FULL(CActionClone, IDS_ACTION_CLONE, -1, -1, guidClone);
ACTION_INFO_FULL(CActionMerge, IDS_ACTION_MERGE, -1, -1, guidMerge);
ACTION_INFO_FULL(CActionExpansion, IDS_ACTION_EXPANSION, -1, -1, guidExpansion);
ACTION_INFO_FULL(CActionRotate270, IDS_ACTION_ROTATE_270, -1, -1, guidRotate270);
ACTION_INFO_FULL(CActionRotate180, IDS_ACTION_ROTATE_180, -1, -1, guidRotate180);
ACTION_INFO_FULL(CActionRotate90, IDS_ACTION_ROTATE_90, -1, -1, guidRotate90);
ACTION_INFO_FULL(CActionMirrorVert, IDS_ACTION_MIRROR_VERT, -1, -1, guidMirrorVert);
ACTION_INFO_FULL(CActionMirrorHorz, IDS_ACTION_MIRROR_HORZ, -1, -1, guidMirrorHorz);
ACTION_INFO_FULL(CActionRotate, IDS_ACTION_ROTATE, -1, -1, guidRotate);
ACTION_INFO_FULL(CActionResize, IDS_ACTION_RESIZE, -1, -1, guidResize);
ACTION_INFO_FULL(CActionShowMasks, IDS_ACTION_SHOW_MASKS, -1, -1, guidShowMasks);
ACTION_INFO_FULL(CActionShowObjects, IDS_ACTION_SHOW_OBJECTS, -1, -1, guidShowObjects);
ACTION_INFO_FULL(CActionImageTransform, IDS_ACTION_TRANSFORM, -1, -1, guidImageTransform);
ACTION_INFO_FULL(CActionShowImage, IDS_ACTION_SHOW_IMAGE, IDS_MENU_VIEW, -1, guidShowImage);
ACTION_INFO_FULL(CActionShowCalibr, IDS_ACTION_SHOW_CALIBR, IDS_MENU_VIEW, -1, guidShowCalibr);
ACTION_INFO_FULL(CActionCipher, IDS_ACTION_CIPHER, -1, -1, guidCipher);

//targets

ACTION_TARGET(CActionShowView, szTargetFrame);
ACTION_TARGET(CActionShowBinaryContour, szTargetFrame);
ACTION_TARGET(CActionShowBinary, szTargetFrame);
ACTION_TARGET(CActionShowBinaryBack, szTargetFrame);
ACTION_TARGET(CActionShowBinaryFore, szTargetFrame);
ACTION_TARGET(CActionShowPseudo, szTargetFrame);
ACTION_TARGET(CActionShowSource, szTargetFrame);
ACTION_TARGET(CActionShowPhase, szTargetFrame);
ACTION_TARGET(CActionCrop, szTargetViewSite);
ACTION_TARGET(CActionBorders, szTargetAnydoc);
ACTION_TARGET(CActionClone, szTargetAnydoc);
ACTION_TARGET(CActionMerge, szTargetAnydoc);
ACTION_TARGET(CActionExpansion, szTargetAnydoc);
ACTION_TARGET(CActionRotate270, szTargetAnydoc);
ACTION_TARGET(CActionRotate180, szTargetAnydoc);
ACTION_TARGET(CActionRotate90, szTargetAnydoc);
ACTION_TARGET(CActionMirrorVert, szTargetAnydoc);
ACTION_TARGET(CActionMirrorHorz, szTargetAnydoc);
ACTION_TARGET(CActionRotate, szTargetAnydoc);
ACTION_TARGET(CActionResize, szTargetAnydoc);
ACTION_TARGET(CActionShowMasks, szTargetFrame);
ACTION_TARGET(CActionShowObjects, szTargetFrame);
ACTION_TARGET(CActionShowImage, szTargetFrame);
ACTION_TARGET(CActionImageTransform, szTargetAnydoc);
ACTION_TARGET(CActionShowCalibr, szTargetViewSite);
ACTION_TARGET(CActionCipher, szTargetAnydoc);

//arguments
ACTION_ARG_LIST(CActionCrop)
	ARG_INT(_T("X"), 0 )	
	ARG_INT(_T("Y"), 0 )	
	ARG_INT(_T("Width"), -1 )	
	ARG_INT(_T("Height"), -1 )	
END_ACTION_ARG_LIST();


ACTION_ARG_LIST(CActionBorders)
	ARG_INT(_T("Left"), 0 )	
	ARG_INT(_T("Top"), 0 )	
	ARG_INT(_T("Right"), 0 )	
	ARG_INT(_T("Bottom"), 0 )	
	ARG_IMAGE( _T("Source") )
	RES_IMAGE( _T("Expanded") )
END_ACTION_ARG_LIST();


ACTION_ARG_LIST(CActionClone)
	ARG_STRING( _T("Source"), "")
END_ACTION_ARG_LIST();


ACTION_ARG_LIST(CActionMerge)
	ARG_STRING( _T("ColorSpace"), "RGB")
	ARG_IMAGE( _T("Pane0Image") )
	ARG_IMAGE( _T("Pane1Image") )
	ARG_IMAGE( _T("Pane2Image") )
	RES_IMAGE( _T("Combined") )
END_ACTION_ARG_LIST();


ACTION_ARG_LIST(CActionRotate270)
	ARG_IMAGE( _T("Source") )
	RES_IMAGE( _T("Rotated270") )
END_ACTION_ARG_LIST();

ACTION_ARG_LIST(CActionRotate180)
	ARG_IMAGE( _T("Source") )
	RES_IMAGE( _T("Rotated180") )
END_ACTION_ARG_LIST();

ACTION_ARG_LIST(CActionRotate90)
	ARG_IMAGE( _T("Source") )
	RES_IMAGE( _T("Rotated90") )
END_ACTION_ARG_LIST();

ACTION_ARG_LIST(CActionMirrorVert)
	ARG_IMAGE( _T("Source") )
	RES_IMAGE( _T("FlipedVert") )
END_ACTION_ARG_LIST();

ACTION_ARG_LIST(CActionMirrorHorz)
	ARG_IMAGE( _T("Source") )
	RES_IMAGE( _T("FlipedHorz") )
END_ACTION_ARG_LIST();

ACTION_ARG_LIST(CActionRotate)
	ARG_INT(_T("Angle"), 0 )
	ARG_IMAGE( _T("Source") )
	RES_IMAGE( _T("Rotated") )
END_ACTION_ARG_LIST();

ACTION_ARG_LIST(CActionResize)
	ARG_INT(_T("Width"), -1 )
	ARG_INT(_T("Height"), -1 )
	ARG_IMAGE( _T("Source") )
	RES_IMAGE( _T("Resized") )
END_ACTION_ARG_LIST();

ACTION_ARG_LIST(CActionImageTransform)
	ARG_DOUBLE("ZoomX", 1)
	ARG_DOUBLE("ZoomY", 1)
	ARG_DOUBLE("Angle", 0)
	ARG_INT("PosX", 0)
	ARG_INT("PosY", 0)
END_ACTION_ARG_LIST()

ACTION_ARG_LIST(CActionShowView)
	ARG_STRING( _T("ViewProgID"), szImageViewProgID)
END_ACTION_ARG_LIST();

ACTION_ARG_LIST(CActionShowCalibr)
	ARG_INT( _T("Show"), -1)
	ARG_INT( _T("SetToShellData"), 0)
END_ACTION_ARG_LIST();

ACTION_ARG_LIST(CActionCipher)
	ARG_INT( _T("NX"), 10)
	ARG_INT( _T("NY"), 10)
	ARG_INT( _T("Cipher"), 13) // взаимно простое с NX, NY
	ARG_IMAGE( _T("Image") )
	RES_IMAGE( _T("Ciphered") )
END_ACTION_ARG_LIST();


//implementation

//////////////////////////////////////////////////////////////////////
//CActionCrop implementation
BEGIN_INTERFACE_MAP(CActionCrop, CInteractiveActionBase)
	INTERFACE_PART(CActionCrop, IID_ILongOperation, Long)
END_INTERFACE_MAP()

class CCropRectCtrl: public CDrawControllerBase
{
public:
	CCropRectCtrl(CSize sizeBound) : m_sizeBound(sizeBound), m_ptOffset(0, 0)
	{
		m_size.cx = 0;
		m_size.cy = 0;
		m_bInBounds = false;
	}
	void DoDraw( CDC &dc, CPoint point, IUnknown *punkVS,  bool bErase )
	{
		CRect	rect2;

		rect2.left = point.x-m_size.cx/2;
		rect2.top = point.y-m_size.cy/2;
		rect2.right = rect2.left + m_size.cx;
		rect2.bottom = rect2.top + m_size.cy;

		rect2 = ::ConvertToWindow( punkVS, rect2 );

		CRect rect = ::ConvertToClient( punkVS, rect2 );
		

		CSize sizeBound = m_sizeBound;//::ConvertToClient(punkVS, m_sizeBound);

		if(rect.left >= 0 && rect.top >= 0 && rect.right < sizeBound.cx && rect.bottom < sizeBound.cy)
		{
			dc.Rectangle( rect2 );
			m_bInBounds = true;
			m_ptOffset = CPoint(0, 0);
		}
		else
		{
			
			if(rect.left < 0)
			{
				m_ptOffset.x = -rect.left;
				CSize size(-rect.left, 0);
				size = ::ConvertToWindow( punkVS, size );
				rect2.OffsetRect(CPoint(size.cx, 0));
			}
			if(rect.top < 0)
			{
				m_ptOffset.y = -rect.top;
				CSize size(0, -rect.top);
				size = ::ConvertToWindow( punkVS, size );
				rect2.OffsetRect(CPoint(0, size.cy));
			}
			if(rect.right > sizeBound.cx)
			{
				m_ptOffset.x = sizeBound.cx-rect.right;
				CSize size(sizeBound.cx-rect.right, 0);
				size = ::ConvertToWindow( punkVS, size );
				rect2.OffsetRect(CPoint(size.cx, 0));
			}
			if(rect.bottom > sizeBound.cy)
			{
				m_ptOffset.y = sizeBound.cy-rect.bottom;
				CSize size(0, sizeBound.cy-rect.bottom);
				size = ::ConvertToWindow( punkVS, size );
				rect2.OffsetRect(CPoint(0, size.cy));
			}

			dc.Rectangle( rect2 );

			m_bInBounds = false;
		}

	}
	void SetSize( CSize	size )
	{
		m_size = size;
	}
	bool IsInBounds(){return m_bInBounds;}
	CPoint GetOffset(){return m_ptOffset;}
protected:
	CSize	m_size;
	CSize	m_sizeBound;
	bool	m_bInBounds;
	CPoint	m_ptOffset;
};


CActionCrop::CActionCrop() : m_point(0, 0), m_size(0, 0)
{
	m_pctrl = 0;
}

CActionCrop::~CActionCrop()
{
}

bool CActionCrop::IsAvaible()
{
	bool	bAvaible;
	IUnknown *punk = GetTargetWindow();

	bAvaible = CheckInterface( punk, IID_IImageView );

	if( bAvaible )
	{
		IUnknown* punkImage = ::GetActiveObjectFromContext( m_punkTarget, szTypeImage );
		if( punkImage )
		{
			punkImage->Release();	punkImage = 0;
		}
		else
			bAvaible = false;



	}

	return bAvaible;
}

bool CActionCrop::IsChecked()
{
	return false;
}


bool CActionCrop::Invoke()
{
	if(!CanDoCrop())
	{
		return false;
	}

	// [Max] Crop initialize on first image and usen on second one
	IUnknownPtr ptrActiveImage;
	ptrActiveImage.Attach( ::GetActiveObjectFromContext( m_punkTarget, szTypeImage ), false );
	if( ::GetObjectKey( m_ptrImage ) !=  ::GetObjectKey( ptrActiveImage ) )
		m_ptrImage = ptrActiveImage;

	CImageWrp imageSource( m_ptrImage );
	
	if(imageSource == 0) return false;

	long nCX = imageSource.GetWidth();
	long nCY = imageSource.GetHeight();

	CImageWrp imageDest;

	CPoint ptOffset = imageSource.GetOffset();

	m_point.x = m_point.x - ( ptOffset.x + m_size.cx/2 );
	m_point.y = m_point.y - ( ptOffset.y + m_size.cy/2 );
	if(m_point.x < 0)
	{
		m_size.cx += m_point.x;
		m_point.x = 0;
	}
	if(m_point.y < 0)
	{
		m_size.cy += m_point.y;
		m_point.y = 0;
	}
	if(m_point.x + m_size.cx > nCX)
	{
		m_size.cx = nCX - m_point.x;
	}
	if(m_point.y + m_size.cy > nCY)
	{
		m_size.cy = nCY - m_point.y;
	}
	if(m_size.cx <= 0 || m_size.cy <= 0)
		return false;

	if( !imageDest.CreateNew(m_size.cx, m_size.cy, imageSource.GetColorConvertor()) )
		return false;
	//imageDest.SetOffset(imageSource.GetOffset());
	IUnknown	*punkParent = 0;
	{
		sptrINamedDataObject2	sptrN( imageSource );
		sptrN->GetParent( &punkParent );
	}
	{
		sptrINamedDataObject2	sptrN( imageDest );
		sptrN->SetParent( punkParent, 0 );
	}
	if( punkParent )
	{
		punkParent->Release();
	}

	if( CheckInterface(imageSource, IID_INamedData))
	{
		INamedDataObject2Ptr	ptr(imageDest);
		ptr->InitAttachedData();
		CopyNamedData(ptr, imageSource);
	}


	int	c, colors = imageDest.GetColorsCount();

	long cy = imageDest.GetHeight();
	long cx = imageDest.GetWidth();

	StartNotification(cy);

	long nStartY = m_point.y;

	for( long y = 0; y < cy; y++ )
	{
		for( c = 0; c < colors; c++ )
		{
			color	*pcolorSrc = imageSource.GetRow( y+nStartY, c );
			pcolorSrc += m_point.x;
			color	*pcolorDest = imageDest.GetRow( y, c );
				
			memcpy(pcolorDest, pcolorSrc, sizeof(color)*cx);
		}
		Notify(y);
	}
		
	FinishNotification();

	imageDest.InitContours();


	IViewPtr sptrV(m_punkTarget);
	IUnknown *punkDoc = 0;
	sptrV->GetDocument(&punkDoc);
	IDocumentSitePtr sptrDoc(punkDoc);
	if(punkDoc)
		punkDoc->Release();

	GuidKey key;
	INamedDataObject2Ptr sptrNDO(imageSource);
	if(sptrNDO != 0)
		sptrNDO->GetBaseKey(&key);
	sptrNDO = imageDest;
	if(sptrNDO != 0)
		sptrNDO->SetBaseKey(&key);

	_bstr_t bstr_name;
	{
		INamedObject2Ptr ptrNO( imageSource );

		if( ptrNO )
		{
			BSTR bstr = 0;
			ptrNO->GetName( &bstr );
			
			bstr_name = bstr;

			if( bstr ) ::SysFreeString( bstr );	
			
			bstr = 0;
		}
	}

	m_changes.RemoveFromDocData( sptrDoc, imageSource, false );

	{
		INamedObject2Ptr ptrNO( imageDest );
		if( ptrNO )
		{
			ptrNO->SetName( bstr_name );
		}

	}

	m_changes.SetToDocData(sptrDoc, imageDest );

	StoreCurrentExecuteParams();

	m_lTargetKey = ::GetObjectKey( sptrDoc );

	LeaveMode();

	return true;
}

bool CActionCrop::DoUndo()
{
	m_changes.DoUndo( m_punkTarget );
	return true;
}

bool CActionCrop::DoRedo()
{
	m_changes.DoRedo( m_punkTarget );
	return true;
}

void CActionCrop::StoreCurrentExecuteParams()
{
	SetArgument( _T("X"), _variant_t( m_point.x ) );
	SetArgument( _T("Y"), _variant_t( m_point.y ) );
	SetArgument( _T("Width"), _variant_t( m_size.cx ) );
	SetArgument( _T("Height"), _variant_t( m_size.cy ) );
}


bool CActionCrop::CanDoCrop()
{
	IScrollZoomSitePtr sptrSZ(m_punkTarget);
	if(sptrSZ != 0)
	{
		CSize size;
		sptrSZ->GetClientSize(&size);
		//size = ::ConvertToWindow( m_punkTarget, size );
		if(!(m_size.cx == size.cx && m_size.cy == size.cy) && m_size.cx <= size.cx && m_size.cy <= size.cy)
		{
			m_hcurActive = AfxGetApp()->LoadCursor(IDC_ARROW);
			return true;
		}
	}

	m_hcurActive = AfxGetApp()->LoadCursor(IDC_CURSOR_KIRPICH);

	return false;
}

void CActionCrop::_CreateController(CSize sizeContr)
{
	if( m_pctrl )
	{
		m_pctrl->Uninstall();
		m_pctrl->GetControllingUnknown()->Release();
		m_pctrl = 0;
	}

	IScrollZoomSitePtr sptrSZ(m_punkTarget);
	if(sptrSZ != 0)
	{
		CSize sizeBounds;
		sptrSZ->GetClientSize(&sizeBounds);
		m_pctrl = new CCropRectCtrl(sizeBounds);
		m_pctrl->SetSize(sizeContr);
		m_pctrl->Install();
	}
}


bool CActionCrop::Initialize()
{
	AddPropertyPageDescription( c_szCCropPageDlg );


	m_ptrImage.Attach( ::GetActiveObjectFromContext( m_punkTarget, szTypeImage ), false );
	if( m_ptrImage == 0 )return false;

	{
		INamedDataObject2Ptr	ptrNamed( m_ptrImage );
		IUnknown	*punkParent = 0;
		ptrNamed->GetParent( &punkParent );

		if( punkParent != 0 )
			m_ptrImage.Attach( punkParent, false );
	}

	if( !CInteractiveActionBase::Initialize() )
		return false;

	m_point.x = GetArgumentInt( _T("X") );
	m_point.y = GetArgumentInt( _T("Y") );
	m_size.cx = GetArgumentInt( _T("Width") );
	m_size.cy = GetArgumentInt( _T("Height") );

	if(::GetValueInt(::GetAppUnknown(), "Crop", "UseCalibration", 0 ) == 1)
	{
		double	fCalibr = 1;

		ICalibrPtr	ptrC( m_ptrImage );
		if( ptrC != 0 )ptrC->GetCalibration( &fCalibr, 0 );

		double	fUnitPerMeter = 1;
		::GetCalibrationUnit( &fUnitPerMeter, 0 );

		m_size.cx=int( m_size.cx/fUnitPerMeter/fCalibr +.5 );
		m_size.cy=int( m_size.cy/fUnitPerMeter/fCalibr +.5 );
	}

	

	if(CanDoCrop())
	{
		_CreateController(m_size);
	}

	if( m_size.cx > 0 && m_size.cy > 0 )
	{
		Finalize();
		return false;
	}
	else
		DoUpdateSettings();
	return true;
}

bool CActionCrop::DoLButtonDown( CPoint pt )
{
	m_point = pt;

	if(m_pctrl)
	{
		if(m_pctrl->IsInBounds())
			Finalize();
		else 
		{
			CPoint ptOffset = m_pctrl->GetOffset();
			if(ptOffset != CPoint(0, 0))
			{
				m_point += ptOffset;
				Finalize();
			}
		}
	}
	else
		AfxMessageBox( IDS_WRONG_CROUP_SIZE );
	return true;
}

bool CActionCrop::DoUpdateSettings()
{
	double	cx = GetValueDouble( GetAppUnknown(), "\\Crop", "Width", 100 );
	double	cy = GetValueDouble( GetAppUnknown(), "\\Crop", "Height", 100 );

	if(::GetValueInt(::GetAppUnknown(), "Crop", "UseCalibration", 0 ) == 1)
	{
		double	fCalibr = 1;

		ICalibrPtr	ptrC( m_ptrImage );
		if( ptrC != 0 )ptrC->GetCalibration( &fCalibr, 0 );

		double	fUnitPerMeter = 1;
		::GetCalibrationUnit( &fUnitPerMeter, 0 );

		m_size.cx=int( cx/fUnitPerMeter/fCalibr +.5 );
		m_size.cy=int( cy/fUnitPerMeter/fCalibr +.5 );
	}
	else
	{
		m_size.cx = (int)cx;
		m_size.cy = (int)cy;
	}

	
	if(CanDoCrop())
	{
		if( m_pctrl )
		{
			m_pctrl->Redraw( false );
			m_pctrl->SetSize( m_size );
			m_pctrl->Redraw( true );
		}
		else
		{
			_CreateController(m_size);
		}

	}
	else
	{
		if( m_pctrl )
		{
			m_pctrl->Uninstall();
			m_pctrl->GetControllingUnknown()->Release();
			m_pctrl = 0;
		}
	}

	return CInteractiveActionBase::DoUpdateSettings();
}

void CActionCrop::Finalize()
{
	if( m_pctrl )
	{
		m_pctrl->Uninstall();
		m_pctrl->GetControllingUnknown()->Release();
		m_pctrl = 0;
	}

	CInteractiveActionBase::Finalize();
}

//////////////////////////////////////////////////////////////////////
//CActionBorders implementation
CActionBorders::CActionBorders()
{
	m_ppColors = 0;
	m_colors = 0;
}

CActionBorders::~CActionBorders()
{
	for(long c = 0; c < m_colors; c++)
		delete m_ppColors[c];
	if(m_ppColors)
		delete m_ppColors;
	m_ppColors = 0;
}

bool CActionBorders::InvokeFilter()
{
	CImageWrp imageSrc(GetDataArgument());
	CImageWrp imageDest(GetDataResult());
	long nLeft = max(0, GetArgumentInt("Left"));
	long nTop = max(0, GetArgumentInt("Top"));
	long nRight = max(0, GetArgumentInt("Right"));
	long nBottom = max(0, GetArgumentInt("Bottom"));

	CRect rcScr = imageSrc.GetRect();
	CRect rcDest = rcScr;

	rcDest.InflateRect(nLeft, nTop, nRight, nBottom);
	if(rcDest.left < 0)
	{
		rcDest.OffsetRect(-CPoint(rcDest.left, 0));
	}
	if(rcDest.top < 0)
	{
		rcDest.OffsetRect(-CPoint(0, rcDest.top));
	}

	//imageDest.CreateCompatibleImage( imageSrc, true, rcDest );
	if(!imageDest.CreateNew(rcDest.Width(), rcDest.Height(), imageSrc.GetColorConvertor()))
		return false;
	
	imageDest.SetOffset(imageSrc.GetOffset());

	bool bCopyMask = false;

	IUnknown	*punkParent = 0;
	{
		sptrINamedDataObject2	sptrN( imageSrc );
		sptrN->GetParent( &punkParent );
	}
	{
		sptrINamedDataObject2	sptrN( imageDest );
		sptrN->SetParent( punkParent, 0 );
	}
	if( punkParent )
	{
		punkParent->Release();
		imageDest.InitRowMasks();
		bCopyMask = true;
	}

	int	c, colors = imageDest.GetColorsCount();

	m_colors = colors;

	long cy = imageDest.GetHeight();
	long cx = imageDest.GetWidth();


	DWORD dwBackColor = ::GetValueColor(GetAppUnknown(), "Editor", "Back", RGB(0,0,0));
	ASSERT(m_ppColors == 0);
	m_ppColors = new color*[colors];
	byte pRGB[3];
	pRGB[0] = GetBValue(dwBackColor);
	pRGB[1] = GetGValue(dwBackColor);
	pRGB[2] = GetRValue(dwBackColor);
	for (long i = 0; i < colors; i++)
	{
		m_ppColors[i] = new color[1];
		m_ppColors[i][0] = 0;
	}

	IColorConvertor5Ptr sptrCC = ::GetCCByName(imageSrc.GetColorConvertor());
	sptrCC->ConvertDIBToImageEx(pRGB, m_ppColors, 1, imageSrc.IsColor(), colors ); 

	StartNotification(cy);

	for( long y = 0; y < cy; y++ )
	{
		bool bFillBorder = false;
		if(y-nTop < 0 || y+nBottom >= cy)
		{
			//fill border
			bFillBorder = true;
		}
		for( c = 0; c < colors; c++ )
		{
			color	*pcolorSrc = 0;
			color	*pcolorDest = imageDest.GetRow( y, c );
			if(!bFillBorder)
				pcolorSrc = imageSrc.GetRow( y-nTop, c );
			for(long x = 0; x < cx; x++)
			{
				if((x-nLeft < 0) || (x+nRight >= cx) || bFillBorder)  
					pcolorDest[x] = (*m_ppColors[c]);
				else
					pcolorDest[x] = pcolorSrc[x-nLeft];
			}
			
		}
		Notify( y );
	}

		/*if(m_bCopyMask)
		{
			byte	*pmaskSrc = imageTarget.GetRowMask( y+dy )+dx;
			byte	*pmaskDest = imageNew.GetRowMask( y );

			memcpy( pmaskDest, pmaskSrc, cx );
		}*/
	
	imageDest.InitContours();

	FinishNotification();

	return true;
}

//////////////////////////////////////////////////////////////////////
//CActionClone implementation
CActionClone::CActionClone()
{
}

CActionClone::~CActionClone()
{
}

bool CActionClone::Invoke()
{
	CImageWrp imageDest;
	
	IDocumentSitePtr sptrDoc(m_punkTarget);
	IUnknown* punkView = 0;
	sptrDoc->GetActiveView(&punkView);
	IDataContext2Ptr sptrC(punkView);
	if(punkView)
		punkView->Release();

	CString	strObject = GetArgumentString("Source");
	IUnknown* punkDataObject = 0;
	if(!strObject.IsEmpty())
		punkDataObject = ::GetObjectByName(m_punkTarget, strObject);

	if( (!punkDataObject) && (sptrC != 0))
	{
		sptrC->GetActiveObject( _bstr_t("Image"), &punkDataObject );
	}

	IUnknown	*punkParent = 0;
	{
		sptrINamedDataObject2	sptrN(punkDataObject);
		if(sptrN != 0)
		{
			sptrN->GetParent( &punkParent );
			if(punkParent)
			{
				punkDataObject->Release();
				punkDataObject = punkParent;
			}
		}
	}
	
	CImageWrp imageSource(punkDataObject);
	if(punkDataObject)
		punkDataObject->Release();
	if(imageSource == 0)return false;

	CRect rcNew = imageSource.GetRect();
	if( !imageDest.CreateNew(rcNew.Width(), rcNew.Height(), imageSource.GetColorConvertor())  )
		return false;
	imageDest.SetOffset(imageSource.GetOffset());
//attach the parent image to the new image
	/*bool bCopyMask = false;
	IUnknown	*punkParent = 0;
	{
		sptrINamedDataObject2	sptrN( imageSource );
		sptrN->GetParent( &punkParent );
	}
	{
		sptrINamedDataObject2	sptrN( imageDest );
		sptrN->SetParent( punkParent, 0 );
	}
	if( punkParent )
	{
		punkParent->Release();
		imageDest.InitRowMasks();
		bCopyMask = true;
	}*/


	int cx = imageSource.GetWidth();
	int cy = imageSource.GetHeight();

	int nPanes = imageSource.GetColorsCount();

	StartNotification(cy);

	for(long y = 0; y < cy; y++)
	{
		//if(bCopyMask)
		//	memcpy( imageDest.GetRowMask( y ), imageSource.GetRowMask( y ), cx );
		for(int i = 0; i < nPanes; i++)
		{
			memcpy(imageDest.GetRow(y, i), imageSource.GetRow( y, i ), sizeof(color)*cx);
		}
		Notify(y);
	}
	FinishNotification();

	//generate the dest name
	{
		char	szClone[] = " Clone";

		CString	strSourceName = ::GetObjectName( imageSource );
		CString	strName;

		int	idxClone = strSourceName.Find( szClone );
		if( idxClone == -1 )
		{
			strName = strSourceName + szClone;
		}
		else
		{	
			strSourceName = strSourceName.Left( idxClone );

			INamedDataPtr	ptrN( m_punkTarget );
			
			int	nCount = 0;

			while( true )
			{
				if( nCount == 0 )
					strName = strSourceName+szClone;
				else
					strName.Format( "%s%s%d", (const char*)strSourceName, szClone, nCount );
				nCount++;

				_bstr_t	bstr = strName;
				long	nExist = 0;
				ptrN->NameExists( bstr, &nExist );

				if( !nExist )
					break;
			}
		}
		::SetObjectName( imageDest, strName );
	}

	imageDest.InitContours();

	m_changes.SetToDocData( m_punkTarget, imageDest );

	return true;
}

static bool __HasObject(IUnknown *punkData, const char *szType)
{
	sptrIDataTypeManager	sptrM( punkData );
	if( sptrM == 0 )
		return false;
	long	nTypesCounter = 0;
	sptrM->GetTypesCount( &nTypesCounter );
	for( long nType = 0; nType < nTypesCounter; nType ++ )
	{
		BSTR	bstrTypeName = 0;
		sptrM->GetType( nType, &bstrTypeName );
		CString	str = bstrTypeName;
		::SysFreeString( bstrTypeName );
		if( szType && str != szType )
			continue;
		IUnknownPtr	punkObj;
		LONG_PTR	lpos = 0;
		sptrM->GetObjectFirstPosition(nType, &lpos );
		return lpos != 0;
	}
	return false;
}
/*
bool CActionClone::IsAvaible()
{
	return __HasObject(m_punkTarget, "Image");
}
*/

bool CChechAvailable::IsAvaible()
{
	IApplicationPtr ptrApp = ::GetAppUnknown();

	IUnknown *punkDoc = 0;

	ptrApp->GetActiveDocument( &punkDoc );

	if( !punkDoc )
		return 0;

	IDocumentSitePtr ptrDoc = punkDoc;
	punkDoc->Release();


	if( ptrDoc == 0 )
		return 0;

	IUnknownPtr punkView;
	ptrDoc->GetActiveView( &punkView );

	if( punkView == 0)
		return  0;

	if (!CheckInterface( punkView, IID_IImageView ))
		return 0;

	IDataContext2Ptr sptrC(punkView);
	IUnknownPtr punkDataObject;
	sptrC->GetActiveObject( _bstr_t("Image"), &punkDataObject );
	return punkDataObject != 0;
}

//////////////////////////////////////////////////////////////////////
//CActionLongUndoBase implementation
BEGIN_INTERFACE_MAP(CActionLongUndoBase, CActionBase)
	INTERFACE_PART(CActionLongUndoBase, IID_ILongOperation, Long)
END_INTERFACE_MAP()

//undo/redo
bool CActionLongUndoBase::DoUndo()
{
	m_changes.DoUndo( m_punkTarget );
	return true;
}

bool CActionLongUndoBase::DoRedo()
{
	m_changes.DoRedo( m_punkTarget );
	return true;
}

//////////////////////////////////////////////////////////////////////
//CActionMerge implementation
CActionMerge::CActionMerge()
{
	m_nImages = 0;
	m_pImagesSource = 0;
}

CActionMerge::~CActionMerge()
{
	// vk begin
	if (m_pImagesSource)
	{
		for(int i = 0; i < m_nImages; i++)
		{
			if (m_pImagesSource[i])
			{
				delete m_pImagesSource[i];
				m_pImagesSource[i] = 0;
			}
		}
		delete[] m_pImagesSource;
		m_pImagesSource = 0;
	}
		
	//for(int i = 0; i < m_nImages; i++)
	//{
	//	m_pImagesSource[i]->Release();
	//}
	//if(m_pImagesSource)
	//	delete m_pImagesSource;
	// vk end
}

bool CActionMerge::InvokeFilter()
{
	CString strCC = GetArgumentString("ColorSpace");
	IColorConvertorPtr sptrCC = ::GetCCByName(strCC);
	if(sptrCC == 0)
		return false;

	sptrCC->GetColorPanesDefCount(&m_nImages);
	
	m_imageDest = CImageWrp( GetDataResult() );
	
	m_pImagesSource = new CImageWrp*[m_nImages];
	CRect	rcNew = NORECT;
	IUnknown	*punkParent = 0;
	CPoint ptOffset(0, 0);
	for(long i = 0; i < m_nImages; i++)
	{
		CString strArg;
		strArg.Format("Pane%dImage", i);
		m_pImagesSource[i] = new CImageWrp(GetDataArgument(strArg));
		if(m_pImagesSource[i]->GetColorConvertor() != "GRAY")	
		{
			m_nImages = i + 1;
			return false;
		}
		CRect rc = m_pImagesSource[i]->GetRect();

		if(rcNew == NORECT)
		{
			rcNew.right = rc.right;
			rcNew.bottom = rc.bottom;
			ptOffset = rc.TopLeft();
		}
		else
		{
			rcNew.right = min(rcNew.Width(), rc.Width());
			rcNew.bottom = min(rcNew.Height(), rc.Height());
		}

		if(!punkParent)
		{
			sptrINamedDataObject2	sptrN( (*m_pImagesSource[i]) );
			sptrN->GetParent( &punkParent );
		}	
	}
	
	if( !m_imageDest.CreateNew(rcNew.Width(), rcNew.Height(), strCC) )
		return false;
	m_imageDest.SetOffset(ptOffset);
	{
		sptrINamedDataObject2	sptrN( m_imageDest );
		sptrN->SetParent( punkParent, 0 );
	}
	if( punkParent )
		punkParent->Release();

	long cx = rcNew.Width();
	long cy = rcNew.Height();

	long nKx = rcNew.left;
	long nKy = rcNew.top;

	double	*pfK = new double[m_nImages*m_nImages];

	int	k1, k2;
	//get coefficientes
	for( k1 = 0; k1 < m_nImages; k1++ )
		for( k2 = 0; k2 < m_nImages; k2++ )
		{
			CString	str;
			str.Format( "Image%d_Pane%d", k1, k2 );

			pfK[k1*m_nImages+k2] = ::GetValueDouble( GetAppUnknown(), "\\CombineFromPanes", str, (k1 == k2)?1:0 );
		}

	//ajust it - sum must be = 1
	for( k1 = 0; k1 < m_nImages; k1++ )
	{
		double	fSum = 0;
		for( k2 = 0; k2 < m_nImages; k2++ )
			fSum += pfK[k1*m_nImages+k2];
		fSum = max( fSum, 0.0001 );
		for( k2 = 0; k2 < m_nImages; k2++ )
			pfK[k1*m_nImages+k2]/=fSum;
	}

	StartNotification(cy, m_nImages);

	color	**ppcolorSrc = new color*[m_nImages];
	color	**ppcolorDst = new color*[m_nImages];
	

	long	x, j;
	double	f;

	for(long y = 0; y < cy; y++)
	{
		for(i = 0; i < m_nImages; i++)
		{
			ppcolorSrc[i] = (*m_pImagesSource[i]).GetRow( y, 0 );
			ppcolorDst[i] = m_imageDest.GetRow(y, i);
		}

		for( x = 0; x < cx; x++ )
		{
			
			for(i = 0; i < m_nImages; i++)
			{
				f = 0;
				
				for(j = 0; j < m_nImages; j++)
					f += ppcolorSrc[j][x]*pfK[i*m_nImages+j];

				ppcolorDst[i][x] = (color)f;
			}
		}
		Notify(y);
	}

	delete pfK;
	delete ppcolorSrc;
	delete ppcolorDst;

	FinishNotification();

	m_imageDest.InitContours();


	return true;
}

// vk begin
bool CActionMerge::IsAvaible()
{
	if (__super::IsAvaible())
	{
		CString strCC = GetArgumentString("ColorSpace");
		IColorConvertorPtr sptrCC = ::GetCCByName(strCC);
		if (sptrCC == 0)
			return false;

		sptrCC->GetColorPanesDefCount(&m_nImages);
		
		CImageWrp** pImagesSource = new CImageWrp*[m_nImages];
		for(int i = 0; i < m_nImages; i++)
		{
			CString strArg;
			strArg.Format("Pane%dImage", i);
			pImagesSource[i] = new CImageWrp(GetDataArgument(strArg));
			if(pImagesSource[i]->GetColorConvertor() != "GRAY")	
			{
				return false;
			}
		}

		if (pImagesSource)
		{
			for(int i = 0; i < m_nImages; i++)
			{
				if (pImagesSource[i])
				{
					delete pImagesSource[i];
					pImagesSource[i] = 0;
				}
			}
			delete[] pImagesSource;
			pImagesSource = 0;
		}

		return true;
	}
	else
	{
		return false;
	}
}
// vk end

//////////////////////////////////////////////////////////////////////
//CActionExpansion implementation
CActionExpansion::CActionExpansion()
{
	m_nImages = 0;
	m_pimagesDest = 0;
}

CActionExpansion::~CActionExpansion()
{
	for(int i = 0; i < m_nImages; i++)
	{
		m_pimagesDest[i]->Release();
	}
	if(m_pimagesDest)
		delete m_pimagesDest;
	m_pimagesDest = 0;
}

bool CActionExpansion::IsAvaible()
{
	IDocumentSitePtr sptrDoc(m_punkTarget);
	IUnknown* punkView = 0;
	sptrDoc->GetActiveView( &punkView );
	IUnknown* punkActive = 0;
	if(punkView)
	{
		punkActive = GetActiveObjectFromContextEx(punkView);
		punkView->Release();
	}
	CImageWrp image(punkActive);
	if(punkActive)
		punkActive->Release();
	if(image == 0)
		return false;
	return true;
}

bool CActionExpansion::Invoke()
{
	IDocumentSitePtr sptrDoc(m_punkTarget);
	IUnknown* punkView = 0;
	sptrDoc->GetActiveView( &punkView );
	IUnknown* punkActive = 0;
	if(punkView)
	{
		punkActive = GetActiveObjectFromContextEx(punkView);
		punkView->Release();
	}
	CImageWrp image(punkActive);
	if(punkActive)
		punkActive->Release();

	if(image == 0)
		return false;

	long colors = image.GetColorsCount();
	long cx = image.GetWidth();
	long cy = image.GetHeight();

	IUnknown *punkCC = 0;
	image->GetColorConvertor(&punkCC);
	IColorConvertor2Ptr sptrCC(punkCC);
	if(punkCC)
		punkCC->Release();

	m_nImages = colors;
	CRect rectNew = image.GetRect();

	m_pimagesDest = new CImageWrp*[colors];
	
	StartNotification(cy, colors);
	CString	strImageName = ::GetObjectName( image );

	//m_changes.RemoveFromDocData( m_punkTarget, image, false );
	
	for(int i = 0; i < colors; i++)
	{
		BSTR	bstrPaneName = 0;
		sptrCC->GetPaneName( i, &bstrPaneName );
		CString	strPaneName = bstrPaneName;
		::SysFreeString( bstrPaneName );

		strPaneName = strImageName+" "+strPaneName;

		m_pimagesDest[i] = new CImageWrp;

		if( !m_pimagesDest[i]->CreateNew(rectNew.Width(), rectNew.Height(), "GRAY") )
			return false;
		//m_pimagesDest[i]->SetOffset(image.GetOffset());
		/*bool bCopyMask = false;
		IUnknown	*punkParent = 0;
		{
			sptrINamedDataObject2	sptrN( image );
			sptrN->GetParent( &punkParent );
		}
		{
			sptrINamedDataObject2	sptrN( m_pimagesDest[i] );
			sptrN->SetParent( punkParent, 0 );
		}
		if( punkParent )
		{
			punkParent->Release();
			bCopyMask = true;
			m_pimagesDest[i]->InitRowMasks();
		}*/

		if(i > 0)
			NextNotificationStage();

		
		for( long y = 0; y < cy; y++ )
		{
			byte	*pmaskScr = image.GetRowMask(y);
			//byte	*pmaskDest = m_pimagesDest[i]->GetRowMask(y);
			//if(bCopyMask)
			//	memcpy(pmaskDest, pmaskScr, cx);

			memcpy(m_pimagesDest[i]->GetRow(y, 0), image.GetRow( y, i ), sizeof(color)*cx);

			Notify(y);
		}

		//m_pimagesDest[i]->InitContours();

		::SetObjectName( *m_pimagesDest[i], strPaneName );

		m_changes.SetToDocData( m_punkTarget, *m_pimagesDest[i]);

	}
	
	
	FinishNotification();

	return true;
}


//////////////////////////////////////////////////////////////////////
//CActionRotate270 implementation
CActionRotate270::CActionRotate270()
{
}

bool CActionRotate270::SetParams(CAphineParams* pparams)
{
	pparams->fAngle = PI/2.;
	pparams->fKx = 1;
	pparams->fKy = 1;
	m_bAphineTransform = false;
	return true;
}

void CActionRotate270::DoTransform(CImageWrp& image, CImageWrp& imageTarget)
{
	int	cx = image.GetWidth(), cy = image.GetHeight();
	int	colors = imageTarget.GetColorsCount();

//preform transphormation
	m_ppcolorscr = new color*[colors];
	m_ppcolordest = new color**[colors];
	m_ppmask = new byte*[cx];
	m_colors = colors;
	{
		for( long c = 0; c  < colors; c++ )
		{
			m_ppcolordest[c] = new color*[cx];
			for(long x = 0; x < cx; x++)
			{
				m_ppmask[x] = imageTarget.GetRowMask( x );
				m_ppcolordest[c][x] = imageTarget.GetRow( x, c );
				Notify(x);
			}
		}
	}


	for( long y = 0; y < cy; y++ )
	{
		byte	*pmaskScr = image.GetRowMask( y );
		for(long c = 0; c  < colors; c++ )
		{
			m_ppcolorscr[c] = image.GetRow( y, c );
			for(long x = 0; x < cx; x++)
			{
				m_ppcolordest[c][cx-x-1][y] = m_ppcolorscr[c][x];
				if(m_bCopyMask)
					m_ppmask[cx-x-1][y] = pmaskScr[x];
			}
		}
	}
}	


//////////////////////////////////////////////////////////////////////
//CActionRotate180 implementation
CActionRotate180::CActionRotate180()
{
}

bool CActionRotate180::SetParams(CAphineParams* pparams)
{
	pparams->fAngle = 0;
	pparams->fKx = 1;
	pparams->fKy = 1;
	m_bAphineTransform = false;
	return true;
}

void CActionRotate180::DoTransform(CImageWrp& image, CImageWrp& imageTarget)
{
	int	x, y, cx = image.GetWidth(), cy = image.GetHeight();
	int	c, colors = imageTarget.GetColorsCount();

//preform transphormation
	m_ppcolorscr = new color*[colors];
	m_ppcolordest = new color**[colors];
	m_colors = colors;

	for(c = 0; c  < colors; c++ )
	{
		m_ppcolordest[c] = new color*[cy];
	}

	for(y = 0; y < cy; y++ )
	{
		byte	*pmaskSrc = image.GetRowMask( y );
		byte	*pmaskDest = imageTarget.GetRowMask( cy-y-1 );
		for(c = 0; c  < colors; c++ )
		{
			m_ppcolorscr[c] = image.GetRow( y, c );
			m_ppcolordest[c][y] = imageTarget.GetRow( cy-y-1, c );
			for(x = 0; x < cx; x++)
			{
				m_ppcolordest[c][y][cx-x-1] = m_ppcolorscr[c][x];
				if(m_bCopyMask)
					pmaskDest[cx-x-1] = pmaskSrc[x];
			}
		}
		Notify(y);
	}
}	


//////////////////////////////////////////////////////////////////////
//CTransformBase implementation
CTransformBase2* g_pActionBase = 0;

void AphineCallBack(int nNotify)
{
	if(g_pActionBase)
		g_pActionBase->Notify(nNotify);
}



CTransformBase2::CTransformBase2()
{
	m_ppcolorscr = 0;
	m_ppcolordest = 0;
	m_colors = 0;
	m_ppmask = 0;
	m_bAphineTransform = false;
	m_bCopyMask = true;
}

CTransformBase2::~CTransformBase2()
{
	if(m_ppcolorscr)
	{
		delete m_ppcolorscr;
		m_ppcolorscr = 0;
	}

	if(m_ppcolordest)
	{
		for( long c = 0; c  < m_colors; c++ )
		{
			delete m_ppcolordest[c];
		}
		delete m_ppcolordest;
		m_ppcolordest = 0;
	}
	if(m_ppmask)
	{
		delete m_ppmask;
		m_ppmask = 0;
	}
}

bool CTransformBase2::InvokeFilter()
{
	CImageWrp	image( GetDataArgument() );
	CImageWrp	imageNew( GetDataResult() );

	CRect	rectSource = image.GetRect();

	CPoint	pointOffset(rectSource.TopLeft());

//fill the transphorm params
	CAphineParams	params;
	params.fAngle = 0;
	params.fKx = rectSource.Width();
	params.fKy = rectSource.Height();
	params.ptCenter = rectSource.CenterPoint();

	if(!SetParams(&params)) return false;


	IUnknown	*punkParent = 0;
	{
		sptrINamedDataObject2	sptrN( image );
		sptrN->GetParent( &punkParent );
	}

	if(punkParent)
		params.ptCenterNew = rectSource.CenterPoint();
	else
	{
		long nNewWidth = abs(cos(params.fAngle)*rectSource.Width()*params.fKx)+abs(sin(params.fAngle)*rectSource.Height()*params.fKy);
		long nNewHeight  = abs(sin(params.fAngle)*rectSource.Width()*params.fKx)+abs(cos(params.fAngle)*rectSource.Height()*params.fKy);
		params.ptCenterNew = CPoint(rectSource.left + nNewWidth/2, rectSource.top  + nNewHeight/2);	
	}


	CRect	rectNew = params.CalcSize( rectSource );

	if(!m_bAphineTransform)
		rectNew = GetRect(rectSource);

//create a new image
	CImageWrp	imageTarget;

	if( !imageTarget.CreateNew( rectNew.Width(), rectNew.Height(), image.GetColorConvertor() ) )
		return false;

	//paul12.04.2002
	fill_background_color( imageTarget );

	imageTarget.SetOffset( rectNew.TopLeft() );

	//for resolve lookup problem
	if( CheckInterface(image, IID_INamedData))
	{
		INamedDataObject2Ptr	ptr(imageTarget);
		ptr->InitAttachedData();
		CopyNamedData(ptr, image);
	}


	//imageTarget.SetOffset( rectNew.TopLeft() );
//attach the parent image to the new image
	{
		sptrINamedDataObject2	sptrN( imageTarget );
		sptrN->SetParent( punkParent, 0 );
	}
	if( punkParent )punkParent->Release();

	bool	bZeroOffset = punkParent == 0;
	m_bCopyMask = punkParent !=0;
	
	
	//if(m_bCopyMask)
	imageTarget.InitRowMasks();
//preform transphormation

	int	x, y, cx = imageTarget.GetWidth(), cy = imageTarget.GetHeight();

	StartNotification( cy, 3 );

	g_pActionBase = this;
	if(m_bAphineTransform)
		DoAphineTransform( imageTarget, image, params, &AphineCallBack);
	else
		DoTransform(image, imageTarget);

	g_pActionBase = 0;

	CRect	rect = NORECT;
	bool	bInit = false;

	int	c, colors = imageTarget.GetColorsCount();

	cx = imageTarget.GetWidth(), cy = imageTarget.GetHeight();

	NextNotificationStage();

	for( y = 0; y < cy; y++ )
	{
		byte	*pmask = imageTarget.GetRowMask( y );
		for( x = 0; x < cx; x++ )
		{
			if( pmask[x] )
			{
				if( !bInit )
				{
					rect.left = x;
					rect.right = x+1;
					rect.top = y;
					rect.bottom = y+1;
					bInit = true;
				}
				else
				{
					rect.left = min( x, rect.left );
					rect.right = max( x+1, rect.right );
					rect.top = min( y, rect.top );
					rect.bottom = max( y+1, rect.bottom );
				}
			}
		}
		Notify( y );
	}

	int	dx = rect.TopLeft().x;
	int	dy = rect.TopLeft().y;

	imageNew.CreateCompatibleImage( imageTarget, false, rect );

	cy = rect.Height();
	cx = rect.Width();

	imageNew.InitRowMasks();

	NextNotificationStage();

	for( y = 0; y < cy; y++ )
	{
		if(m_bCopyMask)
		{
			byte	*pmaskSrc = imageTarget.GetRowMask( y+dy )+dx;
			byte	*pmaskDest = imageNew.GetRowMask( y );

			memcpy( pmaskDest, pmaskSrc, cx );
		}
		for( c = 0; c < colors; c++ )
		{
			color	*pcolorSrc = imageTarget.GetRow( y+dy, c )+dx;
			color	*pcolorDest = imageNew.GetRow( y, c );
			memcpy( pcolorDest, pcolorSrc, cx*sizeof( color) );
		}
		Notify( y );
	}


	if( bZeroOffset )
		pointOffset.x = pointOffset.y = 0;
	else
	{
		pointOffset.x = rectSource.left-(cx-rectSource.Width())/2;
		pointOffset.y = rectSource.top-(cy-rectSource.Height())/2;
	}

		
	imageNew.SetOffset(pointOffset);

	{
		CRect	rectTest = imageNew.GetRect();
		CPoint	pointTest = rectTest.CenterPoint();
	}


	imageNew.InitContours();
	
	FinishNotification();

	return true;
}

//////////////////////////////////////////////////////////////////////
//CActionRotate90 implementation
CActionRotate90::CActionRotate90()
{
}

bool CActionRotate90::SetParams(CAphineParams* pparams)
{
	pparams->fAngle = PI/2.;
	pparams->fKx = 1;
	pparams->fKy = 1;
	m_bAphineTransform = false;
	return true;
}

void CActionRotate90::DoTransform(CImageWrp& image, CImageWrp& imageTarget)
{
	int	x, y, cx = image.GetWidth(), cy = image.GetHeight();
	int	c, colors = imageTarget.GetColorsCount();

//preform transphormation
	m_ppcolorscr = new color*[colors];
	m_ppcolordest = new color**[colors];
	m_ppmask = new byte*[cx];
	m_colors = colors;

	
	for(c = 0; c  < colors; c++ )
	{
		m_ppcolordest[c] = new color*[cx];
		for(long x = 0; x < cx; x++)
		{
			m_ppmask[x] = imageTarget.GetRowMask( x );
			//ZeroMemory(m_ppmask[x], cy);
			m_ppcolordest[c][x] = imageTarget.GetRow( x, c );
			Notify( x );
		}
	}
	for( y = 0; y < cy; y++ )
	{
		byte	*pmaskScr = image.GetRowMask( y );
		for( c = 0; c  < colors; c++ )
		{
			m_ppcolorscr[c] = image.GetRow( y, c );
			for(x = 0; x < cx; x++)
			{
				m_ppcolordest[c][x][cy-y-1] = m_ppcolorscr[c][x];
				if(m_bCopyMask)
					m_ppmask[x][cy-y-1] = pmaskScr[x];
			}
		}
	}

}	


//////////////////////////////////////////////////////////////////////
//CActionMirrorVert implementation
CActionMirrorVert::CActionMirrorVert()
{
}

bool CActionMirrorVert::SetParams(CAphineParams* pparams)
{
	pparams->fAngle = 0;
	pparams->fKx = 1;
	pparams->fKy = 1;
	m_bAphineTransform = false;
	return true;
}

void CActionMirrorVert::DoTransform(CImageWrp& image, CImageWrp& imageTarget)
{
	int	x, y, cx = image.GetWidth(), cy = image.GetHeight();
	int	c, colors = imageTarget.GetColorsCount();

//preform transphormation
	m_ppcolorscr = new color*[colors];
	m_ppcolordest = new color**[colors];
	m_colors = colors;

	for(c = 0; c  < colors; c++ )
	{
		m_ppcolordest[c] = new color*[cy];
	}

	for(y = 0; y < cy; y++ )
	{
		byte	*pmaskSrc = image.GetRowMask( y );
		byte	*pmaskDest = imageTarget.GetRowMask( y );
		for(c = 0; c  < colors; c++ )
		{
			m_ppcolorscr[c] = image.GetRow( y, c );
			m_ppcolordest[c][y] = imageTarget.GetRow( y, c );
			for(x = 0; x < cx; x++)
			{
				m_ppcolordest[c][y][cx-x-1] = m_ppcolorscr[c][x];
				if(m_bCopyMask)
					pmaskDest[cx-x-1] = pmaskSrc[x];
			}
		}
		Notify(y);
	}
}	


//////////////////////////////////////////////////////////////////////
//CActionMirrorHorz implementation
CActionMirrorHorz::CActionMirrorHorz()
{
}

bool CActionMirrorHorz::SetParams(CAphineParams* pparams)
{
	pparams->fAngle = 0;
	pparams->fKx = 1;
	pparams->fKy = 1;
	m_bAphineTransform = false;
	return true;
}

void CActionMirrorHorz::DoTransform(CImageWrp& image, CImageWrp& imageTarget)
{
	int	y, cx = image.GetWidth(), cy = image.GetHeight();
	int	c, colors = imageTarget.GetColorsCount();

//preform transphormation
	m_ppcolorscr = new color*[colors];
	m_ppcolordest = new color**[colors];
	m_colors = colors;

	for(c = 0; c  < colors; c++ )
	{
		m_ppcolordest[c] = new color*[cy];
	}

	for(y = 0; y < cy; y++ )
	{
		byte	*pmaskSrc = image.GetRowMask( y );
		byte	*pmaskDest = imageTarget.GetRowMask( cy-y-1 );
		for(c = 0; c  < colors; c++ )
		{
			m_ppcolorscr[c] = image.GetRow( y, c );
			m_ppcolordest[c][y] = imageTarget.GetRow( cy-y-1, c );

			memcpy(m_ppcolordest[c][y], m_ppcolorscr[c], sizeof(color)*cx);
			if(m_bCopyMask)
				memcpy(pmaskDest,  pmaskSrc, cx);
		}
		Notify(y);
	}
}	

/////////////////////////////////////////////////////////////////////
//CActionRotate implementation
CActionRotate::CActionRotate()
{
}


bool CActionRotate::SetParams(CAphineParams* pparams)
{
	long nAngle = GetArgumentInt( "Angle" );
	
	m_bAphineTransform = true;

	if(nAngle == 0)
	{
		CRotateDlg dlg;
		dlg.m_nAngle = nAngle;
		
		if(dlg.DoModal() == IDOK)
		{
			nAngle = dlg.m_nAngle;
			
			if(nAngle <= 0 || nAngle >= 360)
				return false;
		}
		else
			return false;
	}

	pparams->fAngle = (360.-nAngle)/360.*2*PI;
	pparams->fKx = 1;
	pparams->fKy = 1;

	return true;
}


//////////////////////////////////////////////////////////////////////
//CActionResize implementation
CActionResize::CActionResize()
{
}

bool CActionResize::SetParams(CAphineParams* pparams)
{
	long nWidth = GetArgumentInt( "Width" );
	long nHeight = GetArgumentInt( "Height" );

	m_bAphineTransform = true;

	if(nWidth < 0 || nHeight < 0)
	{
		CResizeDlg dlg;
		dlg.m_nWidth = (long)pparams->fKx;
		dlg.m_nHeight = (long)pparams->fKy;

		if(dlg.DoModal() == IDOK)
		{
			nWidth = dlg.m_nWidth;
			nHeight = dlg.m_nHeight;

			if(nWidth < 0 || nHeight < 0)
				return false;
		}
		else
			return false;
	}

	pparams->fKx = nWidth/pparams->fKx;
	pparams->fKy = nHeight/pparams->fKy;

	return true;
}


//////////////////////////////////////////////////////////////////////
//CActionImageTransform implementation
CActionImageTransform::CActionImageTransform()
{
	m_bObject = false;
}

CActionImageTransform::~CActionImageTransform()
{
}

static void _CalcObject(IUnknown *punk)
{
	bool bRecalc = ::GetValueInt(GetAppUnknown(), "\\measurement", "DynamicRecalc", 1) == 1;
	if( bRecalc )
	{
		if( punk && CheckInterface( punk, IID_IMeasureObject ) )
		{
			IMeasureObjectPtr pObject = punk;
			if( pObject != 0 )
			{
				IUnknownPtr punkImage;
				pObject->GetImage( &punkImage );
				::CalcObject( punk , punkImage, true, 0 );
			}
		}
	}
}

bool CActionImageTransform::Invoke()
{
//get the params
	double	fZoomX = GetArgumentDouble( _T("ZoomX") );
	double	fZoomY = GetArgumentDouble( _T("ZoomY") );
	double	fAngle = GetArgumentDouble( _T("Angle") );
	CPoint	pointOffset;
	pointOffset.x = GetArgumentDouble( _T("PosX") );
	pointOffset.y = GetArgumentDouble( _T("PosY") );
//get the active context
	sptrIDocumentSite	sptrDS( m_punkTarget );

	IUnknown	*punkContext = 0;
	sptrDS->GetActiveView( &punkContext );
	if( !punkContext )
		return false;

	IViewPtr	ptrV( punkContext );
	punkContext->Release();

	IUnknown	*punkMF  = 0;

	ptrV->GetMultiFrame( &punkMF, false );
	IMultiSelectionPtr	ptrSel( punkMF );

	if( !punkMF )
		return false;
	punkMF->Release();

	DWORD nSelCount = 0;
	ptrSel->GetObjectsCount( &nSelCount );
	
	if( nSelCount < 1 )
		return false;

	IUnknown	*punkSelected = 0;
	ptrSel->GetObjectByIdx( 0, &punkSelected );

	if( !punkSelected )
		return false;

	IUnknownPtr	ptrObject = punkSelected;
	punkSelected->Release();

	ICompositeSupportPtr cs(ptrObject);
	if(cs!=0)
	{
		long pc;
		cs->GetPixCount(&pc);
		if(pc!=0) return false;
	}

	m_bObject = CheckInterface( ptrObject, IID_IMeasureObject );


	if( m_bObject )
	{
		m_object = (IUnknown*)ptrObject;
		m_imageSource.Attach( m_object.GetImage(), false );
	}
	else
	{
		m_imageSource = ptrObject;
	}

	if( m_imageSource == 0 )
		return false;

	CRect	rectSource = m_imageSource.GetRect();
//fill the transphorm params
	CAphineParams	params;

	params.fAngle = fAngle;
	params.fKx = fZoomX;
	params.fKy = fZoomY;

	params.ptCenter = rectSource.CenterPoint();
	params.ptCenterNew = rectSource.CenterPoint();

	CRect	rectNew = params.CalcSize( rectSource );
//create a new image
	CImageWrp	imageTarget;

	if( !imageTarget.CreateNew( rectNew.Width(), rectNew.Height(), m_imageSource.GetColorConvertor() ) )
		return false;
	imageTarget.SetOffset( rectNew.TopLeft() );
//attach the parent image to the new image
	IUnknown	*punkParent = 0;
	{
		sptrINamedDataObject2	sptrN( m_imageSource );
		sptrN->GetParent( &punkParent );
	}

	{
		sptrINamedDataObject2	sptrN( imageTarget );
		sptrN->SetParent( punkParent, 0 );

	}
	if( punkParent )
		punkParent->Release();

	imageTarget.InitRowMasks();
//preform transphormation
	DoAphineTransform( imageTarget, m_imageSource, params );

	CRect	rect;
	bool	bInit = false;

	int	x, y, cx = imageTarget.GetWidth(), cy = imageTarget.GetHeight();
	int	c, colors = imageTarget.GetColorsCount();

	for( y = 0; y < cy; y++ )
	{
		byte	*pmask = imageTarget.GetRowMask( y );
		for( x = 0; x < cx; x++ )
		{
			if( pmask[x] )
			{
				if( !bInit )
				{
					rect.left = x;
					rect.right = x+1;
					rect.top = y;
					rect.bottom = y+1;
					bInit = true;
				}
				else
				{
					rect.left = min( x, rect.left );
					rect.right = max( x+1, rect.right );
					rect.top = min( y, rect.top );
					rect.bottom = max( y+1, rect.bottom );
				}
			}
		}
	}

	int	dx = rect.TopLeft().x;
	int	dy = rect.TopLeft().y;

	rect+=imageTarget.GetOffset();
	m_imageDest.CreateCompatibleImage( imageTarget, false, rect );

	cy = m_imageDest.GetHeight();
	cx = m_imageDest.GetWidth();


	for( y = 0; y < cy; y++ )
	{
		byte	*pmaskSrc = imageTarget.GetRowMask( y+dy )+dx;
		byte	*pmaskDest = m_imageDest.GetRowMask( y );

		memcpy( pmaskDest, pmaskSrc, cx );
		for( c = 0; c < colors; c++ )
		{
			color	*pcolorSrc = imageTarget.GetRow( y+dy, c )+dx;
			color	*pcolorDest = m_imageDest.GetRow( y, c );
			memcpy( pcolorDest, pcolorSrc, cx*sizeof( color) );
		}
	}
	
	// Maxim
	if( pointOffset != CPoint( -1, -1 ) )
	{
		pointOffset.x += dx;
		pointOffset.y += dy;
	}
	else
		pointOffset = rect.TopLeft();


	m_imageDest.InitContours();
	m_imageDest.SetOffset( pointOffset );
//set data and store undo info

	if( !m_bObject )
	{
		m_changes.RemoveFromDocData( m_punkTarget, m_imageSource, false, true );
		m_changes.SetToDocData( m_punkTarget, m_imageDest );
	}
	else
	{
		_UpdateObject();
		m_object.SetImage( m_imageDest );
		_UpdateObject();
	}

	_CalcObject(ptrObject);
/*	bool bRecalc = ::GetValueInt(GetAppUnknown(), "\\measurement", "DynamicRecalc", 1) == 1;
	if( bRecalc )
	{
		IUnknown *punk = ptrObject;
		if( punk && CheckInterface( punk, IID_IMeasureObject ) )
		{
			IMeasureObjectPtr pObject = ptrObject;

			if( pObject != 0 )
			{
				IUnknown *punkImage = 0;
				pObject->GetImage( &punkImage );

				::CalcObject( punk , punkImage, true, 0 );
				punkImage->Release();
			}
		}
	}*/
	{
		CWnd *pWnd = GetWindowFromUnknown( punkContext );

		if( pWnd )
			pWnd->UpdateWindow();
	}

	return true;
}

//undo/redo
bool CActionImageTransform::DoUndo()
{
	if( m_bObject )
	{
		_UpdateObject();
		m_object.SetImage( m_imageSource );
		_UpdateObject();
		_CalcObject(m_object);
	}
	else
		m_changes.DoUndo( m_punkTarget );
	return true;
}

bool CActionImageTransform::DoRedo()
{
	if( m_bObject )
	{
		_UpdateObject();
		m_object.SetImage( m_imageDest );
		_UpdateObject();
		_CalcObject(m_object);
	}
	else
		m_changes.DoRedo( m_punkTarget );
	return true;
}

void CActionImageTransform::_UpdateObject()
{
	ASSERT(m_bObject);
	ASSERT(m_object!=0);

	INamedDataObject2Ptr	ptrN( m_object );
	IUnknown			*punkParent = 0;
	ptrN->GetParent( &punkParent );

	ASSERT(punkParent);
	IDataObjectListPtr	ptrPList( punkParent );
	punkParent->Release();

	ptrPList->UpdateObject( m_object );
}

//extended UI
bool CActionImageTransform::IsAvaible()
{
	sptrIDocumentSite	sptrDS( m_punkTarget );

	IUnknown	*punkContext = 0;
	sptrDS->GetActiveView( &punkContext );
	sptrIDataContext	sptrDC( punkContext );

	if( !punkContext )
		return false;
	punkContext->Release();

	IUnknown	*punkObject = 0;
	_bstr_t	bstrObjectType = _T("Image");
	sptrDC->GetActiveObject( bstrObjectType, &punkObject );

	if( !punkObject )
		return false;

	punkObject->Release();

	return true;
}


//////////////////////////////////////////////////////////////////////
//CActionShowImage implementation
CString CActionShowImage::GetViewProgID()
{	return szImageViewProgID;}

CString CActionShowMasks::GetViewProgID()
{	return szMaskViewProgID;}

CString CActionShowObjects::GetViewProgID()
{	return szObjectsViewProgID;}

CString CActionShowPseudo::GetViewProgID()
{	return szPseudoViewProgID;}

CString CActionShowSource::GetViewProgID()
{	return szSourceViewProgID;}

CString CActionShowBinaryFore::GetViewProgID()
{	return szBinaryForeViewProgID;}

CString CActionShowBinaryBack::GetViewProgID()
{	return szBinaryBackViewProgID;}

CString CActionShowBinary::GetViewProgID()
{	return szBinaryViewProgID;}

CString CActionShowBinaryContour::GetViewProgID()
{	return szBinaryContourViewProgID;}

CString CActionShowView::GetViewProgID()
{	return GetArgumentString("ViewProgID");}

CString CActionShowPhase::GetViewProgID()
{	return "ImageDoc.Phase";}


//paul12.04.2002
bool fill_background_color( IUnknown* punk_image )
{
	IImage3Ptr ptrImage = punk_image;
	if( ptrImage == 0 )
		return false;

	DWORD dw_flags = 0;
	ptrImage->GetImageFlags( &dw_flags );
	if( dw_flags & ifVirtual )
		return false;

	IColorConvertor5Ptr ptrCC;
	{
		IUnknown* punkCC = 0;
		ptrImage->GetColorConvertor( &punkCC );
		ptrCC = punkCC;
		if( punkCC )
			punkCC->Release();
	}

	if( ptrCC == 0 )
		return false;

	int npanes = ::GetImagePaneCount( ptrImage );
	if( npanes < 1 )
		return false;

	int cx, cy;

	cx = cy = 0;

	ptrImage->GetSize( &cx, &cy );



	color		colorsBacks[10];
	COLORREF	cr = ::GetValueColor( GetAppUnknown(), "\\Editor", "Back", RGB( 0, 0, 0 ) );

	{
		byte	dib[3];
		dib[0] = (byte)GetBValue(cr); 
		dib[1] = (byte)GetGValue(cr);
		dib[2] = (byte)GetRValue(cr);

		color	*ppcolor[10];
		for( int i = 0; i < 10; i++ )
			ppcolor[i] = &colorsBacks[i];

		ptrCC->ConvertDIBToImageEx( dib, ppcolor, 1, true, npanes );
	}

	for( int npane=0;npane<npanes;npane++ )
	{
		color	colorBack = colorsBacks[npane];

		for( int y=0;y<cy;y++ )
		{			
			color* pcolor = 0;
			BYTE*  pmask = 0;

			ptrImage->GetRow( y, npane, &pcolor );
			ptrImage->GetRowMask( y, &pmask );

			for( int x=0;x<cx;x++ )
			{
				if( pmask[x] == 0xFF )
					pcolor[x] = colorBack;
			}			
		}
	}

	return true;
}

bool CActionShowCalibr::Invoke()
{
	if (!m_punkTarget)
		return false;

	IViewPtr view(m_punkTarget);
	if(view==0) return false;

	INamedPropBagPtr bag(m_punkTarget);
	if(bag==0) return false;

	long lShow = GetArgumentInt("Show");
	bool bSetToShellData = GetArgumentInt("SetToShellData")!=0;

	if(lShow==-1)
	{
		long lShowOld=::_IsCalibrVisible(m_punkTarget);
		lShow = lShowOld ? 0 : 1;
	}
	bag->SetProperty(_bstr_t("ShowCalibr"),_variant_t(lShow));
	if(bSetToShellData)
	{
		::SetValue( ::GetAppUnknown(), "\\ImageView", "ShowMarker", long(0) );
	}

	CWnd * pwnd = GetWindowFromUnknown(view);
	if( pwnd ) pwnd->InvalidateRect(0);

	return true;
}

bool CActionShowCalibr::IsChecked()
{
	return _IsCalibrVisible(m_punkTarget);
}

bool CActionShowCalibr::IsAvaible()
{
	if(!CheckInterface(m_punkTarget,IID_IImageView)) return false;
	return __super::IsAvaible();
}

//////////////////////////////////////////////////////////////////////
//CActionCipher implementation
CActionCipher::CActionCipher()
{
}

CActionCipher::~CActionCipher()
{
}

bool CActionCipher::InvokeFilter()
{
	IImage3Ptr	Src( GetDataArgument() );
	IImage3Ptr	Dst( GetDataResult() );
	if( Src==0 || Dst==0 ) return false;

	int nx = GetArgumentInt( _T("NX") );
	int ny = GetArgumentInt( _T("NY") );
	int nCipher = GetArgumentInt( _T("Cipher") );

	{	// create result image
		ICompatibleObjectPtr ptrCO( Dst );
		if( ptrCO == NULL ) return false;
		if( S_OK != ptrCO->CreateCompatibleObject( Src, NULL, 0 ) ) return false;
	}

	int nPaneCount = ::GetImagePaneCount( Src );
	int	cx=0, cy=0;
	Src->GetSize( &cx, &cy );
	int wx=cx/nx, wy=cy/ny; // размеры квадратиков для перемешивания

	StartNotification( cy, nPaneCount );
	smart_alloc(srcRows, color *, cy);
	smart_alloc(dstRows, color *, cy);
	for(int nPane=0;nPane<nPaneCount;nPane++ )
	{
		for(int y=0; y<cy; y++)
		{
			Src->GetRow(y, nPane, &srcRows[y]);
			Dst->GetRow(y, nPane, &dstRows[y]);
		}

		for(int iy=0; iy<ny; iy++)
		{
			for(int ix=0; ix<nx; ix++)
			{
				int i = iy*nx+ix;
				int i1 = (i*nCipher)%(nx*ny);
				int ix1=i1%nx, iy1=i1/nx;
				int x0src=ix*wx, y0src=iy*wy;
				int x0dst=ix1*wx, y0dst=iy1*wy;
				for(int dy=0; dy<wy; dy++)
				{
					for(int dx=0; dx<wx; dx++)
					{
						dstRows[y0dst+dy][x0dst+dx] = srcRows[y0src+dy][x0src+dx];
					}
				}
			}
			Notify( iy*wy );
		}

		for(int y=ny*wy; y<cy; y++)
		{
			for(int x=0; x<cx; x++)
			{
				dstRows[y][x]=srcRows[y][x];
			}
		}

		for(int y=0; y<cy; y++)
		{
			for(int x=nx*wx; x<cx; x++)
			{
				dstRows[y][x]=srcRows[y][x];
			}
		}
		NextNotificationStage( );
	}
	FinishNotification();

	return true;
}

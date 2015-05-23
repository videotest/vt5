#include "stdafx.h"
#include "DBaseDoc.h"
#include "DBactions.h"
#include "constant.h"
#include "ArchiveEx.h"
#include "GridOptionsDlg.h"
#include "DBFindDlg.h"

#include "DBaseInfoDlg.h"
#include "ChoiceActiveQuery.h"
#include "ExecuteDlg.h"
#include "Generate.h"
#include "FilterOrganizerDlg.h"

#include "FieldProp.h"

#include "utils.h"
#include "StreamEx.h"


#include <afxpriv2.h>

#include "DBStructDlg.h"

#include "Wizards.h"

#include "RecordInfoDlg.h"

#include "zlib.h"

//[ag]1. dyncreate

IMPLEMENT_DYNCREATE(CActionCloseQuery, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionOpenQuery, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionFilterOrganizer, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionSaveFilter, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionApplyFilter, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionCreateFilter, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionChooseFilter, CCmdTargetEx);
//IMPLEMENT_DYNCREATE(CActionDBGenerate, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionDBExecuteSQL, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionDBGenerateReportByAXForm, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionDBaseInfo, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionGalleryOptions, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionRequery, CCmdTargetEx);
//IMPLEMENT_DYNCREATE(CActionDBGridOptions, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionShowDBGrid, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionDBaseFindNext, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionDBaseFind, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionDBaseRestore, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionDBaseBackUp, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionShowGallery, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionShowBlankPreview, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionShowBlankDesign, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionChoiseQuery, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionCancel, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionUpdate, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionEditRecord, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionInsertRecord, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionDeleteRecord, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionMovePrev, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionMoveNext, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionMoveLast, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionMoveFirst, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionQueryWizard, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionTableWizard, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionLoadDatabase, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionAdvancedConnection, CCmdTargetEx);

IMPLEMENT_DYNCREATE(CActionAddSortFieldAsc, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionAddSortFieldDesc, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionRemoveSortFields, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionRecordInfo, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionFieldProperties, CCmdTargetEx);

IMPLEMENT_DYNCREATE(CActionDeleteRecords, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionUpdateRecordsetInfo, CCmdTargetEx);

IMPLEMENT_DYNCREATE(CActionEmptyDBField, CCmdTargetEx);

//[ag]4. olecreate release
// {E88DC2A0-2570-4795-B125-74D1F43255B2}
GUARD_IMPLEMENT_OLECREATE(CActionCloseQuery, "DBaseDoc.CloseQuery",
0xe88dc2a0, 0x2570, 0x4795, 0xb1, 0x25, 0x74, 0xd1, 0xf4, 0x32, 0x55, 0xb2);
// {DCB47802-CA06-4374-AB51-E76CB97F0758}
GUARD_IMPLEMENT_OLECREATE(CActionOpenQuery, "DBaseDoc.OpenQuery",
0xdcb47802, 0xca06, 0x4374, 0xab, 0x51, 0xe7, 0x6c, 0xb9, 0x7f, 0x7, 0x58);
// {F9E074FB-B93A-4232-A976-2E41504C9159}
GUARD_IMPLEMENT_OLECREATE(CActionFilterOrganizer, "DBaseDoc.FilterOrganizer",
0xf9e074fb, 0xb93a, 0x4232, 0xa9, 0x76, 0x2e, 0x41, 0x50, 0x4c, 0x91, 0x59);
// {AB732080-2864-47a5-8BDE-91506AEA5927}
GUARD_IMPLEMENT_OLECREATE(CActionSaveFilter, "DBaseDoc.SaveFilter",
0xab732080, 0x2864, 0x47a5, 0x8b, 0xde, 0x91, 0x50, 0x6a, 0xea, 0x59, 0x27);
// {0884F207-5C30-46ea-9E22-5A3CEA3D8997}
GUARD_IMPLEMENT_OLECREATE(CActionApplyFilter, "DBaseDoc.ApplyFilter",
0x884f207, 0x5c30, 0x46ea, 0x9e, 0x22, 0x5a, 0x3c, 0xea, 0x3d, 0x89, 0x97);
// {381C94BE-E8C1-4fe5-8390-A936EB82E640}
GUARD_IMPLEMENT_OLECREATE(CActionCreateFilter, "DBaseDoc.CreateFilter",
0x381c94be, 0xe8c1, 0x4fe5, 0x83, 0x90, 0xa9, 0x36, 0xeb, 0x82, 0xe6, 0x40);
// {964876AF-4F27-408f-B726-824BF8292914}
GUARD_IMPLEMENT_OLECREATE(CActionChooseFilter, "DBDoc.ChooseFilter",
0x964876af, 0x4f27, 0x408f, 0xb7, 0x26, 0x82, 0x4b, 0xf8, 0x29, 0x29, 0x14);
/*// {92C7DC94-5B64-4ca6-8077-91A29275E27B}
GUARD_IMPLEMENT_OLECREATE(CActionDBGenerate, "DBaseDoc.DBGenerate",
0x92c7dc94, 0x5b64, 0x4ca6, 0x80, 0x77, 0x91, 0xa2, 0x92, 0x75, 0xe2, 0x7b);
*/// {933F44D3-6721-11d4-AF0A-0000E8DF68C3}
GUARD_IMPLEMENT_OLECREATE(CActionDBExecuteSQL, "DBaseDoc.DBExecuteSQL",
0x933f44d3, 0x6721, 0x11d4, 0xaf, 0xa, 0x0, 0x0, 0xe8, 0xdf, 0x68, 0xc3);
// {E1FC98BF-2E3E-48a9-878C-D6F3076515DB}
GUARD_IMPLEMENT_OLECREATE(CActionDBGenerateReportByAXForm, "DBaseDoc.DBGenerateReportByAXForm",
0xe1fc98bf, 0x2e3e, 0x48a9, 0x87, 0x8c, 0xd6, 0xf3, 0x7, 0x65, 0x15, 0xdb);
// {DC2304CA-CBD6-4a18-9FA9-7BF9A78A3995}
GUARD_IMPLEMENT_OLECREATE(CActionDBaseInfo, "DBaseDoc.DBaseInfo",
0xdc2304ca, 0xcbd6, 0x4a18, 0x9f, 0xa9, 0x7b, 0xf9, 0xa7, 0x8a, 0x39, 0x95);
// {C0B38ED3-66B7-11d4-AF09-0000E8DF68C3}
GUARD_IMPLEMENT_OLECREATE(CActionGalleryOptions, "DBaseDoc.GalleryOptions",
0xc0b38ed3, 0x66b7, 0x11d4, 0xaf, 0x9, 0x0, 0x0, 0xe8, 0xdf, 0x68, 0xc3);
// {E67DF7D3-0E47-44fc-95EC-4F3C111034C4}
GUARD_IMPLEMENT_OLECREATE(CActionRequery, "DBaseDoc.Requery",
0xe67df7d3, 0xe47, 0x44fc, 0x95, 0xec, 0x4f, 0x3c, 0x11, 0x10, 0x34, 0xc4);
/*// {27FBBFA5-346C-40cf-AF62-733F96EEA5E2}
GUARD_IMPLEMENT_OLECREATE(CActionDBGridOptions, "DBaseDoc.DBGridOptions",
0x27fbbfa5, 0x346c, 0x40cf, 0xaf, 0x62, 0x73, 0x3f, 0x96, 0xee, 0xa5, 0xe2);
*/// {ACC9ED15-6585-4e33-AD90-B89DD4DEE084}
GUARD_IMPLEMENT_OLECREATE(CActionShowDBGrid, "DBaseDoc.ShowDBGrid",
0xacc9ed15, 0x6585, 0x4e33, 0xad, 0x90, 0xb8, 0x9d, 0xd4, 0xde, 0xe0, 0x84);
// {C27FEDC0-219A-48b6-9DCF-36B8D85DBE67}
GUARD_IMPLEMENT_OLECREATE(CActionDBaseFindNext, "DBaseDoc.DBaseFindNext",
0xc27fedc0, 0x219a, 0x48b6, 0x9d, 0xcf, 0x36, 0xb8, 0xd8, 0x5d, 0xbe, 0x67);
// {3C7FA488-C502-40b6-86BD-BEF68AAD79E4}
GUARD_IMPLEMENT_OLECREATE(CActionDBaseFind, "DBaseDoc.DBaseFind",
0x3c7fa488, 0xc502, 0x40b6, 0x86, 0xbd, 0xbe, 0xf6, 0x8a, 0xad, 0x79, 0xe4);
// {13B0CCFE-EEE6-4927-88AB-76A121E257ED}
GUARD_IMPLEMENT_OLECREATE(CActionDBaseRestore, "DBaseDoc.Restore",
0x13b0ccfe, 0xeee6, 0x4927, 0x88, 0xab, 0x76, 0xa1, 0x21, 0xe2, 0x57, 0xed);
// {43432ADB-38C2-42fb-8BA2-A4FD7D085285}
GUARD_IMPLEMENT_OLECREATE(CActionDBaseBackUp, "DBaseDoc.BackUp",
0x43432adb, 0x38c2, 0x42fb, 0x8b, 0xa2, 0xa4, 0xfd, 0x7d, 0x8, 0x52, 0x85);
/*// {671F04E9-736E-43df-9DA4-4B77D46816A2}
GUARD_IMPLEMENT_OLECREATE(CActionFilterMode, "DBaseDoc.FilterMode",
0x671f04e9, 0x736e, 0x43df, 0x9d, 0xa4, 0x4b, 0x77, 0xd4, 0x68, 0x16, 0xa2);
*/
// {06BF969B-3873-4f29-9758-CC4E0C74DB6E}
GUARD_IMPLEMENT_OLECREATE(CActionShowGallery, "DBaseDoc.ShowGallery",
0x6bf969b, 0x3873, 0x4f29, 0x97, 0x58, 0xcc, 0x4e, 0xc, 0x74, 0xdb, 0x6e);
// {FEAD40CE-0B14-4a31-8C13-CCB3476AA73A}
GUARD_IMPLEMENT_OLECREATE(CActionShowBlankPreview, "DBaseDoc.ShowBlankViewPreview",
0xfead40ce, 0xb14, 0x4a31, 0x8c, 0x13, 0xcc, 0xb3, 0x47, 0x6a, 0xa7, 0x3a);
// {C1366A75-2A27-49e7-A982-F08E4EEEC6D0}
GUARD_IMPLEMENT_OLECREATE(CActionShowBlankDesign, "DBaseDoc.ShowBlankView",
0xc1366a75, 0x2a27, 0x49e7, 0xa9, 0x82, 0xf0, 0x8e, 0x4e, 0xee, 0xc6, 0xd0);
// {016AC25C-462D-4d77-832D-5679199D9CC2}
GUARD_IMPLEMENT_OLECREATE(CActionChoiseQuery, "DBaseDoc.ChoiseQuery",
0x16ac25c, 0x462d, 0x4d77, 0x83, 0x2d, 0x56, 0x79, 0x19, 0x9d, 0x9c, 0xc2);

// {9FE9185A-1524-4c93-ABD4-952CB81473BC}
GUARD_IMPLEMENT_OLECREATE(CActionCancel, "DBaseDoc.Cancel",
0x9fe9185a, 0x1524, 0x4c93, 0xab, 0xd4, 0x95, 0x2c, 0xb8, 0x14, 0x73, 0xbc);
// {FC6FAAAC-98EF-46e0-99E1-62409E2992F7}
GUARD_IMPLEMENT_OLECREATE(CActionUpdate, "DBaseDoc.UpdateRecord",
0xfc6faaac, 0x98ef, 0x46e0, 0x99, 0xe1, 0x62, 0x40, 0x9e, 0x29, 0x92, 0xf7);
// {F46A1F02-53E9-4014-8FCF-16AA199497CF}
GUARD_IMPLEMENT_OLECREATE(CActionEditRecord, "DBaseDoc.EditRecord",
0xf46a1f02, 0x53e9, 0x4014, 0x8f, 0xcf, 0x16, 0xaa, 0x19, 0x94, 0x97, 0xcf);
// {2B355008-B689-4680-845B-EF442BBFDC36}

GUARD_IMPLEMENT_OLECREATE(CActionInsertRecord, "DBaseDoc.InsertRecord",
0x2b355008, 0xb689, 0x4680, 0x84, 0x5b, 0xef, 0x44, 0x2b, 0xbf, 0xdc, 0x36);
// {018C7CFC-CEAF-494c-ADFF-3103C402A1A2}
GUARD_IMPLEMENT_OLECREATE(CActionDeleteRecord, "DBaseDoc.DeleteRecord",
0x18c7cfc, 0xceaf, 0x494c, 0xad, 0xff, 0x31, 0x3, 0xc4, 0x2, 0xa1, 0xa2);
// {D74BD115-5526-448d-969E-82F282F69C38}
GUARD_IMPLEMENT_OLECREATE(CActionMovePrev, "DBaseDoc.MovePrev",
0xd74bd115, 0x5526, 0x448d, 0x96, 0x9e, 0x82, 0xf2, 0x82, 0xf6, 0x9c, 0x38);
// {53061489-EE0E-4905-ADB7-3471F711554D}
GUARD_IMPLEMENT_OLECREATE(CActionMoveNext, "DBaseDoc.MoveNext",
0x53061489, 0xee0e, 0x4905, 0xad, 0xb7, 0x34, 0x71, 0xf7, 0x11, 0x55, 0x4d);
// {D8DFD75C-A2D4-4123-963C-B0610D4FEB27}
GUARD_IMPLEMENT_OLECREATE(CActionMoveLast, "DBaseDoc.MoveLast",
0xd8dfd75c, 0xa2d4, 0x4123, 0x96, 0x3c, 0xb0, 0x61, 0xd, 0x4f, 0xeb, 0x27);
// {48E79A05-614B-4590-BBFD-E9D84A154033}
GUARD_IMPLEMENT_OLECREATE(CActionMoveFirst, "DBaseDoc.MoveFirst",
0x48e79a05, 0x614b, 0x4590, 0xbb, 0xfd, 0xe9, 0xd8, 0x4a, 0x15, 0x40, 0x33);
GUARD_IMPLEMENT_OLECREATE(CActionQueryWizard, "DBaseDoc.QueryWizard",
0x63841421, 0x208b, 0x44cd, 0xa8, 0x92, 0xea, 0x1c, 0xf, 0xa0, 0x49, 0x13);
// {8AFF6324-9FEC-47e4-B004-AC1918414F35}
GUARD_IMPLEMENT_OLECREATE(CActionTableWizard, "DBaseDoc.TableWizard",
0x8aff6324, 0x9fec, 0x47e4, 0xb0, 0x4, 0xac, 0x19, 0x18, 0x41, 0x4f, 0x35);

// {4CAA4C5B-B635-4651-A172-AFB1D68EBAC6}
GUARD_IMPLEMENT_OLECREATE(CActionLoadDatabase, "DBaseDoc.NewDatabase",
0x4caa4c5b, 0xb635, 0x4651, 0xa1, 0x72, 0xaf, 0xb1, 0xd6, 0x8e, 0xba, 0xc6);


// {041B3B27-FC14-4205-BBA8-8606FC838373}
GUARD_IMPLEMENT_OLECREATE(CActionAdvancedConnection, "DBaseDoc.AddvancedConnection",
0x41b3b27, 0xfc14, 0x4205, 0xbb, 0xa8, 0x86, 0x6, 0xfc, 0x83, 0x83, 0x73);



// {8171D564-4CF7-4c3c-B6A4-40052B12151C}
GUARD_IMPLEMENT_OLECREATE(CActionAddSortFieldAsc, "DBaseDoc.AddSortFieldAsc", 
0x8171d564, 0x4cf7, 0x4c3c, 0xb6, 0xa4, 0x40, 0x5, 0x2b, 0x12, 0x15, 0x1c);


// {AF931FE8-B2FB-4683-9C36-5BFB2C836689}
GUARD_IMPLEMENT_OLECREATE(CActionAddSortFieldDesc, "DBaseDoc.AddSortFieldDesc", 
0xaf931fe8, 0xb2fb, 0x4683, 0x9c, 0x36, 0x5b, 0xfb, 0x2c, 0x83, 0x66, 0x89);


// {1432AA6E-AEF8-46ec-A3A3-02BAA6F157B9}
GUARD_IMPLEMENT_OLECREATE(CActionRemoveSortFields, "DBaseDoc.RemoveSortFields", 
0x1432aa6e, 0xaef8, 0x46ec, 0xa3, 0xa3, 0x2, 0xba, 0xa6, 0xf1, 0x57, 0xb9);

// {EDB41AD6-569B-47d7-A6B4-F3D4DF250411}
GUARD_IMPLEMENT_OLECREATE(CActionRecordInfo, "DBaseDoc.RecordInfo", 
0xedb41ad6, 0x569b, 0x47d7, 0xa6, 0xb4, 0xf3, 0xd4, 0xdf, 0x25, 0x4, 0x11);


// {8F0312DA-2F37-4293-A504-53E37F4A6EFB}
GUARD_IMPLEMENT_OLECREATE(CActionFieldProperties, "DBaseDoc.FieldProperties",
0x8f0312da, 0x2f37, 0x4293, 0xa5, 0x4, 0x53, 0xe3, 0x7f, 0x4a, 0x6e, 0xfb);

// {AE6A24E6-602C-4c5d-9A90-EB3267F0C384}
GUARD_IMPLEMENT_OLECREATE(CActionDeleteRecords, "DBaseDoc.DeleteRecords", 
0xae6a24e6, 0x602c, 0x4c5d, 0x9a, 0x90, 0xeb, 0x32, 0x67, 0xf0, 0xc3, 0x84);

// {216AC3D8-F0E8-4611-8E41-B83E7F54C78B}
GUARD_IMPLEMENT_OLECREATE(CActionUpdateRecordsetInfo, "DBaseDoc.UpdateRecordsetInfo", 
0x216ac3d8, 0xf0e8, 0x4611, 0x8e, 0x41, 0xb8, 0x3e, 0x7f, 0x54, 0xc7, 0x8b);


// {5470051E-6754-47fa-BCC9-6489DEA0D4C9}
GUARD_IMPLEMENT_OLECREATE(CActionEmptyDBField, "DBaseDoc.EmptyDBField", 
0x5470051e, 0x6754, 0x47fa, 0xbc, 0xc9, 0x64, 0x89, 0xde, 0xa0, 0xd4, 0xc9);


//[ag]5. guidinfo release

// {32915261-3FC3-426a-A3AF-EFDA24D85BD2}
static const GUID guidCloseQuery =
{ 0x32915261, 0x3fc3, 0x426a, { 0xa3, 0xaf, 0xef, 0xda, 0x24, 0xd8, 0x5b, 0xd2 } };
// {D82C92AF-8E16-4590-A417-A9DA27EA917E}
static const GUID guidOpenQuery =
{ 0xd82c92af, 0x8e16, 0x4590, { 0xa4, 0x17, 0xa9, 0xda, 0x27, 0xea, 0x91, 0x7e } };
// {0B2F1508-DA0D-43f3-A510-A771058E035B}
static const GUID guidDBaseImport =
{ 0xb2f1508, 0xda0d, 0x43f3, { 0xa5, 0x10, 0xa7, 0x71, 0x5, 0x8e, 0x3, 0x5b } };
// {20B70481-F93C-4937-862C-6209DFA03F1D}
static const GUID guidFilterOrganizer =
{ 0x20b70481, 0xf93c, 0x4937, { 0x86, 0x2c, 0x62, 0x9, 0xdf, 0xa0, 0x3f, 0x1d } };
// {802CB1EC-7493-4ab0-89DE-35498A7427BB}
static const GUID guidSaveFilter =
{ 0x802cb1ec, 0x7493, 0x4ab0, { 0x89, 0xde, 0x35, 0x49, 0x8a, 0x74, 0x27, 0xbb } };
// {D72799B9-E8E0-482b-99A6-52934F64DF2C}
static const GUID guidApplyFilter =
{ 0xd72799b9, 0xe8e0, 0x482b, { 0x99, 0xa6, 0x52, 0x93, 0x4f, 0x64, 0xdf, 0x2c } };
// {A66EE25F-D878-4c47-ABC7-760D04024DD8}
static const GUID guidCreateFilter =
{ 0xa66ee25f, 0xd878, 0x4c47, { 0xab, 0xc7, 0x76, 0xd, 0x4, 0x2, 0x4d, 0xd8 } };
// {A19BA5A7-3E2C-4a28-A99C-EDC946002C08}
static const GUID guidChooseFilter =
{ 0xa19ba5a7, 0x3e2c, 0x4a28, { 0xa9, 0x9c, 0xed, 0xc9, 0x46, 0x0, 0x2c, 0x8 } };
// {88399421-274A-4d29-9C24-C303C05999D9}
static const GUID guidDBGenerate =
{ 0x88399421, 0x274a, 0x4d29, { 0x9c, 0x24, 0xc3, 0x3, 0xc0, 0x59, 0x99, 0xd9 } };
// {933F44D1-6721-11d4-AF0A-0000E8DF68C3}
static const GUID guidDBExecuteSQL =
{ 0x933f44d1, 0x6721, 0x11d4, { 0xaf, 0xa, 0x0, 0x0, 0xe8, 0xdf, 0x68, 0xc3 } };
// {8EB2FD43-68A2-42e7-9389-6934D832844D}
static const GUID guidDBGenerateReportByAXForm =
{ 0x8eb2fd43, 0x68a2, 0x42e7, { 0x93, 0x89, 0x69, 0x34, 0xd8, 0x32, 0x84, 0x4d } };
// {CC82DCFB-FC22-4ff4-90EC-920A6E182EDF}
static const GUID guidDBaseInfo =
{ 0xcc82dcfb, 0xfc22, 0x4ff4, { 0x90, 0xec, 0x92, 0xa, 0x6e, 0x18, 0x2e, 0xdf } };
// {C0B38ED1-66B7-11d4-AF09-0000E8DF68C3}
static const GUID guidGalleryOptions =
{ 0xc0b38ed1, 0x66b7, 0x11d4, { 0xaf, 0x9, 0x0, 0x0, 0xe8, 0xdf, 0x68, 0xc3 } };
// {9DB99471-BFC9-4eac-B97F-B58D559F2F3C}
static const GUID guidRequery =
{ 0x9db99471, 0xbfc9, 0x4eac, { 0xb9, 0x7f, 0xb5, 0x8d, 0x55, 0x9f, 0x2f, 0x3c } };
// {769E74E6-3408-4bc8-8D58-B3D2DCE13F40}
static const GUID guidDBGridOptions =
{ 0x769e74e6, 0x3408, 0x4bc8, { 0x8d, 0x58, 0xb3, 0xd2, 0xdc, 0xe1, 0x3f, 0x40 } };
// {FFED2516-BB27-4dde-B283-1B950A5B23E4}
static const GUID guidShowDBGrid =
{ 0xffed2516, 0xbb27, 0x4dde, { 0xb2, 0x83, 0x1b, 0x95, 0xa, 0x5b, 0x23, 0xe4 } };
// {89C422BA-8718-4038-8DFB-C1634A0642D2}
static const GUID guidDBaseFindNext =
{ 0x89c422ba, 0x8718, 0x4038, { 0x8d, 0xfb, 0xc1, 0x63, 0x4a, 0x6, 0x42, 0xd2 } };
// {3259B5A2-0BD3-4ff8-A226-D723AC8BF5EA}
static const GUID guidDBaseFind =
{ 0xffb514b0, 0x9e89, 0x4853, { 0x86, 0xb2, 0x9f, 0x7d, 0xb8, 0x7d, 0x98, 0xf9 } };
// {8C5579A2-72DD-4c3f-B244-628B345C7A86}
static const GUID guidRestore =
{ 0x8c5579a2, 0x72dd, 0x4c3f, { 0xb2, 0x44, 0x62, 0x8b, 0x34, 0x5c, 0x7a, 0x86 } };
// {9803E9F1-00A5-4fd9-914B-FE6DFD4CD0A4}
static const GUID guidBackUp =
{ 0x9803e9f1, 0xa5, 0x4fd9, { 0x91, 0x4b, 0xfe, 0x6d, 0xfd, 0x4c, 0xd0, 0xa4 } };
// {8F908CA3-5777-43a0-A912-6B1A98BAAA8C}
static const GUID guidFilterMode =
{ 0x8f908ca3, 0x5777, 0x43a0, { 0xa9, 0x12, 0x6b, 0x1a, 0x98, 0xba, 0xaa, 0x8c } };
// {2EDE1D68-CDF8-405a-80FC-2D2FC8437D91}
static const GUID guidShowGallery =
{ 0x2ede1d68, 0xcdf8, 0x405a, { 0x80, 0xfc, 0x2d, 0x2f, 0xc8, 0x43, 0x7d, 0x91 } };
// {1F99BB46-7526-4128-A118-5A7DA3D7464F}
static const GUID guidShowBlankViewPreview =
{ 0x1f99bb46, 0x7526, 0x4128, { 0xa1, 0x18, 0x5a, 0x7d, 0xa3, 0xd7, 0x46, 0x4f } };
// {024E57A3-1F93-4295-ABB1-0A31340D982C}
static const GUID guidShowBlankView =
{ 0x24e57a3, 0x1f93, 0x4295, { 0xab, 0xb1, 0xa, 0x31, 0x34, 0xd, 0x98, 0x2c } };
// {46878872-73F6-4cee-8EDE-21B51A8735B5}
static const GUID guidChoiseQuery =
{ 0x46878872, 0x73f6, 0x4cee, { 0x8e, 0xde, 0x21, 0xb5, 0x1a, 0x87, 0x35, 0xb5 } };
// {66F706C7-0567-4f65-AEF3-FA313795364F}
static const GUID guidCancel =
{ 0x66f706c7, 0x567, 0x4f65, { 0xae, 0xf3, 0xfa, 0x31, 0x37, 0x95, 0x36, 0x4f } };
// {9B24BE20-D6ED-499a-AFF0-CF3AA78ABFBE}
static const GUID guidUpdateRecord =
{ 0x9b24be20, 0xd6ed, 0x499a, { 0xaf, 0xf0, 0xcf, 0x3a, 0xa7, 0x8a, 0xbf, 0xbe } };
// {A844533C-FF2B-414a-B4FF-E36E627ABDF7}
static const GUID guidEditRecord =
{ 0xa844533c, 0xff2b, 0x414a, { 0xb4, 0xff, 0xe3, 0x6e, 0x62, 0x7a, 0xbd, 0xf7 } };
// {6C68F022-1974-4f4a-BA27-6A51BA71691C}
static const GUID guidInsertRecord =
{ 0x6c68f022, 0x1974, 0x4f4a, { 0xba, 0x27, 0x6a, 0x51, 0xba, 0x71, 0x69, 0x1c } };
// {2A564890-9F4F-45c2-ABBB-B8317C056CFA}
static const GUID guidDeleteRecord =
{ 0x2a564890, 0x9f4f, 0x45c2, { 0xab, 0xbb, 0xb8, 0x31, 0x7c, 0x5, 0x6c, 0xfa } };
// {3A4AD800-49D5-46b9-91EA-DB267EED7DE8}
static const GUID guidMovePrev =
{ 0x3a4ad800, 0x49d5, 0x46b9, { 0x91, 0xea, 0xdb, 0x26, 0x7e, 0xed, 0x7d, 0xe8 } };
// {30187459-0352-4bf3-A658-885325DD4DA3}
static const GUID guidMoveNext =
{ 0x30187459, 0x352, 0x4bf3, { 0xa6, 0x58, 0x88, 0x53, 0x25, 0xdd, 0x4d, 0xa3 } };
// {049D9C05-4143-4b8e-A6A5-9B11397BC959}
static const GUID guidMoveLast =
{ 0x49d9c05, 0x4143, 0x4b8e, { 0xa6, 0xa5, 0x9b, 0x11, 0x39, 0x7b, 0xc9, 0x59 } };
// {DF69292E-5297-4f9a-80BE-FEBCF5D25E73}
static const GUID guidMoveFirst =
{ 0xdf69292e, 0x5297, 0x4f9a, { 0x80, 0xbe, 0xfe, 0xbc, 0xf5, 0xd2, 0x5e, 0x73 } };
// {010B66D1-37D7-4665-8B5E-4AA0CBEACE94}
static const GUID guidQuery =
{ 0x10b66d1, 0x37d7, 0x4665, { 0x8b, 0x5e, 0x4a, 0xa0, 0xcb, 0xea, 0xce, 0x94 } };
// {08E0F341-A254-45aa-A7A2-0336FE9615E0}
static const GUID guidNewTable =
{ 0x8e0f341, 0xa254, 0x45aa, { 0xa7, 0xa2, 0x3, 0x36, 0xfe, 0x96, 0x15, 0xe0 } };
// {C2964503-56AA-4c4c-9D14-368EA46EB93B}
static const GUID guidNewDatabase =
{ 0xc2964503, 0x56aa, 0x4c4c, { 0x9d, 0x14, 0x36, 0x8e, 0xa4, 0x6e, 0xb9, 0x3b } };
// {E491D738-0C13-4787-B0A5-8AFE0672A8AA}
static const GUID guidAdvancedConnection = 
{ 0xe491d738, 0xc13, 0x4787, { 0xb0, 0xa5, 0x8a, 0xfe, 0x6, 0x72, 0xa8, 0xaa } };


// {0B08DA11-1AFC-47e7-9028-138BBB1359A8}
static const GUID guidAddSortFieldAsc = 
{ 0xb08da11, 0x1afc, 0x47e7, { 0x90, 0x28, 0x13, 0x8b, 0xbb, 0x13, 0x59, 0xa8 } };

// {F93E07F2-6A92-4b61-8187-7467711E5D3B}
static const GUID guidAddSortFieldDesc = 
{ 0xf93e07f2, 0x6a92, 0x4b61, { 0x81, 0x87, 0x74, 0x67, 0x71, 0x1e, 0x5d, 0x3b } };

// {7FC4DFC5-0152-430d-B7BD-ABC6C43DDE26}
static const GUID guidRemoveSortFields = 
{ 0x7fc4dfc5, 0x152, 0x430d, { 0xb7, 0xbd, 0xab, 0xc6, 0xc4, 0x3d, 0xde, 0x26 } };


// {C27E4509-616F-46c0-9242-FA03A67CFBDA}
static const GUID guidRecordInfo = 
{ 0xc27e4509, 0x616f, 0x46c0, { 0x92, 0x42, 0xfa, 0x3, 0xa6, 0x7c, 0xfb, 0xda } };

// {0605F77F-ABD5-48ce-867B-CB18EEA823D2}
static const GUID guidFieldProperties = 
{ 0x605f77f, 0xabd5, 0x48ce, { 0x86, 0x7b, 0xcb, 0x18, 0xee, 0xa8, 0x23, 0xd2 } };


// {3CB272D1-20AB-45f9-903B-5138871930B5}
static const GUID guidDeleteRecords = 
{ 0x3cb272d1, 0x20ab, 0x45f9, { 0x90, 0x3b, 0x51, 0x38, 0x87, 0x19, 0x30, 0xb5 } };

// {C661BC75-4754-4387-8E27-37BD5BE6E1D2}
static const GUID guidUpdateRecordsetInfo = 
{ 0xc661bc75, 0x4754, 0x4387, { 0x8e, 0x27, 0x37, 0xbd, 0x5b, 0xe6, 0xe1, 0xd2 } };

// {BC938DEF-4A61-4f30-916B-90C70D0823A9}
static const GUID guidEmptyDBField = 
{ 0xbc938def, 0x4a61, 0x4f30, { 0x91, 0x6b, 0x90, 0xc7, 0xd, 0x8, 0x23, 0xa9 } };


//[ag]6. action info
ACTION_INFO_FULL(CActionQueryWizard, IDS_QUERY_WIZARD, -1, -1, guidQuery);
ACTION_INFO_FULL(CActionGalleryOptions, IDS_GALLERY_OPTIONS, -1, -1, guidGalleryOptions);
ACTION_INFO_FULL(CActionCloseQuery, IDS_QUERY_CLOSE, -1, -1, guidCloseQuery);
ACTION_INFO_FULL(CActionOpenQuery, IDS_QUERY_OPEN, -1, -1, guidOpenQuery);
ACTION_INFO_FULL(CActionFilterOrganizer, IDS_FILTER_ORGANIZER, -1, -1, guidFilterOrganizer);
ACTION_INFO_FULL(CActionSaveFilter, IDS_SAVE_FILTER, -1, -1, guidSaveFilter);
ACTION_INFO_FULL(CActionApplyFilter, IDS_APPLY_FILTER, -1, -1, guidApplyFilter);
ACTION_INFO_FULL(CActionCreateFilter, IDS_CREATE_FILTER, -1, -1, guidCreateFilter);
ACTION_INFO_FULL(CActionChooseFilter, IDS_CHOOSE_FILTER, -1, -1, guidChooseFilter);
//ACTION_INFO_FULL(CActionDBGenerate, IDS_ACTION_DB_GENERATE, -1, -1, guidDBGenerate);
ACTION_INFO_FULL(CActionDBExecuteSQL, IDS_EXECUTE_SQL, -1, -1, guidDBExecuteSQL);
ACTION_INFO_FULL(CActionDBGenerateReportByAXForm, IDS_GENERATE_REPORT_BY_AXFORM, -1, -1, guidDBGenerateReportByAXForm);
ACTION_INFO_FULL(CActionDBaseInfo, IDS_DBASE_INFO, -1, -1, guidDBaseInfo);
ACTION_INFO_FULL(CActionRequery, IDS_REQUERY, -1, -1, guidRequery);
//ACTION_INFO_FULL(CActionDBGridOptions, IDS_DBGRID_OPTIONS, -1, -1, guidDBGridOptions);
ACTION_INFO_FULL(CActionShowDBGrid, IDS_SHOW_DBGRID, -1, -1, guidShowDBGrid);
ACTION_INFO_FULL(CActionDBaseFindNext, IDS_DBASE_FINDTEXT_NEXT, -1, -1, guidDBaseFindNext);
ACTION_INFO_FULL(CActionDBaseFind, IDS_DBASE_FINDTEXT, -1, -1, guidDBaseFind);
ACTION_INFO_FULL(CActionDBaseRestore, IDS_DBASE_RESTORE, -1, -1, guidRestore);
ACTION_INFO_FULL(CActionDBaseBackUp, IDS_DBASE_BACKUP, -1, -1, guidBackUp);
ACTION_INFO_FULL(CActionShowGallery, IDS_SHOW_GALLERY, -1, -1, guidShowGallery);
ACTION_INFO_FULL(CActionShowBlankPreview, IDS_SHOW_BLANK_PREVIEW, -1, -1, guidShowBlankViewPreview);
ACTION_INFO_FULL(CActionShowBlankDesign, IDS_SHOW_BLANK_DESIGN, -1, -1, guidShowBlankView);
ACTION_INFO_FULL(CActionChoiseQuery, IDS_CHOICE_QUERY, -1, -1, guidChoiseQuery);
ACTION_INFO_FULL(CActionCancel, IDS_CANCEL, -1, -1, guidCancel);
ACTION_INFO_FULL(CActionUpdate, IDS_UPDATE, -1, -1, guidUpdateRecord);
ACTION_INFO_FULL(CActionEditRecord, IDS_EDIT, -1, -1, guidEditRecord);
ACTION_INFO_FULL(CActionInsertRecord, IDS_INSERT_RECORD, -1, -1, guidInsertRecord);
ACTION_INFO_FULL(CActionDeleteRecord, IDS_DELETE_RECORD, -1, -1, guidDeleteRecord);
ACTION_INFO_FULL(CActionMovePrev, IDS_MOVE_PREV, -1, -1, guidMovePrev);
ACTION_INFO_FULL(CActionMoveNext, IDS_MOVE_NEXT, -1, -1, guidMoveNext);
ACTION_INFO_FULL(CActionMoveLast, IDS_MOVE_LAST, -1, -1, guidMoveLast);
ACTION_INFO_FULL(CActionMoveFirst, IDS_MOVE_FIRST, -1, -1, guidMoveFirst);
ACTION_INFO_FULL(CActionTableWizard, IDS_TABLE_WIZARD, -1, -1, guidNewTable);
ACTION_INFO_FULL(CActionLoadDatabase, IDS_LOAD_DATABASE, -1, -1, guidNewDatabase);
ACTION_INFO_FULL(CActionAdvancedConnection, IDS_ADVANSED_CONNECTION, -1, -1, guidAdvancedConnection);

ACTION_INFO_FULL(CActionAddSortFieldAsc, IDS_ADD_SORT_FIELD_ASC, -1, -1, guidAddSortFieldAsc);
ACTION_INFO_FULL(CActionAddSortFieldDesc, IDS_ADD_SORT_FIELD_DESC, -1, -1, guidAddSortFieldDesc);
ACTION_INFO_FULL(CActionRemoveSortFields, IDS_REMOVE_ALL_SORT_FIELDS, -1, -1, guidRemoveSortFields);
ACTION_INFO_FULL(CActionRecordInfo, IDS_ACTION_RECORD_INFO, -1, -1, guidRecordInfo);
ACTION_INFO_FULL(CActionFieldProperties, IDS_FIELD_PROPERTY, -1, -1, guidFieldProperties);
ACTION_INFO_FULL(CActionDeleteRecords, IDS_DELETE_RECORDS, -1, -1, guidDeleteRecords);
ACTION_INFO_FULL(CActionUpdateRecordsetInfo, IDS_UPDATE_RECORDSET_INFO, -1, -1, guidUpdateRecordsetInfo);
ACTION_INFO_FULL(CActionEmptyDBField, IDS_EMPTY_DBFIELD, -1, -1, guidEmptyDBField);

//[ag]7. targets

ACTION_TARGET(CActionCloseQuery, "anydoc");
ACTION_TARGET(CActionOpenQuery, "anydoc");
ACTION_TARGET(CActionChooseFilter, "anydoc");
ACTION_TARGET(CActionFilterOrganizer, "anydoc");
ACTION_TARGET(CActionSaveFilter, "anydoc");
ACTION_TARGET(CActionApplyFilter, "anydoc");
ACTION_TARGET(CActionCreateFilter, szTargetViewSite );
//ACTION_TARGET(CActionDBGenerate, "anydoc");
ACTION_TARGET(CActionDBExecuteSQL, "anydoc");
ACTION_TARGET(CActionDBGenerateReportByAXForm, "anydoc");
ACTION_TARGET(CActionDBaseInfo, "anydoc");
ACTION_TARGET(CActionRequery, "anydoc");
//ACTION_TARGET(CActionDBGridOptions, szTargetViewSite);
ACTION_TARGET(CActionShowDBGrid, szTargetFrame);
ACTION_TARGET(CActionDBaseFindNext, "anydoc");
ACTION_TARGET(CActionDBaseFind, "anydoc");
ACTION_TARGET(CActionDBaseRestore, "anydoc");
ACTION_TARGET(CActionDBaseBackUp, "anydoc");
ACTION_TARGET(CActionShowGallery, szTargetFrame);
ACTION_TARGET(CActionShowBlankPreview, szTargetFrame);
ACTION_TARGET(CActionShowBlankDesign, szTargetFrame);
ACTION_TARGET(CActionChoiseQuery, "anydoc");
ACTION_TARGET(CActionCancel, "anydoc");
ACTION_TARGET(CActionUpdate, "anydoc");
ACTION_TARGET(CActionEditRecord, "anydoc");
ACTION_TARGET(CActionInsertRecord, "anydoc");
ACTION_TARGET(CActionDeleteRecord, "anydoc");
ACTION_TARGET(CActionMovePrev, "anydoc");
ACTION_TARGET(CActionMoveNext, "anydoc");
ACTION_TARGET(CActionMoveLast, "anydoc");
ACTION_TARGET(CActionMoveFirst, "anydoc");
ACTION_TARGET(CActionQueryWizard, "anydoc");
ACTION_TARGET(CActionGalleryOptions, "anydoc");
ACTION_TARGET(CActionTableWizard, "anydoc");
ACTION_TARGET(CActionLoadDatabase, "anydoc");
ACTION_TARGET(CActionAdvancedConnection, "anydoc");

ACTION_TARGET(CActionAddSortFieldAsc, szTargetViewSite);
ACTION_TARGET(CActionAddSortFieldDesc, szTargetViewSite);
ACTION_TARGET(CActionRemoveSortFields, "anydoc" );
ACTION_TARGET(CActionRecordInfo, "anydoc" );
ACTION_TARGET(CActionFieldProperties, "anydoc" );
ACTION_TARGET(CActionDeleteRecords, "anydoc" );
ACTION_TARGET(CActionUpdateRecordsetInfo, "anydoc" );

ACTION_TARGET(CActionEmptyDBField, "anydoc" );


ACTION_ARG_LIST(CActionDeleteRecords)
	ARG_INT("Question", 1 )
END_ACTION_ARG_LIST()



CDBaseFilterBase::CDBaseFilterBase()
{

}

CDBaseFilterBase::~CDBaseFilterBase()
{

}

sptrIQueryObject CDBaseFilterBase::GetActiveQueryFromViewTarget( )
{
	IViewPtr ptrView( m_punkTarget );
	if( ptrView == 0 )
		return 0;

	IUnknown* punkDoc = 0;
	ptrView->GetDocument( &punkDoc );

	if( punkDoc == 0 )
		return 0;

	sptrIDBaseDocument spDBDoc = punkDoc;
	punkDoc->Release();	punkDoc = 0;


	if( spDBDoc == NULL )
		return NULL;

	IUnknown* punkQuery = NULL;
	spDBDoc->GetActiveQuery( &punkQuery );
	if( punkQuery == NULL )
		return NULL;

	sptrIQueryObject sptrQuery( punkQuery );
	punkQuery->Release();

	if( sptrQuery == NULL )
		return NULL;

	return sptrQuery;

}

sptrIQueryObject CDBaseFilterBase::GetActiveQuery( IUnknown* punkDocument )
{
	sptrIDBaseDocument spDBDoc;
	if( punkDocument == NULL )
		spDBDoc = m_punkTarget;
	else
		spDBDoc = punkDocument;

	if( spDBDoc == NULL )
		return NULL;

	IUnknown* punkQuery = NULL;
	spDBDoc->GetActiveQuery( &punkQuery );
	if( punkQuery == NULL )
		return NULL;

	sptrIQueryObject sptrQuery( punkQuery );
	punkQuery->Release();

	if( sptrQuery == NULL )
		return NULL;

	return sptrQuery;
}



//[ag]9. implementation



//////////////////////////////////////////////////////////////////////
//CActionFilterOrganizer implementation
CActionFilterOrganizer::CActionFilterOrganizer()
{
}

CActionFilterOrganizer::~CActionFilterOrganizer()
{
}

bool CActionFilterOrganizer::Invoke()
{
	sptrIQueryObject spQuery = GetActiveQuery();
	if( spQuery == NULL )
		return false;

	IDBaseFilterProviderPtr ptrFP( m_punkTarget );
	if( ptrFP == NULL )
		return false;

	IDBaseFilterHolderPtr ptrFH( m_punkTarget );
	if( ptrFH == NULL )
		return false;

	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	::ExecuteAction( GetActionNameByResID( IDS_SAVE_FILTER ) );


	BSTR bstr = 0;
	ptrFH->GetActiveFilter( &bstr );

	CString strOldFilter = bstr;
	::SysFreeString( bstr );	bstr = 0;
	
	
	ptrFH->SetFilterInOrganizerMode( TRUE );

	CFilterOrganizerDlg dlg;

	dlg.m_strActiveFilter = strOldFilter;
	dlg.SetFilterHolder( ptrFH );
	if( dlg.DoModal() == IDOK )
		::SetModifiedFlagToDoc( m_punkTarget );

	ptrFH->SetFilterInOrganizerMode( FALSE );	


	if( !dlg.m_bSelectFilter )
	{
		BOOL bValid = FALSE;
		ptrFH->IsValidFilter( _bstr_t( (LPCSTR)strOldFilter ), FALSE, &bValid );

		if( !bValid )
		{
			CString strNone;
			strNone.LoadString( IDS_FILTER_NONE );
			ptrFH->SetActiveFilter( _bstr_t( (LPCSTR)strNone ) );
		}
	}
	else
	{
		ptrFH->SetActiveFilter( _bstr_t( (LPCSTR)dlg.m_strActiveFilter ) );
	}

	ptrFP->ReBuildFilterChooserList();

	return true;
}

//extended UI
bool CActionFilterOrganizer::IsAvaible()
{
	IDBaseFilterProviderPtr ptrFP( m_punkTarget );
	if( ptrFP == NULL )
		return false;

	return true;
}

bool CActionFilterOrganizer::IsChecked()
{
	return false;
}

//////////////////////////////////////////////////////////////////////
//CActionSaveFilter implementation
CActionSaveFilter::CActionSaveFilter()
{
}

CActionSaveFilter::~CActionSaveFilter()
{
}

bool CActionSaveFilter::Invoke()
{
	IDBaseFilterHolderPtr ptrFH( m_punkTarget );
	if( ptrFH == NULL )
		return false;

	/*
	sptrIQueryObject spQuery = GetActiveQuery();
	if( spQuery == NULL )
		return false;
		*/

	
	short nStatus;
	ptrFH->GetWorkingFilterStatus( &nStatus );
	if( !((WorkingFilterStatus)nStatus == wfsNew || 
		(WorkingFilterStatus)nStatus == wfsSelectedChange ) )
		return false;

	ptrFH->SaveWorkingFilter();
	

	IDBaseFilterProviderPtr ptrFP( m_punkTarget );
	if( ptrFP == NULL )
		return false;
		
	
	ptrFP->ReBuildFilterChooserList();

	return true;
}

//extended UI
bool CActionSaveFilter::IsAvaible()
{
	sptrIQueryObject spQuery = GetActiveQuery();
	if( spQuery == NULL )
		return false;

	IDBaseFilterHolderPtr ptrFH( m_punkTarget );
	if( ptrFH == NULL )
		return false;

	
	short nStatus;
	ptrFH->GetWorkingFilterStatus( &nStatus );
	if( (WorkingFilterStatus)nStatus == wfsNew || 
		(WorkingFilterStatus)nStatus == wfsSelectedChange )
		return true;
		


	return false;
}

bool CActionSaveFilter::IsChecked()
{
	return false;
}

//////////////////////////////////////////////////////////////////////
//CActionApplyFilter implementation
CActionApplyFilter::CActionApplyFilter()
{
}

CActionApplyFilter::~CActionApplyFilter()
{
}

bool CActionApplyFilter::Invoke()
{
	sptrIQueryObject spQuery = GetActiveQuery();
	if( spQuery == NULL )
		return false;


	IDBaseFilterProviderPtr ptrFP( m_punkTarget );
	if( ptrFP == NULL )
		return false;


	IDBaseFilterHolderPtr ptrFH( m_punkTarget );
	if( ptrFH == NULL )
		return false;	
	
	BOOL bApply;
	ptrFH->GetIsActiveFilterApply( &bApply );
	if( bApply )
	{
		ptrFH->SetIsActiveFilterApply( FALSE );
		AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
		::ExecuteAction( GetActionNameByResID( IDS_REQUERY ) );
		CString strNone;
		strNone.LoadString( IDS_FILTER_NONE );
		ptrFH->SetActiveFilter( _bstr_t( (LPCSTR)strNone ) );
		ptrFP->ReBuildFilterChooserList();
		return true;
	}

	BSTR bstrActiveFilter = 0;
	ptrFH->GetActiveFilter( &bstrActiveFilter );
	BOOL bValidFilter = TRUE;
	ptrFH->IsValidFilter( bstrActiveFilter, FALSE, &bValidFilter );

	if( bstrActiveFilter )
		::SysFreeString( bstrActiveFilter );

	if( !bValidFilter )
	{
		ptrFP->ReBuildFilterChooserList();
		return false;
	}

	ptrFH->SetIsActiveFilterApply( TRUE );	
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	::ExecuteAction( GetActionNameByResID( IDS_REQUERY ) );
	
	ptrFP->ReBuildFilterChooserList();
	

	return true;
}

//extended UI
bool CActionApplyFilter::IsAvaible()
{
	sptrIQueryObject spQuery = GetActiveQuery();
	if( spQuery == NULL )
		return false;

	IDBaseFilterHolderPtr ptrFH( m_punkTarget );
	if( ptrFH == NULL )
		return false;

	
	BSTR bstrActiveFilter = NULL;
	ptrFH->GetActiveFilter( &bstrActiveFilter );

	BOOL bApply;
	ptrFH->GetIsActiveFilterApply( &bApply );
	if( bApply )
	{
		return true;
	}

	BOOL bValidFilter = TRUE;
	ptrFH->IsValidFilter( bstrActiveFilter, FALSE, &bValidFilter );

	if( bstrActiveFilter )
		::SysFreeString( bstrActiveFilter );

	if( !bValidFilter )
		return false;
	

	return true;
}

bool CActionApplyFilter::IsChecked()
{
	sptrIQueryObject spQuery = GetActiveQuery();
	if( spQuery == NULL )
		return false;

	IDBaseFilterHolderPtr ptrFH( m_punkTarget );
	if( ptrFH == NULL )
		return false;

	
	
	BSTR bstrActiveFilter = NULL;
	ptrFH->GetActiveFilter( &bstrActiveFilter );

	BOOL bApply;
	ptrFH->GetIsActiveFilterApply( &bApply );
	if( bApply )
	{
		return true;
	}

	BOOL bValidFilter = TRUE;
	ptrFH->IsValidFilter( bstrActiveFilter, FALSE, &bValidFilter );

	if( bstrActiveFilter )
		::SysFreeString( bstrActiveFilter );

	if( !bValidFilter )
		return false;
		

	return false;
}

//////////////////////////////////////////////////////////////////////
//CActionCreateFilter implementation
CActionCreateFilter::CActionCreateFilter()
{
}

CActionCreateFilter::~CActionCreateFilter()
{
}

bool CActionCreateFilter::Invoke()
{
	sptrIDBFilterView spFilterV( m_punkTarget );
	if( spFilterV == NULL )
		return false;

	BSTR bstrField, bstrTable, bstrValue;
	
	_variant_t var;
	bstrField = bstrTable = bstrValue = NULL;
	if( S_OK != spFilterV->GetActiveFieldValue( &bstrTable, &bstrField, &bstrValue ) )
		return false;

	_bstr_t _bstrTable( bstrTable );
	_bstr_t _bstrField( bstrField );
	_bstr_t _bstrValue( bstrValue );




	if( bstrField )
		::SysFreeString( bstrField );

	if( bstrTable )
		::SysFreeString( bstrTable );

	if( bstrValue )
		::SysFreeString( bstrValue );

	sptrIApplication sptrApp( ::GetAppUnknown() );
	if( sptrApp == NULL )
		return false;

	IUnknown* punkDoc = NULL;
	sptrApp->GetActiveDocument( &punkDoc );
	if( punkDoc == NULL )
		return false;

	IDBaseFilterProviderPtr ptrFP( punkDoc );
	punkDoc->Release();

	if( ptrFP == NULL )
		return false;


	IDBaseFilterHolderPtr ptrFH( ptrFP );
	if( ptrFH == NULL )
		return false;

	
	BSTR bstrActiveFilter;
	ptrFH->GetActiveFilter( &bstrActiveFilter );
	_bstr_t _bstrFilter( bstrActiveFilter );

	if( bstrActiveFilter )
		::SysFreeString( bstrActiveFilter );

	ptrFH->AddFilterCondition( _bstrFilter, _bstrTable, _bstrField, _bstr_t("="), 
		_bstrValue, _bstr_t(""), TRUE, 0, 0 );
	
	ptrFH->SetIsActiveFilterApply( TRUE );

	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	::ExecuteAction( GetActionNameByResID( IDS_REQUERY ) );
	ptrFP->ReBuildFilterChooserList();		


	::SetModifiedFlagToDoc( m_punkTarget );	
	
	return true;
}

//extended UI
bool CActionCreateFilter::IsAvaible()
{
	sptrIDBFilterView spFilterV( m_punkTarget );
	if( spFilterV == NULL )
		return false;

	
	if( S_OK != spFilterV->GetActiveFieldValue( 0, 0, 0 ) )	
		return false;

	sptrIApplication sptrApp( ::GetAppUnknown() );
	if( sptrApp == NULL )
		return false;

	IUnknown* punkDoc = NULL;
	sptrApp->GetActiveDocument( &punkDoc );
	if( punkDoc == NULL )
		return false;

	sptrIDBaseDocument sptrDBase( punkDoc );
	punkDoc->Release();

	if( sptrDBase == NULL )
		return false;

	sptrIQueryObject sptrQuery = GetActiveQuery( sptrDBase );
	
	if( sptrQuery == NULL )
		return false;




	return true;
}

bool CActionCreateFilter::IsChecked()
{
	return false;
}

//////////////////////////////////////////////////////////////////////
//
//	Class CFilterChooser
//
//////////////////////////////////////////////////////////////////////
class CFilterChooser : public CCmdTargetEx
{
	DECLARE_DYNCREATE(CFilterChooser);
protected:
	CFilterChooser();

	HWND m_hWndCombo;
	DECLARE_INTERFACE_MAP();
	BEGIN_INTERFACE_PART(Combo,IUIComboBox)
		com_call OnInitList( HWND hWnd );
		com_call OnSelChange();
		com_call OnDetachWindow();
	END_INTERFACE_PART(Combo)
public:
	void SetHWnd( HWND hWnd){ m_hWndCombo = hWnd;}
	HWND GetHWnd( ){ return m_hWndCombo;}
	
};

IMPLEMENT_DYNCREATE(CFilterChooser, CCmdTargetEx)
IMPLEMENT_UNKNOWN(CFilterChooser, Combo)

BEGIN_INTERFACE_MAP(CFilterChooser, CCmdTargetEx)
	INTERFACE_PART(CFilterChooser, IID_IUIComboBox, Combo)
END_INTERFACE_MAP()

CFilterChooser::CFilterChooser()
{		
	m_hWndCombo = NULL;
}



//////////////////////////////////////////////////////////////////////
HRESULT CFilterChooser::XCombo::OnInitList( HWND hWnd )
{
	METHOD_PROLOGUE_EX(CFilterChooser, Combo)
	
	pThis->SetHWnd( hWnd );

	sptrIDBaseDocument spDBaseDoc = NULL;
	if( ::GetDBaseDocument( spDBaseDoc ) )
	{
		IDBaseFilterProviderPtr ptrFP( spDBaseDoc );
		if( ptrFP )
			ptrFP->OnAddFilterChooser( pThis->GetHWnd( ) );
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CFilterChooser::XCombo::OnSelChange()
{
	METHOD_PROLOGUE_EX(CFilterChooser, Combo)

	sptrIDBaseDocument spDBaseDoc = NULL;
	if( ::GetDBaseDocument( spDBaseDoc ) )
	{
		IDBaseFilterProviderPtr ptrFP( spDBaseDoc );
		if( ptrFP )
			ptrFP->OnFilterChooserChange( pThis->GetHWnd( ) );
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CFilterChooser::XCombo::OnDetachWindow()
{
	METHOD_PROLOGUE_EX(CFilterChooser, Combo)

	sptrIDBaseDocument spDBaseDoc = NULL;
	if( ::GetDBaseDocument( spDBaseDoc ) )
	{
		IDBaseFilterProviderPtr ptrFP( spDBaseDoc );
		if( ptrFP )
			ptrFP->OnRemoveFilterChooser( pThis->GetHWnd( ) );
	}
		
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
ACTION_ARG_LIST(CActionChooseFilter)
	ARG_STRING("FilterName", "")
END_ACTION_ARG_LIST()

ACTION_UI(CActionChooseFilter, CFilterChooser)


//////////////////////////////////////////////////////////////////////
//CActionChooseFilter implementation
CActionChooseFilter::CActionChooseFilter()
{
}

CActionChooseFilter::~CActionChooseFilter()
{
}

bool CActionChooseFilter::Invoke()
{
	CString	strFilterName = GetArgumentString( "FilterName" );
	return true;
}

//extended UI
bool CActionChooseFilter::IsAvaible()
{
	sptrIDBaseDocument spDBaseDoc( m_punkTarget );
	if( spDBaseDoc == NULL )
		return false;

	/*
	IUnknown* punkQuery = NULL;
	spDBaseDoc->GetActiveQuery( &punkQuery );
	if( punkQuery )
	{
		punkQuery->Release();
		return true;
	}	
	*/

	return true;
}

bool CActionChooseFilter::IsChecked()
{
	return false;
}


/*
ACTION_ARG_LIST(CActionDBGenerate)	
	ARG_STRING("Event", "")
	ARG_STRING("UIName", "")
END_ACTION_ARG_LIST()

//////////////////////////////////////////////////////////////////////
//CActionDBGenerate implementation
CActionDBGenerate::CActionDBGenerate()
{
}

CActionDBGenerate::~CActionDBGenerate()
{
}



bool CActionDBGenerate::Invoke()
{
	sptrIDBaseDocument spDBaseDoc( m_punkTarget );
	if( spDBaseDoc == NULL )
		return false;

	_bstr_t bstrEvent = GetArgumentString( "Event" );
	_bstr_t bstrUIName = GetArgumentString( "UIName" );	
	//spDBaseDoc->Generate( bstrEvent, bstrUIName );
	return true;
}

//extended UI
bool CActionDBGenerate::IsAvaible()
{
	sptrIDBaseDocument spDBaseDoc( m_punkTarget );
	if( spDBaseDoc == NULL )
		return false;

	return true;
}

bool CActionDBGenerate::IsChecked()
{
	return false;
}
*/

//////////////////////////////////////////////////////////////////////
//CActionDBExecuteSQL implementation
CActionDBExecuteSQL::CActionDBExecuteSQL()
{
}

CActionDBExecuteSQL::~CActionDBExecuteSQL()
{
}

bool CActionDBExecuteSQL::Invoke()
{
	sptrIDBaseDocument spDBaseDoc( m_punkTarget );
	if( spDBaseDoc == NULL )
		return false;

	CExecuteDlg dlg( ::GetMainFrameWnd() );
	dlg.SetDBaseDocument( spDBaseDoc );
	dlg.DoModal( );

	return true;
}

//extended UI
bool CActionDBExecuteSQL::IsAvaible()
{
	sptrIDBaseDocument spDBaseDoc( m_punkTarget );
	if( spDBaseDoc == NULL )
		return false;



	return true;
}

bool CActionDBExecuteSQL::IsChecked()
{
	return false;
}


ACTION_ARG_LIST(CActionDBGenerateReportByAXForm)
	ARG_INT(_T("Landscape"), 0 )
	ARG_STRING(_T("Name"), "" )
END_ACTION_ARG_LIST();

//////////////////////////////////////////////////////////////////////
//CActionDBGenerateReportByAXForm implementation
CActionDBGenerateReportByAXForm::CActionDBGenerateReportByAXForm()
{
}

CActionDBGenerateReportByAXForm::~CActionDBGenerateReportByAXForm()
{
}

//////////////////////////////////////////////////////////////////////
bool CActionDBGenerateReportByAXForm::Invoke()
{
	IQueryObjectPtr ptrQ = GetActiveQuery();
	if( ptrQ == 0 )
		return false;

	IDocumentSitePtr ptrDS( m_punkTarget );
	if( ptrDS == 0 )
		return false;

	IDataContextPtr ptrDC;
	
	{
		IUnknown* punk = 0;
		ptrDS->GetActiveView( &punk );
		if( punk == 0 )
			return false;

		ptrDC = punk;
		punk->Release();	punk = 0;
	}

	if( ptrDC == 0 )
		return false;

	IReportFormPtr ptrReport;
	{
		IUnknown* punk = ::CreateTypedObject( szTypeReportForm );
		ptrReport = punk;
		punk->Release();	punk = 0;
	}

	if( ptrReport == 0 )
		return false;

	if( GetArgumentInt( "Landscape" ) == 1 )
	{
		CSize size = CSize( 0, 0 );
		ptrReport->GetPaperSize( &size );
 		ptrReport->SetPaperSize( CSize( size.cy, size.cx ) );
		ptrReport->SetPaperOrientation( 1 );
	}
	CString str_name = GetArgumentString( "Name" );

	::SetObjectName( ptrReport, str_name.GetLength() ? (const char*)str_name : (const char*)0 );


	if( !ProcessReportByQuery( ptrReport, ptrQ ) )
		return false;


	m_changes.SetToDocData( m_punkTarget, ptrReport );

	::AjustViewForObject( ptrDC, ptrReport );
	//ptrDC->SetActiveObject( _bstr_t( szTypeReportForm ), ptrReport, 1 );


	return false;
}

//////////////////////////////////////////////////////////////////////
bool CActionDBGenerateReportByAXForm::ProcessReportByQuery( IReportForm* pIReport, IQueryObject* pIQuery )
{
	if( !pIReport || !pIQuery )
		return false;


	IActiveXFormPtr ptrAXForm( pIQuery );
	if( ptrAXForm == 0 )
		return 0;

	IActiveXForm* pIAXForm = ptrAXForm;
	

	//Wanna place all controls to one page. Calc zoom factor.	

	
	//Get report work area
	CRect rcFields;
	pIReport->GetPaperField( &rcFields );
	CSize pageSize;
	pIReport->GetPaperSize( &pageSize );	
	
	
	CSize sizePrnArea(	pageSize.cx - rcFields.left - rcFields.right, 
						pageSize.cy - rcFields.top - rcFields.bottom );
	
	//Get AXForm work area
	CSize sizeAxFormArea;
	pIAXForm->SetSize( CSize( 100, 100 ) );
	pIAXForm->AutoUpdateSize( );

	pIAXForm->GetSize( &sizeAxFormArea );	

	if( sizeAxFormArea.cx < 1 || sizeAxFormArea.cy < 1 || 
		sizePrnArea.cx < 1 || sizePrnArea.cy < 1
		)
		return false;
	
	double fReportWidth		= (double)sizePrnArea.cx;
	double fReportHeight	= (double)sizePrnArea.cy;
	
	double fFormWidth		= (double)sizeAxFormArea.cx;
	double fFormHeight		= (double)sizeAxFormArea.cy;

	double fZoom = min( fReportWidth / fFormWidth, fReportHeight / fFormHeight );

	//Now put controls
	CObjectListWrp	list( pIAXForm );
	POSITION	pos = list.GetFirstObjectPosition();	


	while( pos )
	{			
		IUnknown *punk = list.GetNextObject( pos );
		sptrIActiveXCtrl sptrAXCtrl( punk );
		punk->Release();	punk = 0;
		if( sptrAXCtrl )
		{
			CRect rcAXCtrl, r1;
			sptrAXCtrl->GetRect( &rcAXCtrl );

			r1 = rcAXCtrl;
			
			rcAXCtrl.left	*= fZoom;
			rcAXCtrl.top	*= fZoom;
			rcAXCtrl.right	*= fZoom;
			rcAXCtrl.bottom	*= fZoom;

			
			rcAXCtrl.left	+= rcFields.left;
			rcAXCtrl.top	+= rcFields.top;
			rcAXCtrl.right	+= rcFields.left;
			rcAXCtrl.bottom	+= rcFields.top;

			ASSERT( rcAXCtrl.right < rcFields.left + sizePrnArea.cx );
			ASSERT( rcAXCtrl.bottom < rcFields.top + sizePrnArea.cy );


			IUnknown* punkReportCtrl = NULL;
			punkReportCtrl = ::CreateTypedObject( szTypeReportCtrl );
			if( punkReportCtrl == NULL )
				continue;

			sptrIReportCtrl	spReportCtrl( punkReportCtrl );
			punkReportCtrl->Release();	punkReportCtrl = 0;
			if( spReportCtrl == NULL )
				continue;


			sptrINamedDataObject2 sptrNDO2( spReportCtrl );
			if( sptrNDO2 == NULL )
				continue;

			{
				IActiveXCtrl* pIAXCtrl = sptrAXCtrl;
				BSTR bstr = 0;
				if( S_OK != pIAXCtrl->GetProgID( &bstr ) )
					continue;

				_bstr_t bstrProgID = bstr;	
				if( bstr )
					::SysFreeString( bstr );	bstr = 0;				

				CLSID clsid;
				::ZeroMemory( &clsid, sizeof(CLSID) );

				if( S_OK != ::CLSIDFromProgID( bstrProgID, &clsid ) )
					continue;

				IUnknown* punkAXControl = 0;
				if( S_OK != ::CoCreateInstance( clsid, NULL, 
												CLSCTX_INPROC_SERVER, IID_IUnknown, 
												(LPVOID*)&punkAXControl ) )
						continue;

				IUnknownPtr ptrAXControl = punkAXControl;	punkAXControl->Release();	punkAXControl = 0;

				::RestoreContainerFromDataObject( sptrAXCtrl, ptrAXControl );

				::StoreContainerToDataObject( spReportCtrl, ptrAXControl );



				UINT uiID = 0;
				pIAXCtrl->GetControlID( &uiID );
				BSTR bstrName = 0;
				pIAXCtrl->GetName( &bstrName );

				CRect rc;
				pIAXCtrl->GetRect( &rc );
				DWORD dwStyle = 0;
				pIAXCtrl->GetStyle( &dwStyle );



				IActiveXCtrlPtr ptrAXCtrlTarget( spReportCtrl );
				if( ptrAXCtrlTarget )
				{
					IActiveXCtrl* pIAXCtrlTarget = ptrAXCtrlTarget;

					pIAXCtrlTarget->SetProgID( bstrProgID );
					pIAXCtrlTarget->SetControlID( uiID );					
					pIAXCtrlTarget->SetName( bstrName );					
					pIAXCtrlTarget->SetRect( rc );					
					pIAXCtrlTarget->SetStyle( dwStyle );

				}

				if(IViewSubTypePtr pViewSub=sptrAXCtrl)
				{
					unsigned	 long ViewSubType=0;
					if(SUCCEEDED(pViewSub->GetViewSubType(&ViewSubType)))
					{
						if(ViewSubType>0){
							if(IViewSubTypePtr pViewSubRpCtrl=spReportCtrl)
							{
								pViewSubRpCtrl->SetViewSubType(ViewSubType);
							}
						}
					}
				}

				if( bstrName )
					::SysFreeString( bstrName );	bstrName = 0;


				
			}

			
			sptrNDO2->SetParent( pIReport, 0 );


			spReportCtrl->SetDesignOwnerPage( 0 );
			spReportCtrl->SetDesignColRow( 0, 0 );
			spReportCtrl->SetDesignPosition( rcAXCtrl );
		}		
	}	

	return true;
}

//////////////////////////////////////////////////////////////////////
bool CActionDBGenerateReportByAXForm::IsAvaible()
{
	IQueryObjectPtr ptr = GetActiveQuery();
	return ptr != 0;
}


//////////////////////////////////////////////////////////////////////
IQueryObjectPtr	CActionDBGenerateReportByAXForm::GetActiveQuery()
{
	IDBaseDocumentPtr ptrDB( m_punkTarget );

	if( ptrDB == 0 )
		return 0;

	IUnknown* punkQ = 0;
	ptrDB->GetActiveQuery( &punkQ );
	if( !punkQ )
		return 0;

	IQueryObjectPtr ptrQ( punkQ );
	punkQ->Release();	punkQ = 0;

	return ptrQ;
}

//////////////////////////////////////////////////////////////////////
bool CActionDBGenerateReportByAXForm::DoUndo()
{
	m_changes.DoUndo( m_punkTarget );
	return true;
}

//////////////////////////////////////////////////////////////////////
bool CActionDBGenerateReportByAXForm::DoRedo()
{
	m_changes.DoRedo( m_punkTarget );
	return true;
}


//////////////////////////////////////////////////////////////////////
//CActionRequery implementation
CActionRequery::CActionRequery()
{
}

CActionRequery::~CActionRequery()
{
}

class _CDisableAutoSelect
{
	BOOL m_bPrev;
public:
	_CDisableAutoSelect()
	{
		m_bPrev = ::GetValueInt(::GetAppUnknown(), "DataContext", "AutoSelectOnEmpty", TRUE);
		::SetValue(::GetAppUnknown(), "DataContext", "AutoSelectOnEmpty", (long)FALSE);
	}
	~_CDisableAutoSelect()
	{
		::SetValue(::GetAppUnknown(), "DataContext", "AutoSelectOnEmpty", (long)m_bPrev);
	}
};

bool CActionRequery::Invoke()
{
	_CDisableAutoSelect DisableAutoSelect;
	GET_ACTIVE_QUERY_FROM_DOC()// spQueryData - active Query data,spSelectQuery - select query	
	BOOL bOpen = FALSE;

	spSelectQuery->UpdateInteractive( TRUE );
	spSelectQuery->Close();
	spSelectQuery->Open();

	/*
	spSelectQuery->IsOpen( &bOpen );
	if( !bOpen )
		return false;

	spSelectQuery->Close();
	spSelectQuery->Open();
	*/
	

	return true;
}

//extended UI
bool CActionRequery::IsAvaible()
{
	GET_ACTIVE_QUERY_FROM_DOC()// spQueryData - active Query data,spSelectQuery - select query	
	//BOOL bOpen = FALSE;
	//spSelectQuery->IsOpen( &bOpen );
	//if( !bOpen )
	//	return false;

	return true;
}

bool CActionRequery::IsChecked()
{
	return false;
}

/*
//////////////////////////////////////////////////////////////////////
//CActionDBGridOptions implementation
CActionDBGridOptions::CActionDBGridOptions()
{
}

CActionDBGridOptions::~CActionDBGridOptions()
{
}

bool CActionDBGridOptions::Invoke()
{
	sptrIDBGridView spGridView( m_punkTarget );
	if( spGridView == NULL )
		return false;

	sptrIView spView( spGridView );
	if( spView == NULL )
		return false;

	IUnknown* pUnkDoc = NULL;
	
	spView->GetDocument( &pUnkDoc );
	if( pUnkDoc == NULL )
		return false;

	sptrIDBaseDocument spDBDoc( pUnkDoc );
	pUnkDoc->Release();

	if( spDBDoc == NULL )
		return false;

	IUnknown* pUnkActiveQuery = NULL;
	spDBDoc->GetActiveQuery( &pUnkActiveQuery );
	if( pUnkActiveQuery == NULL )
		return false;

	if( !CheckInterface( pUnkActiveQuery, IID_IQueryObject ))
		return false;

	spGridView->SaveGridSettings();

	CGridOptionsDlg dlg;
	dlg.SetActiveQuery( pUnkActiveQuery );
	if( dlg.DoModal() == IDOK )
	{
		_variant_t var;
		spDBDoc->FireEvent( _bstr_t(szDBaseEventGridOptionsChange), pUnkActiveQuery, spDBDoc, NULL, NULL, var );
	}


	pUnkActiveQuery->Release();
	
	

	return true;
}

//extended UI
bool CActionDBGridOptions::IsAvaible()
{
	sptrIDBGridView spView( m_punkTarget );
	if( spView == NULL )
		return false;

	return true;
}

bool CActionDBGridOptions::IsChecked()
{
	return false;
}
*/
//////////////////////////////////////////////////////////////////////
//CActionShowDBGrid implementation

bool CActionShowDBGrid::Invoke()
{
	if( !CActionShowViewBase::Invoke() )
		return false;	

	return true;
}

//extended UI
bool CActionShowDBGrid::IsAvaible()
{
	sptrIApplication sptrA( GetAppUnknown( ) );
	if( sptrA == NULL )
		return false;
	
	IUnknown* punk = NULL;
	sptrA->GetActiveDocument( &punk );
	if( punk == NULL )
		return false;

	sptrIDBaseDocument spIDBDoc = punk;

	punk->Release();

	if( spIDBDoc != NULL )
		return true;

	return false;	
}

bool CActionShowDBGrid::IsChecked()
{
	return CActionShowViewBase::IsChecked();
}

CString CActionShowDBGrid::GetViewProgID()
{
	return szDBaseGridViewProgID;
}



ACTION_ARG_LIST(CActionDBaseBackUp)
	ARG_STRING("TargetFileName", "")	
END_ACTION_ARG_LIST()

ACTION_ARG_LIST(CActionDBaseRestore)
	ARG_STRING("SourceFileName", "")	
END_ACTION_ARG_LIST()

BEGIN_INTERFACE_MAP(CActionDBaseBackUp, CActionBase)
	INTERFACE_PART(CActionDBaseBackUp, IID_ILongOperation, Long)
END_INTERFACE_MAP()

BEGIN_INTERFACE_MAP(CActionDBaseRestore, CActionBase)
	INTERFACE_PART(CActionDBaseRestore, IID_ILongOperation, Long)
END_INTERFACE_MAP()

BEGIN_INTERFACE_MAP(CActionDBaseFindNext, CActionBase)
	INTERFACE_PART(CActionDBaseFindNext, IID_ILongOperation, Long)
END_INTERFACE_MAP()

BEGIN_INTERFACE_MAP(CActionDBaseFind, CActionBase)
	INTERFACE_PART(CActionDBaseFind, IID_ILongOperation, Long)
END_INTERFACE_MAP()


//////////////////////////////////////////////////////////////////////
CFindImpl::CFindImpl()
{

}

//////////////////////////////////////////////////////////////////////
CFindImpl::~CFindImpl()
{

}

//////////////////////////////////////////////////////////////////////
bool CFindImpl::GetSettingsFromQuery( sptrISelectQuery spSelectQuery, sptrIDBaseDocument spDBDoc )
{

	if( spSelectQuery == NULL )
		return false;

	if( spDBDoc == NULL )
		return false;
	

	BOOL bFindInField, bMatchCase, bRegularExpression;
	BSTR bstrTable, bstrField, bstrTextToFind;

	spSelectQuery->GetFindSettings( &bFindInField, &bstrTable, &bstrField,
								&bMatchCase, &bRegularExpression, &bstrTextToFind  );


	m_bFindInField				= (bFindInField == TRUE );
	m_strFindTable				= bstrTable;
	m_strFindField				= bstrField;
	m_bFindMatchCase			= (bMatchCase == TRUE);
	m_bFindRegularExpression	= (bRegularExpression == TRUE);
	m_strTextToFind				= bstrTextToFind;

	if( bstrTable )
		::SysFreeString( bstrTable );

	if( bstrField )
		::SysFreeString( bstrField );

	if( bstrTextToFind )
		::SysFreeString( bstrTextToFind );	

	return true;
}

//////////////////////////////////////////////////////////////////////
bool CFindImpl::ExecuteSettings( sptrISelectQuery spSelectQuery, sptrIDBaseDocument spDBDoc )
{
	if( spSelectQuery == NULL )
		return false;

	if( spDBDoc == NULL )
		return false;

	if( !GetSettingsFromQuery( spSelectQuery, spDBDoc) )
		return false;


	CDBFindDlg dlg( ::GetMainFrameWnd() );
	dlg.SetQueryObject( spSelectQuery );
	
	if( dlg.DoModal() != IDOK )
		return false;
	
	
	m_bFindInField				= (dlg.m_bFindInField == 0 );
	m_bFindMatchCase			= (dlg.m_bMatchCase == TRUE);
	m_bFindRegularExpression	= (dlg.m_bRegularExpression == TRUE);
	m_strTextToFind				= dlg.m_strTextToFind;

	m_bFindFromCurRecord		= dlg.m_bFindFromCurRecord == TRUE;

	m_strFindTable				= dlg.m_strTable;
	m_strFindField				= dlg.m_strField;

	m_strFindTable.MakeLower();
	m_strFindField.MakeLower();

	spSelectQuery->SetFindSettings( m_bFindInField, 
									_bstr_t( (LPCTSTR)m_strFindTable ), 
									_bstr_t( (LPCTSTR)m_strFindField ),
									m_bFindMatchCase, m_bFindRegularExpression, 
									_bstr_t( (LPCTSTR)m_strTextToFind ) );


	return true;
}

long CFindImpl::ProcessFind( IUnknown* punkQ, IUnknown* punkDoc, bool bFoundFromCurRecord, int* pnField )
{
	CWaitCursor wait;

	ISelectQueryPtr		ptrQ( punkQ );
	IQueryObjectPtr		ptrQO( punkQ );
	IDBConnectionPtr	ptrDBC( punkDoc );
	IDBaseStructPtr		ptrDBS( punkDoc );


	if( ptrQ == 0 || ptrQO == 0 || ptrDBC == 0 || ptrDBS == 0 )
		return -1;


	BOOL bOpen = FALSE;
	ptrQ->IsOpen( &bOpen );
	if( !bOpen )
		return -1;

	long lRecord = -1;
	ptrQ->GetCurrentRecord( &lRecord );
	if( lRecord < 1 )
		return -1;

	IUnknown* pUnkConn = NULL;
	ptrDBC->GetConnection( &pUnkConn );
	if( pUnkConn == NULL)
		return -1;

	ADO::_ConnectionPtr ptrConn( pUnkConn );
	pUnkConn->Release();	pUnkConn = 0;

	BSTR bstr = 0;
	ptrQO->GetSQL( &bstr );
	 _bstr_t bstrSQL = bstr;

	 if( bstr )
		::SysFreeString( bstr );	bstr = 0;

	 try
	 {

		ADO::_RecordsetPtr ptrRecordset;
		_variant_t vConn = (IDispatch*)ptrConn;
		ptrRecordset.CreateInstance( __uuidof(ADO::Recordset) );

		ptrRecordset->CursorLocation = ::GetCursorLocation( (LPCSTR)bstrSQL );

		ptrRecordset->Open( bstrSQL, vConn, ADO::adOpenKeyset, ADO::adLockOptimistic, ADO::adCmdText );

		
		long lCurRecord = 0;
		if( bFoundFromCurRecord )
		{
			ptrRecordset->AbsolutePosition = (ADO::PositionEnum)( lRecord + 1 );
			lCurRecord = lRecord + 1;
		}
		else
		{
			ptrRecordset->MoveFirst();
			lCurRecord = 1;
		}

		long lFirstRecord = lCurRecord;

		long lRecordCount = ptrRecordset->RecordCount;

		//StartNotification( lRecordCount - lFirstRecord + 1 );
			
		while( VARIANT_FALSE == ptrRecordset->ADOEOF )
		{
			ADO::FieldsPtr ptrFields = ptrRecordset->Fields;
			if( ptrFields != NULL )
			{
				long nCount = ptrFields->Count;
				for( long i=0;i<nCount;i++  )
				{
					ADO::FieldPtr ptrField = ptrFields->GetItem( (long)i );
					if( ptrField == 0 )
						continue;

					ADO::PropertiesPtr ptrProperties = ptrField->Properties;
					if( ptrProperties == 0 )
						continue;

					ADO::PropertyPtr ptrPropTable = ptrProperties->GetItem( "BASETABLENAME" );
					ADO::PropertyPtr ptrPropField = ptrProperties->GetItem( "BASECOLUMNNAME" );

					if( ptrPropTable == 0 || ptrPropField == 0 )
						continue;

					CString strTable	= ::_MakeLower( ptrPropTable->Value.bstrVal );
					CString strField	= ::_MakeLower( ptrPropField->Value.bstrVal );


					FieldType fieldType = ::GetFieldType( punkDoc, strTable, strField );

					if( !( fieldType == ftDigit || fieldType == ftString || fieldType == ftDateTime ) )
						continue;

					if( m_bFindInField && ! ( m_strFindTable == strTable && m_strFindField == strField ) )
						continue;

					if( pnField )
						*pnField = i;


					_variant_t var;
					var = ptrField->GetValue();		

					if( var.vt == VT_NULL || var.vt == VT_EMPTY )
					{
						if( fieldType == ftString && m_strTextToFind.IsEmpty() )
							return lCurRecord;
						continue;
					}
					
					var.ChangeType( VT_BSTR );

					CString str = var.bstrVal;					

					CString str1 = str;
					CString str2 = m_strTextToFind;

					if( m_bFindMatchCase )
					{
						if( m_bFindRegularExpression )
						{
							if( str1 == str2 )
								return lCurRecord;
						}
						else
						{
							if( str1.Find( str2 ) != -1 )
								return lCurRecord;
						}
					}
					else
					{
						str1.MakeLower();
						str2.MakeLower();
						if( m_bFindRegularExpression )
						{
							if( str1 == str2 )
								return lCurRecord;
						}
						else
						{
							if( str1.Find( str2 ) != -1 )
								return lCurRecord;
						}
					}					

				}			
			}
			ptrRecordset->MoveNext();
			lCurRecord++;
			//Notify( lCurRecord - lFirstRecord );
		}			

	 }
	 catch(...)
	 {
		 return -1;		 
	 }	


	 return -1;

}

//////////////////////////////////////////////////////////////////////
//CActionDBaseFind implementation
CActionDBaseFind::CActionDBaseFind()
{
}

CActionDBaseFind::~CActionDBaseFind()
{
}

bool CActionDBaseFind::ExecuteSettings( CWnd *pwndParent )
{
	GET_ACTIVE_QUERY_FROM_DOC()

	return true;
}

bool CActionDBaseFind::Invoke()
{
	GET_ACTIVE_QUERY_FROM_DOC()

	BOOL bOpen = FALSE;
	spSelectQuery->IsOpen( &bOpen );
	if( !bOpen )
		return false;

	if( !CFindImpl::ExecuteSettings( spSelectQuery, spDBDoc ) )
		return false;

	sptrIQueryObject spQueryObject( spSelectQuery );
	if( spQueryObject == NULL )
		return false;

	long nRecordCount = -1;
	spSelectQuery->GetRecordCount( &nRecordCount );
	if( nRecordCount < 1 )
		return false;


	int nField = 0;

	long nRecordFound = ProcessFind( spSelectQuery, spDBDoc, m_bFindFromCurRecord, &nField );
	//FinishNotification();
	if( nRecordFound > 0 )
	{
		spSelectQuery->SetCanProcessFind( TRUE );
		spSelectQuery->GoToRecord( nRecordFound );

		spQueryObject->SetActiveField( nField );

		if( nRecordFound == nRecordCount )
			spSelectQuery->SetCanProcessFind( FALSE );
	}
	else
	{
		spSelectQuery->SetCanProcessFind( FALSE );
		AfxMessageBox( IDS_NO_FIND_RESULT, MB_ICONEXCLAMATION );
	}

	return true;
}

//extended UI
bool CActionDBaseFind::IsAvaible()
{
	GET_ACTIVE_QUERY_FROM_DOC()

	BOOL bOpen = FALSE;
	spSelectQuery->IsOpen( &bOpen );
	if( !bOpen )
		return false;

	return true;
}

bool CActionDBaseFind::IsChecked()
{
	return false;
}

//////////////////////////////////////////////////////////////////////
//CActionDBaseFindNext implementation
CActionDBaseFindNext::CActionDBaseFindNext()
{
}

CActionDBaseFindNext::~CActionDBaseFindNext()
{
}

bool CActionDBaseFindNext::Invoke()
{
	GET_ACTIVE_QUERY_FROM_DOC()

	BOOL bOpen = FALSE;
	spSelectQuery->IsOpen( &bOpen );
	if( !bOpen )
		return false;

	BOOL bAvailable = FALSE;

	spSelectQuery->GetCanProcessFind( &bAvailable );
	if( !bAvailable )
		return false;


	if( !GetSettingsFromQuery( spSelectQuery, spDBDoc) )
		return false;

	sptrIQueryObject spQueryObject( spSelectQuery );
	if( spQueryObject == NULL )
		return false;

	long nRecordCount = -1;
	spSelectQuery->GetRecordCount( &nRecordCount );
	if( nRecordCount < 1 )
		return false;

	long nCurRecord = -1;
	spSelectQuery->GetCurrentRecord( &nCurRecord );
	if( nCurRecord < 1 || nCurRecord + 1 > nRecordCount )
		return false;

	int nField = 0;
	long nRecordFound = ProcessFind( spSelectQuery, spDBDoc, true, &nField );
	//FinishNotification();

	if( nRecordFound > 0 )
	{
		spSelectQuery->SetCanProcessFind( TRUE );
		spSelectQuery->GoToRecord( nRecordFound );

		spQueryObject->SetActiveField( nField );

		if( nRecordFound == nRecordCount )
			spSelectQuery->SetCanProcessFind( FALSE );
	}
	else
	{
		spSelectQuery->SetCanProcessFind( FALSE );
		AfxMessageBox( IDS_NO_FIND_RESULT, MB_ICONEXCLAMATION );
	}


	return true;
}

//extended UI
bool CActionDBaseFindNext::IsAvaible()
{
	GET_ACTIVE_QUERY_FROM_DOC()

	BOOL bOpen = FALSE;
	spSelectQuery->IsOpen( &bOpen );
	if( !bOpen )
		return false;

	BOOL bAvailable = FALSE;

	spSelectQuery->GetCanProcessFind( &bAvailable );
	if( !bAvailable )
		return false;

	return true;
}

bool CActionDBaseFindNext::IsChecked()
{
	return false;
}




//////////////////////////////////////////////////////////////////////
//CActionDBaseRestore implementation
CActionDBaseRestore::CActionDBaseRestore()
{
	m_pBufRaw	= 0;
	m_pBufPack	= 0;

}

CActionDBaseRestore::~CActionDBaseRestore()
{
	if( m_pBufRaw )
		delete m_pBufRaw;	m_pBufRaw = 0;

	if( m_pBufPack )
		delete m_pBufPack;	m_pBufPack = 0;
}

bool CActionDBaseRestore::Invoke()
{
	sptrIDBaseDocument spDBDoc( m_punkTarget );
	if( spDBDoc == NULL )
		return false;

	IDBConnectionPtr ptrDBC( spDBDoc );
	if( ptrDBC == 0 )
		return false;



	//Update active query
	IUnknown* pUnkActiveQuery = NULL;
	spDBDoc->GetActiveQuery( &pUnkActiveQuery );
	sptrISelectQuery spSelectQuery( pUnkActiveQuery );

	if( pUnkActiveQuery != NULL )
	{		
		pUnkActiveQuery->Release();

		spSelectQuery = pUnkActiveQuery;

		if( spSelectQuery )
		{
			spSelectQuery->Update();
		}
	}


	BSTR bstrFileName;
	ptrDBC->GetMDBFileName( &bstrFileName );
	CString strMDBFileName = bstrFileName;

	if( bstrFileName )
		::SysFreeString( bstrFileName );

	CFileFind ff;
	if( !ff.FindFile( strMDBFileName ) )
	{
		CString strWarning;
		strWarning.LoadString( IDS_WARNING_NO_MDB_FILES );
		CString strError;
		strError.Format( (LPCTSTR)strWarning, (LPCTSTR)strMDBFileName );
		AfxMessageBox( strError, MB_ICONSTOP );
		return false;
	}

	CString strSourceFileName = GetArgumentString( "SourceFileName" );

	
	if( !ff.FindFile( strSourceFileName ) )
	{
		CFileDialog dlg(TRUE, 0, 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 0, 0, _FILE_OPEN_SIZE_ );

		CString strLastBackUpLocation = ::_GetValueString( spDBDoc, 
									SECTION_SETTINGS, SECTION_LASTBACKUPLOCATION, "" );		


		strcpy(dlg.m_ofn.lpstrFile, strLastBackUpLocation);
		dlg.m_ofn.lpstrFilter = "VT archive files(*.vta)\0*.vta\0\0";
		if(dlg.DoModal() != IDOK)
		{
			ptrDBC->OpenConnection();

			if( spSelectQuery )
				spSelectQuery->Open();
			
			return false;
		}
			

		strSourceFileName = dlg.GetPathName();						

		::_SetValue( spDBDoc, SECTION_SETTINGS, SECTION_LASTBACKUPLOCATION, strSourceFileName );

		
	}

	bool bErrorOccured = false;

	try{

		CString strUnpackFileName = strMDBFileName + "_unpack";

		CFile fileRead( strSourceFileName, 
					CFile::modeRead | CFile::shareDenyNone | 
					CFile::typeBinary 					
					);				  

		CFile fileWrite( strUnpackFileName, 
					CFile::modeCreate | CFile::modeWrite |
					CFile::typeBinary |
					CFile::shareExclusive
					);


		DWORD dwFileLength = 0;
		DWORD dwCurPosition = 0;

		char szBuf[3];

		fileRead.Read( szBuf, sizeof(char) * 3 );
		if( szBuf[0] != 'V' || szBuf[1] != 'T' || szBuf[2] != 'A' )
			return false;		

		fileRead.Read( &dwFileLength, sizeof(DWORD) );		

										 
		StartNotification( 1001, 1, 1 );

		DWORD dwBufRawSize = 0;

		fileRead.Read( &dwBufRawSize, sizeof(DWORD) );		

		m_pBufRaw = new BYTE[ dwBufRawSize ];		
		DWORD dwBufPackSize = dwBufRawSize * 1.2;

		m_pBufPack = new BYTE[ dwBufRawSize * 1.2 ];
										 
		StartNotification( 1001, 1, 1 );

		bool b_enough = false;
		while( !b_enough )
		{						 
			DWORD nRealLength = 0;
			DWORD dwPack = 0;
			fileRead.Read( &nRealLength, sizeof(DWORD) );
			fileRead.Read( &dwPack, sizeof(DWORD) );
			fileRead.Read( m_pBufPack, dwPack );			
			
			DWORD dwDecompSize = nRealLength;
			if( dwDecompSize > dwBufRawSize )
				return false;

			if( Z_OK != uncompress( m_pBufRaw, &dwDecompSize, m_pBufPack, dwPack ) )
				return false;

			if( dwDecompSize != nRealLength )
				return false;


			fileWrite.Write( m_pBufRaw, nRealLength );
		
			if( nRealLength != dwBufRawSize )
				b_enough = true;

			dwCurPosition += nRealLength;

			int nPercent = int( double(dwCurPosition) / double(dwFileLength) * 1000.);

			Notify( nPercent );
		}

		fileRead.Close();
		fileWrite.Close();
	

		if( spSelectQuery )
			spSelectQuery->Close();	
	
		ptrDBC->CloseConnection();
		
		CString strTempCopy = strMDBFileName + "_cur_copy";
		
		if( !MoveFile( strMDBFileName, strTempCopy ) )
		{
			ptrDBC->OpenConnection();

			if( spSelectQuery )
				spSelectQuery->Open();

			FinishNotification();

			return false;
		}

		if( !MoveFile( strUnpackFileName, strMDBFileName ) )
		{
			MoveFile( strTempCopy, strMDBFileName );			

			ptrDBC->OpenConnection();

			if( spSelectQuery )
				spSelectQuery->Open();
			
			FinishNotification();

			return false;
		}
		
		DeleteFile( strTempCopy );

		FinishNotification();

		AfxMessageBox( IDS_UNPACK_SUCCEDED, MB_ICONINFORMATION );
		

	}
	catch(CFileException* pe)
	{		

		pe->ReportError();
		pe->Delete();
		
		FinishNotification();
		
		bErrorOccured = true;

	}
	catch( CTerminateException* pe )
	{
		ptrDBC->OpenConnection();

		if( spSelectQuery )
			spSelectQuery->Open();

		pe->Delete();

		FinishNotification();
	}


	ptrDBC->OpenConnection();

	if( spSelectQuery )
		spSelectQuery->Open();

	FinishNotification();

	return true;
}

//extended UI
bool CActionDBaseRestore::IsAvaible()
{
	sptrIDBaseDocument spDBDoc( m_punkTarget );
	if( spDBDoc == NULL )
		return false;

	return true;
}

bool CActionDBaseRestore::IsChecked()
{
	return false;
}

//////////////////////////////////////////////////////////////////////
//CActionDBaseBackUp implementation
CActionDBaseBackUp::CActionDBaseBackUp()
{
	m_pBufRaw	= 0;
	m_pBufPack	= 0;
}

CActionDBaseBackUp::~CActionDBaseBackUp()
{
	if( m_pBufRaw )
		delete m_pBufRaw;	m_pBufRaw = 0;

	if( m_pBufPack )
		delete m_pBufPack;	m_pBufPack = 0;
}


bool CActionDBaseBackUp::Invoke()
{
	sptrIDBaseDocument spDBDoc( m_punkTarget );
	if( spDBDoc == NULL )
		return false;

	IDBConnectionPtr ptrDBC( spDBDoc );
	if( ptrDBC == 0 )
		return false;

	//Update active query
	sptrISelectQuery spSelectQuery;
	BOOL bWasOpen = false;
	{
		IUnknown* pUnkActiveQuery = NULL;
		spDBDoc->GetActiveQuery( &pUnkActiveQuery );
		if( pUnkActiveQuery )
		{
			spSelectQuery = pUnkActiveQuery;
			pUnkActiveQuery->Release();

			if( spSelectQuery )
			{
				spSelectQuery->IsOpen( &bWasOpen );
				spSelectQuery->Update();				
			}

		}
	}


	BSTR bstrFileName = 0;
	ptrDBC->GetMDBFileName( &bstrFileName );
	CString strMDBFileName = bstrFileName;
	
	if( bstrFileName )
		::SysFreeString( bstrFileName );

	CFileFind ff;
	if( !ff.FindFile( strMDBFileName ) )
	{
		CString strWarning;
		strWarning.LoadString( IDS_WARNING_NO_MDB_FILES );
		CString strError;
		strError.Format( (LPCTSTR)strWarning, (LPCTSTR)strMDBFileName );
		AfxMessageBox( strError, MB_ICONSTOP );
		return false;
	}

	CString strTargetFileName = GetArgumentString( "TargetFileName" );

	
	if( !ff.FindFile( strTargetFileName ) )
	{
		CFileDialog dlg(FALSE, 0, 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 0, 0, _FILE_OPEN_SIZE_ );
			
		CString strLastBackUpLocation = ::_GetValueString( spDBDoc, 
									SECTION_SETTINGS, SECTION_LASTBACKUPLOCATION, "" );		

		strcpy(dlg.m_ofn.lpstrFile, strLastBackUpLocation);
		dlg.m_ofn.lpstrFilter = "VT archive files(*.vta)\0*.vta\0\0";		

		if(dlg.DoModal() != IDOK)
			return false;

		strTargetFileName = dlg.GetPathName();						

		CString strLower = strTargetFileName;
		strLower.MakeLower();

		if( -1 == strLower.Find( ".vta" ) )
			strTargetFileName += ".vta";			

		::_SetValue( spDBDoc, SECTION_SETTINGS, SECTION_LASTBACKUPLOCATION, strTargetFileName );		
	}

	if( spSelectQuery )
		spSelectQuery->Close();

	if( ptrDBC )
		ptrDBC->CloseConnection();

	try{
		CFile fileRead( strMDBFileName, 
					CFile::modeRead | CFile::shareExclusive | 
					CFile::typeBinary 					
					);

		CFile fileWrite( strTargetFileName, 
					CFile::modeCreate | CFile::modeWrite |
					CFile::typeBinary 
					);

		//CArchiveExt arStore( &fileWrite, CArchive::store, 1024*1024, NULL, "", TRUE );

		DWORD dwFileLength = fileRead.GetLength();
		DWORD dwCurPosition = 0;


		fileWrite.Write( "VTA", sizeof(char) * 3 );
		fileWrite.Write( &dwFileLength, sizeof(DWORD) );		
		

		DWORD dwBufRawSize = dwFileLength / 1000;
		if( dwBufRawSize < 1024 )
			dwBufRawSize = 1024;

		fileWrite.Write( &dwBufRawSize, sizeof(DWORD) );		

		

		m_pBufRaw = new BYTE[ dwBufRawSize ];		
		DWORD dwBufPackSize = dwBufRawSize * 1.2;


		m_pBufPack = new BYTE[ dwBufRawSize * 1.2 ];
										 
		StartNotification( 1001, 1, 1 );

		bool b_enough = false;
		while( !b_enough )
		{						 
			UINT nRealLength = fileRead.Read( m_pBufRaw, dwBufRawSize );			

			DWORD dwPack = dwBufPackSize;

			if( Z_OK != compress( m_pBufPack, &dwPack, m_pBufRaw, nRealLength ) )
				return false;

			fileWrite.Write( &nRealLength, sizeof(DWORD) );
			fileWrite.Write( &dwPack, sizeof(DWORD) );
			fileWrite.Write( m_pBufPack, dwPack );			

			//arStore.Write( g_szBuf, nRealLength );
			if( nRealLength != dwBufRawSize )
				b_enough = true;

			dwCurPosition += nRealLength;

			int nPercent = int( double(dwCurPosition) / double(dwFileLength) * 1000.);

			Notify( nPercent );
		}

		fileRead.Close();
		//arStore.Close();
		fileWrite.Close();

		FinishNotification();

		AfxMessageBox( IDS_PACK_SUCCEDED, MB_ICONINFORMATION );


	}
	catch(CFileException* pe)
	{		

		pe->ReportError();
		pe->Delete();
		
		FinishNotification();

	}
	catch(CTerminateException* pe)
	{
		if( ptrDBC )
			ptrDBC->OpenConnection();

		if( spSelectQuery != 0 && bWasOpen )
			spSelectQuery->Open();

		pe->Delete();

		FinishNotification();
	}


	if( ptrDBC )
		ptrDBC->OpenConnection();

	if( spSelectQuery != 0 && bWasOpen )
		spSelectQuery->Open();


	FinishNotification();

	return true;
}

//extended UI
bool CActionDBaseBackUp::IsAvaible()
{
	sptrIDBaseDocument spDBDoc( m_punkTarget );
	if( spDBDoc == NULL )
		return false;

	return true;
}

bool CActionDBaseBackUp::IsChecked()
{
	return false;
}


//////////////////////////////////////////////////////////////////////
//CActionShowGallery implementation
CString CActionShowGallery::GetViewProgID()
{	
	return szGalleryViewProgID;
}

//////////////////////////////////////////////////////////////////////
bool CActionShowGallery::IsAvaible()
{
	sptrIApplication sptrA( GetAppUnknown( ) );
	if( sptrA == NULL )
		return false;
	
	IUnknown* punk = NULL;
	sptrA->GetActiveDocument( &punk );
	if( punk == NULL )
		return false;

	sptrIDBaseDocument spIDBDoc = punk;

	punk->Release();

	if( spIDBDoc != NULL )
		return true;

	return false;

}

//////////////////////////////////////////////////////////////////////
//CActionShowBlankPreview implementation

bool CActionShowBlankPreview::Invoke()
{
	if( !CActionShowViewBase::Invoke() )
		return false;	
	
	sptrIFrameWindow spF( m_punkTarget );

	if( spF == NULL )
		return false;

	IUnknown* punkView = NULL;
	spF->GetActiveView( &punkView );

	if( punkView == NULL )
		return false;
	
	sptrIBlankView sptrV( punkView );

	punkView->Release();

	if( sptrV == NULL )
		return false;

	sptrV->SetMode( (short)bvmPreview );

	return true;

}

bool CActionShowBlankPreview::IsAvaible()
{
	sptrIFrameWindow2 spF( m_punkTarget );
	if (spF != 0)
	{
		BOOL bLockedFlag;
		spF->GetLockedFlag(&bLockedFlag);
		if (bLockedFlag)
			return FALSE;
	}

	sptrIApplication sptrA( GetAppUnknown( ) );
	if( sptrA == NULL )
		return false;
	
	IUnknown* punk = NULL;
	sptrA->GetActiveDocument( &punk );
	if( punk == NULL )
		return false;

	sptrIDBaseDocument spIDBDoc = punk;

	punk->Release();

	if( spIDBDoc != NULL )
		return true;

	return false;	
}

bool CActionShowBlankPreview::IsChecked()
{
	sptrIFrameWindow spF( m_punkTarget );

	if( spF == NULL )
		return false;

	IUnknown* punkView = NULL;
	spF->GetActiveView( &punkView );

	if( punkView == NULL )
		return false;
	
	sptrIBlankView sptrV( punkView );

	punkView->Release();

	if( sptrV == NULL )
		return false;

	
	
	short nBlankViewMode;
	sptrV->GetMode( &nBlankViewMode );

	if( (BlankViewMode)nBlankViewMode == bvmPreview )
		return true;

	return false;

}

CString CActionShowBlankPreview::GetViewProgID()
{
	return szBlankViewProgID;
}


//////////////////////////////////////////////////////////////////////
//CActionShowBlankDesign implementation

bool CActionShowBlankDesign::Invoke()
{
	if( !CActionShowViewBase::Invoke() )
		return false;

	sptrIFrameWindow spF( m_punkTarget );

	if( spF == NULL )
		return false;

	IUnknown* punkView = NULL;
	spF->GetActiveView( &punkView );

	if( punkView == NULL )
		return false;
	
	sptrIBlankView sptrV( punkView );

	punkView->Release();

	if( sptrV == NULL )
		return false;

	sptrV->SetMode( (short) bvmDesign );
	
	
	return true;

}

bool CActionShowBlankDesign::IsAvaible()
{
	sptrIApplication sptrA( GetAppUnknown( ) );
	if( sptrA == NULL )
		return false;
	
	IUnknown* punk = NULL;
	sptrA->GetActiveDocument( &punk );
	if( punk == NULL )
		return false;

	sptrIDBaseDocument spIDBDoc = punk;

	punk->Release();

	if( spIDBDoc != NULL )
		return true;

	return false;
}

bool CActionShowBlankDesign::IsChecked()
{
	sptrIFrameWindow spF( m_punkTarget );

	if( spF == NULL )
		return false;

	IUnknown* punkView = NULL;
	spF->GetActiveView( &punkView );

	if( punkView == NULL )
		return false;
	
	sptrIBlankView sptrV( punkView );

	punkView->Release();

	if( sptrV == NULL )
		return false;

	short nBlankViewMode;
	sptrV->GetMode( &nBlankViewMode );

	if( (BlankViewMode)nBlankViewMode == bvmDesign )
		return true;

	return false;
}



CString CActionShowBlankDesign::GetViewProgID()
{
	return szBlankViewProgID;
}





//////////////////////////////////////////////////////////////////////
//CActionChoiseQuery implementation
CActionChoiseQuery::CActionChoiseQuery()
{
}

CActionChoiseQuery::~CActionChoiseQuery()
{
}

bool CActionChoiseQuery::Invoke()
{
	CREATE_DB_DOCSITE()//spDBDoc - Document


	ISelectQueryPtr ptrOldQ;
	IUnknown* punk = 0;
	spDBDoc->GetActiveQuery( &punk );
	if( punk )
	{
		ptrOldQ = punk;
		punk->Release();	punk = 0;	
	}		
	
	

	CChoiceActiveQuery dlg( ::GetMainFrameWnd() );
	dlg.SetDBaseDocument( spDBDoc );
	if( dlg.DoModal() == IDOK )
	{
		if( !dlg.m_strActiveQuery.IsEmpty() )
		{
			IUnknown* punkNewQ = ::GetObjectByName( spDBDoc, (LPCSTR)dlg.m_strActiveQuery, 0 );
			if( punkNewQ )
			{
				ISelectQueryPtr ptrNewQ( punkNewQ );
				punkNewQ->Release();

				if( ptrNewQ )
				{
					if( ptrOldQ )
						ptrOldQ->Close();

					spDBDoc->SetActiveQuery( ptrNewQ );
					ptrNewQ->Open();

					::SetModifiedFlagToDoc( spDBDoc );
				}
			}						
		}
	}
	
	return true;
}

//extended UI
bool CActionChoiseQuery::IsAvaible()
{
	CREATE_DB_DOCSITE()//spDBDoc - Document
	return true;
}

bool CActionChoiseQuery::IsChecked()
{
	return false;
}



//////////////////////////////////////////////////////////////////////
//CActionCloseQuery implementation
CActionCloseQuery::CActionCloseQuery()
{
}

CActionCloseQuery::~CActionCloseQuery()
{
}

bool CActionCloseQuery::Invoke()
{
	GET_ACTIVE_QUERY_FROM_DOC() // spQueryData - active Query data,spSelectQuery - select query

#ifdef _DEBUG
	{
		long	lpos;
		INamedDataPtr	ptrNamedData( m_punkTarget );
		ptrNamedData->GetBaseGroupFirstPos( &lpos );
		while( lpos )
		{
			GUID guid;
			ptrNamedData->GetNextBaseGroup( &guid, &lpos );
			long	lpos1;
			ptrNamedData->GetBaseGroupObjectFirstPos( &guid, &lpos1 );
			while( lpos1 )
			{
				IUnknown	*punk = 0;
				ptrNamedData->GetBaseGroupNextObject( &guid, &lpos1, &punk );
				punk->Release();

				GUID	base;
				INamedDataObject2Ptr	ptrNamed( punk );
				ptrNamed->GetBaseKey( &base );

				CString	str = GetObjectName( punk );

				ASSERT( base == guid );
			}
		}
	}
#endif //_DEBUG

	
	spSelectQuery->UpdateInteractive( TRUE );
	spSelectQuery->Close();
	
#ifdef _DEBUG
	{
		long	lpos;
		INamedDataPtr	ptrNamedData( m_punkTarget );
		ptrNamedData->GetBaseGroupFirstPos( &lpos );
		while( lpos )
		{
			GUID guid;
			ptrNamedData->GetNextBaseGroup( &guid, &lpos );
			long	lpos1;
			ptrNamedData->GetBaseGroupObjectFirstPos( &guid, &lpos1 );
			while( lpos1 )
			{
				IUnknown	*punk = 0;
				ptrNamedData->GetBaseGroupNextObject( &guid, &lpos1, &punk );
				punk->Release();
			}
		}
	}
#endif //_DEBUG
	return true;
}

//extended UI
bool CActionCloseQuery::IsAvaible()
{
	GET_ACTIVE_QUERY_FROM_DOC() // spQueryData - active Query data,spSelectQuery - select query
	short nState;
	spSelectQuery->GetState( &nState );
	if( (QueryState)nState == qsClose )
		return false;

	return true;
}

bool CActionCloseQuery::IsChecked()
{
	GET_ACTIVE_QUERY_FROM_DOC() // spQueryData - active Query data,spSelectQuery - select query
	short nState;
	spSelectQuery->GetState( &nState );
	if( (QueryState)nState == qsClose )
		return true;
	
	return false;
}

//////////////////////////////////////////////////////////////////////
//CActionOpenQuery implementation
CActionOpenQuery::CActionOpenQuery()
{
}

CActionOpenQuery::~CActionOpenQuery()
{
}

bool CActionOpenQuery::Invoke()
{
	GET_ACTIVE_QUERY_FROM_DOC() // spQueryData - active Query data,spSelectQuery - select query
	spSelectQuery->Open();
	return true;
}

//extended UI
bool CActionOpenQuery::IsAvaible()
{
	GET_ACTIVE_QUERY_FROM_DOC() // spQueryData - active Query data,spSelectQuery - select query
	short nState;
	spSelectQuery->GetState( &nState );
	if( (QueryState)nState == qsClose )
		return true;

	return false;
}

bool CActionOpenQuery::IsChecked()
{
	GET_ACTIVE_QUERY_FROM_DOC() // spQueryData - active Query data,spSelectQuery - select query
	short nState;
	spSelectQuery->GetState( &nState );
	if( (QueryState)nState != qsClose )
		return true;

	return false;
}

//////////////////////////////////////////////////////////////////////
//CActionCancel implementation
CActionCancel::CActionCancel()
{
}

CActionCancel::~CActionCancel()
{
}

bool CActionCancel::Invoke()
{
	GET_ACTIVE_QUERY_FROM_DOC() // spQueryData - active Query data,spSelectQuery - select query
	spSelectQuery->Cancel();
	return true;
}

//extended UI
bool CActionCancel::IsAvaible()
{
	GET_ACTIVE_QUERY_FROM_DOC()// spQueryData - active Query data,spSelectQuery - select query	

	long nRecordCount = 0;
	spSelectQuery->GetRecordCount( &nRecordCount );
	if( nRecordCount < 1 )
		return false;

	short nState;
	spSelectQuery->GetState( &nState );
	if( (QueryState)nState == qsEdit || (QueryState)nState == qsInsert )
		return true;
	
	return false;
}

bool CActionCancel::IsChecked()
{
	return false;
}

//////////////////////////////////////////////////////////////////////
//CActionUpdate implementation
CActionUpdate::CActionUpdate()
{
}

CActionUpdate::~CActionUpdate()
{
}

bool CActionUpdate::Invoke()
{
	GET_ACTIVE_QUERY_FROM_DOC() // spQueryData - active Query data,spSelectQuery - select query
	spSelectQuery->Update();

	return true;
}

//extended UI
bool CActionUpdate::IsAvaible()
{
	GET_ACTIVE_QUERY_FROM_DOC()// spQueryData - active Query data,spSelectQuery - select query	
	
	BOOL bReadOnly = FALSE;
	spDBDoc->IsReadOnly( &bReadOnly );
	if( bReadOnly )	return false;

	long nRecordCount = 0;
	spSelectQuery->GetRecordCount( &nRecordCount );
	if( nRecordCount < 1 )
		return false;

	short nState;
	spSelectQuery->GetState( &nState );
	if( (QueryState)nState == qsEdit || (QueryState)nState == qsInsert )
		return true;

	return false;
}

bool CActionUpdate::IsChecked()
{
	return false;
}

//////////////////////////////////////////////////////////////////////
//CActionEditRecord implementation
CActionEditRecord::CActionEditRecord()
{
}

CActionEditRecord::~CActionEditRecord()
{
}

bool CActionEditRecord::Invoke()
{
	GET_ACTIVE_QUERY_FROM_DOC()// spQueryData - active Query data,spSelectQuery - select query	
	spSelectQuery->Edit();
	return true;
}

//extended UI
bool CActionEditRecord::IsAvaible()
{
	GET_ACTIVE_QUERY_FROM_DOC()// spQueryData - active Query data,spSelectQuery - select query	

	BOOL bReadOnly = FALSE;
	spDBDoc->IsReadOnly( &bReadOnly );
	if( bReadOnly )	return false;


	long nRecordCount = 0;
	spSelectQuery->GetRecordCount( &nRecordCount );
	if( nRecordCount < 1 )
		return false;
		
	short nState;
	spSelectQuery->GetState( &nState );
	if( (QueryState)nState == qsBrowse )
		return true;		


	return false;
}

bool CActionEditRecord::IsChecked()
{
	GET_ACTIVE_QUERY_FROM_DOC()// spQueryData - active Query data,spSelectQuery - select query	

	short nState;
	spSelectQuery->GetState( &nState );
	if( (QueryState)nState == qsEdit || (QueryState)nState == qsInsert )
		return true;		

	return false;

}


//////////////////////////////////////////////////////////////////////
//CActionInsertRecord implementation
CActionInsertRecord::CActionInsertRecord()
{
}

CActionInsertRecord::~CActionInsertRecord()
{
}

bool CActionInsertRecord::Invoke()
{
	GET_ACTIVE_QUERY_FROM_DOC()// spQueryData - active Query data,spSelectQuery - select query	
	spSelectQuery->Insert();
	return true;
}

//extended UI
bool CActionInsertRecord::IsAvaible()
{
	GET_ACTIVE_QUERY_FROM_DOC()// spQueryData - active Query data,spSelectQuery - select query	

	BOOL bReadOnly = FALSE;
	spDBDoc->IsReadOnly( &bReadOnly );
	if( bReadOnly )	return false;
		
	short nState = 0;
	spSelectQuery->GetState( &nState );
	if( (QueryState)nState != qsClose )
		return true;
	
	return false;
}

bool CActionInsertRecord::IsChecked()
{
	GET_ACTIVE_QUERY_FROM_DOC()// spQueryData - active Query data,spSelectQuery - select query	

	short nState;
	spSelectQuery->GetState( &nState );
	if( (QueryState)nState == qsInsert )
		return true;

	return false;	
}

//////////////////////////////////////////////////////////////////////
//CActionDeleteRecord implementation
CActionDeleteRecord::CActionDeleteRecord()
{
}

CActionDeleteRecord::~CActionDeleteRecord()
{
}

bool CActionDeleteRecord::Invoke()
{
	GET_ACTIVE_QUERY_FROM_DOC()// spQueryData - active Query data,spSelectQuery - select query	
	spSelectQuery->Delete();
	return true;
}

//extended UI
bool CActionDeleteRecord::IsAvaible()
{
	GET_ACTIVE_QUERY_FROM_DOC()// spQueryData - active Query data,spSelectQuery - select query	

	BOOL bReadOnly = FALSE;
	spDBDoc->IsReadOnly( &bReadOnly );
	if( bReadOnly )	return false;

	long nRecordCount = 0;
	spSelectQuery->GetRecordCount( &nRecordCount );
	if( nRecordCount < 1 )
		return false;

	return true;
}

bool CActionDeleteRecord::IsChecked()
{
	return false;
}

//////////////////////////////////////////////////////////////////////
//CActionMovePrev implementation
CActionMovePrev::CActionMovePrev()
{
}

CActionMovePrev::~CActionMovePrev()
{
}

bool CActionMovePrev::Invoke()
{
	GET_ACTIVE_QUERY_FROM_DOC()// spQueryData - active Query data,spSelectQuery - select query	
	spSelectQuery->MovePrev();
	return true;
}

//extended UI
bool CActionMovePrev::IsAvaible()
{
	GET_ACTIVE_QUERY_FROM_DOC()// spQueryData - active Query data,spSelectQuery - select query	
	BOOL bAvailable = FALSE;
	spSelectQuery->IsBOF( &bAvailable );
	if( bAvailable )
		return false;
	return true;
}

bool CActionMovePrev::IsChecked()
{
	return false;
}

//////////////////////////////////////////////////////////////////////
//CActionMoveNext implementation
CActionMoveNext::CActionMoveNext()
{
}

CActionMoveNext::~CActionMoveNext()
{
}

bool CActionMoveNext::Invoke()
{
	GET_ACTIVE_QUERY_FROM_DOC()// spQueryData - active Query data,spSelectQuery - select query	
	spSelectQuery->MoveNext();
	return true;
}

//extended UI
bool CActionMoveNext::IsAvaible()
{
	GET_ACTIVE_QUERY_FROM_DOC()// spQueryData - active Query data,spSelectQuery - select query	
	BOOL bAvailable = FALSE;
	spSelectQuery->IsEOF( &bAvailable );
	if( bAvailable )
		return false;
	return true;
}

bool CActionMoveNext::IsChecked()
{
	return false;
}

//////////////////////////////////////////////////////////////////////
//CActionMoveLast implementation
CActionMoveLast::CActionMoveLast()
{
}

CActionMoveLast::~CActionMoveLast()
{
}

bool CActionMoveLast::Invoke()
{
	GET_ACTIVE_QUERY_FROM_DOC()// spQueryData - active Query data,spSelectQuery - select query	
	spSelectQuery->MoveLast();
	return true;
}

//extended UI
bool CActionMoveLast::IsAvaible()
{
	GET_ACTIVE_QUERY_FROM_DOC()// spQueryData - active Query data,spSelectQuery - select query	
	BOOL bAvailable = FALSE;
	spSelectQuery->IsEOF( &bAvailable );
	if( bAvailable )
		return false;
	return true;
}

bool CActionMoveLast::IsChecked()
{
	return false;
}

//////////////////////////////////////////////////////////////////////
//CActionMoveFirst implementation
CActionMoveFirst::CActionMoveFirst()
{
}

CActionMoveFirst::~CActionMoveFirst()
{
}

bool CActionMoveFirst::Invoke()
{
	GET_ACTIVE_QUERY_FROM_DOC()// spQueryData - active Query data,spSelectQuery - select query	
	spSelectQuery->MoveFirst();
	return true;
}

//extended UI
bool CActionMoveFirst::IsAvaible()
{
	GET_ACTIVE_QUERY_FROM_DOC()// spQueryData - active Query data,spSelectQuery - select query	
	BOOL bAvailable = FALSE;
	spSelectQuery->IsBOF( &bAvailable );
	if( bAvailable )
		return false;
	return true;
}

bool CActionMoveFirst::IsChecked()
{
	return false;
}



//////////////////////////////////////////////////////////////////////
//CDBaseActionBase implementation
CDBaseActionBase::CDBaseActionBase()
{

}

CDBaseActionBase::~CDBaseActionBase()
{

}

bool CDBaseActionBase::Invoke()
{
	return true;
}

bool CDBaseActionBase::IsAvaible()
{
	return IsDBaseDocument();
}
bool CDBaseActionBase::IsChecked()
{
	return false;
}
bool CDBaseActionBase::IsDBaseDocument()
{
	sptrIDocumentSite	sptrDocSite( m_punkTarget );
	if( CheckInterface(sptrDocSite, IID_IDBaseDocument) )
		return true;
	else
		return false;
}

//////////////////////////////////////////////////////////////////////
//CActionQueryWizard implementation
CActionQueryWizard::CActionQueryWizard()
{
}

CActionQueryWizard::~CActionQueryWizard()
{
}

bool CActionQueryWizard::Invoke()
{
	GET_ACTIVE_QUERY_FROM_DOC();

	CWizardPool wp;
	if( !wp.QueryProperties( spDBDoc, spSelectQuery, true ) )
		return false;

	::SetModifiedFlagToObj( spSelectQuery );

	return true;
}

//////////////////////////////////////////////////////////////////////
//CActionGalleryOptions implementation
CActionGalleryOptions::CActionGalleryOptions()
{
}

CActionGalleryOptions::~CActionGalleryOptions()
{
}

bool CActionGalleryOptions::Invoke()
{
	GET_ACTIVE_QUERY_FROM_DOC();

	CWizardPool wp;
	if( !wp.QueryProperties( spDBDoc, spSelectQuery, false ) )
		return false;

	::SetModifiedFlagToObj( spSelectQuery );
	return true;
}


//////////////////////////////////////////////////////////////////////
//CActionTableWizard implementation
CActionTableWizard::CActionTableWizard()
{
}

CActionTableWizard::~CActionTableWizard()
{
}

bool CActionTableWizard::Invoke()
{
	CREATE_DB_DOCSITE();

	//AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	//::ExecuteAction( GetActionNameByResID( IDS_QUERY_CLOSE ) );	

	ISelectQueryPtr ptrQuery;	
	{
		IUnknown* punk = 0;
		spDBDoc->GetActiveQuery( &punk );
		if( punk )
		{
			ptrQuery = punk;
			punk->Release();	punk = 0;

			BOOL bopen = FALSE;
			if( ptrQuery ) 
			{
				ptrQuery->IsOpen( &bopen );
				if( !bopen )
				{
					ptrQuery = 0;
				}
				else
				{					
					ptrQuery->UpdateInteractive( TRUE );
					ptrQuery->Close();
				}
			}			
		}
	}

	CDBStructDlg dlg( ::GetMainFrameWnd() );
	dlg.SetDBaseDoc( spDBDoc );
	int nres = dlg.DoModal( );

	if( nres == IDOK )
	{
		::SetModifiedFlagToDoc( m_punkTarget );

		SaveDBaseDocument( spDBDoc );
	}


	if( ptrQuery )
		ptrQuery->Open();
	//::ExecuteAction( GetActionNameByResID( IDS_QUERY_OPEN ) );

	//spDBDoc->TableWizard();

	return true;
}


//////////////////////////////////////////////////////////////////////
//CActionLoadDatabase implementation
CActionLoadDatabase::CActionLoadDatabase()
{
}

CActionLoadDatabase::~CActionLoadDatabase()
{
}

bool CActionLoadDatabase::Invoke()
{
	CREATE_DB_DOCSITE();

	IDBConnectionPtr ptrDBC( spDBDoc );
	if( ptrDBC == 0 )
		return false;

	BSTR bstrMDBFileLocation = 0;
	ptrDBC->GetMDBFileName( &bstrMDBFileLocation );

	CString strMDBFileLocation = bstrMDBFileLocation;

	if( bstrMDBFileLocation )
		::SysFreeString( bstrMDBFileLocation );

	CFileDialog dlg(TRUE, 0, 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 0, 0, _FILE_OPEN_SIZE_ );
	strcpy(dlg.m_ofn.lpstrFile, strMDBFileLocation);
	dlg.m_ofn.lpstrFilter = "MSAccess files(*.mdb)\0*.mdb\0\0";
	if(dlg.DoModal() == IDOK)
	{
		strMDBFileLocation = dlg.GetPathName();		
		ptrDBC->LoadAccessDBase( _bstr_t( (LPCTSTR)strMDBFileLocation ) );
		
		::SetModifiedFlagToDoc( m_punkTarget );
	}	

	return true;
}

//extended UI
bool CActionLoadDatabase::IsAvaible()
{
	sptrIDocumentSite	sptrDocSite( m_punkTarget );
	if( CheckInterface(sptrDocSite, IID_IDBaseDocument) )
		return true;
	else
		return false;
}

bool CActionLoadDatabase::IsChecked()
{
	return false;
}




//////////////////////////////////////////////////////////////////////
//CActionDBaseInfo implementation
CActionDBaseInfo::CActionDBaseInfo()
{
}

CActionDBaseInfo::~CActionDBaseInfo()
{
}



bool CActionDBaseInfo::Invoke()
{
	sptrIDBaseDocument spDBaseDoc( m_punkTarget );
	if( spDBaseDoc == NULL )
		return false;

	CDBaseInfoDlg dlg( ::GetMainFrameWnd() );
	dlg.SetDBaseDocument( spDBaseDoc );
	dlg.DoModal();

	return true;
}

//extended UI
bool CActionDBaseInfo::IsAvaible()
{
	sptrIDBaseDocument spDBaseDoc( m_punkTarget );
	if( spDBaseDoc == NULL )
		return false;

	return true;
}

bool CActionDBaseInfo::IsChecked()
{
	return false;
}


//////////////////////////////////////////////////////////////////////
//CActionAdvancedConnection implementation
bool CActionAdvancedConnection::Invoke()
{
	CREATE_DB_DOCSITE();

	IDBConnectionPtr ptrDBC( spDBDoc );
	if( ptrDBC == 0 )
		return false;
	
	ptrDBC->CreateAdvancedConnection();

	::SetModifiedFlagToDoc( m_punkTarget );

	return true;
}




//////////////////////////////////////////////////////////////////////
//CActionAddSortFieldAsc implementation
bool CActionAddSortFieldAsc::IsAvaible()
{
	sptrIDBFilterView spFilterV( m_punkTarget );
	if( spFilterV == NULL )
		return false;

	BSTR bstrTable = 0;
	BSTR bstrField = 0;

	if( S_OK != spFilterV->GetActiveField( &bstrTable, &bstrField ) )
		return false;

	bool bCanSortOnField = false;
	IViewPtr ptrView( spFilterV );
	if( ptrView )
	{
		IUnknown* punkDoc = 0;
		ptrView->GetDocument( &punkDoc );

		if( punkDoc )
		{
			FieldType ft = ::GetFieldType( punkDoc, bstrTable, bstrField );
			punkDoc->Release();

			bCanSortOnField = ( ft == ftDigit || ft == ftString || ft == ftDateTime );
		}
	}


	if( bstrTable )
		::SysFreeString( bstrTable );	bstrTable = 0;

	if( bstrField )
		::SysFreeString( bstrField );	bstrField = 0;

	if( !bCanSortOnField )
		return false;

	ISelectQueryPtr  ptrSelectQ = GetActiveQueryFromViewTarget( );

	if( ptrSelectQ == 0 )
		return false;

	BOOL bOpen = false;
	ptrSelectQ->IsOpen( &bOpen );	

	return ( bOpen == TRUE );

}

//////////////////////////////////////////////////////////////////////
bool CActionAddSortFieldAsc::Invoke()
{

	sptrIDBFilterView spFilterV( m_punkTarget );
	if( spFilterV == NULL )
		return false;

	BSTR bstrField = 0;
	BSTR bstrTable = 0;	
	
	
	if( S_OK != spFilterV->GetActiveField( &bstrTable, &bstrField ) )
		return false;

	_bstr_t _bstrTable( bstrTable );
	_bstr_t _bstrField( bstrField );

	if( bstrField )
		::SysFreeString( bstrField );

	if( bstrTable )
		::SysFreeString( bstrTable );


	ISelectQueryPtr  ptrSelectQ = GetActiveQueryFromViewTarget( );

	if( ptrSelectQ == 0 )
		return false;


	ptrSelectQ->AddSortField( _bstrTable, _bstrField, GetSortOrder() );


	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	::ExecuteAction( GetActionNameByResID( IDS_REQUERY ) );	

	::SetModifiedFlagToObj( ptrSelectQ );

	return true;

}


//////////////////////////////////////////////////////////////////////
//CActionAddSortFieldDesc implementation

//////////////////////////////////////////////////////////////////////
//CActionAddSortFieldDesc implementation
bool CActionRemoveSortFields::IsAvaible()
{
	ISelectQueryPtr  ptrSelectQ = GetActiveQuery( );

	if( ptrSelectQ == 0 )
		return false;

	int nNum = 0;
	ptrSelectQ->GetSortFieldsCount( &nNum );

	return nNum > 0;

}

//////////////////////////////////////////////////////////////////////
bool CActionRemoveSortFields::Invoke()
{
	ISelectQueryPtr  ptrSelectQ = GetActiveQuery( );

	if( ptrSelectQ == 0 )
		return false;

	ptrSelectQ->RemoveSortFields();

	::SetModifiedFlagToObj( ptrSelectQ );


	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	::ExecuteAction( GetActionNameByResID( IDS_REQUERY ) );


	return true;

}

//////////////////////////////////////////////////////////////////////
bool CActionRecordInfo::Invoke()
{
	GET_ACTIVE_QUERY_FROM_DOC()// spQueryData - active Query data,spSelectQuery - select query		


	CRecordInfoDlg dlg;
	dlg.SetQuery( spSelectQuery );
	dlg.DoModal( );

	return true;
}

//////////////////////////////////////////////////////////////////////
bool CActionRecordInfo::IsAvaible()
{
	GET_ACTIVE_QUERY_FROM_DOC()// spQueryData - active Query data,spSelectQuery - select query		

	long lCurRecord = 0;
	spSelectQuery->GetCurrentRecord( &lCurRecord );

	return lCurRecord > 0;
}

//////////////////////////////////////////////////////////////////////
CActionFieldProperties::CActionFieldProperties()
{

}

//////////////////////////////////////////////////////////////////////
bool CActionFieldProperties::Invoke()
{
	IDBaseStructPtr	ptrDBaseDoc( m_punkTarget );
	if( ptrDBaseDoc == 0 )
		return false;

	IUnknownPtr ptrAXData = GetActiveControlData( );
	IUnknownPtr ptrAXCtrl = GetActiveAXControl( ptrAXData );

	if( ptrAXData == 0 || ptrAXCtrl == 0 )
		return false;

	IDBControlPtr ptrDBCtrl( ptrAXCtrl );
	if( ptrDBCtrl == 0 )
		return false;

	BSTR bstrTable = 0;
	BSTR bstrField = 0;

	ptrDBCtrl->GetTableName( &bstrTable );
	ptrDBCtrl->GetFieldName( &bstrField );

	if( bstrTable == 0 )	return false;
	if( bstrField == 0 )	return false;

	m_bstrTable = bstrTable;
	m_bstrField = bstrField;

	if( bstrTable )
		::SysFreeString( bstrTable );	bstrTable = 0;

	if( bstrField )
		::SysFreeString( bstrField );	bstrField = 0;


	ptrDBCtrl->GetAutoLabel( &m_nAutoLabel );
	ptrDBCtrl->GetLabelPosition( &m_nLabelPos );
	ptrDBCtrl->GetEnableEnumeration( &m_bEnableEnumeration );



	BSTR bstrUserName = 0;
	BOOL bDef = FALSE;
	BSTR bstrDefValue = 0;
	BSTR bstrVT5ObjType = 0;
	short nFieldType = 0;
	if( S_OK != ptrDBaseDoc->GetFieldInfo( 
			m_bstrTable, m_bstrField, 
			&bstrUserName, &nFieldType,
			0, 0,
			0, &bDef,
			&bstrDefValue, &bstrVT5ObjType
		) )
		return false;

	m_bstrCaption	= bstrUserName;
	m_bHasDefValue	= bDef;
	m_bstrDefValue	= bstrDefValue;

	if( bstrUserName )
		::SysFreeString( bstrUserName );	bstrUserName = 0;

	if( bstrDefValue )
		::SysFreeString( bstrDefValue );	bstrDefValue = 0;	


	CString strTitle;
	strTitle.LoadString( IDS_FIELD_PROPERTIES );	

	CPropertySheet		ps;
	CFieldProp			field_pp;
	CObjFieldPropPage	obj_field_pp;

	ps.m_psh.dwFlags |= PSH_NOAPPLYNOW;

	ps.AddPage( &field_pp );

	if( nFieldType == ftVTObject && ::GetValueInt(::GetAppUnknown(), SHELL_DATA_DB_SECTION,
		"EnableObjectProperties", 1) )
		ps.AddPage( &obj_field_pp );

	ps.SetTitle( strTitle );

	field_pp.m_bAutoLabel			= m_nAutoLabel;
	field_pp.m_strCaption			= (LPCSTR)m_bstrCaption;
	field_pp.m_strDefValue			= (LPCSTR)m_bstrDefValue;
	field_pp.m_bEnableEnumeration	= m_bEnableEnumeration;
	field_pp.m_strField				= (LPCSTR)m_bstrField;
	field_pp.m_strTable				= (LPCSTR)m_bstrTable;
	field_pp.m_bHasDefValue			= m_bHasDefValue;
	field_pp.m_nLabelPos				= m_nLabelPos;	

	field_pp.SetDocument( ptrDBaseDoc ); 



	IDBObjectControlPtr ptrDBaseObj( ptrDBCtrl );
	if( ptrDBaseObj )
	{
		obj_field_pp.SetBaseType( bstrVT5ObjType );

		BSTR bstrType = 0;
		ptrDBaseObj->GetViewType( &bstrType );
		obj_field_pp.m_strViewType = bstrType;

		if( bstrType )
			::SysFreeString( bstrType );	bstrType = 0;

		ptrDBaseObj->GetAutoViewType( &obj_field_pp.m_bAutoAsign );
	}	
	

	if( bstrVT5ObjType )
		::SysFreeString( bstrVT5ObjType );

	if( ps.DoModal() != IDOK )
		return false;

	{
		short nFieldType = 0;
		BOOL bRequiredValue = FALSE;
		BSTR bstrVTObjectType = 0;

		if( S_OK != ptrDBaseDoc->GetFieldInfo( 
			m_bstrTable, m_bstrField, 
			0, &nFieldType,
			0, 0,
			&bRequiredValue, 0,
			0, &bstrVTObjectType
			) )
			return false;


		HRESULT hRes = ptrDBaseDoc->SetFieldInfo( 
				m_bstrTable, m_bstrField, _bstr_t( (LPCSTR)field_pp.m_strCaption ),
				bRequiredValue, field_pp.m_bHasDefValue,
				_bstr_t( (LPCSTR)field_pp.m_strDefValue ), bstrVTObjectType
			);

		if( bstrVTObjectType )
			::SysFreeString( bstrVTObjectType );

		VERIFY( hRes == S_OK );
	}


	ptrDBCtrl->SetAutoLabel( field_pp.m_bAutoLabel );
	ptrDBCtrl->SetLabelPosition( field_pp.m_nLabelPos );
	ptrDBCtrl->SetEnableEnumeration( field_pp.m_bEnableEnumeration );


	if( ptrDBaseObj )
	{
		ptrDBaseObj->SetViewType( _bstr_t( obj_field_pp.m_strViewType ) );
		ptrDBaseObj->SetAutoViewType( obj_field_pp.m_bAutoAsign );
	}

	::StoreContainerToDataObject( ptrAXData, ptrAXCtrl );

	::SetModifiedFlagToDoc( m_punkTarget );

	if( ptrDBaseObj )
		ptrDBaseObj->Build();

	return true;
}

//////////////////////////////////////////////////////////////////////
bool CActionFieldProperties::IsAvaible()
{
	if( !CheckInterface( m_punkTarget, IID_IDBaseDocument) )
		return false;

	if( !CheckInterface( GetActiveAXControl( GetActiveControlData( ) ), IID_IDBControl ) )
		return false;
	
	return true;
}

/*
//////////////////////////////////////////////////////////////////////
bool CActionFieldProperties::DoUndo()
{
	return true;
}

//////////////////////////////////////////////////////////////////////
bool CActionFieldProperties::DoRedo()
{
	return true;
}
*/

//////////////////////////////////////////////////////////////////////
IBlankViewPtr CActionFieldProperties::GetActiveBlankView()
{
	sptrIDocumentSite	sptrDocSite( m_punkTarget );
	IUnknown	*punkV = 0;
	sptrDocSite->GetActiveView( &punkV );

	if( !punkV )
		return 0;

	IBlankViewPtr ptrBV = punkV;
	punkV->Release();	punkV = 0;

	return ptrBV;
}

//////////////////////////////////////////////////////////////////////
IUnknownPtr CActionFieldProperties::GetActiveControlData(  )
{
	IBlankViewPtr ptrV( GetActiveBlankView() );
	if( ptrV == 0 )
		return 0;

	IUnknown *punkObj = 0;
	ptrV->GetActiveAXData( &punkObj );

	IUnknownPtr ptrObj = punkObj;

	if( punkObj )
		punkObj->Release();	punkObj = 0;

	return ptrObj;
}

//////////////////////////////////////////////////////////////////////
IUnknownPtr CActionFieldProperties::GetActiveAXControl( IUnknown* punkAXData )
{
	if( punkAXData == 0 )
		return 0;

	ILayoutViewPtr	ptrLView( GetActiveBlankView() );
	if( ptrLView == 0 )
		return 0;	

	IUnknown	*punkAXCtrl = 0;
	ptrLView->GetControlObject( punkAXData, &punkAXCtrl );

	if( punkAXCtrl == 0 )
		return 0;

	IUnknownPtr ptrAXCtrl = punkAXCtrl;
	punkAXCtrl->Release();	punkAXCtrl = 0;

	return ptrAXCtrl;
}

//////////////////////////////////////////////////////////////////////
//CActionDeleteRecords implementation
CActionDeleteRecords::CActionDeleteRecords(){}

CActionDeleteRecords::~CActionDeleteRecords(){}

bool CActionDeleteRecords::Invoke()
{
	GET_ACTIVE_QUERY_FROM_DOC();

	CString str_sql = get_delete_sql();
	if( str_sql.IsEmpty() )
		return false;

	//show message box	
	long lcount = 0;
	spSelectQuery->GetRecordCount( &lcount );
	CString str_warning;
	str_warning.Format( IDS_DELETE_WARNING, lcount );

	if( GetArgumentInt( "Question" ) )
		if( IDYES != AfxMessageBox( str_warning, MB_ICONSTOP|MB_YESNO ) )
			return false;


	IDBConnectionPtr ptr_db_conn = spDBDoc;
	if( ptr_db_conn == 0 )	return false;

	//close recordset
	spSelectQuery->Close();		

	//execute SQL
	try
	{
		IUnknown* punk = 0;
		ptr_db_conn->GetConnection( &punk );
		if( !punk )	return false;
		ADO::_ConnectionPtr ptr_ado_conn = punk;
		punk->Release();	punk = 0;
		if( ptr_ado_conn == 0 )	return false;

		ptr_ado_conn->Execute( _bstr_t( (LPCSTR)str_sql ), NULL, ADO::adCmdText );
	}
	catch (_com_error &e)
	{
		dump_com_error(e);						
	}		

	//open
	spSelectQuery->Open();

	return true;
}

bool CActionDeleteRecords::IsAvaible()
{
	CString str = get_delete_sql();
	if( str.IsEmpty() )
		return false;

	return true;
}

CString CActionDeleteRecords::get_delete_sql()
{
	sptrIDBaseDocument spDBDoc( m_punkTarget );
	if( spDBDoc == NULL )	return "";

	BOOL bReadOnly = FALSE;
	spDBDoc->IsReadOnly( &bReadOnly );
	if( bReadOnly )	return "";


	IUnknown* pUnkQuery = NULL;
	spDBDoc->GetActiveQuery( &pUnkQuery );
					 
	if( pUnkQuery == NULL )		return "";
					 
	sptrISelectQuery spSelectQuery( pUnkQuery );
	pUnkQuery->Release();	pUnkQuery = 0;

	BOOL bopen = FALSE;
	spSelectQuery->IsOpen( &bopen );
	if( !bopen )	return "";

	long lcount = 0;
	spSelectQuery->GetRecordCount( &lcount );
	if( !lcount )	return "";

	if( spSelectQuery == NULL )		return "";
	
	IQueryObjectPtr ptr_query = spSelectQuery;
	BSTR bstr_sql = 0;
	ptr_query->GetSQL( &bstr_sql );
	if( !bstr_sql )	return "";

	CString str_sql = bstr_sql;
	str_sql.MakeLower();
	::SysFreeString( bstr_sql );	bstr_sql = 0;
	if( 0 != str_sql.Find( "select" ) )		return "";

	if( -1 != str_sql.Find( " group " ) )		return "";
	
	int nidx = str_sql.Find( " order " );
	if( nidx != -1 )
		str_sql.SetAt( nidx, 0 );
	
	str_sql.SetAt( 0, 'd' );
	str_sql.SetAt( 1, 'e' );
	str_sql.SetAt( 2, 'l' );
	str_sql.SetAt( 3, 'e' );
	str_sql.SetAt( 4, 't' );
	str_sql.SetAt( 5, 'e' );

	return str_sql;
}

//////////////////////////////////////////////////////////////////////
//CActionDeleteRecords implementation
CActionUpdateRecordsetInfo::CActionUpdateRecordsetInfo(){}

CActionUpdateRecordsetInfo::~CActionUpdateRecordsetInfo(){}

bool CActionUpdateRecordsetInfo::Invoke()
{
	GET_ACTIVE_QUERY_FROM_DOC();

	//show message box	
	long lcount = 0;
	spSelectQuery->GetRecordCount( &lcount );

	CString strConn;
	
	strConn.Format( "%ld", lcount );

	_SetValue( m_punkTarget, SECTION_INFO, SECTION_RECORDCOUNT, strConn );

	return true;
}

bool CActionUpdateRecordsetInfo::IsAvaible()
{
	return true;
}

//////////////////////////////////////////////////////////////////////
//class CActionEmptyDBField
//////////////////////////////////////////////////////////////////////
bool CActionEmptyDBField::Invoke()
{
	IUnknownPtr ptr_obj = get_active_db_object();
	if( ptr_obj == 0 )	return false;

	m_changes.RemoveFromDocData( m_punkTarget, ptr_obj );

	return true;
}

//////////////////////////////////////////////////////////////////////
bool CActionEmptyDBField::IsAvaible()
{
	IUnknownPtr ptr_obj = get_active_db_object();
	return ( ptr_obj != 0 );
}

//////////////////////////////////////////////////////////////////////
IUnknownPtr CActionEmptyDBField::get_active_db_object()
{
	//Берем активный документ
	IDBaseDocumentPtr	ptr_db_doc = m_punkTarget;
	if( ptr_db_doc == 0 )		return 0;

	//т.к. договорились, что только в бланке акция работает
	IDocumentSitePtr ptr_ds( m_punkTarget );
	if( ptr_ds == 0 )			return 0;
	{
		IUnknownPtr ptr_view;
		ptr_ds->GetActiveView( &ptr_view );
		if( ptr_view == 0 )		return 0;

		IBlankViewPtr ptr_blank_view( ptr_view );
		if( ptr_blank_view == 0 )	return 0;
	}

	//Теперь достаем активный запрос
	IQueryObjectPtr ptr_query;
	{
		IUnknownPtr ptr_punk;
		ptr_db_doc->GetActiveQuery( &ptr_punk );
		ptr_query = ptr_punk;
	}

	if( ptr_query == 0 )		return 0;

	//Активное поле из запроса
	int idx = 0;
	ptr_query->GetActiveField( &idx );

	BSTR _bstr_table = 0;
	BSTR _bstr_field = 0;

	ptr_query->GetField( idx, 0, &_bstr_table, &_bstr_field );

	bstr_t bstr_table, bstr_field;
	if( _bstr_table )
	{
		bstr_table = _bstr_table; 
		::SysFreeString( _bstr_table );	
		_bstr_table = 0;
	}
	if( _bstr_field )
	{
		bstr_field = _bstr_field; 
		::SysFreeString( _bstr_field );	
		_bstr_field = 0;
	}

	if( !bstr_table.length() || !bstr_field.length() )
		return 0;

	CString str_object_name;
	str_object_name.Format( "%s.%s", (char*)bstr_table, (char*)bstr_field);

	IUnknown* punk = ::GetObjectByName( ptr_db_doc, str_object_name, 0 );
	if( !punk )	return false;

    IUnknownPtr ptr_obj = punk;
	punk->Release();	punk = 0;

	return ptr_obj;
}

//////////////////////////////////////////////////////////////////////
bool CActionEmptyDBField::DoUndo()
{
	m_changes.DoUndo( m_punkTarget );
	return true;
}

//////////////////////////////////////////////////////////////////////
bool CActionEmptyDBField::DoRedo()
{
	m_changes.DoRedo( m_punkTarget );
	return true;
}